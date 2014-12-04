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

void populateNetwork(unsigned int seed, 
                     const Ptr<TravelNetworkManager>& mgr, 
                     unsigned int numResidences, 
                     unsigned int numRoads,
                     unsigned int numCars) {
    string roadNamePrefix = "seg";
    string locNamePrefix = "loc";
    string carNamePrefix = "car";

    // Create residences
    for(auto i = 0; i < numResidences; i++) {
        mgr->residenceNew(locNamePrefix + std::to_string(i));
    }

    const auto maxNumResidences = numResidences * numResidences;
    if (numRoads > maxNumResidences) {
        numRoads = maxNumResidences;
    }

    const auto residenceRng = UniformDistributionRandom::instanceNew(seed, 0, numResidences);
    const auto lengthRng = UniformDistributionRandom::instanceNew(seed, 1, 100);

    for (auto i = 0; i < numRoads; i++) {
        const auto source = mgr->location(locNamePrefix + std::to_string((int)(residenceRng->value())));
        const auto destination = mgr->location(locNamePrefix + std::to_string((int)(residenceRng->value())));
        const auto length = Miles(lengthRng->value());
        const auto road = mgr->roadNew(roadNamePrefix + std::to_string(i));

        road->sourceIs(source);
        road->destinationIs(destination);
        road->lengthIs(length);
    }

    const auto speedRng = UniformDistributionRandom::instanceNew(seed, 1, 20);

    for (auto i = 0; i < numCars; i++) {
        const auto car = mgr->carNew(carNamePrefix + std::to_string(i));
        const auto locName = locNamePrefix + std::to_string((int)(residenceRng->value()));
        car->locationIs(mgr->location(locName));
        car->speedIs(speedRng->value());
    }
}

void runSmallSizeSimulation(unsigned int totalTime) {
    const auto travelNetworkManager = TravelNetworkManager::instanceNew("mgr");
    const auto sim = TravelSim::instanceNew(travelNetworkManager);
    const auto tripGenerator = sim->tripGenerator();

    tripGenerator->tripCountGeneratorIs(ConstGenerator::instanceNew(4));
    tripGenerator->tripIntervalGeneratorIs(ConstGenerator::instanceNew(7));

    tripGenerator->tripCountGeneratorIs(UniformDistributionRandom::instanceNew(2,10));
    tripGenerator->tripIntervalGeneratorIs(NormalDistributionRandom::instanceNew(5, 3, 1, 9));

    const auto loc1 = travelNetworkManager->residenceNew("stanford");
    const auto loc2 = travelNetworkManager->airportNew("sfo");
    const auto seg1 = travelNetworkManager->roadNew("road-1");
    seg1->sourceIs(loc2);
    seg1->destinationIs(loc1);
    seg1->lengthIs(10);

    const auto seg2 = travelNetworkManager->roadNew("road-2");
    seg2->sourceIs(loc1);
    seg2->destinationIs(loc2);
    seg2->lengthIs(60);

    createCar(travelNetworkManager, loc1);
    createCar(travelNetworkManager, loc1);

    sim->simulationEndTimeIsOffset(totalTime);
}

void runLargeSizeSimulation(unsigned int seed, unsigned int totalTime, unsigned int isPathCachingEnabled) {
    const auto travelNetworkManager = TravelNetworkManager::instanceNew("mgr");
    const auto conn = travelNetworkManager->conn();
    const auto sim = TravelSim::instanceNew(travelNetworkManager);
    const auto tripGenerator = sim->tripGenerator();

    conn->shortestPathCacheIsEnabledIs(isPathCachingEnabled);
    tripGenerator->seedIs(seed);

    tripGenerator->tripCountGeneratorIs(UniformDistributionRandom::instanceNew(seed, 2,10));
    tripGenerator->tripIntervalGeneratorIs(NormalDistributionRandom::instanceNew(seed, 5, 3, 1, 9));

    const auto networkModifier = sim->networkModifier();
    networkModifier->activityIntervalGeneratorIs(ConstGenerator::instanceNew(5));
    networkModifier->probOfDeletingLocationIs(1);
    networkModifier->probOfDeletingSegmentIs(1);

    //populateNetwork(mgr, numResidences, numRoads, numCars)
    populateNetwork(seed, travelNetworkManager, 200, 10000, 200);

    sim->simulationEndTimeIsOffset(totalTime);
    const auto pathCacheStats = conn->pathL2CacheStats();

    cout << "Request count: " << pathCacheStats->requestCount() << endl;
    cout << "Hit count: " << pathCacheStats->hitCount() << endl;
    cout << "Miss count: " << pathCacheStats->missCount() << endl;

    const auto stats = travelNetworkManager->stats();
    cout << "Location count: " << stats->locationCount() << endl;
    cout << "Segment count: " << stats->segmentCount() << endl;
}

int main(const int argc, const char* const argv[]) {
    
    //runSmallSizeSimulation(30);

    //const auto seed = U32(SystemTime::now().value() & 0xffffffff);
    const auto seed = 248056471;
    cout << "Seed: " << seed << endl;
    const auto simTime = 60 * 1;
    runLargeSizeSimulation(seed, simTime, true);
    //runLargeSizeSimulation(seed, simTime, false);

    return 0;
}
