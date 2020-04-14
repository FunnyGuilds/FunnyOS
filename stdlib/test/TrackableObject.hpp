#ifndef FUNNYOS_STDLIB_TEST_TRACKABLEOBJECT_HPP
#define FUNNYOS_STDLIB_TEST_TRACKABLEOBJECT_HPP

#include <iostream>

class TrackableObject {
   public:
    explicit TrackableObject() {
        m_testFlag = VALID_OBJECT_FLAG;
        s_standardConstructionCount++;

        std::cout << "Constructed " << static_cast<void*>(this) << "\n";
    }

    explicit TrackableObject(bool& flag) : TrackableObject() {
        flag = true;
    }

    ~TrackableObject() {
        s_destructionCount++;
        std::cout << "Deconstructed " << static_cast<void*>(this) << "\n";
    }

    TrackableObject(const TrackableObject& other) {
        s_copyConstructionCount++;
        m_testFlag = other.m_testFlag;
        std::cout << "Copied " << static_cast<void*>(this) << "\n";
    }

    TrackableObject& operator=(const TrackableObject& other) {
        m_testFlag = other.m_testFlag;
        std::cout << "Copy assigned " << static_cast<void*>(this) << "\n";
        return *this;
    }

    TrackableObject(TrackableObject&& other) {
        s_moveConstructionCount++;
        m_testFlag = other.m_testFlag;
        other.m_testFlag = 0;
        std::cout << "Moved " << static_cast<void*>(this) << "\n";
    }

    TrackableObject& operator=(TrackableObject&& other) {
        m_testFlag = other.m_testFlag;
        other.m_testFlag = 0;
        std::cout << "Move assigned " << static_cast<void*>(this) << "\n";
        return *this;
    }

   public:
    inline bool IsValidObject() const {
        return m_testFlag == VALID_OBJECT_FLAG;
    }

    static void ResetAll() {
        s_standardConstructionCount = 0;
        s_moveConstructionCount = 0;
        s_copyConstructionCount = 0;
        s_destructionCount = 0;
    }

    static size_t GetStandardConstructionCount() {
        return s_standardConstructionCount;
    }

    static size_t GetMoveConstructionCount() {
        return s_moveConstructionCount;
    }

    static size_t GetCopyConstructionCount() {
        return s_copyConstructionCount;
    }

    static size_t GetDestructionCount() {
        return s_destructionCount;
    }

    static size_t GetTotalConstructionCount() {
        return s_standardConstructionCount + s_moveConstructionCount + s_copyConstructionCount;
    }

   private:
    static constexpr const ::size_t VALID_OBJECT_FLAG = 0x5634312;
    static inline ::size_t s_standardConstructionCount = 0;
    static inline ::size_t s_moveConstructionCount = 0;
    static inline ::size_t s_copyConstructionCount = 0;
    static inline ::size_t s_destructionCount = 0;

    ::size_t m_testFlag;
};

#endif  // FUNNYOS_STDLIB_TEST_TRACKABLEOBJECT_HPP
