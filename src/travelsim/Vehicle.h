
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
	};

	static Ptr<Vehicle> instanceNew(const string& name) {
		return new Vehicle(name);
	}

protected:

	typedef std::list<Notifiee*> NotifieeList;

public:

	PassengerCount capacity() const {
		return capacity_;
	}

	MilesPerHour speed() const {
		return speed_;
	}

	DollarsPerMile cost() const {
		return cost_;
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
		cost_(defaultCost)
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
		cost_(cost)
	{
		// Nothing to do
	}

	~Vehicle() {}

private:
	PassengerCount capacity_;
	MilesPerHour speed_;
	DollarsPerMile cost_;
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