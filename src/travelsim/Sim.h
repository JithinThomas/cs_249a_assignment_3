
#ifndef SIM_H
#define SIM_H

#include "fwk/fwk.h"

#include <random>

using namespace fwk;
using NotifierLib::post;

class Random : public PtrInterface {
public:

    static Ptr<Random> instanceNew() {
        return new Random();
    }


    double normal(const double mean, const double dev) {
        return dev * distribution_(generator_) + mean;
    }

    double normalRange(
        const double mean, const double dev,
        const double lower, const double upper
    ) {
        const auto r = normal(mean, dev);

        if (r < lower) {
            return lower;
        }

        if (r > upper) {
            return upper;
        }

        return r;
    }

protected:

    std::default_random_engine generator_;
    std::normal_distribution<double> distribution_;


    Random() :
        generator_(U32(SystemTime::now().value() & 0xffffffff)),
        distribution_(0.0, 1.0)
    {
        // Nothing else to do.
    }

};


static Ptr<Random> rng = Random::instanceNew();

static void logEntryNew(const Time t, const string& s) {
    std::cout << timeAsString(t) << " " << s << std::endl;
}

/**
 * Common base class for simulations.
 */
class Sim : public Activity::Notifiee {
public:

    Ptr<Activity> activity() {
        return notifier();
    }

    void activityDel() {
        const auto a = notifier();
        a->manager()->activityDel(a->name());
    }

};

#endif
