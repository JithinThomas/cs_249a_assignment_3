#ifndef FWK_TIME_H
#define FWK_TIME_H

/**
 * Time is an ordered value representing a double-precision number of seconds.
 */
class TimeType { };
typedef Ordinal<TimeType, double> Time;

Time time(const SystemTime ms) {
    return double(ms.value()) / 1000;
}

#endif
