
#ifndef VEHICLE_MANAGER_H
#define VEHICLE_MANAGER_H

#include "TravelNetworkManager.h"

class VehicleManager;

//=======================================================
// VehicleTracker class
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
//=======================================================

class TravelSim;

class VehicleManager : public TravelNetworkManager::Notifiee {
public:

	static Ptr<VehicleManager> instanceNew(const string& name,
										   const Ptr<TravelSim>& travelSim) {
		return new VehicleManager(name, travelSim);
	}

	Ptr<Vehicle> nearestVehicle(const Ptr<Location>& loc) {
		/*const auto dummy = Vehicle::instanceNew("hjkh"); // TODO: This is a dummy object for testing. Remove this!
		dummy->locationIs(travelNetworkManager_->location("stanford"));
		dummy->speedIs(5);
		return dummy;
		*/
		
		if (vehiclesAvailForTrip_.size() == 0) {
			return null;
		}

		const auto conn = travelNetworkManager_->conn();
		Ptr<Conn::Path> pathFromNearestVehicleToLoc = null;
		Ptr<Vehicle> nearestVehicle = null;

		for (auto it = vehiclesAvailForTrip_.begin(); it != vehiclesAvailForTrip_.end(); it++) {
			const auto vehicle = *it;
			const auto p = conn->shortestPath(vehicle->location(), loc);
			if ((pathFromNearestVehicleToLoc != null) && (p != null) && 
				(pathFromNearestVehicleToLoc->length() > p->length())) {
				pathFromNearestVehicleToLoc = p;
				nearestVehicle = vehicle;
			}
		}

		return nearestVehicle;
	}

	void onCarNew(const Ptr<Car>& vehicle);

	// TODO: Check if the vehicle provided as input is indeed a car [For defensive programming]
	void onVehicleDel(const Ptr<Vehicle>& vehicle);

	void onVehicleStatus(const Ptr<Vehicle>& vehicle);

protected:

	typedef std::set< Ptr<Vehicle> > Vehicles;

	explicit VehicleManager(const string& name, const Ptr<TravelSim>& travelSim);

private:

	void removeVehicleFromAvailList(const Ptr<Vehicle>& vehicle) {
		auto it = vehiclesAvailForTrip_.find(vehicle);
		if (it != vehiclesAvailForTrip_.end()) {
			vehiclesAvailForTrip_.erase(it);
		}
	}

	string name_;
	Ptr<TravelNetworkManager> travelNetworkManager_;
	Vehicles vehiclesAvailForTrip_;
	unordered_map<string, VehicleTracker*> vehicleToTracker_;
	Ptr<TravelSim> travelSim_;

};

//=======================================================

void VehicleTracker::onStatus() {
	vehicleManager_->onVehicleStatus(notifier());
}

#endif