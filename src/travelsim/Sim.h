
#ifndef SIM_H
#define SIM_H

#include "fwk/fwk.h"

#include <random>

using namespace fwk;
using NotifierLib::post;

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
