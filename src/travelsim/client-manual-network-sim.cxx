
#include "TravelNetworkManager.h"
#include "ConnImpl.h"
#include "TravelSim.h"
#include "VehicleManagerImpl.h"

#include <ostream>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

unsigned int MEAN_TRIP_INTERVAL_MINS = 25;
unsigned int DEV_TRIP_INTERVAL_MINS = 40;
unsigned int MIN_TRIP_INTERVAL_MINS = 10;
unsigned int MAX_TRIP_INTERVAL_MINS = 60;

unsigned int MIN_NETWORK_MODIFICATION_INTERVAL_MINS = 20;
unsigned int MAX_NETWORK_MODIFICATION_INTERVAL_MINS = 50;

void initializeSegment(const Ptr<Segment> seg, 
						   const Ptr<Location>& source, 
						   const Ptr<Location>& destination, 
						   const Miles& length) {
	seg->sourceIs(source);
	seg->destinationIs(destination);
	seg->lengthIs(length);
}

Ptr<Road> createRoadSegment(const Ptr<TravelNetworkManager>& travelNetwork,
						   const string& name, 
						   const Ptr<Location>& source, 
						   const Ptr<Location>& destination, 
						   const Miles& length) {
	const auto seg = travelNetwork->roadNew(name);
	initializeSegment(seg, source, destination, length);

	return seg;
}

Ptr<Car> createCar(const Ptr<TravelNetworkManager>& travelNetwork,
				   const string& name,
				   const Ptr<Location> loc,
				   const MilesPerHour& speed) {
	const auto car = travelNetwork->carNew(name);
	car->locationIs(loc);
	car->speedIs(speed);
	return car;
}

void populateNetwork(const Ptr<TravelNetworkManager>& manager) {
	const auto loc1 = manager->residenceNew("loc1");
	const auto loc2 = manager->residenceNew("loc2");
	const auto loc3 = manager->residenceNew("loc3");
	const auto loc4 = manager->residenceNew("loc4");
	const auto loc5 = manager->residenceNew("loc5");
	const auto loc6 = manager->residenceNew("loc6");

	const auto seg12 = createRoadSegment(manager, "road-12", loc1, loc2, 15);
	const auto seg13 = createRoadSegment(manager, "road-13", loc1, loc3, 5); 
	const auto seg14 = createRoadSegment(manager, "road-14", loc1, loc4, 20);
	const auto seg15 = createRoadSegment(manager, "road-15", loc1, loc5, 100);

	const auto seg24 = createRoadSegment(manager, "road-24", loc2, loc4, 30);

	const auto seg31 = createRoadSegment(manager, "road-31", loc3, loc1, 2);
	const auto seg34 = createRoadSegment(manager, "road-34", loc3, loc4, 10);
	const auto seg35 = createRoadSegment(manager, "road-35", loc3, loc5, 60);
	const auto seg36 = createRoadSegment(manager, "road-36", loc3, loc6, 25);

	const auto seg41 = createRoadSegment(manager, "road-41", loc4, loc1, 35);
	const auto seg43 = createRoadSegment(manager, "road-43", loc4, loc1, 17);
	const auto seg45 = createRoadSegment(manager, "road-45", loc4, loc5, 120);
	const auto seg46 = createRoadSegment(manager, "road-46", loc4, loc6, 3);

	const auto seg61 = createRoadSegment(manager, "road-61", loc6, loc1, 12);
	const auto seg64 = createRoadSegment(manager, "road-64", loc6, loc1, 41);
	const auto seg65 = createRoadSegment(manager, "road-65", loc6, loc5, 10);

	createCar(manager, "car1", loc1, 0);
	createCar(manager, "car2", loc2, 2);
	createCar(manager, "car3", loc3, 15);
	createCar(manager, "car4", loc4, 8);
	createCar(manager, "car5", loc5, 4);
	createCar(manager, "car6", loc6, 150);
}


void runSimulation(int seed, int totalTimeInMins, 
				   int enableShortestPathCaching, int enableNetworkModification,
				   int useConstDistr) {

	cout << "enableNetworkModification: " << enableNetworkModification << endl;
	cout << "enableShortestPathCaching: " << enableShortestPathCaching << endl;
	cout << "seed: " << seed << endl;
	cout << "totalTimeInMins: " << totalTimeInMins << endl;
	cout << "useConstDistr: " << useConstDistr << endl;

    const auto travelNetworkManager = TravelNetworkManager::instanceNew("mgr");
    const auto conn = travelNetworkManager->conn();
    const auto sim = TravelSim::instanceNew(travelNetworkManager);
    const auto tripGenerator = sim->tripGenerator();

    conn->shortestPathCacheIsEnabledIs(enableShortestPathCaching);

    if (useConstDistr) {
		tripGenerator->tripCountGeneratorIs(ConstGenerator::instanceNew(4));
		tripGenerator->tripIntervalGeneratorIs(ConstGenerator::instanceNew(30 * 60));
    } else {
	    tripGenerator->tripCountGeneratorIs(UniformDistributionRandom::instanceNew(seed, 2,4));
	    tripGenerator->tripIntervalGeneratorIs(NormalDistributionRandom::instanceNew(seed, 
	    	MEAN_TRIP_INTERVAL_MINS * 60,
	    	DEV_TRIP_INTERVAL_MINS * 60,
	    	MIN_TRIP_INTERVAL_MINS * 60,
	    	MAX_TRIP_INTERVAL_MINS * 60));
	}

	if (enableNetworkModification != 0) {
	    const auto networkModifier = sim->networkModifier();
	    networkModifier->activityIntervalGeneratorIs(UniformDistributionRandom::instanceNew(
	    	MIN_NETWORK_MODIFICATION_INTERVAL_MINS * 60,
	    	MAX_NETWORK_MODIFICATION_INTERVAL_MINS * 60));
	    networkModifier->probOfDeletingLocationIs(1);
	    networkModifier->probOfDeletingSegmentIs(1);
	}

    sim->locAndSegManager()->locAndSegIndexRngIs(UniformDistributionRandom::instanceNew(seed, 0, 10));

    populateNetwork(travelNetworkManager);

    sim->simulationEndTimeIsOffset(totalTimeInMins * 60);

    // Print trip stats
    const auto stats = travelNetworkManager->stats();
    cout << "Trips in the network: " << stats->tripCount() << endl;
    cout << "Trips completed: " << stats->tripCompletedCount() << endl;
    const double avgWaitTimeInHours = stats->tripAverageWaitTime().value() / 3600;
    cout << "Avg passenger wait time: " << avgWaitTimeInHours << " hours" << endl;

    // Print path cache stats
    const auto pathCacheStats = conn->shortestPathCacheStats();

    cout << "Request count: " << pathCacheStats->requestCount() << endl;
    cout << "Hit count: " << pathCacheStats->hitCount() << endl;
    cout << "Miss count: " << pathCacheStats->missCount() << endl;

    // Print location and segment stats
    cout << "Location count: " << stats->locationCount() << endl;
    cout << "Segment count: " << stats->segmentCount() << endl;

    sim->activitiesDel();
}

int main(int argv, char** argc) {
	int seed = std::stoi(argc[1]);
	int totalTimeInMins = std::stoi(argc[2]);
	int enableNetworkModification = std::stoi(argc[3]);
	int useConstDistr = std::stoi(argc[4]);
	int enableShortestPathCaching = std::stoi(argc[5]);

	runSimulation(seed, totalTimeInMins, enableShortestPathCaching, enableNetworkModification, useConstDistr);
}