#include <string>
#include <chrono>
#include <mutex>
#include <fstream>
#include <algorithm>

struct ProfilingResult {
    std::string name;
    long long start, end;
    friend std::ostream& operator<<(std::ostream& os, const ProfilingResult& res);
};

struct InstrumentationSession {
    std::string name;
};

class ScopedTimer {
private:
    std::chrono::high_resolution_clock::time_point m_start, m_end;
    bool m_running;
    const char* m_name;
public:
    ScopedTimer(const char* name);
    ~ScopedTimer();
    void stop();
};

class Instrumentation {
private:
    std::mutex m_mutex;
    InstrumentationSession* m_session;
    std::ofstream m_output_stream;
    int m_profile_count;
    bool headerNotWritten = true;
    Instrumentation();
public:
    void begin_profiling(const std::string& sessionName, const std::string& fileName="profile_result.json");
    void end_profiling();
    void write_header();
    void write_footer();
    void write_profiling(const ProfilingResult& result);

    static Instrumentation& instance() {
        static Instrumentation inst;
        return inst;
    }
};

#define PROFILING 1
#if PROFILING
#define PROFILE_TIMER(name) ScopedTimer timer##__LINE__{name}
#define PROFILE_FUNCTION() PROFILE_TIMER(__func__)
#else 
#define PROFILE_TIMER(name)
#define PROFILE_FUNCTION() PROFILE_TIMER("")
#endif