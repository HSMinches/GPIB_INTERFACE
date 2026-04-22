// GPIB INTERFACE
// HEITOR SAMPAIO MENDES


#include <windows.h>

#include <chrono>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>

using namespace std::chrono_literals;

namespace {
constexpr const char* SERIAL_PORT = "COM4";
constexpr DWORD BAUDRATE = 115200;
constexpr int GPIB_ADDR = 10;

[[noreturn]] void throw_system_error(const std::string& message) {
    throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), message);
}

std::string normalize_port_name(const std::string& port) {
    if (port.rfind("\\\\.\\", 0) == 0) {
        return port;
    }
    return "\\\\.\\" + port;
}

std::string trim(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
        s.erase(s.begin());
    }
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
        s.pop_back();
    }
    return s;
}
}  // namespace

class AR488 {
public:
    AR488(std::string port, DWORD baudrate = BAUDRATE, int gpib_addr = GPIB_ADDR)
        : handle_(INVALID_HANDLE_VALUE),
          port_(normalize_port_name(std::move(port))),
          baudrate_(baudrate),
          gpib_addr_(gpib_addr) {
        open_port();
        std::this_thread::sleep_for(2s);
        clear();
        init_adapter();
        clear();
    }

    ~AR488() {
        close();
    }

    AR488(const AR488&) = delete;
    AR488& operator=(const AR488&) = delete;

    AR488(AR488&& other) noexcept
        : handle_(other.handle_),
          port_(std::move(other.port_)),
          baudrate_(other.baudrate_),
          gpib_addr_(other.gpib_addr_) {
        other.handle_ = INVALID_HANDLE_VALUE;
    }

    AR488& operator=(AR488&& other) noexcept {
        if (this != &other) {
            close();
            handle_ = other.handle_;
            port_ = std::move(other.port_);
            baudrate_ = other.baudrate_;
            gpib_addr_ = other.gpib_addr_;
            other.handle_ = INVALID_HANDLE_VALUE;
        }
        return *this;
    }

    void close() noexcept {
        if (handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
            handle_ = INVALID_HANDLE_VALUE;
        }
    }

    void clear() {
        ensure_open();
        if (!PurgeComm(handle_, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT)) {
            throw_system_error("PurgeComm failed");
        }
    }

    void write(const std::string& cmd) {
        ar488_cmd("++addr " + std::to_string(gpib_addr_));
        clear();
        write_line(cmd);
    }

    std::string query(const std::string& cmd, double timeout_seconds = 4.0, double settle_seconds = 0.15) {
        ar488_cmd("++addr " + std::to_string(gpib_addr_));
        clear();
        write_line(cmd);
        sleep_seconds(settle_seconds);
        write_line("++read eoi");

        std::string reply = trim(read_until_idle(timeout_seconds));
        if (!reply.empty()) {
            return reply;
        }

        clear();
        write_line(cmd);
        sleep_seconds(settle_seconds);
        write_line("++read");
        return trim(read_until_idle(timeout_seconds));
    }

private:
    HANDLE handle_;
    std::string port_;
    DWORD baudrate_;
    int gpib_addr_;

    void ensure_open() const {
        if (handle_ == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Serial port is not open");
        }
    }

