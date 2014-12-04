#ifndef TRAVEL_SIM_H
#define TRAVEL_SIM_H

#include "Sim.h"
#include "RandomNumberGenerators.h"
#include "TripSim.h"
#include "VehicleManager.h"
#include "TravelSimStats.h"

using std::unordered_map;

class TravelSim;

class TripGenerator : public Sim {
public:
	static Ptr<TripGenerator> instanceNew(const Ptr<TravelSim>& travelSim);

	Ptr<RandomNumberGenerator> tripCountGenerator() const {
		return tripCountGenerator_;
	}

	Ptr<RandomNumberGenerator> tripIntervalGenerator() const {
		return tripIntervalGenerator_;
	}

	void tripIntervalGeneratorIs(const Ptr<RandomNumberGenerator>& rng) {
		if (tripIntervalGenerator_ != rng) {
			tripIntervalGenerator_ = rng;
		}
	}

	void tripCountGeneratorIs(const Ptr<RandomNumberGenerator>& rng) {
		if (tripCountGenerator_ != rng) {
			tripCountGenerator_ = rng;
		}
	}

	void seedIs(const unsigned int seed) {
		if (seed_ != seed) {
			seed_ = seed;
		}
	}

	unsigned int nextTimeOffset() {
		return ceil(tripIntervalGenerator_->value());
	}

	unsigned int tripCount() {
		return ceil(tripCountGenerator_->value());
	}

	void onStatus();

protected:

	TripGenerator(const Ptr<TravelSim>& travelSim):
		travelSim_(travelSim),
		tripCountGenerator_(ConstGenerator::instanceNew(1)),
		tripIntervalGenerator_(ConstGenerator::instanceNew(1)),
		seed_(U32(SystemTime::now().value() & 0xffffffff))
	{
		// Nothing else to do
	}

private:

	Ptr<TravelSim> travelSim_;
	unsigned int nextTripId_ = 0;
	Ptr<RandomNumberGenerator> tripCountGenerator_;
	Ptr<RandomNumberGenerator> tripIntervalGenerator_;
	unsigned int seed_;

};

class NetworkModifier : public Sim {
public:

	static Ptr<NetworkModifier> instanceNew(const Ptr<TravelSim>& travelSim);

	void onStatus();

	double probOfDeletingLocation() const {
		return probOfDeletingLocation_;
	}

	double probOfDeletingSegment() const {
		return probOfDeletingSegment_;
	}

	Ptr<RandomNumberGenerator> activityIntervalGenerator() const {
		return activityIntervalGenerator_;
	}

	void probOfDeletingLocationIs(double p) {
		if (probOfDeletingLocation_ != p) {
			probOfDeletingLocation_ = p;
		}
	}

	void probOfDeletingSegmentIs(double p) {
		if (probOfDeletingSegment_ != p) {
			probOfDeletingSegment_ = p;
		}
	}

	void activityIntervalGeneratorIs(const Ptr<RandomNumberGenerator>& rng) {
		if (activityIntervalGenerator_ != rng) {
			activityIntervalGenerator_ = rng;
		}
	}

protected:

	NetworkModifier(const Ptr<TravelSim>& travelSim) :
		travelSim_(travelSim),
		probOfDeletingLocation_(0),
		probOfDeletingSegment_(0),
		probGenerator_(UniformDistributionRandom::instanceNew(0,1)),
		activityIntervalGenerator_(ConstGenerator::instanceNew(1))
	{
		// Nothing else to do
	}

private:

	void residenceDel();

	void segmentDel();

	unsigned int nextTimeOffset() {
		return ceil(activityIntervalGenerator_->value());
	}

	void setProb(double currProb, const double newProb) {
		if (currProb != newProb) {
			if (newProb < 0) {
				currProb = 0.0;
			} else if (newProb > 1) {
				currProb = 1.0;
			} else {
				currProb = newProb;
			}
		}
	}

	Ptr<TravelSim> travelSim_;
	double probOfDeletingLocation_;
	double probOfDeletingSegment_;
	Ptr<RandomNumberGenerator> probGenerator_;
	Ptr<RandomNumberGenerator> activityIntervalGenerator_;

};

class LocAndSegManager : public TravelNetworkManager::Notifiee {
public:

