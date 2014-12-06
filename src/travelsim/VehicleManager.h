
#ifndef VEHICLE_MANAGER_H
#define VEHICLE_MANAGER_H

#include "TravelNetworkManager.h"

class VehicleManager;

//=======================================================
// VehicleTracker class
//    Merely trampolines updates the status updates from
//    a vehicle to the VehicleManager.
//=======================================================

class VehicleTracker : public Vehicle::Notifiee {
public:

	static VehicleTracker* instanceNew(const Ptr<Vehicle> vehicle, const Ptr<VehicleManager> vehicleManager) {
		const auto tracker = new VehicleTracker(vehicleManager);
		tracker->notifierIs(vehicle);
		return tracker;
	}

	void onStatus();

protected:

	explicit VehicleTracker(const Ptr<VehicleManager> vehicleManager) :
		vehicleManager_(vehicleManager)
	{
		// Nothing else to do
	}

private:

	Ptr<VehicleManager> vehicleManager_;
};

//=======================================================

//=======================================================
// VehicleManager class
//
//   Keeps track of available vehicles in the network. 
//   Also, finds the vehicle nearest to a given location.
//=======================================================

class TravelSim;

class VehicleManager : public TravelNetworkManager::Notifiee {
public:

	static Ptr<VehicleManager> instanceNew(const string& name, const Ptr<TravelSim>& travelSim);

	Ptr<Vehicle> nearestVehicle(const Ptr<Location>& loc) {
		if (vehiclesAvailForTrip_.size() == 0) {
			return null;
		}

		const auto travelNetworkManager = notifier();
		const auto conn = travelNetworkManager->conn();
		Ptr<Conn::Path> pathFromNearestVehicleToLoc = null;
		Ptr<Vehicle> nearestVehicle = null;

		for (auto it = vehiclesAvailForTrip_.begin(); it != vehiclesAvailForTrip_.end(); it++) {
			const auto vehicleName = *it;
			const auto vehicle = travelNetworkManager->vehicle(vehicleName);
			const auto p = conn->shortestPath(vehicle->location(), loc);
			if ( (p != null) && (vehicle->speed().value() > 0) ) {
				if (pathFromNearestVehicleToLoc == null) {
					pathFromNearestVehicleToLoc = p;
					nearestVehicle = vehicle;
				} else if (pathFromNearestVehicleToLoc->length() > p->length()) {
					pathFromNearestVehicleToLoc = p;
					nearestVehicle = vehicle;
				}
			}
		}

		return nearestVehicle;
	}

	unsigned int availableVehicleCount() const {
		return vehiclesAvailForTrip_.size();
	}

	void onCarNew(const Ptr<Car>& vehicle);

	void onVehicleDel(const Ptr<Vehicle>& vehicle);

protected:

	friend class VehicleTracker;

	typedef std::set< string > Vehicles;

	void onVehicleStatus(const Ptr<Vehicle>& vehicle);

	explicit VehicleManager(const string& name, const Ptr<TravelSim>& travelSim);

private:

	void removeVehicleFromAvailList(const Ptr<Vehicle>& vehicle) {
		auto it = vehiclesAvailForTrip_.find(vehicle->name());
		if (it != vehiclesAvailForTrip_.end()) {
			vehiclesAvailForTrip_.erase(it);
		}
	}

	string name_;
	Vehicles vehiclesAvailForTrip_;
	unordered_map<string, VehicleTracker*> vehicleToTracker_;
	Ptr<TravelSim> travelSim_;

};

//=======================================================

void VehicleTracker::onStatus() {
	vehicleManager_->onVehicleStatus(notifier());
}

#endif