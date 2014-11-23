#include "TravelNetworkManager.h"
#include "TravelSim.h"

#include <ostream>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

int main(const int argc, const char* const argv[]) {
    const auto travelNetworkManager = TravelNetworkManager::instanceNew("mgr");
    travelNetworkManager->residenceNew("stanford");
    travelNetworkManager->airportNew("sfo");

    const auto sim = TravelSim::instanceNew(travelNetworkManager);
    sim->simulationEndTimeIsOffset(20);

    return 0;
}