    void open_port() {
        handle_ = CreateFileA(
            port_.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (handle_ == INVALID_HANDLE_VALUE) {
            throw_system_error("CreateFile failed for " + port_);
        }

        if (!SetupComm(handle_, 4096, 4096)) {
            throw_system_error("SetupComm failed");
        }

        DCB dcb{};
        dcb.DCBlength = sizeof(dcb);
        if (!GetCommState(handle_, &dcb)) {
            throw_system_error("GetCommState failed");
        }

        dcb.BaudRate = baudrate_;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        dcb.fBinary = TRUE;
        dcb.fParity = FALSE;
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fDsrSensitivity = FALSE;
        dcb.fTXContinueOnXoff = TRUE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
        dcb.fAbortOnError = FALSE;

        if (!SetCommState(handle_, &dcb)) {
            throw_system_error("SetCommState failed");
        }

        COMMTIMEOUTS timeouts{};
        timeouts.ReadIntervalTimeout = MAXDWORD;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.ReadTotalTimeoutConstant = 0;
        timeouts.WriteTotalTimeoutMultiplier = 0;
        timeouts.WriteTotalTimeoutConstant = 1000;

        if (!SetCommTimeouts(handle_, &timeouts)) {
            throw_system_error("SetCommTimeouts failed");
        }
    }

    void write_raw(const std::string& data) {
        ensure_open();

        const char* buffer = data.data();
        DWORD remaining = static_cast<DWORD>(data.size());

        while (remaining > 0) {
            DWORD written = 0;
            if (!WriteFile(handle_, buffer, remaining, &written, nullptr)) {
                throw_system_error("WriteFile failed");
            }
            if (written == 0) {
                throw std::runtime_error("WriteFile wrote 0 bytes");
            }

            buffer += written;
            remaining -= written;
        }

        if (!FlushFileBuffers(handle_)) {
            throw_system_error("FlushFileBuffers failed");
        }
    }

    void write_line(const std::string& s) {
        write_raw(trim_right_newlines(s) + "\n");
    }

    static std::string trim_right_newlines(std::string s) {
        while (!s.empty() && (s.back() == '\r' || s.back() == '\n')) {
            s.pop_back();
        }
        return s;
    }

    DWORD bytes_available() const {
        ensure_open();

        COMSTAT status{};
        DWORD errors = 0;
        if (!ClearCommError(handle_, &errors, &status)) {
            throw_system_error("ClearCommError failed");
        }
        return status.cbInQue;
    }

    std::string read_some(DWORD max_bytes) {
        ensure_open();

        if (max_bytes == 0) {
            return {};
        }

        std::string buffer(max_bytes, '\0');
        DWORD bytes_read = 0;

        if (!ReadFile(handle_, buffer.data(), max_bytes, &bytes_read, nullptr)) {
            throw_system_error("ReadFile failed");
        }

        buffer.resize(bytes_read);
        return buffer;
    }

    std::string read_until_idle(double total_timeout_seconds = 4.0, double idle_timeout_seconds = 0.25) {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::duration<double>(total_timeout_seconds);
        auto last_rx = std::chrono::steady_clock::now();
        std::string result;

        while (std::chrono::steady_clock::now() < deadline) {
            DWORD waiting = bytes_available();
            if (waiting > 0) {
                std::string chunk = read_some(waiting);
                if (!chunk.empty()) {
                    result += chunk;
                    last_rx = std::chrono::steady_clock::now();
                    continue;
                }
            }

            std::string one = read_some(1);
            if (!one.empty()) {
                result += one;
                last_rx = std::chrono::steady_clock::now();
                continue;
            }

            if (!result.empty()) {
                const auto idle = std::chrono::duration<double>(std::chrono::steady_clock::now() - last_rx).count();
                if (idle >= idle_timeout_seconds) {
                    break;
                }
            }

            std::this_thread::sleep_for(10ms);
        }

        return trim(result);
    }

    std::string ar488_cmd(const std::string& cmd, bool expect_reply = false, double timeout_seconds = 2.0) {
        clear();
        write_line(cmd);
        if (expect_reply) {
            return read_until_idle(timeout_seconds);
        }
        return {};
    }

    void init_adapter() {
        ar488_cmd("++auto 0");
        ar488_cmd("++eoi 1");
        ar488_cmd("++eos 2");
        ar488_cmd("++eot_enable 1");
        ar488_cmd("++eot_char 10");
        ar488_cmd("++read_tmo_ms 5000");
        ar488_cmd("++ifc");
        ar488_cmd("++addr " + std::to_string(gpib_addr_));
    }

    static void sleep_seconds(double seconds) {
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }
};

int main() {
    try {
        AR488 bus(SERIAL_PORT, BAUDRATE, GPIB_ADDR);

        std::cout << "IDN   : " << std::quoted(bus.query("*IDN?")) << '\n';

        bus.write("*CLS");
        std::this_thread::sleep_for(100ms);

        bus.write("VOLT 5");
        bus.write("CURR 1");
        bus.write("OUTP ON");
        std::this_thread::sleep_for(500ms);

        std::cout << "OUTP? : " << std::quoted(bus.query("OUTP?")) << '\n';
        std::cout << "VOLT? : " << std::quoted(bus.query("VOLT?")) << '\n';
        std::cout << "CURR? : " << std::quoted(bus.query("CURR?")) << '\n';
        std::cout << "MEASV : " << std::quoted(bus.query("MEAS:VOLT?")) << '\n';
        std::cout << "MEASI : " << std::quoted(bus.query("MEAS:CURR?")) << '\n';
        std::cout << "QUES? : " << std::quoted(bus.query("STAT:QUES:COND?")) << '\n';
        std::cout << "RI?   : " << std::quoted(bus.query("OUTP:RI:MODE?")) << '\n';
        std::cout << "ERR?  : " << std::quoted(bus.query("SYST:ERR?")) << '\n';

        std::cout << "Press Enter to turn output OFF...";
        std::string line;
        std::getline(std::cin, line);

        bus.write("OUTP OFF");
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    }
}