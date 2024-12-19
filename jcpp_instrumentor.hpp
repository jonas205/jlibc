#pragma once

// Include the header and use the macros
// define JC_PROFILE to activate macros, don't do this in prod,
// this will slow down everything!
// without JC_PROFILE, nothing is done

//     JC_PROFILE_BEGIN_SESSION("FOO", "foo.json");
//     {
//         JC_PROFILE_SCOPE("foo");
//         foo();
//     }
//     JC_PROFILE_END_SESSION();

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>

namespace JC {

using fp_ms = std::chrono::duration<double, std::micro>;

struct ProfileResult {
  std::string name;
  fp_ms start, elapsed;
  std::thread::id thread;
};

struct InstrumentorSession {
  std::string name;
};

class Instrumentor {
  Instrumentor() : m_current_session(nullptr), m_profile_count(0) {}

public:
  void begin_session(const std::string &name, const std::string &filepath) {
    m_output.open(filepath);
    write_header();
    m_current_session = new InstrumentorSession{name};
  }

  void end_session() {
    write_footer();
    m_output.close();
    delete m_current_session;
    m_current_session = nullptr;
    m_profile_count = 0;
  }

  void write_profile(const ProfileResult &result) {
    if (m_profile_count > 0) {
      m_output << ',';
    }
    m_profile_count++;

    std::string name = result.name;
    std::replace(name.begin(), name.end(), '"', '\'');

    m_output << std::setprecision(3) << std::fixed;
    m_output << '{';
    m_output << "\"cat\":\"function\",";
    m_output << "\"dur\":" << (result.elapsed.count()) << ',';
    m_output << "\"name\":\"" << name << "\",";
    m_output << "\"ph\":\"X\",";
    m_output << "\"pid\":0,";
    m_output << "\"tid\":" << result.thread << ',';
    m_output << "\"ts\":" << result.start.count();
    m_output << '}';

    m_output.flush();
  }

  static Instrumentor &get() {
    static Instrumentor instance;
    return instance;
  }

private:
  void write_header() {
    m_output << "{\"otherData\": {},\"traceEvents\":[";
    m_output.flush();
  }

  void write_footer() {
    m_output << "]}";
    m_output.flush();
  }

  InstrumentorSession *m_current_session;
  std::ofstream m_output;
  int m_profile_count;
};

class InstrumentationTimer {
public:
  InstrumentationTimer(const char *name)
      : m_name(name), m_start(std::chrono::steady_clock::now()),
        m_stopped(false) {}

  ~InstrumentationTimer() {
    if (!m_stopped) {
      stop();
    }
  }

  void stop() {
    auto end_time = std::chrono::steady_clock::now();
    auto start = fp_ms{m_start.time_since_epoch()};
    auto elapsed =
        std::chrono::time_point_cast<std::chrono::microseconds>(end_time)
            .time_since_epoch() -
        std::chrono::time_point_cast<std::chrono::microseconds>(m_start)
            .time_since_epoch();

    Instrumentor::get().write_profile(
        {m_name, start, elapsed, std::this_thread::get_id()});

    m_stopped = true;
  }

private:
  const char *m_name;
  std::chrono::time_point<std::chrono::steady_clock> m_start;
  bool m_stopped;
};

#ifdef JC_PROFILE

#define JC_PROFILE_BEGIN_SESSION(name, filepath)                               \
  JC::Instrumentor::get().begin_session(name, filepath)
#define JC_PROFILE_END_SESSION() JC::Instrumentor::get().end_session()
#define JC_PROFILE_SCOPE(name) JC::InstrumentationTimer timer##__LINE__(name)
#define JC_PROFILE_FUNCTION() JC_PROFILE_SCOPE(__PRETTY_FUNCTION__)

#else
#define JC_PROFILE_BEGIN_SESSION(name, filepath)
#define JC_PROFILE_END_SESSION()
#define JC_PROFILE_SCOPE(name)
#define JC_PROFILE_FUNCTION()
#endif

} // namespace JC