	static Ptr<LocAndSegManager> instanceNew(const Ptr<TravelNetworkManager>& mgr) {
		auto locationMgr = new LocAndSegManager();
		locationMgr->notifierIs(mgr);
		return locationMgr;
	}

	static Ptr<LocAndSegManager> instanceNew(const Ptr<TravelNetworkManager>& mgr, const double seed) {
		auto locationMgr = new LocAndSegManager(seed);
		locationMgr->notifierIs(mgr);
		return locationMgr;
	}

protected:

	typedef std::vector<string> LocationNames;
	typedef std::vector<string> SegmentNames;

public:

	void onResidenceNew(const Ptr<Residence>& res) {
		locationNames_.push_back(res->name());
	}

	void onRoadNew(const Ptr<Road>& road) {
		segmentNames_.push_back(road->name());
	}

	void onLocationDel(const Ptr<Location>& loc) {
		auto it = std::find(locationNames_.begin(), locationNames_.end(), loc->name());
		if (it != locationNames_.end()) {
			locationNames_.erase(it);
		}
	}

	void onSegmentDel(const Ptr<Segment>& seg) {
		auto it = std::find(segmentNames_.begin(), segmentNames_.end(), seg->name());
		if (it != segmentNames_.end()) {
			segmentNames_.erase(it);
		}
	}

	Ptr<Location> locationRandom() {
		const auto numLocations = locationNames_.size();
		if (numLocations > 0) {
			locAndSegIndexRng_->upperIs(numLocations);
			const auto idx = (int)(locAndSegIndexRng_->value());
			const auto locName = locationNames_[idx];
			const auto travelNetworkManager = notifier();
			return travelNetworkManager->location(locName);
		}

		return null;	
	}

	Ptr<Segment> segmentRandom() {
		const auto numSegments = segmentNames_.size();
		if (numSegments > 0) {
			locAndSegIndexRng_->upperIs(numSegments);
			const auto idx = (int)(locAndSegIndexRng_->value());
			const auto segName = segmentNames_[idx];
			const auto travelNetworkManager = notifier();
			return travelNetworkManager->segment(segName);
		}

		return null;	
	}

	void locAndSegIndexRngIs(const Ptr<RandomNumberGenerator> rng) {
		if (locAndSegIndexRng_ != rng) {
			locAndSegIndexRng_ = rng;
		}
	}

protected:

	LocAndSegManager() :
		locAndSegIndexRng_(UniformDistributionRandom::instanceNew(U32(SystemTime::now().value() & 0xffffffff), 0, 0))
	{
		// Nothing else to do
	}

	LocAndSegManager(const double seed) :
		locAndSegIndexRng_(UniformDistributionRandom::instanceNew(seed, 0, 0))
	{
		// Nothing else to do
	}

	~LocAndSegManager() { }

private:

	LocationNames locationNames_; // Locations present in the network
	SegmentNames segmentNames_;
	Ptr<RandomNumberGenerator> locAndSegIndexRng_;

};

class TravelSim : public Sim {
public:

	static Ptr<TravelSim> instanceNew(const Ptr<TravelNetworkManager> travelNetworkManager) {
		const Ptr<TravelSim> sim = new TravelSim(travelNetworkManager);
		sim->tripGeneratorIs(TripGenerator::instanceNew(sim));
		sim->networkModifierIs(NetworkModifier::instanceNew(sim));

		const auto vehicleMgr = VehicleManager::instanceNew("VehicleManager", sim);
		sim->vehicleManagerIs(VehicleManager::instanceNew("VehicleManager", sim));
		sim->stats()->notifierIs(travelNetworkManager);

		const auto mgr = sim->activityManager();
		const auto a = mgr->activityNew("TravelSim");
		a->nextTimeIs(mgr->now());
		a->statusIs(Activity::scheduled);
		mgr->activityAdd(a);
		sim->notifierIs(a);
		return sim;
	}

	void simulationEndTimeIsOffset(const Time offset) {
		const auto startTime = time(SystemTime::now());
		
		logEntryNew(startTime, "Starting travel simulation");
	    activityManager_->nowIs(startTime);

	    activityManager_->nowIs(startTime + offset);
	    logEntryNew(startTime + offset, "Stopping travel simulation");

	    tripGenerator_->activityDel();

	    stats_->printStats();
	}

