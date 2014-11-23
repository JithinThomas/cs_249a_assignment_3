
#ifndef TRIPSIM_H
#define TRIPSIM_H

#include "Trip.h"

class TripSim : public Sim {
public:

	static Ptr<TripSim> instanceNew(const string& name,
								    const Ptr<Location>& startLocation,
								    const Ptr<Location>& destination,
								    const Ptr<ActivityManager>& mgr) {
		const Ptr<TripSim> sim = new TripSim(startLocation, destination);
		const auto a = mgr->activityNew(name);
		a->nextTimeIs(mgr->now());
		a->statusIs(Activity::scheduled);
		mgr->activityAdd(a);
		sim->notifierIs(a);

		return sim;
	}

    void onStatus() {
        const auto a = notifier();
        if (a->status() == Activity::running) {
        	const auto t = a->manager()->now();
        	switch(trip_->status()) {
        		case Trip::requested:
        			logEntryNew(t, "Dispatching vehicle for trip");
        			trip_->statusIs(Trip::vehicleDispatched);
        			a->nextTimeIsOffset(1);
        			break;

        		case Trip::vehicleDispatched:
        			logEntryNew(t, "Passenger picked up. Transporting passenger to destination");
        			trip_->statusIs(Trip::transportingPassenger);
        			a->nextTimeIsOffset(1);
        			break;

        		case Trip::transportingPassenger:
        			logEntryNew(t, "Passenger dropped off. Trip completed");
        			trip_->statusIs(Trip::completed);
        			break;

        		default:
        			logError(ERROR, "Unexpected status for trip : " + std::to_string(trip_->status()));
        			break;
        	}
        }
    }

    Ptr<Trip> trip() const {
    	return trip_;
    }

protected:

	TripSim(const Ptr<Location>& startLocation,
		    const Ptr<Location>& destination) :
		trip_(Trip::instanceNew(startLocation, destination))
	{
		// Nothing else to do
	}

private:

	Ptr<Trip> trip_;
};

#endif