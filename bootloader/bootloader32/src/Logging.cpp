#include "Logging.hpp"

#include <FunnyOS/Misc/TerminalManager/TerminalManagerLoggingSink.hpp>
#include <FunnyOS/Hardware/GFX/VGA.hpp>
#include <FunnyOS/Hardware/Serial.hpp>

namespace FunnyOS::Bootloader32::Logging {
    using namespace Stdlib;
    using namespace Misc::TerminalManager;

    class SerialLoggingSink : public ILoggingSink {
       public:
        constexpr static HW::Serial::COMPort SERIAL_PORT = HW::Serial::COMPort::COM1;

       public:
        static void WriteToSerial(const char* message) {
            while (*message != 0) {
                while (!CanWrite(SERIAL_PORT)) {
                    // Wait
                }

                Write(SERIAL_PORT, static_cast<uint8_t>(*message));
                message++;
            }
        }

        void SubmitMessage(LogLevel level, const char* message) override {
            using namespace HW::Serial;

            if (!s_serialInitialized) {
                InitializeCOMPort(SERIAL_PORT, DataBits::BITS_8, StopBits::STOP_1, ParityBits::NONE, 115200);
                s_serialInitialized = true;
            }

            WriteToSerial(GetLogLevelName(level));
            WriteToSerial(": ");
            WriteToSerial(message);
            WriteToSerial("\r\n");
        }

       private:
        inline static bool s_serialInitialized = false;
    };

    Ref<FilteringLoggingSink>& GetVgaOutputSink() {
        static Ref<FilteringLoggingSink> c_vgaSink;
        return c_vgaSink;
    }

    Ref<FilteringLoggingSink>& GetSerialLoggingSink() {
        static Ref<FilteringLoggingSink> c_serialSink;
        return c_serialSink;
    }

    void InitLogging() {
        auto& tm = GetTerminalManager();
        GetVgaOutputSink().Reset(new FilteringLoggingSink(Ref<ILoggingSink>(new TerminalManagerLoggingSink(tm))));
        GetSerialLoggingSink().Reset(new FilteringLoggingSink(Ref<ILoggingSink>(new SerialLoggingSink())));

#ifdef F_DEBUG
        GetVgaOutputSink()->SetLevel(LogLevel::Debug);
        GetSerialLoggingSink()->SetLevel(LogLevel::Debug);
#endif

        GetLogger().AddSink(StaticRefCast<ILoggingSink>(GetSerialLoggingSink()));
        GetLogger().AddSink(StaticRefCast<ILoggingSink>(GetVgaOutputSink()));
    }

    Ref<Misc::TerminalManager::TerminalManager>& GetTerminalManager() {
        static auto c_interface = MakeRef<HW::VGAInterface>();
        static auto c_terminalManager = MakeRef<TerminalManager>(StaticRefCast<ITerminalInterface>(c_interface));

        return c_terminalManager;
    }

    Logger& GetLogger() {
        static Logger c_logger;
        return c_logger;
    }
}  // namespace FunnyOS::Bootloader32::Logging