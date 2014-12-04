
#ifndef VEHICLE_MANAGER_IMPL_H
#define VEHICLE_MANAGER_IMPL_H

#include "CommonLib.h"
#include "TravelSim.h"

Ptr<VehicleManager> VehicleManager::instanceNew(const string& name,
									   const Ptr<TravelSim>& travelSim) {
	auto mgr = new VehicleManager(name, travelSim);
	mgr->notifierIs(travelSim->travelNetworkManager());
	return mgr;
}

void VehicleManager::onCarNew(const Ptr<Car>& vehicle) {
	const auto vehicleTracker = VehicleTracker::instanceNew(vehicle, this);
	vehicleToTracker_[vehicle->name()] = vehicleTracker;
	vehiclesAvailForTrip_.insert(vehicle->name());

	if (vehiclesAvailForTrip_.size() == 1) {
		travelSim_->vehiclesAvailForTripIsNonZero();
	}
}

void VehicleManager::onVehicleDel(const Ptr<Vehicle>& vehicle) {
	if (isInstanceOf<Vehicle, Car>(vehicle)) {
		auto vehicleTracker = vehicleToTracker_[vehicle->name()];
		delete vehicleTracker;
		vehicleToTracker_.erase(vehicle->name());

		removeVehicleFromAvailList(vehicle);
	}
}

void VehicleManager::onVehicleStatus(const Ptr<Vehicle>& vehicle) {
	if (vehicle->status() == Vehicle::available) {
		vehiclesAvailForTrip_.insert(vehicle->name());
		if (vehiclesAvailForTrip_.size() == 1) {
			travelSim_->vehiclesAvailForTripIsNonZero();
		}
	} else {
		removeVehicleFromAvailList(vehicle);
	}
}

VehicleManager::VehicleManager(const string& name,
							   const Ptr<TravelSim>& travelSim):
	name_(name),
	travelSim_(travelSim)
{
	// Nothing else to do
}

#endif