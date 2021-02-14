#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_HPP
#error "Include Dynamic.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_TCC

namespace FunnyOS::Stdlib {
    namespace _Internal {
        template <typename T>
        struct DefaultDeleter {
            inline void operator()(T* ptr) {
                delete ptr;
            }
        };

        template <typename T>
        struct DefaultDeleter<T[]> {
            inline void operator()(T* ptr) {
                delete[] ptr;
            }
        };
    }  // namespace _Internal

    template <typename T>
    SmartSizedBuffer<T>::SmartSizedBuffer(const SmartSizedBuffer& other) {
        this->Data = nullptr;
        Memory::ReallocateBuffer(*this, other.Size);
        Memory::Copy(*this, other.Data);
    }

    template <typename T>
    SmartSizedBuffer<T>& SmartSizedBuffer<T>::operator=(const SmartSizedBuffer& other) {
        if (&other == this) {
            return *this;
        }

        Memory::ReallocateBuffer(*this, other.Size);
        Memory::Copy(*this, other.Data);
        return *this;
    }

    template <typename T>
    SmartSizedBuffer<T>::SmartSizedBuffer(SmartSizedBuffer&& other) noexcept {
        Memory::FreeBuffer(*this);
        this->Data = other.Data;
        this->Size = other.Size;
        other.Data = nullptr;
        other.Size = 0;
    }

    template <typename T>
    SmartSizedBuffer<T>& SmartSizedBuffer<T>::operator=(SmartSizedBuffer&& other) noexcept {
        if (&other == this) {
            return *this;
        }

        Memory::FreeBuffer(*this);
        this->Data = other.Data;
        this->Size = other.Size;
        other.Data = nullptr;
        other.Size = 0;

        return *this;
    }

    template <typename T>
    SmartSizedBuffer<T>::SmartSizedBuffer(size_t size) {
        this->Data = nullptr;
        Memory::ReallocateBuffer(*this, size);
    }

    template <typename T>
    SmartSizedBuffer<T>::~SmartSizedBuffer() {
        Free();
    }

    template <typename T>
    void SmartSizedBuffer<T>::Free() {
        Memory::FreeBuffer(*this);

        this->Data = nullptr;
        this->Size = 0;
    }

    template <typename T, typename Deleter>
    Owner<T, Deleter>& Owner<T, Deleter>::operator=(Owner<T, Deleter>&& other) noexcept {
        if (&other == this) {
            return *this;
        }

        Reset(other.m_data);
        other.m_data = nullptr;
        return *this;
    }

    template <typename T, typename Deleter>
    Owner<T, Deleter>::Owner(Owner<T, Deleter>&& other) noexcept {
        m_data = other.m_data;
        other.m_data = nullptr;
    }

    template <typename T, typename Deleter>
    Owner<T, Deleter>::Owner() : m_data(nullptr) {}

    template <typename T, typename Deleter>
    Owner<T, Deleter>::Owner(T* ptr) : m_data(ptr) {}

    template <typename T, typename Deleter>
    Owner<T, Deleter>::~Owner() {
        Reset(nullptr);
    }

    template <typename T, typename Deleter>
    void Owner<T, Deleter>::Reset(T* ptr) noexcept {
        if (m_data != nullptr) {
            m_deleter(m_data);
        }

        m_data = ptr;
    }

    template <typename T, typename Deleter>
    T* Owner<T, Deleter>::Release() noexcept {
        T* ptr = m_data;
        m_data = nullptr;
        return ptr;
    }

    template <typename T, typename Deleter>
    T* Owner<T, Deleter>::Get() noexcept {
        return m_data;
    }

    template <typename T, typename Deleter>
    const T* Owner<T, Deleter>::Get() const noexcept {
        return m_data;
    }

    template <typename T, typename Deleter>
    T* Owner<T, Deleter>::operator->() {
        return m_data;
    }

