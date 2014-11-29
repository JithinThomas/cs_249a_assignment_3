#ifndef TRAVEL_SIM_H
#define TRAVEL_SIM_H

#include "Sim.h"
#include "TripSim.h"
#include "VehicleManager.h"
#include "TravelSimStats.h"

using std::unordered_map;

class TravelSim;

class TripGenerator : public Sim {
public:
	static Ptr<TripGenerator> instanceNew(const Ptr<TravelSim>& travelSim);

	void onStatus();

protected:

	TripGenerator(const Ptr<TravelSim>& travelSim):
		travelSim_(travelSim)
	{
		// Nothing else to do
	}

private:

	Ptr<TravelSim> travelSim_;
	unsigned int nextTripId_ = 0;
};

class TravelSim : public Sim {
public:

	static Ptr<TravelSim> instanceNew(const Ptr<TravelNetworkManager> travelNetworkManager) {
		const Ptr<TravelSim> sim = new TravelSim(travelNetworkManager);
		sim->tripGeneratorIs(TripGenerator::instanceNew(sim));
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
		vehicleManager_(null),
		stats_(TravelSimStats::instanceNew("TravelSimStats"))
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

	Ptr<TripSim> createTripSim(const string& name, const Ptr<Trip>& trip) {
		const Ptr<TripSim> tripSim = TripSim::instanceNew(name, trip, activityManager_);
		tripSimMap_.insert(TripSimMap::value_type(name, tripSim));
		return tripSim;
	}

	TripSimMap tripSimMap_;

	Ptr<ActivityManager> activityManager_;
	Ptr<TripGenerator> tripGenerator_;	
	Ptr<TravelNetworkManager> travelNetworkManager_;
	Ptr<VehicleManager> vehicleManager_;
	Ptr<TravelSimStats> stats_;
	Trips pendingTripRequests_;
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
		const auto tripName = "TripSim-" + std::to_string(nextTripId_);
		logEntryNew(a->manager()->now(), "[" + tripName + "] Requesting for a trip");
		const auto travelNetworkMgr = travelSim_->travelNetworkManager();
		travelSim_->tripNew(tripName, 
							travelNetworkMgr->location("sfo"), 
							travelNetworkMgr->location("stanford"));
		a->nextTimeIsOffset(5);
		nextTripId_++;
	}
}

#endif