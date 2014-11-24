
#ifndef TRIP_H
#define TRIP_H

#include "Sim.h"
#include "Location.h"
#include "ValueTypes.h"

class Trip : public Sim {
public:

	static Ptr<Trip> instanceNew(const string& name) {
		const Ptr<Trip> trip = new Trip(name);
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

    Status status() const {
    	return status_;
    }

    PassengerCount passengerCount() const {
    	return passengerCount_;
    }

    void statusIs(Status status) {
    	status_ = status;
    }

    void startLocationIs(const Ptr<Location>& loc) {
    	startLocation_ = loc;
    }

    void destinationIs(const Ptr<Location>& loc) {
    	destination_ = loc;
    }

    Trip(const Trip&) = delete;

	void operator =(const Trip&) = delete;
	void operator ==(const Trip&) = delete;

protected: 

	explicit Trip(const string& name) :
		name_(name),
		status_(requested),
		startLocation_(null),
		destination_(null),
		timeOfRequest_(0),
		timeOfVehicleDispatch_(0),
		timeOfPassengerPickup_(0),
		timeOfCompletion_(0),
		distance_(0),
		passengerCount_(1)
	{
		// Nothing else to do
	}

private:

	string name_;
	Status status_;
	Ptr<Location> startLocation_;
	Ptr<Location> destination_;
	Time timeOfRequest_;
	Time timeOfVehicleDispatch_;
	Time timeOfPassengerPickup_;
	Time timeOfCompletion_;
	Miles distance_;
	PassengerCount passengerCount_;

};

#endif