	Ptr<TripSim> tripNew(const string& name, 
					     const Ptr<Location>& startLocation, 
					     const Ptr<Location>& destination) {
		const Ptr<Trip> trip = travelNetworkManager_->tripNew(name);
		trip->startLocationIs(startLocation);
		trip->destinationIs(destination);

		const auto conn = travelNetworkManager_->conn();
		const auto shortestPath = conn->shortestPath(startLocation, destination);
		if (shortestPath == null) {
			logEntryNew(notifier()->manager()->now(), "[" + name + "] Aborting trip from '" + 
						startLocation->name() + "' to '" + destination->name() + "' since no path exists.");
			return null;
		}

		trip->pathIs(conn->shortestPath(startLocation, destination));
		trip->timeOfRequestIs(activityManager_->now());
		
		const auto nearestVehicle = vehicleManager_->nearestVehicle(startLocation);
		if (nearestVehicle == null) {
			pendingTripRequests_.push_back(trip);
			return null;
		}

		trip->vehicleIs(nearestVehicle);
		trip->distanceOfVehicleDispatchIs(conn->shortestPath(nearestVehicle->location(), startLocation)->length());
		nearestVehicle->statusIs(Vehicle::assignedForTrip);

		return createTripSim(name, trip);
	}

	Ptr<ActivityManager> activityManager() const {
		return activityManager_;
	}

	Ptr<TravelNetworkManager> travelNetworkManager() const {
		return travelNetworkManager_;
	}

	Ptr<TravelSimStats> stats() const {
		return stats_;
	}

	Ptr<TripGenerator> tripGenerator() const {
		return tripGenerator_;
	}

	Ptr<LocAndSegManager> locAndSegManager() const {
		return locationManager_;
	}

	Ptr<NetworkModifier> networkModifier() const {
		return networkModifier_;
	}

protected:

	friend class VehicleManager;

	typedef unordered_map< string, Ptr<TripSim> > TripSimMap;
	typedef vector< Ptr<Trip> > Trips;

	Ptr<TripSim> vehiclesAvailForTripIsNonZero() {
		if (pendingTripRequests_.size() > 0) {
			const auto it = pendingTripRequests_.begin();
			auto trip = *it;
			const auto nearestVehicle = vehicleManager_->nearestVehicle(trip->startLocation());
			trip->vehicleIs(nearestVehicle);
			const auto conn = travelNetworkManager_->conn();
			trip->distanceOfVehicleDispatchIs(conn->shortestPath(nearestVehicle->location(), trip->startLocation())->length());
			nearestVehicle->statusIs(Vehicle::assignedForTrip);
			
			pendingTripRequests_.erase(it);

			return createTripSim(trip->name(), trip);
		}

		return null;
	}

	TravelSim(const Ptr<TravelNetworkManager>& travelNetworkManager) :
		activityManager_(SequentialManager::instance()),
		tripGenerator_(null),
		travelNetworkManager_(travelNetworkManager),
		locationManager_(LocAndSegManager::instanceNew(travelNetworkManager)),
		vehicleManager_(null),
		stats_(TravelSimStats::instanceNew("TravelSimStats")),
		networkModifier_(null)
	{
		activityManager_->nowIs(time(SystemTime::now()));
		//activityManager_->verboseIs(true);
	}

private:

	void tripGeneratorIs(const Ptr<TripGenerator>& tg) {
		if (tripGenerator_ != tg) {
			tripGenerator_ = tg;
		}
	}

	void vehicleManagerIs(const Ptr<VehicleManager>& mgr) {
		if (vehicleManager_ != mgr) {
			vehicleManager_ = mgr;
		}
	}

	void networkModifierIs(const Ptr<NetworkModifier>& nm) {
		if (networkModifier_ != nm) {
			networkModifier_ = nm;
		}
	}

	Ptr<TripSim> createTripSim(const string& name, const Ptr<Trip>& trip) {
		const Ptr<TripSim> tripSim = TripSim::instanceNew(name, trip, activityManager_);
		tripSimMap_.insert(TripSimMap::value_type(name, tripSim));
		return tripSim;
	}

	TripSimMap tripSimMap_;

