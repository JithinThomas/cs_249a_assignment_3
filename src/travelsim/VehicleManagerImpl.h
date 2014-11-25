
#ifndef VEHICLE_MANAGER_IMPL_H
#define VEHICLE_MANAGER_IMPL_H

#include "TravelSim.h"

void VehicleManager::onCarNew(const Ptr<Car>& vehicle) {
	const auto vehicleTracker = VehicleTracker::instanceNew(vehicle, this);
	vehicleToTracker_[vehicle->name()] = vehicleTracker;
	vehiclesAvailForTrip_.insert(vehicle);

	if (vehiclesAvailForTrip_.size() == 1) {
		travelSim_->vehiclesAvailForTripIsNonZero();
	}
}

void VehicleManager::onVehicleDel(const Ptr<Vehicle>& vehicle) {
	auto vehicleTracker = vehicleToTracker_[vehicle->name()];
	delete vehicleTracker;
	vehicleToTracker_.erase(vehicle->name());

	removeVehicleFromAvailList(vehicle);
}

void VehicleManager::onVehicleStatus(const Ptr<Vehicle>& vehicle) {
	if (vehicle->status() == Vehicle::available) {
		vehiclesAvailForTrip_.insert(vehicle);
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
	travelNetworkManager_(travelSim->travelNetworkManager()),
	travelSim_(travelSim)
{
	// Nothing else to do
}

#endif