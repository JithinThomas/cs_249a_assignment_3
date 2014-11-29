
#ifndef TRIP_H
#define TRIP_H

#include "Conn.h"
#include "Sim.h"
#include "Location.h"
#include "ValueTypes.h"

// TODO: Should Trip have a path() attribute

class Trip : public Sim {
public:

	class Notifiee : public BaseNotifiee<Trip> {
	public:

		void notifierIs(const Ptr<Trip>& trip) {
			connect(trip, this);
		}

		/* Notification that the status of the trip has been changed. */
		virtual void onStatus() { }

	};

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

    Ptr<Location> startLocation() const {
    	return startLocation_;
    }

    Ptr<Location> destination() const {
    	return destination_;
    }

    Ptr<Conn::Path> path() const {
        return path_;
    }

    Time timeOfRequest() const {
        return timeOfRequest_;
    }

    Time timeOfVehicleDispatch() const {
        return timeOfVehicleDispatch_;
    }

    Time timeOfPassengerPickup() const {
        return timeOfPassengerPickup_;
    }

    Time timeOfCompletion() const {
        return timeOfCompletion_;
    }

    Ptr<Vehicle> vehicle() const {
    	return vehicle_;
    }

    Miles distanceOfVehicleDispatch() const {
        return distanceOfVehicleDispatch_;
    }

    string name() const {
    	return name_;
    }

    Status status() const {
    	return status_;
    }

    PassengerCount passengerCount() const {
    	return passengerCount_;
    }

protected:

	typedef std::list<Notifiee*> NotifieeList;

public:

    void statusIs(Status status) {
    	if (status_ != status) {
    		status_ = status;
    		post(this, &Notifiee::onStatus);
    	}
    }

    void startLocationIs(const Ptr<Location>& loc) {
    	if (startLocation_ != loc) {
    		startLocation_ = loc;
    	}
    }

    void destinationIs(const Ptr<Location>& loc) {
    	if (destination_ != loc) {
    		destination_ = loc;
    	}
    }

    void timeOfRequestIs(const Time& t) {
        if (timeOfRequest_ != t) {
            timeOfRequest_ = t;
        }
    }

    void timeOfVehicleDispatchIs(const Time& t) {
        if (timeOfVehicleDispatch_ != t) {
            timeOfVehicleDispatch_ = t;
        }
    }

    void timeOfPassengerPickupIs(const Time& t) {
        if (timeOfPassengerPickup_ != t) {
            timeOfPassengerPickup_ = t;
        }
    }

    void timeOfCompletionIs(const Time& t) {
        if (timeOfCompletion_ != t) {
            timeOfCompletion_ = t;
        }
    }

    void pathIs(const Ptr<Conn::Path>& p) {
        if (path_ != p) {
            path_ = p;
        }
    }

    void passengerCountIs(const PassengerCount& c) {
        if (passengerCount_ != c) {
            passengerCount_ = c;
        }
    }

    void vehicleIs(const Ptr<Vehicle> vehicle) {
    	if (vehicle_ != vehicle) {
    		vehicle_ = vehicle;
    	}
    }

    void distanceOfVehicleDispatchIs(const Miles& m) {
        if (distanceOfVehicleDispatch_ != m) {
            distanceOfVehicleDispatch_ = m;
        }
    }

    NotifieeList& notifiees() {
        return notifiees_;
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
        path_(null),
		passengerCount_(1),
		vehicle_(null),
        distanceOfVehicleDispatch_(0)
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
    Ptr<Conn::Path> path_;
	PassengerCount passengerCount_;
	Ptr<Vehicle> vehicle_;
    Miles distanceOfVehicleDispatch_;

	NotifieeList notifiees_;

};

#endif