    template <typename T, typename Deleter>
    const T* Owner<T, Deleter>::operator->() const {
        return m_data;
    }

    template <typename T, typename... Args, typename Deleter>
    Owner<T, Deleter> MakeOwner(Args&&... args) {
        return Owner<T, Deleter>(new T(Forward<Args>(args)...));
    }

    namespace _Internal {
        RefCounter::RefCounter() : m_counter{nullptr} {}

        void RefCounter::Initialize() {
            m_counter = new unsigned int{1};
        }

        RefCounter RefCounter::Acquire() {
            if (m_counter == nullptr) {
                return *this;
            }

            (*m_counter)++;

            RefCounter counter;
            counter.m_counter = m_counter;
            return counter;
        }

        bool RefCounter::Release() {
            if (m_counter == nullptr) {
                return false;
            }

            return --(*m_counter) == 0;
        }

        void RefCounter::Invalidate() {
            m_counter = nullptr;
        }
    }  // namespace _Internal

    template <typename T>
    Ref<T>& Ref<T>::operator=(const Ref<T>& other) {
        if (&other == this) {
            return *this;
        }

        Reset(nullptr);
        m_refCounter = other.m_refCounter.Acquire();
        m_data = other.m_data;

        return *this;
    }

    template <typename T>
    Ref<T>::Ref(const Ref<T>& other) : m_refCounter(other.m_refCounter.Acquire()), m_data(other.m_data) {}

    template <typename T>
    Ref<T>& Ref<T>::operator=(Ref<T>&& other) noexcept {
        if (&other == this) {
            return *this;
        }

        Reset(nullptr);
        m_refCounter = other.m_refCounter;
        m_data = other.m_data;
        other.m_refCounter.Invalidate();
        other.m_data = nullptr;

        return *this;
    }

    template <typename T>
    Ref<T>::Ref(Ref<T>&& other) noexcept : m_refCounter(other.m_refCounter), m_data(other.m_data) {
        other.m_refCounter.Invalidate();
        other.m_data = nullptr;
    }

    template <typename T>
    Ref<T>::Ref() : m_refCounter{}, m_data{nullptr} {}

    template <typename T>
    Ref<T>::Ref(T* ptr) : m_refCounter{}, m_data{nullptr} {
        Reset(ptr);
    }

    template <typename T>
    Ref<T>::~Ref() {
        Reset(nullptr);
    }

    template <typename T>
    void Ref<T>::Reset(T* data) noexcept {
        if (m_refCounter.Release()) {
            delete m_data;
            m_data = nullptr;
        }
        m_refCounter.Invalidate();

        if (data != nullptr) {
            m_refCounter.Initialize();
            m_data = data;
        }
    }

    template <typename T>
    T* Ref<T>::Get() const noexcept {
        return m_data;
    }

    template <typename T>
    T* Ref<T>::operator->() const noexcept {
        return m_data;
    }

    template <typename T>
    template <typename U>
    Ref<T>::Ref(const Ref<U>& ref, T* ptr) : m_refCounter{ref.m_refCounter.Acquire()}, m_data{ptr} {}

    template <typename T>
    template <typename U>
    Ref<T>::Ref(Ref<U>&& ref, T* ptr) : m_refCounter{ref.m_refCounter}, m_data{ptr} {
        ref.m_data = nullptr;
        ref.m_refCounter.Invalidate();
    }

    template <typename T, typename... Args>
    Ref<T> MakeRef(Args&&... args) {
        return Ref<T>(new T(Forward<Args>(args)...));
    }

    template <typename T, typename U>
    Ref<T> StaticRefCast(const Ref<U>& ref) {
        return Ref<T>(ref, static_cast<T*>(ref.Get()));
    }

    template <typename T, typename U>
    Ref<T> StaticRefCast(Ref<U>&& ref) {
        return Ref<T>(Forward<Ref<U>&&>(ref), static_cast<T*>(ref.Get()));
    }
}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_TCC
