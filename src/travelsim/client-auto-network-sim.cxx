
#include "TravelNetworkManager.h"
#include "ConnImpl.h"
#include "TravelSim.h"
#include "VehicleManagerImpl.h"

#include <ostream>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

unsigned int MIN_ROAD_LENGTH_IN_MILES = 40;
unsigned int MAX_ROAD_LENGTH_IN_MILES = 800;

unsigned int MIN_CAR_SPEED_IN_MPH = 20;
unsigned int MAX_CAR_SPEED_IN_MPH = 60;

unsigned int MEAN_TRIP_INTERVAL_MINS = 25;
unsigned int DEV_TRIP_INTERVAL_MINS = 40;
unsigned int MIN_TRIP_INTERVAL_MINS = 10;
unsigned int MAX_TRIP_INTERVAL_MINS = 60;

unsigned int MIN_NETWORK_MODIFICATION_INTERVAL_MINS = 20;
unsigned int MAX_NETWORK_MODIFICATION_INTERVAL_MINS = 50;

void populateNetwork(unsigned int seed, 
                     const Ptr<TravelNetworkManager>& mgr, 
                     unsigned int numResidences, 
                     unsigned int numRoads,
                     unsigned int numCars) {
    string roadNamePrefix = "seg";
    string locNamePrefix = "loc";
    string carNamePrefix = "car";

    // Create residences
    for(auto i = 0u; i < numResidences; i++) {
        mgr->residenceNew(locNamePrefix + std::to_string(i));
    }

    // Create roads
    const auto maxNumResidences = numResidences * numResidences;
    if (numRoads > maxNumResidences) {
        numRoads = maxNumResidences;
    }

    const auto residenceRng = UniformDistributionRandom::instanceNew(seed, 0, numResidences);
    const auto lengthRng = UniformDistributionRandom::instanceNew(seed, MIN_ROAD_LENGTH_IN_MILES, MAX_ROAD_LENGTH_IN_MILES);

    for (auto i = 0u; i < numRoads; i++) {
        const auto source = mgr->location(locNamePrefix + std::to_string((int)(residenceRng->value())));
        const auto destination = mgr->location(locNamePrefix + std::to_string((int)(residenceRng->value())));
        const auto length = Miles(lengthRng->value());
        const auto road = mgr->roadNew(roadNamePrefix + std::to_string(i));

        road->sourceIs(source);
        road->destinationIs(destination);
        road->lengthIs(length);
    }

    // Create cars
    const auto speedRng = UniformDistributionRandom::instanceNew(seed, MIN_CAR_SPEED_IN_MPH, MAX_CAR_SPEED_IN_MPH);

    for (auto i = 0u; i < numCars; i++) {
        const auto car = mgr->carNew(carNamePrefix + std::to_string(i));
        const auto locName = locNamePrefix + std::to_string((int)(residenceRng->value()));
        car->locationIs(mgr->location(locName));
        car->speedIs(speedRng->value());
    }
}

void runSimulation(int numResidences, int numRoads,
				   int numCars, int enableNetworkModification,
				   int seed, unsigned int totalTimeInMins,
				   int enableShortestPathCaching) {

	cout << "enableNetworkModification: " << enableNetworkModification << endl;
	cout << "enableShortestPathCaching: " << enableShortestPathCaching << endl;
	cout << "numCars: " << numCars << endl;
	cout << "numResidences: " << numResidences << endl;
	cout << "numRoads: " << numRoads << endl;
	cout << "seed: " << seed << endl;
	cout << "totalTimeInMins: " << totalTimeInMins << endl << endl;

    const auto travelNetworkManager = TravelNetworkManager::instanceNew("mgr");
    const auto conn = travelNetworkManager->conn();
    const auto sim = TravelSim::instanceNew(travelNetworkManager);
    const auto tripGenerator = sim->tripGenerator();

    conn->shortestPathCacheIsEnabledIs(enableShortestPathCaching);

    tripGenerator->tripCountGeneratorIs(UniformDistributionRandom::instanceNew(seed, 5,10));
    tripGenerator->tripIntervalGeneratorIs(NormalDistributionRandom::instanceNew(seed, 
    	MEAN_TRIP_INTERVAL_MINS * 60,
    	DEV_TRIP_INTERVAL_MINS * 60,
    	MIN_TRIP_INTERVAL_MINS * 60,
    	MAX_TRIP_INTERVAL_MINS * 60));

    if (enableNetworkModification != 0) {
	    const auto networkModifier = sim->networkModifier();
	    networkModifier->activityIntervalGeneratorIs(UniformDistributionRandom::instanceNew(
	    	MIN_NETWORK_MODIFICATION_INTERVAL_MINS * 60,
	    	MAX_NETWORK_MODIFICATION_INTERVAL_MINS * 60));
	    networkModifier->probOfDeletingLocationIs(1);
	    networkModifier->probOfDeletingSegmentIs(1);
	}

    sim->locAndSegManager()->locAndSegIndexRngIs(UniformDistributionRandom::instanceNew(seed, 0, 10));

    populateNetwork(seed, travelNetworkManager, numResidences, numRoads, numCars);

    sim->simulationEndTimeIsOffset(totalTimeInMins * 60);

     // Print trip stats
    const auto stats = travelNetworkManager->stats();
    cout << endl;
    cout << "=================================================" << endl;
    cout << "Trip stats" << endl;
    cout << "=================================================" << endl;
    cout << "Trips in the network: " << stats->tripCount() << endl;
    cout << "Trips completed: " << stats->tripCompletedCount() << endl;
    const double avgWaitTimeInHours = stats->tripAverageWaitTime().value() / 3600;
    cout << "Avg passenger wait time: " << avgWaitTimeInHours << " hours" << endl;

    // Print path cache stats
    const auto pathCacheStats = conn->shortestPathCacheStats();

    cout << endl;
    cout << "=================================================" << endl;
    cout << "Shortest path cache stats" << endl;
    cout << "=================================================" << endl;
    cout << "Cache request count: " << pathCacheStats->requestCount() << endl;
    cout << "Cache hit count: " << pathCacheStats->hitCount() << endl;
    cout << "Cache miss count: " << pathCacheStats->missCount() << endl;

    // Print location and segment stats
    cout << endl;
    cout << "=================================================" << endl;
    cout << "TravelNetwork stats" << endl;
    cout << "=================================================" << endl;
    cout << "Location count: " << stats->locationCount() << endl;
    cout << "Segment count: " << stats->segmentCount() << endl;

    sim->activitiesDel();
}

int main(int argv, char** argc) {
	int numResidences = std::stoi(argc[1]);
	int numRoads = std::stoi(argc[2]);
	int numCars = std::stoi(argc[3]);
	int enableNetworkModification = std::stoi(argc[4]);
	int seed = std::stoi(argc[5]);
	int totalTimeInMins = std::stoi(argc[6]);
	int enableShortestPathCaching = std::stoi(argc[7]);

	runSimulation(numResidences, numRoads, numCars, enableNetworkModification, seed, totalTimeInMins, enableShortestPathCaching);
}