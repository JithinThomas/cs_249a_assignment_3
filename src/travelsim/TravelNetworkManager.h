
#ifndef TRAVEL_MANAGER_H
#define TRAVEL_MANAGER_H

#include "CommonLib.h"
#include "Conn.h"
#include "Segment.h"
#include "Location.h"
#include "SegmentImpl.h"
#include "Flight.h"
#include "Vehicle.h"

using fwk::BaseNotifiee;
using fwk::NamedInterface;
using fwk::NotifierLib::post;
using fwk::Ptr;

using std::cout;
using std::endl;
using std::unordered_map;

//=======================================================
// Common functions
//=======================================================

template<typename K, typename V>
bool isKeyPresent(unordered_map<K,V> map, K key) {
	return (map.find(key) != map.end());
}

bool isFlight(const Ptr<Segment>& segment) {
	return isInstanceOf<Segment, Flight>(segment);
}

bool isRoad(const Ptr<Segment>& segment) {
	return isInstanceOf<Segment, Road>(segment);
}

bool isAirplane(const Ptr<Vehicle>& vehicle) {
	return isInstanceOf<Vehicle, Airplane>(vehicle);
}

bool isCar(const Ptr<Vehicle>& vehicle) {
	return isInstanceOf<Vehicle, Car>(vehicle);
}

//=======================================================


//=======================================================
// TravelNetworkManager class
//=======================================================

class TravelNetworkTracker;

class TravelNetworkManager : public NamedInterface {
public:

	class Notifiee : public BaseNotifiee<TravelNetworkManager> {
	public:

		void notifierIs(const Ptr<TravelNetworkManager>& travelManager) {
			connect(travelManager, this);
		}

		/* Notification that a new Residence has been instantiated */
		virtual void onResidenceNew(const Ptr<Residence>& residence) { }

		/* Notification that a new Airport has been instantiated */
		virtual void onAirportNew(const Ptr<Airport>& airport) { }

		/* Notification that a new Flight has been instantiated */
		virtual void onFlightNew(const Ptr<Flight>& flight) { }

		/* Notification that a new Road has been instantiated */
		virtual void onRoadNew(const Ptr<Road>& road) { }

		/* Notification that a new Airplane has been instantiated */
		virtual void onAirplaneNew(const Ptr<Airplane>& airplane) { }

		/* Notification that a new Car has been instantiated */
		virtual void onCarNew(const Ptr<Car>& car) { }

		/* Notification that a Location has been deleted */
		virtual void onLocationDel(const Ptr<Location>& location) { }

		/* Notification that a Segment has been deleted */
		virtual void onSegmentDel(const Ptr<Segment>& segment) { }

		/* Notification that a Vehicle has been deleted */
		virtual void onVehicleDel(const Ptr<Vehicle>& vehicle) { }
	};

	static Ptr<TravelNetworkManager> instanceNew(const string& name) {
		return new TravelNetworkManager(name);
	}

protected:

	typedef unordered_map< string, Ptr<Location> > LocationMap;
	typedef unordered_map< string, Ptr<Segment> > SegmentMap;
	typedef unordered_map< string, Ptr<Vehicle> > VehicleMap;

	typedef LocationMap::const_iterator locationConstIter;
	typedef SegmentMap::const_iterator segmentConstIter;
	typedef VehicleMap::const_iterator vehicleConstIter;

	typedef LocationMap::iterator locationIterator;
	typedef SegmentMap::iterator segmentIterator;
	typedef VehicleMap::iterator vehicleIterator;

	typedef std::list<Notifiee*> NotifieeList;

public:

	/* Location accessors by name */
	Ptr<Location> location(const string& name) const {
		return getEntityFromMap<Location>(locationMap_, name);
	}

	Ptr<Airport> airport(const string& name) const {
		return findLocationOfSpecificType<Airport>(name);
	}

	Ptr<Residence> residence(const string& name) const {
		return findLocationOfSpecificType<Residence>(name);
	}

	/* Segment accessors by name */
	Ptr<Segment> segment(const string& name) const {
		return getEntityFromMap<Segment>(segmentMap_, name);
	}

	Ptr<Flight> flight(const string& name) const {
		return findSegmentOfSpecificType<Flight>(name);
	}

	Ptr<Road> road(const string& name) const {
		return findSegmentOfSpecificType<Road>(name);
	}

	/* Vehicle accessors by name */
	Ptr<Vehicle> vehicle(const string& name) const {
		return getEntityFromMap<Vehicle>(vehicleMap_, name);
	}

	Ptr<Airplane> airplane(const string& name) const {
		return findVehicleOfSpecificType<Airplane>(name);
	}

	Ptr<Car> car(const string& name) const {
		return findVehicleOfSpecificType<Car>(name);
	}

	Ptr<Conn> conn() const {
		return conn_;
	}

	Ptr<TravelNetworkTracker> stats() const {
		return stats_;
	}

	locationConstIter locationIter() {
		return locationMap_.cbegin();
	}

