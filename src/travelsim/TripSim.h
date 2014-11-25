
#ifndef TRIPSIM_H
#define TRIPSIM_H

#include "Trip.h"

class TripSim : public Sim {
public:

	static Ptr<TripSim> instanceNew(const string& name,
									const Ptr<Trip> trip, 
									const Ptr<ActivityManager>& mgr) {
		const Ptr<TripSim> sim = new TripSim(name, trip);
		const auto a = mgr->activityNew(name);
		a->nextTimeIs(mgr->now());
		a->statusIs(Activity::scheduled);
		mgr->activityAdd(a);
		sim->notifierIs(a);

		return sim;
	}

    void onStatus() {
        const auto a = notifier();
        const auto mgr = a->manager();
        if (a->status() == Activity::running) {
        	const auto t = mgr->now();
        	switch(trip_->status()) {
        		case Trip::requested:
        			logEntryNew(t, "Dispatching vehicle for trip");
        			trip_->statusIs(Trip::vehicleDispatched);
        			trip_->timeOfVehicleDispatchIs(mgr->now());
        			a->nextTimeIsOffset(1);
        			break;

        		case Trip::vehicleDispatched:
        			logEntryNew(t, "Passenger picked up. Transporting passenger to destination");
        			trip_->statusIs(Trip::transportingPassenger);
        			trip_->timeOfPassengerPickupIs(mgr->now());
        			a->nextTimeIsOffset(1);
        			break;

        		case Trip::transportingPassenger:
        			logEntryNew(t, "Passenger dropped off. Trip completed");
        			trip_->statusIs(Trip::completed);
        			trip_->timeOfCompletionIs(mgr->now());
        			break;

        		default:
        			logError(ERROR, "Unexpected status for trip : " + std::to_string(trip_->status()));
        			break;
        	}
        }
    }

    string name() const {
    	return name_;
    }

    Ptr<Trip> trip() const {
    	return trip_;
    }

protected:

	TripSim(const string& name,
			const Ptr<Trip>& trip) :
		name_(name),
		trip_(trip)
	{
		// Nothing else to do
	}

private:

	string name_;
	Ptr<Trip> trip_;

};

#endif