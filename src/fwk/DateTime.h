#ifndef FWK_DATETIME_H
#define FWK_DATETIME_H

class CenturyType { };
typedef Ordinal<CenturyType, S64> Century;

class YearType { };
typedef Ordinal<YearType, U8> Year;

class MonthType { };
typedef Ordinal<MonthType, U8> Month;

class DayType { };
typedef Ordinal<DayType, U8> Day;

class HourType { };
typedef Ordinal<HourType, U8> Hour;

class MinuteType { };
typedef Ordinal<MinuteType, U8> Minute;

class SecondType { };
typedef Ordinal<SecondType, U16> Second;

class MillisecondType { };
typedef Ordinal<MillisecondType, U16> Millisecond;

class DateTime {
public:

    static DateTime now() {
        return DateTime(SystemTime::now());
    }


    _noinline
    DateTime() :
        century_(0),
        year_(0),
        month_(0),
        day_(0),
        hour_(0),
        minute_(0),
        second_(0),
        millisecond_(0),
        bias_(0),
        dst_(false),
        timeZone_("")
    {
        // Nothing else to do.
    }

    _noinline
    DateTime(const DateTime& dateTime) :
        century_(dateTime.century_),
        year_(dateTime.year_),
        month_(dateTime.month_),
        day_(dateTime.day_),
        hour_(dateTime.hour_),
        minute_(dateTime.minute_),
        second_(dateTime.second_),
        millisecond_(dateTime.millisecond_),
        bias_(dateTime.bias_),
        dst_(dateTime.dst_),
        timeZone_(dateTime.timeZone_)
    {
        // Nothing else to do.
    }

    DateTime(const SystemTime ms) {
        loadFrom(ms);
    }

    DateTime(const Time sec) {
        loadFrom(U64(sec.value() * 1000 + 0.5));
    }

    _noinline
    void operator =(const DateTime& dateTime) {
        century_ = dateTime.century_;
        year_ = dateTime.year_;
        month_ = dateTime.month_;
        day_ = dateTime.day_;
        hour_ = dateTime.hour_;
        minute_ = dateTime.minute_;
        second_ = dateTime.second_;
        millisecond_ = dateTime.millisecond_;
        bias_ = dateTime.bias_;
        dst_ = dateTime.dst_;
        timeZone_ = dateTime.timeZone_;
    }

    void operator =(const SystemTime ms) {
        loadFrom(ms);
    }

    void operator =(const double sec) {
        loadFrom(U64(sec * 1000 + 0.5));
    }

private:

    _noinline
    void loadFrom(const SystemTime ms) {
        const auto t = ms.value();
        time_t sec = time_t(t / 1000);
        if (sec < 0) {
            sec = 0;
        }

        struct tm tm;
    #ifdef _WIN32
        if (localtime_s(&tm, &sec) != 0) {
            sec = 0;
            localtime_s(&tm, &sec);
        }
    #else
        if (localtime_r(&sec, &tm) == 0) {
            sec = 0;
            localtime_r(&sec, &tm);
        }
    #endif

        century_ = tm.tm_year / 100 + 19;
        year_ = U8(tm.tm_year % 100);
        month_ = U8(tm.tm_mon + 1);
        day_ = U8(tm.tm_mday);
        hour_ = U8(tm.tm_hour);
        minute_ = U8(tm.tm_min);
        second_ = U8(tm.tm_sec);
        millisecond_ = t % 1000;
        dst_ = tm.tm_isdst != 0;      
        // TODO: bias, timeZone
    }

public:

    Century century() const {
        return century_;
    }

    Year year() const {
        return year_;
    }

    void yearIs(const Year yy) {
        // TODO: Add checking code.
        year_ = yy;
    }

    Month month() const {
        return month_;
    }

    void monthIs(const Month mo) {
        // TODO: Add checking code.
        month_ = mo;
    }

    Day day() const {
        return day_;
    }

    void dayIs(const Day dd) {
        // TODO: Add checking code.
        day_ = dd;
    }

    Hour hour() const {
        return hour_;
    }

    void hourIs(const Hour hh) {
        // TODO: Add checking code.
        hour_ = hh;
    }

    Minute minute() const {
        return minute_;
    }

    void minuteIs(const Minute mm) {
        // TODO: Add checking code.
        minute_ = mm;
    }

    Second second() const {
        return second_;
    }

    void secondIs(const Second ss) {
        // TODO: Add checking code.
        second_ = ss;
    }

    Millisecond millisecond() const {
        return millisecond_;
    }

    void millisecondIs(const Millisecond ms) {
        // TODO: Add checking code.
        millisecond_ = ms;
    }

    S8 bias() const {
        return bias_;
    }

    void biasIs(const S8 b) {
        // TODO: Add checking code.
        bias_ = b;
        // TODO: Set timeZone.
    }

    bool dst() const {
        return dst_;
    }

    void dstIs(const bool inUse) {
        dst_ = inUse;
    }

    string timeZone() const {
        return timeZone_;
    }

    void timeZoneIs(const string& zone) {
        // TODO: Add checking code.
        timeZone_ = zone;
        // TODO: Set bias.
    }

private:

    Century century_;
    Year year_;
    Month month_;
    Day day_;
    Hour hour_;
    Minute minute_;
    Second second_;
    Millisecond millisecond_;
    S8 bias_;
    bool dst_;
    string timeZone_;

};


_noinline
string timeAsString(const DateTime& d) {
    const auto hh = d.hour().value();
    const auto mm = d.minute().value();
    const auto ss = d.second().value();

    char* const buffer = new char[32];
    char* p = buffer;
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

_noinline
string dateTimeAsString(const DateTime& d) {
    const auto cc = d.century().value();
    const auto yy = d.year().value();
    const auto mo = d.month().value();
    const auto dd = d.day().value();
    const auto hh = d.hour().value();
    const auto mm = d.minute().value();
    const auto ss = d.second().value();

    char* buffer = new char[32];
    char* p = buffer;
    *p++ = '0' + char(cc / 10);
    *p++ = '0' + char(cc % 10);
    *p++ = '0' + char(yy / 10);
    *p++ = '0' + char(yy % 10);
    *p++ = '-';
    *p++ = '0' + char(mo / 10);
    *p++ = '0' + char(mo % 10);
    *p++ = '-';
    *p++ = '0' + char(dd / 10);
    *p++ = '0' + char(dd % 10);
    *p++ = ' ';

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


string timeAsString(const Time sec) {
    return timeAsString(DateTime(sec));
}

string dateTimeAsString(const Time sec) {
    return dateTimeAsString(DateTime(sec));
}


#endif
