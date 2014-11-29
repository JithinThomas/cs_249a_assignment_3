
#ifndef VEHICLE_H
#define VEHICLE_H

#include "CommonLib.h"
#include "ValueTypes.h"

using fwk::BaseNotifiee;
using fwk::NamedInterface;
using fwk::Ordinal;
using fwk::NotifierLib::post;
using fwk::Ptr;


class Vehicle : public NamedInterface {
public:

	class Notifiee : public BaseNotifiee<Vehicle> {
	public:
		void notifierIs(const Ptr<Vehicle>& vehicle) {
			connect(vehicle, this);
		}

		/* Notification that the vehicle's capacity has been modified */
		virtual void onCapacity() { }

		/* Notification that the vehicle's speed has been modified */
		virtual void onSpeed() { }

		/* Notification that the vehicle's cost has been modified */
		virtual void onCost() { }

		/* Notification that the vehicle's location has been modified */
		virtual void onLocation() { }

		/* Notification that the vehicle's status has been modified */
		virtual void onStatus() { }
	};

	static Ptr<Vehicle> instanceNew(const string& name) {
		return new Vehicle(name);
	}

protected:

	typedef std::list<Notifiee*> NotifieeList;

public:

	enum Status {
		/** Vehicle is available to be assigned for a trip. */
        available,

		/** Vehicle has been assigned for a trip. */
        assignedForTrip,

        /** Vehicle has been dipatched to pick up passengers for a trip. */
        dispatchedForPassengerPickup,

        /** Vehicle is transporting passengers on a trip. */
        transportingPassengers
	};

	PassengerCount capacity() const {
		return capacity_;
	}

	MilesPerHour speed() const {
		return speed_;
	}

	DollarsPerMile cost() const {
		return cost_;
	}

	Ptr<Location> location() const {
		return location_;
	}

	Status status() const {
		return status_;
	}

	void capacityIs(const PassengerCount& capacity) {
		if (capacity_ != capacity) {
			capacity_ = capacity;
			post(this, &Notifiee::onCapacity);
		}
	}

	void speedIs(const MilesPerHour& speed) {
		if (speed_ != speed) {
			speed_ = speed;
			post(this, &Notifiee::onSpeed);
		}
	}

	void costIs(const DollarsPerMile& cost) {
		if (cost_ != cost) {
			cost_ = cost;
			post(this, &Notifiee::onCost);
		}
	}

	void locationIs(const Ptr<Location> loc) {
		if (location_ != loc) {
			location_ = loc;
			post(this, &Notifiee::onLocation);
		}
	}

	void statusIs(const Status status) {
		if (status_ != status) {
			status_ = status;
			post(this, &Notifiee::onStatus);
		}
	}

	NotifieeList& notifiees() {
        return notifiees_;
    }

	Vehicle(const Vehicle&) = delete;

	void operator =(const Vehicle&) = delete;
	void operator ==(const Vehicle&) = delete;

protected:

	static const PassengerCount defaultCapacity;
	static const MilesPerHour defaultSpeed;
	static const DollarsPerMile defaultCost;

	NotifieeList notifiees_;

	Vehicle(const string& name) :
		NamedInterface(name),
		capacity_(defaultCapacity),
		speed_(defaultSpeed),
		cost_(defaultCost),
		location_(null),
		status_(available)
	{
		// Nothing to do
	}

	Vehicle(const string& name,
			const PassengerCount& capacity,
			const MilesPerHour& speed,
			const DollarsPerMile& cost) :
		NamedInterface(name),
		capacity_(capacity),
		speed_(speed),
		cost_(cost),
		location_(null),
		status_(available)
	{
		// Nothing to do
	}

	~Vehicle() {
		location_ = null;
	}

private:
	PassengerCount capacity_;
	MilesPerHour speed_;
	DollarsPerMile cost_;
	Ptr<Location> location_;
	Status status_;
};

const PassengerCount Vehicle::defaultCapacity = PassengerCount(0);
const DollarsPerMile Vehicle::defaultCost = DollarsPerMile(0);
const MilesPerHour Vehicle::defaultSpeed = MilesPerHour(0);

class Airplane : public Vehicle {
public:
	static Ptr<Airplane> instanceNew(const string& name) {
		return new Airplane(name);
	}

protected:

	Airplane(const string& name) :
		Vehicle(name)
	{
		// Nothing to do
	}

	Airplane(const string& name,
			 const PassengerCount& capacity,
			 const MilesPerHour& speed,
			 const DollarsPerMile& cost) :
		Vehicle(name, capacity, speed, cost)
	{
		// Nothing to do
	}
};

class Car : public Vehicle {
public:
	static Ptr<Car> instanceNew(const string& name) {
		return new Car(name);
	}

protected:
	
	Car(const string& name) :
		Vehicle(name)
	{
		// Nothing to do
	}

	Car(const string& name,
		const PassengerCount& capacity,
		const MilesPerHour& speed,
		const DollarsPerMile& cost) :
		Vehicle(name, capacity, speed, cost)
	{
		// Nothing to do
	}
};

#endif