#include "TravelNetworkManager.h"
#include "ConnImpl.h"
#include "TravelSim.h"
#include "VehicleManagerImpl.h"

#include <ostream>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

unsigned int carId = 0;

void createCar(const Ptr<TravelNetworkManager>& travelNetworkManager, const Ptr<Location>& loc) {
	auto c = travelNetworkManager->carNew("car-" + std::to_string(carId++));
	c->locationIs(loc);
	c->speedIs(5);
}

int main(const int argc, const char* const argv[]) {
    const auto travelNetworkManager = TravelNetworkManager::instanceNew("mgr");
    const auto sim = TravelSim::instanceNew(travelNetworkManager);

    const auto loc1 = travelNetworkManager->residenceNew("stanford");
    const auto loc2 = travelNetworkManager->airportNew("sfo");
    const auto seg1 = travelNetworkManager->roadNew("road-1");
    seg1->sourceIs(loc2);
    seg1->destinationIs(loc1);
    seg1->lengthIs(10);

    const auto seg2 = travelNetworkManager->roadNew("road-2");
    seg2->sourceIs(loc1);
    seg2->destinationIs(loc2);
    seg2->lengthIs(15);

    createCar(travelNetworkManager, loc1);
    createCar(travelNetworkManager, loc1);
    createCar(travelNetworkManager, loc1);
    createCar(travelNetworkManager, loc1);
    createCar(travelNetworkManager, loc1);

    sim->simulationEndTimeIsOffset(30);

    return 0;
}
