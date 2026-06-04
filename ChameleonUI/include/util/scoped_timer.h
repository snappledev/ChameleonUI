#pragma once
#include <chrono>
#include <map>
#include <sstream>
#include <string>
#include <windows.h>

class ScopedTimer
{
public:
    explicit ScopedTimer(const char* name)
        : m_name(name), m_start(std::chrono::steady_clock::now()) {}

    ~ScopedTimer()
    {
        auto end = std::chrono::steady_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - m_start).count();
        Accum()[m_name] += ms;
        Count()[m_name]++;
        auto& last = LastReport();
        if (end - last > std::chrono::seconds(2))
        {
            std::stringstream ss;
            ss << "[UI profile] ";
            for (auto& kv : Accum())
            {
                int n = Count()[kv.first];
                double avg = n ? (kv.second / n) : 0.0;
                ss << kv.first << ": total=" << kv.second << "ms avg=" << avg << "ms n=" << n << "; ";
            }
            ss << "\n";
            OutputDebugStringA(ss.str().c_str());
            Accum().clear();
            Count().clear();
            last = end;
        }
    }

private:
    const char* m_name;
    std::chrono::steady_clock::time_point m_start;

    static std::map<std::string, double>& Accum()
    {
        static std::map<std::string, double> s; return s;
    }
    static std::map<std::string, int>& Count()
    {
        static std::map<std::string, int> s; return s;
    }
    static std::chrono::steady_clock::time_point& LastReport()
    {
        static auto s = std::chrono::steady_clock::now(); return s;
    }
};

#define PROFILE_SCOPE(name) ScopedTimer _prof_##name(#name)