	Ptr<ActivityManager> activityManager_;
	Ptr<TripGenerator> tripGenerator_;	
	Ptr<TravelNetworkManager> travelNetworkManager_;
	Ptr<LocAndSegManager> locationManager_;
	Ptr<VehicleManager> vehicleManager_;
	Ptr<TravelSimStats> stats_;
	Trips pendingTripRequests_;
	Ptr<NetworkModifier> networkModifier_;
};

Ptr<TripGenerator> TripGenerator::instanceNew(const Ptr<TravelSim>& travelSim) {
	const Ptr<TripGenerator> sim = new TripGenerator(travelSim);
	const auto mgr = travelSim->activityManager();
	const auto a = mgr->activityNew("TripGenerator");
	a->nextTimeIs(mgr->now());
	a->statusIs(Activity::scheduled);
	mgr->activityAdd(a);
	sim->notifierIs(a);
	return sim;
}

void TripGenerator::onStatus() {
	const auto a = notifier();
	if (a->status() == Activity::running) {
		const auto numTrips = tripCount();
		const auto travelNetworkMgr = travelSim_->travelNetworkManager();
		const auto locAndSegMgr = travelSim_->locAndSegManager();

		logEntryNew(a->manager()->now(), "Generating " + std::to_string(numTrips) + " trips");

		for (auto i = 0; i < numTrips; i++) {
			const auto tripName = "TripSim-" + std::to_string(nextTripId_);
			const auto source = locAndSegMgr->locationRandom();
			const auto destination = locAndSegMgr->locationRandom();

			logEntryNew(a->manager()->now(), "[" + tripName + "] Requesting for a trip from '" + 
						source->name() + "' to '" + destination->name() + "'");

			travelSim_->tripNew(tripName, source, destination);

			a->nextTimeIsOffset(nextTimeOffset());
			nextTripId_++;
		}
	}
}

/*
void TripGenerator::onStatus() {
	const auto a = notifier();
	if (a->status() == Activity::running) {
		const auto numTrips = tripCount();
		const auto travelNetworkMgr = travelSim_->travelNetworkManager();

		logEntryNew(a->manager()->now(), "Generating " + std::to_string(numTrips) + " trips");

		for (auto i = 0; i < numTrips; i++) {
			const auto tripName = "TripSim-" + std::to_string(nextTripId_);
			logEntryNew(a->manager()->now(), "[" + tripName + "] Requesting for a trip");
			
			travelSim_->tripNew(tripName, 
								travelNetworkMgr->location("sfo"), 
								travelNetworkMgr->location("stanford"));
			
			a->nextTimeIsOffset(nextTimeOffset());
			nextTripId_++;
		}
	}
}
*/

Ptr<NetworkModifier> NetworkModifier::instanceNew(const Ptr<TravelSim>& travelSim) {
	const Ptr<NetworkModifier> sim = new NetworkModifier(travelSim);
	const auto mgr = travelSim->activityManager();
	const auto a = mgr->activityNew("NetworkModifier");
	a->nextTimeIs(mgr->now());
	a->statusIs(Activity::scheduled);
	mgr->activityAdd(a);
	sim->notifierIs(a);
	return sim;
}

void NetworkModifier::residenceDel() {
	double v = probGenerator_->value();
	if (v < probOfDeletingLocation_) {
		const auto loc = travelSim_->locAndSegManager()->locationRandom();
		const auto locName = loc->name();
		const auto a = notifier();
		logEntryNew(a->manager()->now(), "Deleting location '" + locName + "'");
		travelSim_->travelNetworkManager()->locationDel(locName);
	}
}

void NetworkModifier::segmentDel() {
	double v = probGenerator_->value();
	if (v < probOfDeletingSegment_) {
		const auto seg = travelSim_->locAndSegManager()->segmentRandom();
		const auto segName = seg->name();
		const auto a = notifier();
		logEntryNew(a->manager()->now(), "Deleting segment '" + segName + "'");
		travelSim_->travelNetworkManager()->segmentDel(seg->name());
	}
}

void NetworkModifier::onStatus() {
	const auto a = notifier();
	if (a->status() == Activity::running) {
		this->residenceDel();
		this->segmentDel();
		a->nextTimeIsOffset(nextTimeOffset());
	}
}

#endif