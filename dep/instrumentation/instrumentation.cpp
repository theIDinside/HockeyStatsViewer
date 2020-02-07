#include "instrumentation.h"

inline constexpr auto to_milliseconds(std::chrono::high_resolution_clock::time_point tp) {
    return std::chrono::time_point_cast<std::chrono::milliseconds>(tp).time_since_epoch().count();
}

inline constexpr auto to_microseconds(std::chrono::high_resolution_clock::time_point tp) {
    return std::chrono::time_point_cast<std::chrono::microseconds>(tp).time_since_epoch().count();
}

std::ostream& operator<<(std::ostream& os, const ProfilingResult& res) {
        os << res.end - res.start;
        return os;
}

ScopedTimer::ScopedTimer(const char* name) : m_name(name), m_running(false) {
    m_start = std::chrono::high_resolution_clock::now();
    m_running = true;
}

void ScopedTimer::stop() {
    m_end = std::chrono::high_resolution_clock::now();
    auto start = to_milliseconds(m_start);
    auto end = to_milliseconds(m_end);
    Instrumentation::instance().write_profiling({m_name, start, end});
    m_running = false;
}

ScopedTimer::~ScopedTimer() {
    if(m_running)
        stop();
}

Instrumentation::Instrumentation() : m_session(nullptr), m_profile_count(0) {

}

void Instrumentation::begin_profiling(const std::string& sessionName, const std::string& file_path) {
    m_output_stream.open(file_path);
    write_header();
    m_session = new InstrumentationSession{ sessionName };
}

void Instrumentation::end_profiling() {
    write_footer();
    m_output_stream.close();
    delete m_session;
    m_session = nullptr;
    m_profile_count = 0;
}

void Instrumentation::write_footer() {
    m_output_stream << R"(]})";
}

void Instrumentation::write_header() {
    m_output_stream << R"({"otherData": {}, "traceEvents":[ )";
    headerNotWritten = false;
}

void Instrumentation::write_profiling(const ProfilingResult& result) {
    if(m_profile_count++ > 0)
        m_output_stream << ",";
    auto pid = 0;
    auto tid = 0;
    std::string name = result.name;
    std::replace(name.begin(), name.end(), '"', '\'');
    m_output_stream 
    << R"({"cat": "function", "dur":)" << result 
    << R"(,"name":")" << name << R"(")" 
    << R"(,"ph":"X")"
    << R"(,"pid":)" << pid 
    << R"(,"tid":)" << tid
    << R"(,"ts":)" << result.start 
    << "}";
    m_output_stream.flush();
}