	locationConstIter locationIterEnd() {
		return locationMap_.cend();
	}

	segmentConstIter segmentIter() {
		return segmentMap_.cbegin();
	}

	segmentConstIter segmentIterEnd() {
		return segmentMap_.cend();
	}

	vehicleConstIter vehicleIter() {
		return vehicleMap_.cbegin();
	}

	vehicleConstIter vehicleIterEnd() {
		return vehicleMap_.cend();
	}

	Ptr<Airport> airportNew(const string& name) {
		if (isNameInUse(name)) {
			logError(WARNING, "An instance with the given name '" + name + "' already exists. Skipping command.");
			return null;
		}

		const auto airport = Airport::instanceNew(name);
		locationMap_.insert(LocationMap::value_type(name, airport));

		post(this, &Notifiee::onAirportNew, airport);

		return airport;
	}

	Ptr<Residence> residenceNew(const string& name) {
		if (isNameInUse(name)) {
			logError(WARNING, "An instance with the given name '" + name + "' already exists. Skipping command.");
			return null;
		}

		const auto residence = Residence::instanceNew(name);
		locationMap_.insert(LocationMap::value_type(name, residence));

		post(this, &Notifiee::onResidenceNew, residence);

		return residence;
	}

  	Ptr<Flight> flightNew(const string& name) {
		if (isNameInUse(name)) {
			logError(WARNING, "An instance with the given name '" + name + "' already exists. Skipping command.");
			return null;
		}

		const auto flight = Flight::instanceNew(name);
		segmentMap_.insert(SegmentMap::value_type(name, flight));

		post(this, &Notifiee::onFlightNew, flight);

		return flight;
	}

  	Ptr<Road> roadNew(const string& name) {
		if (isNameInUse(name)) {
			logError(WARNING, "An instance with the given name '" + name + "' already exists. Skipping command.");
			return null;
		}

		const auto road = Road::instanceNew(name);
		segmentMap_.insert(SegmentMap::value_type(name, road));

		post(this, &Notifiee::onRoadNew, road);

		return road;
	}

	Ptr<Airplane> airplaneNew(const string& name) {
		if (isNameInUse(name)) {
			logError(WARNING, "An instance with the given name '" + name + "' already exists. Skipping command.");
			return null;
		}

		const auto airplane = Airplane::instanceNew(name);
		vehicleMap_.insert(VehicleMap::value_type(name, airplane));

		post(this, &Notifiee::onAirplaneNew, airplane);

		return airplane;
	}

	Ptr<Car> carNew(const string& name) {
		if (isNameInUse(name)) {
			logError(WARNING, "An instance with the given name '" + name + "' already exists. Skipping command.");
			return null;
		}

		const auto car = Car::instanceNew(name);
		vehicleMap_.insert(VehicleMap::value_type(name, car));

		post(this, &Notifiee::onCarNew, car);

		return car;
	}

	Ptr<Location> locationDel(const string& name) {
		auto iter = locationMap_.find(name);
		if (iter == locationMap_.end()) {
			return null;
		}

		const auto location = iter->second;
		locationDel(iter);
		
		return location;
	}

	locationIterator locationDel(locationConstIter iter) {
		const auto location = iter->second;
		auto next = locationMap_.erase(iter);

		location->sourceSegmentDelAll();
		location->destinationSegmentDelAll();

		post(this, &Notifiee::onLocationDel, location);

		return next;
	}

	segmentIterator segmentDel(segmentConstIter iter) {
		const auto segment = iter->second;
		segment->sourceDel();
		segment->destinationDel();

		auto next = segmentMap_.erase(iter);

		post(this, &Notifiee::onSegmentDel, segment);

		return next;
	}

	Ptr<Segment> segmentDel(const string& name) {
		auto iter = segmentMap_.find(name);
		if (iter == segmentMap_.end()) {
			return null;
		}

		const auto segment = iter->second;
		segmentDel(iter);

		return segment;
	}

	vehicleIterator vehicleDel(vehicleConstIter iter) {
		const auto vehicle = iter->second;
		auto next = vehicleMap_.erase(iter);

		post(this, &Notifiee::onVehicleDel, vehicle);

		return next;
	}

	Ptr<Vehicle> vehicleDel(const string& name) {
		auto iter = vehicleMap_.find(name);
		if (iter == vehicleMap_.end()) {
			return null;
		}

		const auto vehicle = iter->second;
		vehicleDel(iter);

		return vehicle;
	}

	NotifieeList& notifiees() {
        return notifiees_;
    }

	TravelNetworkManager(const TravelNetworkManager&) = delete;

	void operator =(const TravelNetworkManager&) = delete;
	void operator ==(const TravelNetworkManager&) = delete;

protected:

	explicit TravelNetworkManager(const string& name);

	NotifieeList notifiees_;

private:

	bool isNameInUse(const string& name) {
		return (isKeyPresent(locationMap_, name) ||
				isKeyPresent(segmentMap_, name)  ||
				isKeyPresent(vehicleMap_, name));
	}

