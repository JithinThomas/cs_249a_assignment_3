#ifndef FWK_SYSTEMTIME_H
#define FWK_SYSTEMTIME_H

class SystemTime {
public:

    _noinline
    static SystemTime now() {
#   ifdef _WIN32
        struct __timeb64 t;
        ::_ftime64_s(&t);

        return t.time * 1000 + t.millitm;
#   else
        struct timeval tv;
        ::gettimeofday(&tv, NULL);

        return U64(tv.tv_sec * 1000 + tv.tv_usec / 1000);
#   endif
    }

    _noinline
    static void sleep(const SystemTime ms) {
#       ifdef _WIN32
            ::Sleep(ms.value());
#       else
            usleep(useconds_t(ms.value() * 1000));
#       endif
    }


    SystemTime() :
        value_(0)
    {
        // Nothing else to do.
    }

    SystemTime(const U64 ms) :
        value_(ms)
    {
        // Nothing else to do.
    }

    SystemTime(const SystemTime& ms) :
        value_(ms.value_)
    {
        // Nothing else to do.
    }


    void operator =(const SystemTime& ms) {
        value_ = ms.value_;
    }

    void operator =(const U64 ms) {
        value_ = ms;
    }


    void operator +=(const SystemTime ms) {
        value_ += ms.value_;
    }

    void operator -=(const SystemTime ms) {
        value_ -= ms.value_;
    }


    SystemTime operator +(const SystemTime ms) const {
        return value_ + ms.value_;
    }

    SystemTime operator -(const SystemTime ms) const {
        return value_ - ms.value_;
    }


    U64 value() const {
        return value_;
    }

protected:

    U64 value_;

};


_noinline
string to_string(const SystemTime t) {
    const auto ms = t.value();
    const auto seconds = ms / 1000;
    const auto minutes = seconds / 60;
    const auto hours = minutes / 60;
    const auto hh = U8(hours % 24);
    const auto mm = U8(minutes % 60);
    const auto ss = U8(seconds % 60);

    char* buffer = new char[32];
    auto p = buffer;
    *p++ = '0' + char(hh / 10);
    *p++ = '0' + char(hh % 10);
    *p++ = ':';
    *p++ = '0' + char(mm / 10);
    *p++ = '0' + char(mm % 10);
    *p++ = ':';
    *p++ = '0' + char(ss / 10);
    *p++ = '0' + char(ss % 10);
    *p = 0;
    return buffer;
}

#endif
