
#ifndef TRIP_H
#define TRIP_H

#include "Sim.h"
#include "Location.h"

class Trip : public Sim {
public:

	static Ptr<Trip> instanceNew(const Ptr<Location>& startLocation,
								 const Ptr<Location>& destination) {
		const Ptr<Trip> trip = new Trip(startLocation, destination);
		return trip;
	}

	enum Status {

        /** The trip has been requested. Waiting for a vehicle to be dispatched to pick up the passenger. */
        requested,

        /** Vehicle dispatched to pick up passenger. */
        vehicleDispatched,

        /** Transporting passenger from starting location to destination. */
        transportingPassenger,

        /** Passenger has been dropped off at destination and trip has been completed. */
        completed
    };

    Status status() {
    	return status_;
    }

    void statusIs(Status status) {
    	status_ = status;
    }

protected:

	Trip(const Ptr<Location>& startLocation,
		 const Ptr<Location>& destination) :
		status_(requested),
		startLocation_(startLocation),
		destination_(destination),
		requestedStartTime_(0),
		actualStartTime_(0),
		endTime_(0),
		duration_(0),
		distance_(0)
	{
		// Nothing else to do
	}

private:

	Status status_;
	Ptr<Location> startLocation_;
	Ptr<Location> destination_;
	Time requestedStartTime_;
	Time actualStartTime_;
	Time endTime_;
	Time duration_;
	Miles distance_;
};

#endif