	template<class T>
	Ptr<T> findLocationOfSpecificType(const string& name) const {
		Ptr<Location> loc = location(name);
		if (loc != null) {
			auto ptr = dynamic_cast<T*>(loc.ptr());
			if (ptr != null) {
				return ptr;
			} 
		}

		return null;
	}

	template<class T>
	Ptr<T> findSegmentOfSpecificType(const string& name) const {
		Ptr<Segment> seg = segment(name);
		if (seg != null) {
			auto ptr = dynamic_cast<T*>(seg.ptr());
			if (ptr != null) {
				return ptr;
			} 
		}

		return null;
	}

	template<class T>
	Ptr<T> findVehicleOfSpecificType(const string& name) const {
		Ptr<Vehicle> v = vehicle(name);
		if (v != null) {
			auto ptr = dynamic_cast<T*>(v.ptr());
			if (ptr != null) {
				return ptr;
			} 
		}

		return null;
	}

	template<class T>
	Ptr<T> getEntityFromMap(const unordered_map<string, Ptr<T>>& entityMap, const string& name) const {
		const auto it = entityMap.find(name);
		if (it != entityMap.end()) {
			return it->second;
		}

		return null;
	}

	LocationMap locationMap_;
	SegmentMap segmentMap_;
	VehicleMap vehicleMap_;
	Ptr<Conn> conn_;
	Ptr<TravelNetworkTracker> stats_;
};

//=======================================================
// TravelNetworkTracker class
//=======================================================

class TravelNetworkTracker : public TravelNetworkManager::Notifiee {
public:

	static Ptr<TravelNetworkTracker> instanceNew(const string& name) {
		return new TravelNetworkTracker(name);
	}

	unsigned int locationCount() const {
		return locationCount_;
	}

	unsigned int segmentCount() const {
		return segmentCount_;
	}

	unsigned int vehicleCount() const {
		return vehicleCount_;
	}

	unsigned int residenceCount() const {
		return residenceCount_;
	}

	unsigned int airportCount() const {
		return airportCount_;
	}

	unsigned int flightCount() const {
		return flightCount_;
	}

	unsigned int roadCount() const {
		return roadCount_;
	}

	unsigned int airplaneCount() const {
		return airplaneCount_;
	}

	unsigned int carCount() const {
		return carCount_;
	}

	void onResidenceNew(const Ptr<Residence>& residence) {
		residenceCount_++;
		locationCount_++;
	}

	void onAirportNew(const Ptr<Airport>& airport) {
		airportCount_++;
		locationCount_++;
	}

	void onFlightNew(const Ptr<Flight>& flight) {
		flightCount_++;
		segmentCount_++;
	}

	void onRoadNew(const Ptr<Road>& road) {
		roadCount_++;
		segmentCount_++;
	}

	void onAirplaneNew(const Ptr<Airplane>& airplane) {
		airplaneCount_++;
		vehicleCount_++;
	}

	void onCarNew(const Ptr<Car>& car) {
		carCount_++;
		vehicleCount_++;
	}

	void onLocationDel(const Ptr<Location>& location) {
		if (isResidence(location)) {
			residenceCount_--;
		} else if (isAirport(location)) {
			airportCount_--;
		} else {
			logError(WARNING, "onLocationDel: Unexpected type of Location.");
		}

		locationCount_--;
	}

	void onSegmentDel(const Ptr<Segment>& segment) {
		if (isFlight(segment)) {
			flightCount_--;
		} else if (isRoad(segment)) {
			roadCount_--;
		} else {
			logError(WARNING, "onSegmentDel: Unexpected type of Segment.");
		}

		segmentCount_--;
	}

	void onVehicleDel(const Ptr<Vehicle>& vehicle) {
		if (isAirplane(vehicle)) {
			airplaneCount_--;
		} else if (isCar(vehicle)) {
			carCount_--;
		} else {
			logError(WARNING, "onVehicleDel: Unexpected type of Vehicle.");
		}

		vehicleCount_--;
	}

	const string& name() const {
		return name_;
	}

protected:

	explicit TravelNetworkTracker(const string& name) :
		airplaneCount_(0),
		airportCount_(0),
		carCount_(0),
		flightCount_(0),
		locationCount_(0),
		residenceCount_(0),
		roadCount_(0),
		segmentCount_(0),
		vehicleCount_(0),
		name_(name)
	{

	}

private:
	unsigned int airplaneCount_;
	unsigned int airportCount_;
	unsigned int carCount_;
	unsigned int flightCount_;
	unsigned int locationCount_;
	unsigned int residenceCount_;
	unsigned int roadCount_;
	unsigned int segmentCount_;
	unsigned int vehicleCount_;

	string name_;
};

TravelNetworkManager::TravelNetworkManager(const string& name) :
	NamedInterface(name)
{
	// Nothing to do
	conn_ = Conn::instanceNew("");
	stats_ = TravelNetworkTracker::instanceNew("");
	stats_->notifierIs(this);
}

#endif