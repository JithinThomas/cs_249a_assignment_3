#include "gtest/gtest.h"
#include "fwk/fwk.h"
#include "TravelNetworkManager.h"
#include "ConnImpl.h"
#include "TravelInstanceManager.h"
#include "TravelSim.h"
#include "VehicleManagerImpl.h"

void initializeSegment(const Ptr<Segment> seg, 
						   const Ptr<Location>& source, 
						   const Ptr<Location>& destination, 
						   const Miles& length) {
	seg->sourceIs(source);
	seg->destinationIs(destination);
	seg->lengthIs(length);
}

Ptr<Flight> createFlightSegment(const Ptr<TravelNetworkManager>& travelNetwork,
						   	  const string& name, 
						      const Ptr<Location>& source, 
						      const Ptr<Location>& destination, 
						      const Miles& length) {
	const auto seg = travelNetwork->flightNew(name);
	initializeSegment(seg, source, destination, length);

	return seg;
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

string getPathSegmentsArrStr(const Ptr<Conn::Path>& path) {
	string s = "";
	for (auto seg : path->segments()) {
		s += seg->source()->name() + " ";
	}

	if (path->segmentCount() > 0) {
		s += path->segment(path->segmentCount() - 1)->destination()->name() + " ";
	}

	return s;
}

void testPath(const Ptr<Conn::Path>& path, const string& expPathStr, const Miles& expPathLength) {
	if (path != null) {
		ASSERT_EQ(getPathSegmentsArrStr(path), expPathStr);
		ASSERT_EQ(path->length().value(), expPathLength.value());
	} else {
		cout << expPathStr << endl;
		ASSERT_TRUE(false);
	}
}

void testshortestPathCache(const Ptr<Conn> conn, 
					 const Ptr<Location>& source, 
					 const Ptr<Location>& destination, 
					 const Ptr<Segment>& seg) {
	const auto destName = destination->name();
	const auto cache = conn->shortestPathCache();
	ASSERT_TRUE(isKeyPresent(cache, destName));

	const auto srcToCacheEntry = cache.find(destName)->second;
	const auto sourceName = source->name();
	ASSERT_TRUE(isKeyPresent(srcToCacheEntry, sourceName));

	ASSERT_EQ(seg->name(), srcToCacheEntry.find(sourceName)->second);
}

TEST(Conn, shortestPath_1) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->residenceNew("loc1");
	const auto loc2 = manager->airportNew("loc2");
	const auto loc3 = manager->residenceNew("loc3");
	const auto loc4 = manager->airportNew("loc4");
	const auto loc5 = manager->residenceNew("loc5");
	const auto loc6 = manager->residenceNew("loc6");

	const auto seg12 = createRoadSegment(manager, "road-12", loc1, loc2, 15);
	const auto seg13 = createRoadSegment(manager, "road-13", loc1, loc3, 5); 
	const auto seg14 = createRoadSegment(manager, "road-14", loc1, loc4, 20);
	const auto seg15 = createRoadSegment(manager, "road-15", loc1, loc5, 100);

	const auto seg24 = createFlightSegment(manager, "road-24", loc2, loc4, 30);

	const auto seg31 = createRoadSegment(manager, "road-31", loc3, loc1, 2);
	const auto seg34 = createRoadSegment(manager, "road-34", loc3, loc4, 10);
	const auto seg35 = createRoadSegment(manager, "road-35", loc3, loc5, 60);
	const auto seg36 = createRoadSegment(manager, "road-36", loc3, loc6, 25);

	const auto seg41 = createRoadSegment(manager, "road-41", loc4, loc1, 35);
	const auto seg43 = createRoadSegment(manager, "road-43", loc4, loc3, 17);
	const auto seg45 = createRoadSegment(manager, "road-45", loc4, loc5, 120);
	const auto seg46 = createRoadSegment(manager, "road-46", loc4, loc6, 3);

	const auto seg61 = createRoadSegment(manager, "road-61", loc6, loc1, 12);
	const auto seg64 = createRoadSegment(manager, "road-64", loc6, loc4, 41);
	const auto seg65 = createRoadSegment(manager, "road-65", loc6, loc5, 10);

	const auto conn = manager->conn();

	conn->shortestPath(loc1, loc5);

	conn->shortestPath(loc3, loc1);
	conn->shortestPath(loc3, loc4);
	conn->shortestPath(loc3, loc5);
	conn->shortestPath(loc3, loc6);

	conn->shortestPath(loc2, loc4);

	conn->shortestPath(loc4, loc1);
	conn->shortestPath(loc4, loc2);
	conn->shortestPath(loc4, loc3);
	conn->shortestPath(loc4, loc5);
	conn->shortestPath(loc4, loc6);

	conn->shortestPath(loc6, loc1);
	conn->shortestPath(loc6, loc4);
	conn->shortestPath(loc6, loc5);

	ASSERT_EQ(conn->shortestPathCached(loc1, loc1), null);
	testPath(conn->shortestPathCached(loc1, loc2), "loc1 loc2 ", 15);
	testPath(conn->shortestPathCached(loc1, loc3), "loc1 loc3 ", 5);
	testPath(conn->shortestPathCached(loc1, loc4), "loc1 loc3 loc4 ", 15);
	testPath(conn->shortestPathCached(loc1, loc5), "loc1 loc3 loc4 loc6 loc5 ", 28);
	testPath(conn->shortestPathCached(loc1, loc6), "loc1 loc3 loc4 loc6 ", 18);

	testPath(conn->shortestPathCached(loc2, loc4), "loc2 loc4 ", 30);

	testPath(conn->shortestPathCached(loc3, loc1), "loc3 loc1 ", 2);
	testPath(conn->shortestPathCached(loc3, loc4), "loc3 loc4 ", 10);
	testPath(conn->shortestPathCached(loc3, loc5), "loc3 loc4 loc6 loc5 ", 23);
	testPath(conn->shortestPathCached(loc3, loc6), "loc3 loc4 loc6 ", 13);

	testPath(conn->shortestPathCached(loc4, loc1), "loc4 loc6 loc1 ", 15);
	testPath(conn->shortestPathCached(loc4, loc2), "loc4 loc6 loc1 loc2 ", 30);
	testPath(conn->shortestPathCached(loc4, loc3), "loc4 loc3 ", 17);
	testPath(conn->shortestPathCached(loc4, loc5), "loc4 loc6 loc5 ", 13);
	testPath(conn->shortestPathCached(loc4, loc6), "loc4 loc6 ", 3);

	testPath(conn->shortestPathCached(loc6, loc1), "loc6 loc1 ", 12);
	testPath(conn->shortestPathCached(loc6, loc4), "loc6 loc1 loc3 loc4 ", 27);
	testPath(conn->shortestPathCached(loc6, loc5), "loc6 loc5 ", 10);
}

TEST(Conn, shortestPath_2) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->residenceNew("loc1");
	const auto loc2 = manager->residenceNew("loc2");
	const auto loc3 = manager->residenceNew("loc3");
	const auto loc4 = manager->residenceNew("loc4");
	const auto loc5 = manager->residenceNew("loc5");
	const auto loc6 = manager->residenceNew("loc6");

	const auto seg12 = createRoadSegment(manager, "road-1", loc1, loc2, 15);
	const auto seg13 = createRoadSegment(manager, "road-2", loc1, loc3, 5); 
	const auto seg14 = createRoadSegment(manager, "road-3", loc1, loc4, 20);
	const auto seg15 = createRoadSegment(manager, "road-4", loc1, loc5, 100);

	const auto seg24 = createRoadSegment(manager, "road-5", loc2, loc4, 30);

	const auto seg31 = createRoadSegment(manager, "road-6", loc3, loc1, 2);
	const auto seg34 = createRoadSegment(manager, "road-7", loc3, loc4, 10);
	const auto seg35 = createRoadSegment(manager, "road-8", loc3, loc5, 60);
	const auto seg36 = createRoadSegment(manager, "road-9", loc3, loc6, 25);

	const auto seg41 = createRoadSegment(manager, "road-10", loc4, loc1, 35);
	const auto seg45 = createRoadSegment(manager, "road-11", loc4, loc5, 120);
	const auto seg46 = createRoadSegment(manager, "road-12", loc4, loc6, 3);

	const auto seg65 = createRoadSegment(manager, "road-13", loc6, loc5, 10);

	const auto conn = manager->conn();

	testPath(conn->shortestPath(loc1, loc1), "", 0);
	testPath(conn->shortestPath(loc1, loc2), "loc1 loc2 ", 15);
	testPath(conn->shortestPath(loc1, loc3), "loc1 loc3 ", 5);
	testPath(conn->shortestPath(loc1, loc4), "loc1 loc3 loc4 ", 15);
	testPath(conn->shortestPath(loc1, loc5), "loc1 loc3 loc4 loc6 loc5 ", 28);
	testPath(conn->shortestPath(loc1, loc6), "loc1 loc3 loc4 loc6 ", 18);

	//conn->printShortestPathCache();

	testshortestPathCache(conn, loc1, loc2, seg12);
	testshortestPathCache(conn, loc1, loc3, seg13);
	testshortestPathCache(conn, loc1, loc4, seg13);
	testshortestPathCache(conn, loc1, loc5, seg13);
	testshortestPathCache(conn, loc1, loc6, seg13);

	testshortestPathCache(conn, loc3, loc4, seg34);
	testshortestPathCache(conn, loc3, loc5, seg34);
	testshortestPathCache(conn, loc3, loc6, seg34);

	testshortestPathCache(conn, loc4, loc5, seg46);
	testshortestPathCache(conn, loc4, loc6, seg46);

	testshortestPathCache(conn, loc6, loc5, seg65);

	ASSERT_EQ(loc4->destinationSegmentCount(), 3);

	manager->locationDel("loc4");

	ASSERT_EQ(seg41->source(), null);
	ASSERT_EQ(seg45->source(), null);
	ASSERT_EQ(seg46->source(), null);
	ASSERT_EQ(seg14->source(), loc1);
	ASSERT_EQ(seg24->source(), loc2);
	ASSERT_EQ(seg34->source(), loc3);

	ASSERT_EQ(seg14->destination(), null);
	ASSERT_EQ(seg24->destination(), null);
	ASSERT_EQ(seg34->destination(), null);
	ASSERT_EQ(seg41->destination(), loc1);
	ASSERT_EQ(seg45->destination(), loc5);
	ASSERT_EQ(seg46->destination(), loc6);

	//conn->printShortestPathCache();

	testPath(conn->shortestPath(loc1, loc5), "loc1 loc3 loc6 loc5 ", 40);
	testPath(conn->shortestPath(loc1, loc6), "loc1 loc3 loc6 ", 30);
	testPath(conn->shortestPath(loc1, loc6), "loc1 loc3 loc6 ", 30);

	const auto stats = conn->shortestPathCacheStats();

	ASSERT_EQ(stats->hitCount(), 4);
	ASSERT_EQ(stats->missCount(), 4);
	ASSERT_EQ(stats->requestCount(), 8);

	manager->locationDel("loc6");
	testPath(conn->shortestPath(loc1, loc5), "loc1 loc3 loc5 ", 65);

	manager->locationDel("loc3");
	testPath(conn->shortestPath(loc1, loc5), "loc1 loc5 ", 100);

	ASSERT_EQ(conn->shortestPath(loc1, loc6), null);
	ASSERT_EQ(conn->shortestPath(loc6, loc5), null);
	ASSERT_EQ(conn->shortestPath(loc6, loc4), null);
	ASSERT_EQ(conn->shortestPath(loc3, loc4), null);
	ASSERT_EQ(conn->shortestPath(loc3, loc1), null);
	ASSERT_EQ(conn->shortestPath(null, null), null);

	//conn->printShortestPathCache();	
}

TEST(Conn, shortestPath_segmentDel) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->residenceNew("loc1");
	const auto loc2 = manager->residenceNew("loc2");
	const auto loc3 = manager->residenceNew("loc3");
	const auto loc4 = manager->residenceNew("loc4");
	const auto loc5 = manager->residenceNew("loc5");
	const auto loc6 = manager->residenceNew("loc6");

	const auto seg12 = createRoadSegment(manager, "road-1", loc1, loc2, 15);
	const auto seg13 = createRoadSegment(manager, "road-2", loc1, loc3, 5); 
	const auto seg14 = createRoadSegment(manager, "road-3", loc1, loc4, 20);
	const auto seg15 = createRoadSegment(manager, "road-4", loc1, loc5, 100);

	const auto seg24 = createRoadSegment(manager, "road-5", loc2, loc4, 30);

	const auto seg31 = createRoadSegment(manager, "road-6", loc3, loc1, 2);
	const auto seg34 = createRoadSegment(manager, "road-7", loc3, loc4, 10);
	const auto seg35 = createRoadSegment(manager, "road-8", loc3, loc5, 60);
	const auto seg36 = createRoadSegment(manager, "road-9", loc3, loc6, 25);

	const auto seg41 = createRoadSegment(manager, "road-10", loc4, loc1, 35);
	const auto seg45 = createRoadSegment(manager, "road-11", loc4, loc5, 120);
	const auto seg46 = createRoadSegment(manager, "road-12", loc4, loc6, 3);

	const auto seg65 = createRoadSegment(manager, "road-13", loc6, loc5, 10);

	const auto conn = manager->conn();

	testPath(conn->shortestPath(loc1, loc1), "", 0);
	testPath(conn->shortestPath(loc1, loc2), "loc1 loc2 ", 15);
	testPath(conn->shortestPath(loc1, loc3), "loc1 loc3 ", 5);
	testPath(conn->shortestPath(loc1, loc4), "loc1 loc3 loc4 ", 15);
	testPath(conn->shortestPath(loc1, loc5), "loc1 loc3 loc4 loc6 loc5 ", 28);
	testPath(conn->shortestPath(loc1, loc6), "loc1 loc3 loc4 loc6 ", 18);

	manager->segmentDel("road-7");

	testPath(conn->shortestPath(loc1, loc4), "loc1 loc4 ", 20);
	testPath(conn->shortestPath(loc1, loc6), "loc1 loc4 loc6 ", 23);
	testPath(conn->shortestPath(loc1, loc5), "loc1 loc4 loc6 loc5 ", 33);

	manager->segmentDel("road-12");

	testPath(conn->shortestPath(loc1, loc6), "loc1 loc3 loc6 ", 30);
	testPath(conn->shortestPath(loc1, loc5), "loc1 loc3 loc6 loc5 ", 40);

	manager->segmentDel("road-9");
	const auto p = conn->shortestPath(loc1, loc6);
	ASSERT_EQ(conn->shortestPath(loc1, loc6), null);
	testPath(conn->shortestPath(loc1, loc5), "loc1 loc3 loc5 ", 65);
}

TEST(Conn, shortestPath_addLocAndSeg) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->residenceNew("loc1");
	const auto loc2 = manager->residenceNew("loc2");
	const auto loc3 = manager->residenceNew("loc3");
	const auto loc4 = manager->residenceNew("loc4");
	const auto loc5 = manager->residenceNew("loc5");
	const auto loc6 = manager->residenceNew("loc6");

	const auto seg12 = createRoadSegment(manager, "road-1", loc1, loc2, 15);
	const auto seg13 = createRoadSegment(manager, "road-2", loc1, loc3, 5); 
	const auto seg14 = createRoadSegment(manager, "road-3", loc1, loc4, 20);
	const auto seg15 = createRoadSegment(manager, "road-4", loc1, loc5, 100);

	const auto seg24 = createRoadSegment(manager, "road-5", loc2, loc4, 30);

	const auto seg31 = createRoadSegment(manager, "road-6", loc3, loc1, 2);
	const auto seg34 = createRoadSegment(manager, "road-7", loc3, loc4, 10);
	const auto seg35 = createRoadSegment(manager, "road-8", loc3, loc5, 60);
	const auto seg36 = createRoadSegment(manager, "road-9", loc3, loc6, 25);

	const auto seg41 = createRoadSegment(manager, "road-10", loc4, loc1, 35);
	const auto seg45 = createRoadSegment(manager, "road-11", loc4, loc5, 120);
	const auto seg46 = createRoadSegment(manager, "road-12", loc4, loc6, 3);

	const auto seg65 = createRoadSegment(manager, "road-13", loc6, loc5, 10);

	const auto conn = manager->conn();

	testPath(conn->shortestPath(loc1, loc1), "", 0);
	testPath(conn->shortestPath(loc1, loc2), "loc1 loc2 ", 15);
	testPath(conn->shortestPath(loc1, loc3), "loc1 loc3 ", 5);
	testPath(conn->shortestPath(loc1, loc4), "loc1 loc3 loc4 ", 15);
	testPath(conn->shortestPath(loc1, loc5), "loc1 loc3 loc4 loc6 loc5 ", 28);
	testPath(conn->shortestPath(loc1, loc6), "loc1 loc3 loc4 loc6 ", 18);

	ASSERT_EQ(5, conn->shortestPathCache().size());

	manager->residenceNew("loc7");
	ASSERT_EQ(0, conn->shortestPathCache().size());

	testPath(conn->shortestPath(loc1, loc5), "loc1 loc3 loc4 loc6 loc5 ", 28);
	ASSERT_EQ(5, conn->shortestPathCache().size());

	const auto seg64 = createRoadSegment(manager, "road-14", loc6, loc4, 1);
	ASSERT_EQ(0, conn->shortestPathCache().size());
}

TEST(TravelNetworkManager, instanceNew) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	ASSERT_TRUE(manager != null);
	ASSERT_TRUE(manager->name() == "manager-1");
}

TEST(TravelNetworkManager, locationNew) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto residence1 = "location-1";
	const auto airport1 = "location-2";

	ASSERT_TRUE(manager->location(residence1) == null);
	ASSERT_TRUE(manager->location(airport1) == null);

	const auto residence = manager->residenceNew(residence1);
	const auto airport = manager->airportNew(airport1);
	auto res = manager->location(residence1);

	ASSERT_TRUE(res != null);
	ASSERT_TRUE(res == residence);
	ASSERT_TRUE(res->name() == residence1);

	res = manager->location(airport1);
	ASSERT_TRUE(res != null);
	ASSERT_TRUE(res == airport);
	ASSERT_TRUE(res->name() == airport1);

	/* Testing airport() and residence() accessor methods */
	auto res1 = manager->airport(airport1);
	ASSERT_TRUE(res1 == airport);
	ASSERT_TRUE(res1->name() == airport1);

	auto res2 = manager->residence(residence1);
	ASSERT_TRUE(res2 == residence);
	ASSERT_TRUE(res2->name() == residence1);
}

TEST(TravelNetworkManager, segmentNew) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto flight1 = "segment-1";
	const auto road1 = "segment-2";

	ASSERT_TRUE(manager->segment(flight1) == null);
	ASSERT_TRUE(manager->segment(road1) == null);

	const auto flight = manager->flightNew(flight1);
	const auto road = manager->roadNew(road1);
	auto res = manager->segment(road1);

	ASSERT_TRUE(res != null);
	ASSERT_TRUE(res == road);
	ASSERT_TRUE(res->name() == road1);

	res = manager->segment(flight1);
	ASSERT_TRUE(res != null);
	ASSERT_TRUE(res == flight);
	ASSERT_TRUE(res->name() == flight1);

	/* Testing flight() and road() accessor methods */
	auto res1 = manager->flight(flight1);
	ASSERT_TRUE(res1 == flight);
	ASSERT_TRUE(res1->name() == flight1);

	auto res2 = manager->road(road1);
	ASSERT_TRUE(res2 == road);
	ASSERT_TRUE(res2->name() == road1);
}

TEST(TravelNetworkManager, vehicleNew) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto car1 = "vehicle-1";
	const auto airplane1 = "vehicle-2";

	ASSERT_TRUE(manager->segment(car1) == null);
	ASSERT_TRUE(manager->segment(airplane1) == null);

	const auto car = manager->carNew(car1);
	const auto airplane = manager->airplaneNew(airplane1);
	auto res = manager->vehicle(car1);

	ASSERT_TRUE(res != null);
	ASSERT_TRUE(res == car);
	ASSERT_TRUE(res->name() == car1);

	res = manager->vehicle(airplane1);
	ASSERT_TRUE(res != null);
	ASSERT_TRUE(res == airplane);
	ASSERT_TRUE(res->name() == airplane1);

	/* Testing car() and airplane() accessor methods */
	auto res1 = manager->car(car1);
	ASSERT_TRUE(res1 == car);
	ASSERT_TRUE(res1->name() == car1);

	auto res2 = manager->airplane(airplane1);
	ASSERT_TRUE(res2 == airplane);
	ASSERT_TRUE(res2->name() == airplane1);
}

TEST(TravelNetworkManager, locationIter) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	vector<string> names { "location1", "location2", "location3", "location4" };

	manager->airportNew(names[0]);
	manager->residenceNew(names[1]);
	manager->residenceNew(names[2]);
	manager->airportNew(names[3]);

	ASSERT_EQ(names.size(), 4);

	for (auto it = manager->locationIter(); it != manager->locationIterEnd(); it++) {
		const auto loc = it->second;
		auto tmp = std::find(names.begin(), names.end(), loc->name());
		names.erase(tmp);
	}

	ASSERT_EQ(names.size(), 0);
}

TEST(TravelNetworkManager, segmentIter) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	vector<string> names { "segment1", "segment2", "segment3", "segment4" };

	manager->flightNew(names[0]);
	manager->roadNew(names[1]);
	manager->flightNew(names[2]);
	manager->roadNew(names[3]);

	ASSERT_EQ(names.size(), 4);

	for (auto it = manager->segmentIter(); it != manager->segmentIterEnd(); it++) {
		const auto seg = it->second;
		auto tmp = std::find(names.begin(), names.end(), seg->name());
		names.erase(tmp);
	}

	ASSERT_EQ(names.size(), 0);
}

TEST(TravelNetworkManager, vehicleIter) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	vector<string> names { "segment1", "segment2", "segment3", "segment4" };

	manager->carNew(names[0]);
	manager->airplaneNew(names[1]);
	manager->airplaneNew(names[2]);
	manager->carNew(names[3]);

	ASSERT_EQ(names.size(), 4);

	for (auto it = manager->vehicleIter(); it != manager->vehicleIterEnd(); it++) {
		const auto v = it->second;
		auto tmp = std::find(names.begin(), names.end(), v->name());
		names.erase(tmp);
	}

	ASSERT_EQ(names.size(), 0);
}

TEST(TravelNetworkManager, locationDel) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	vector<string> names { "location1", "location2", "location3", "location4" };

	const auto loc0 = manager->airportNew(names[0]);
	const auto loc1 = manager->residenceNew(names[1]);
	const auto loc2 = manager->residenceNew(names[2]);
	const auto loc3 = manager->airportNew(names[3]);

	auto res = manager->locationDel(names[2]);

	ASSERT_TRUE(res == loc2);
	ASSERT_TRUE(manager->location(names[2]) == null);
	ASSERT_TRUE(loc2 != null);

	/* Ensure that the other locations remain intact */
	ASSERT_TRUE(manager->location(names[0]) == loc0);
	ASSERT_TRUE(manager->location(names[1]) == loc1);
	ASSERT_TRUE(manager->location(names[3]) == loc3);

	res = manager->locationDel(names[0]);

	ASSERT_TRUE(res == loc0);
	ASSERT_TRUE(manager->location(names[0]) == null);
	ASSERT_TRUE(loc0 != null);

	res = manager->locationDel("fakeName");
	ASSERT_TRUE(res == null);

	/* Ensure that the other locations remain intact */
	ASSERT_TRUE(manager->location(names[1]) == loc1);
	ASSERT_TRUE(manager->location(names[3]) == loc3);
}

TEST(TravelNetworkManager, segmentDel) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	vector<string> names { "segment1", "segment2", "segment3", "segment4" };

	const auto seg0 = manager->roadNew(names[0]);
	const auto seg1 = manager->flightNew(names[1]);
	const auto seg2 = manager->flightNew(names[2]);
	const auto seg3 = manager->roadNew(names[3]);

	auto res = manager->segmentDel(names[2]);

	ASSERT_TRUE(res == seg2);
	ASSERT_TRUE(manager->segment(names[2]) == null);
	ASSERT_TRUE(seg2 != null);

	/* Ensure that the other locations remain intact */
	ASSERT_TRUE(manager->segment(names[0]) == seg0);
	ASSERT_TRUE(manager->segment(names[1]) == seg1);
	ASSERT_TRUE(manager->segment(names[3]) == seg3);

	res = manager->segmentDel(names[0]);

	ASSERT_TRUE(res == seg0);
	ASSERT_TRUE(manager->segment(names[0]) == null);
	ASSERT_TRUE(seg0 != null);

	res = manager->segmentDel("fakeName");
	ASSERT_TRUE(res == null);

	/* Ensure that the other locations remain intact */
	ASSERT_TRUE(manager->segment(names[1]) == seg1);
	ASSERT_TRUE(manager->segment(names[3]) == seg3);
}

TEST(TravelNetworkManager, vehicleDel) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	vector<string> names { "vehicle1", "vehicle2", "vehicle3", "vehicle4" };

	const auto v0 = manager->carNew(names[0]);
	const auto v1 = manager->airplaneNew(names[1]);
	const auto v2 = manager->carNew(names[2]);
	const auto v3 = manager->airplaneNew(names[3]);

	auto res = manager->vehicleDel(names[1]);

	ASSERT_TRUE(res == v1);
	ASSERT_TRUE(manager->vehicle(names[1]) == null);
	ASSERT_TRUE(v1 != null);

	/* Ensure that the other locations remain intact */
	ASSERT_TRUE(manager->vehicle(names[0]) == v0);
	ASSERT_TRUE(manager->vehicle(names[2]) == v2);
	ASSERT_TRUE(manager->vehicle(names[3]) == v3);

	res = manager->vehicleDel(names[0]);

	ASSERT_TRUE(res == v0);
	ASSERT_TRUE(manager->vehicle(names[0]) == null);
	ASSERT_TRUE(v0 != null);

	res = manager->vehicleDel("fakeName");
	ASSERT_TRUE(res == null);

	/* Ensure that the other locations remain intact */
	ASSERT_TRUE(manager->vehicle(names[2]) == v2);
	ASSERT_TRUE(manager->vehicle(names[3]) == v3);
}

TEST(TravelNetworkManager, locationDel_Update_Segment_SrcAndDst) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	vector<string> locationNames { "location0", "location1", "location2" };
	vector<string> segmentNames { "segment0", "segment1", "segment2" };

	const auto loc0 = manager->airportNew(locationNames[0]);
	const auto loc1 = manager->residenceNew(locationNames[1]);
	const auto loc2 = manager->airportNew(locationNames[2]);

	const auto seg0 = manager->flightNew(segmentNames[0]);
	const auto seg1 = manager->roadNew(segmentNames[1]);
	const auto seg2 = manager->roadNew(segmentNames[2]);

	seg0->sourceIs(loc0);
	seg0->destinationIs(loc2);

	seg1->sourceIs(loc0);
	seg1->destinationIs(loc1);

	seg2->sourceIs(loc1);
	seg2->destinationIs(loc2);

	ASSERT_TRUE(seg0->source() == loc0);
	ASSERT_TRUE(seg0->destination() == loc2);

	ASSERT_TRUE(seg1->source() == loc0);
	ASSERT_TRUE(seg1->destination() == loc1);

	ASSERT_TRUE(seg2->source() == loc1);
	ASSERT_TRUE(seg2->destination() == loc2);

	manager->locationDel(locationNames[0]);
	manager->locationDel(locationNames[2]);

	ASSERT_TRUE(seg0->source() == null);
	ASSERT_TRUE(seg0->destination() == null);

	ASSERT_TRUE(seg1->source() == null);
	ASSERT_TRUE(seg1->destination() == loc1);

	ASSERT_TRUE(seg2->source() == loc1);
	ASSERT_TRUE(seg2->destination() == null);
}

TEST(TravelNetworkManager, segmentDel_Update_Loc_SrcAndDst) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	vector<string> locationNames { "location0", "location1", "location2" };
	vector<string> segmentNames { "segment0", "segment1", "segment2" };

	const auto loc0 = manager->airportNew(locationNames[0]);
	const auto loc1 = manager->residenceNew(locationNames[1]);
	const auto loc2 = manager->airportNew(locationNames[2]);

	const auto seg0 = manager->flightNew(segmentNames[0]);
	const auto seg1 = manager->roadNew(segmentNames[1]);
	const auto seg2 = manager->roadNew(segmentNames[2]);

	seg0->sourceIs(loc0);
	seg0->destinationIs(loc2);

	seg1->sourceIs(loc0);
	seg1->destinationIs(loc1);

	seg2->sourceIs(loc1);
	seg2->destinationIs(loc2);

	ASSERT_TRUE(loc0->sourceSegment(0) == seg0);
	ASSERT_TRUE(loc2->destinationSegment(0) == seg0);

	manager->segmentDel(segmentNames[0]);

	ASSERT_EQ(loc0->sourceSegmentCount(), 1);
	ASSERT_EQ(loc0->destinationSegmentCount(), 0);

	ASSERT_EQ(loc1->sourceSegmentCount(), 1);
	ASSERT_EQ(loc1->destinationSegmentCount(), 1);

	ASSERT_EQ(loc2->sourceSegmentCount(), 0);
	ASSERT_EQ(loc2->destinationSegmentCount(), 1);

	ASSERT_TRUE(loc0->sourceSegment(0) == seg1);
	ASSERT_TRUE(loc0->sourceSegment(1) == null);
	ASSERT_TRUE(loc0->destinationSegment(0) == null);

	ASSERT_TRUE(loc1->sourceSegment(0) == seg2);
	ASSERT_TRUE(loc1->sourceSegment(1) == null);
	ASSERT_TRUE(loc1->destinationSegment(0) == seg1);
	ASSERT_TRUE(loc1->destinationSegment(1) == null);

	ASSERT_TRUE(loc2->sourceSegment(0) == null);
	ASSERT_TRUE(loc2->destinationSegment(0) == seg2);
	ASSERT_TRUE(loc2->destinationSegment(1) == null);
}


TEST(Stats, onLocation) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto stats = manager->stats();
	vector<string> names { "location1", "location2", "location3", "location4", "location5" };

	ASSERT_EQ(stats->locationCount(), 0);
	ASSERT_EQ(stats->airportCount(), 0);
	ASSERT_EQ(stats->residenceCount(), 0);

	manager->airportNew(names[0]);
	manager->residenceNew(names[1]);
	manager->residenceNew(names[2]);
	manager->airportNew(names[3]);
	manager->residenceNew(names[4]);

	ASSERT_EQ(stats->locationCount(), 5);
	ASSERT_EQ(stats->airportCount(), 2);
	ASSERT_EQ(stats->residenceCount(), 3);

	manager->locationDel(names[2]);

	ASSERT_EQ(stats->locationCount(), 4);
	ASSERT_EQ(stats->airportCount(), 2);
	ASSERT_EQ(stats->residenceCount(), 2);

	manager->locationDel(names[0]);
	manager->locationDel(names[3]);

	ASSERT_EQ(stats->locationCount(), 2);
	ASSERT_EQ(stats->airportCount(), 0);
	ASSERT_EQ(stats->residenceCount(), 2);

	manager->locationDel(names[0]);
	manager->locationDel(names[3]);

	ASSERT_EQ(stats->locationCount(), 2);
	ASSERT_EQ(stats->airportCount(), 0);
	ASSERT_EQ(stats->residenceCount(), 2);
}

TEST(Stats, onSegment) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto stats = manager->stats();
	vector<string> names { "segment1", "segment2", "segment3", "segment4", "segment5" };

	ASSERT_EQ(stats->segmentCount(), 0);
	ASSERT_EQ(stats->flightCount(), 0);
	ASSERT_EQ(stats->roadCount(), 0);

	manager->roadNew(names[0]);
	manager->flightNew(names[1]);
	manager->roadNew(names[2]);
	manager->flightNew(names[3]);
	manager->roadNew(names[4]);

	ASSERT_EQ(stats->segmentCount(), 5);
	ASSERT_EQ(stats->flightCount(), 2);
	ASSERT_EQ(stats->roadCount(), 3);

	manager->segmentDel(names[2]);
	manager->segmentDel(names[1]);
	manager->segmentDel(names[4]);

	ASSERT_EQ(stats->segmentCount(), 2);
	ASSERT_EQ(stats->flightCount(), 1);
	ASSERT_EQ(stats->roadCount(), 1);

	manager->segmentDel(names[1]);
	manager->segmentDel(names[4]);

	ASSERT_EQ(stats->segmentCount(), 2);
	ASSERT_EQ(stats->flightCount(), 1);
	ASSERT_EQ(stats->roadCount(), 1);
}

TEST(Stats, onVehicle) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto stats = manager->stats();
	vector<string> names { "vehicle1", "vehicle2", "vehicle3", "vehicle4", "vehicle5" };

	ASSERT_EQ(stats->vehicleCount(), 0);
	ASSERT_EQ(stats->airplaneCount(), 0);
	ASSERT_EQ(stats->carCount(), 0);

	manager->carNew(names[0]);
	manager->airplaneNew(names[1]);
	manager->carNew(names[2]);
	manager->airplaneNew(names[3]);
	manager->carNew(names[4]);

	ASSERT_EQ(stats->vehicleCount(), 5);
	ASSERT_EQ(stats->airplaneCount(), 2);
	ASSERT_EQ(stats->carCount(), 3);

	manager->vehicleDel(names[1]);
	manager->vehicleDel(names[2]);
	manager->vehicleDel(names[4]);

	ASSERT_EQ(stats->vehicleCount(), 2);
	ASSERT_EQ(stats->airplaneCount(), 1);
	ASSERT_EQ(stats->carCount(), 1);

	manager->vehicleDel(names[2]);
	manager->vehicleDel(names[4]);

	ASSERT_EQ(stats->vehicleCount(), 2);
	ASSERT_EQ(stats->airplaneCount(), 1);
	ASSERT_EQ(stats->carCount(), 1);
}

TEST(LocationAndSegment, SegmentCounts) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	vector<string> locationNames { "location1", "location2", "location3" };
	vector<string> segmentNames { "segment1", "segment2", "segment3" };

	const auto loc0 = manager->airportNew(locationNames[0]);
	const auto loc1 = manager->residenceNew(locationNames[1]);
	const auto loc2 = manager->airportNew(locationNames[2]);

	const auto seg0 = manager->flightNew(segmentNames[0]);
	const auto seg1 = manager->roadNew(segmentNames[1]);
	const auto seg2 = manager->roadNew(segmentNames[2]);

	seg0->sourceIs(loc0);
	seg0->destinationIs(loc2);

	seg1->sourceIs(loc0);
	seg1->destinationIs(loc1);

	seg2->sourceIs(loc1);
	seg2->destinationIs(loc2);

	ASSERT_EQ(loc0->sourceSegmentCount(), 2);
	ASSERT_EQ(loc0->destinationSegmentCount(), 0);

	ASSERT_EQ(loc1->sourceSegmentCount(), 1);
	ASSERT_EQ(loc1->destinationSegmentCount(), 1);

	ASSERT_EQ(loc2->sourceSegmentCount(), 0);
	ASSERT_EQ(loc2->destinationSegmentCount(), 2);
}

TEST(LocationAndSegment, segmentSourceAndDestChanges) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->residenceNew("residence-1");
	const auto loc2 = manager->residenceNew("residence-2");
	const auto loc3 = manager->residenceNew("residence-3");
	const auto loc4 = manager->airportNew("airport-1");
	const auto loc5 = manager->airportNew("airport-2");

	const auto seg1 = manager->roadNew("segment-1");
	seg1->sourceIs(loc1);
	seg1->destinationIs(loc2);

	ASSERT_EQ(loc1->sourceSegmentCount(), 1);
	ASSERT_EQ(loc1->destinationSegmentCount(), 0);

	ASSERT_EQ(loc2->sourceSegmentCount(), 0);
	ASSERT_EQ(loc2->destinationSegmentCount(), 1);

	ASSERT_EQ(loc1->sourceSegment(0), seg1);
	ASSERT_EQ(loc2->destinationSegment(0), seg1);

	seg1->sourceIs(loc3);
	seg1->destinationIs(loc4);

	ASSERT_EQ(loc1->sourceSegmentCount(), 0);
	ASSERT_EQ(loc1->destinationSegmentCount(), 0);

	ASSERT_EQ(loc2->sourceSegmentCount(), 0);
	ASSERT_EQ(loc2->destinationSegmentCount(), 0);

	ASSERT_EQ(loc3->sourceSegmentCount(), 1);
	ASSERT_EQ(loc3->destinationSegmentCount(), 0);

	ASSERT_EQ(loc4->sourceSegmentCount(), 0);
	ASSERT_EQ(loc4->destinationSegmentCount(), 1);

	ASSERT_EQ(loc3->sourceSegment(0),seg1);
	ASSERT_EQ(loc4->destinationSegment(0), seg1);
}

TEST(LocationAndSegment, multipleSegments) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->airportNew("location-1");
	const auto loc2 = manager->airportNew("location-2");
	const auto loc3 = manager->residenceNew("location-3");

	const auto seg1 = manager->roadNew("segment-1");
	seg1->sourceIs(loc1);
	seg1->destinationIs(loc2);

	const auto seg2 = manager->roadNew("segment-2");
	seg2->sourceIs(loc1);
	seg2->destinationIs(loc3);

	const auto seg3 = manager->roadNew("segment-3");
	seg3->sourceIs(loc2);
	seg3->destinationIs(loc3);

	ASSERT_EQ(loc1->sourceSegmentCount(), 2);
	ASSERT_EQ(loc1->destinationSegmentCount(), 0);

	ASSERT_EQ(loc2->sourceSegmentCount(), 1);
	ASSERT_EQ(loc2->destinationSegmentCount(), 1);

	ASSERT_EQ(loc3->sourceSegmentCount(), 0);
	ASSERT_EQ(loc3->destinationSegmentCount(), 2);
}

TEST(LocationAndSegment, locationDel) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->residenceNew("location-1");
	const auto loc2 = manager->airportNew("location-2");
	const auto loc3 = manager->residenceNew("location-3");

	const auto seg1 = manager->roadNew("segment-1");
	seg1->sourceIs(loc2);
	seg1->destinationIs(loc3);

	const auto seg2 = manager->roadNew("segment-2");
	seg2->sourceIs(loc1);
	seg2->destinationIs(loc2);

	ASSERT_EQ(seg1->source(), loc2);
	ASSERT_EQ(seg1->destination(), loc3);

	ASSERT_EQ(seg2->source(), loc1);
	ASSERT_EQ(seg2->destination(), loc2);

	manager->locationDel(loc3->name());

	ASSERT_EQ(seg1->source(), loc2);
	ASSERT_EQ(seg1->destination(), null);

	manager->locationDel(loc2->name());	

	ASSERT_EQ(seg1->source(), null);
	ASSERT_EQ(seg1->destination(), null);

	ASSERT_EQ(seg2->source(), loc1);
	ASSERT_EQ(seg2->destination(), null);
}

string getSourceSegmentList(const Ptr<Location> loc) {
	string res = "";
	for (auto it = loc->sourceSegmentIter(); it != loc->sourceSegmentIterEnd(); it++) {
		auto seg = *it;
		res += seg->name() + ", ";
	}

	return res;
}

string getDestinationSegmentList(const Ptr<Location> loc) {
	string res = "";
	for (auto it = loc->destinationSegmentIter(); it != loc->destinationSegmentIterEnd(); it++) {
		auto seg = *it;
		res += seg->name() + ", ";
	}

	return res;
}

TEST(Location, SegmentColls) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->residenceNew("location-1");
	const auto loc2 = manager->residenceNew("location-2");
	const auto loc3 = manager->residenceNew("location-3");
	const auto loc4 = manager->airportNew("location-4");
	const auto loc5 = manager->airportNew("location-5");

	const auto seg1 = createRoadSegment(manager, "road-1", loc1, loc2, 30);
	const auto seg2 = createRoadSegment(manager, "road-2", loc1, loc3, 20);
	const auto seg3 = createRoadSegment(manager, "road-3", loc2, loc3, 10);
	const auto seg4 = createRoadSegment(manager, "flight-1", loc4, loc5, 80);
	const auto seg5 = createRoadSegment(manager, "flight-2", loc4, loc5, 50);

	/* source segment counts */
	ASSERT_EQ(loc1->sourceSegmentCount(), 2);
	ASSERT_EQ(loc2->sourceSegmentCount(), 1);
	ASSERT_EQ(loc3->sourceSegmentCount(), 0);
	ASSERT_EQ(loc4->sourceSegmentCount(), 2);
	ASSERT_EQ(loc5->sourceSegmentCount(), 0);

	/* destination segment counts */
	ASSERT_EQ(loc1->destinationSegmentCount(), 0);
	ASSERT_EQ(loc2->destinationSegmentCount(), 1);
	ASSERT_EQ(loc3->destinationSegmentCount(), 2);
	ASSERT_EQ(loc4->destinationSegmentCount(), 0);
	ASSERT_EQ(loc5->destinationSegmentCount(), 2);

	/* source segment accessor */
	ASSERT_EQ(loc1->sourceSegment(0), seg1);
	ASSERT_EQ(loc1->sourceSegment(1), seg2);
	ASSERT_EQ(loc1->sourceSegment(2), null);

	/* destination segment accessor */
	ASSERT_EQ(loc5->destinationSegment(0), seg4);
	ASSERT_EQ(loc5->destinationSegment(1), seg5);
	ASSERT_EQ(loc5->destinationSegment(2), null);

	/* source segment iter */
	ASSERT_EQ(getSourceSegmentList(loc1), "road-1, road-2, ");
	ASSERT_EQ(getSourceSegmentList(loc2), "road-3, ");
	ASSERT_EQ(getSourceSegmentList(loc3), "");
	ASSERT_EQ(getSourceSegmentList(loc4), "flight-1, flight-2, ");
	ASSERT_EQ(getSourceSegmentList(loc5), "");

	/* destination segment iter */
	ASSERT_EQ(getDestinationSegmentList(loc1), "");
	ASSERT_EQ(getDestinationSegmentList(loc2), "road-1, ");
	ASSERT_EQ(getDestinationSegmentList(loc3), "road-2, road-3, ");
	ASSERT_EQ(getDestinationSegmentList(loc4), "");
	ASSERT_EQ(getDestinationSegmentList(loc5), "flight-1, flight-2, ");

	/* source segment iter delAll */
	loc1->sourceSegmentDelAll();
	ASSERT_EQ(getSourceSegmentList(loc1), "");
	ASSERT_EQ(loc1->sourceSegmentCount(), 0);

	/* destination segment iter delAll */
	loc5->destinationSegmentDelAll();
	ASSERT_EQ(getDestinationSegmentList(loc5), "");
	ASSERT_EQ(loc5->destinationSegmentCount(), 0);

	/* source segment del */
	loc2->sourceSegmentDel(seg3);
	ASSERT_EQ(getSourceSegmentList(loc2), "");

	/* destination segment del */
	loc3->destinationSegmentDel(seg3);
	ASSERT_EQ(getDestinationSegmentList(loc3), "road-2, ");
}

TEST(Location, Source_SegmentId) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->residenceNew("location-1");
	const auto loc2 = manager->residenceNew("location-2");

	const auto seg1 = manager->roadNew("segment-1");
	seg1->sourceIs(loc1);
	seg1->destinationIs(loc2);

	ASSERT_TRUE(loc1->sourceSegment(0) == seg1);
	ASSERT_TRUE(loc1->sourceSegment(1) == null);

	try {
		loc1->sourceSegment(-1);
	} catch (fwk::RangeException& e) {
		ASSERT_TRUE(true);
		return;
	}

	ASSERT_TRUE(false);
}

TEST(Location, Destination_SegmentId) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto loc1 = manager->residenceNew("location-1");
	const auto loc2 = manager->residenceNew("location-2");

	const auto seg1 = manager->roadNew("segment-1");
	seg1->sourceIs(loc1);
	seg1->destinationIs(loc2);

	ASSERT_TRUE(loc2->destinationSegment(0) == seg1);
	ASSERT_TRUE(loc2->destinationSegment(1) == null);

	try {
		loc1->destinationSegment(-1);
	} catch (fwk::RangeException& e) {
		ASSERT_TRUE(true);
		return;
	}

	ASSERT_TRUE(false);
}

TEST(HelperMethods, isResidence) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	ASSERT_FALSE(isResidence(null));
	ASSERT_TRUE(isResidence(manager->residenceNew("abc")));
	ASSERT_FALSE(isResidence(manager->airportNew("wer")));
}

TEST(HelperMethods, isAirport) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	ASSERT_FALSE(isAirport(null));
	ASSERT_TRUE(isAirport(manager->airportNew("abc")));
	ASSERT_FALSE(isAirport(manager->residenceNew("wer")));
}

TEST(HelperMethods, isFlight) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	ASSERT_FALSE(isFlight(null));
	ASSERT_TRUE(isFlight(manager->flightNew("abc")));
	ASSERT_FALSE(isFlight(manager->roadNew("wer")));
}

TEST(HelperMethods, isRoad) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	ASSERT_FALSE(isRoad(null));
	ASSERT_TRUE(isRoad(manager->roadNew("abc")));
	ASSERT_FALSE(isRoad(manager->flightNew("wer")));
}

TEST(HelperMethods, isAirplane) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	ASSERT_FALSE(isAirplane(null));
	ASSERT_FALSE(isAirplane(manager->carNew("abc")));
	ASSERT_TRUE(isAirplane(manager->airplaneNew("wer")));
}

TEST(HelperMethods, isCar) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	ASSERT_FALSE(isCar(null));
	ASSERT_TRUE(isCar(manager->carNew("abc")));
	ASSERT_FALSE(isCar(manager->airplaneNew("wer")));
}

TEST(Miles, addition) {
	Miles m1(3.4);
	Miles m2(5.2);
	Miles m3 = m1 + m2;

	ASSERT_TRUE(m3 == Miles(8.6));
}

TEST(MilesPerHour, addition) {
	MilesPerHour m1(3);
	MilesPerHour m2(7);
	MilesPerHour m3 = m1 + m2;

	ASSERT_EQ(m3.value(), 10);
}

TEST(SegmentId, addition) {
	SegmentId m1(3);
	SegmentId m2(7);
	SegmentId m3 = m1 + m2;

	ASSERT_EQ(m3.value(), 10);
}

TEST(PassengerCount, addition) {
	PassengerCount m1(3);
	PassengerCount m2(7);
	PassengerCount m3 = m1 + m2;

	ASSERT_EQ(m3.value(), 10);
}

TEST(DollarsPerMile, addition) {
	DollarsPerMile m1(3.9);
	DollarsPerMile m2(7.4);
	DollarsPerMile m3 = m1 + m2;

	ASSERT_TRUE(m3 == DollarsPerMile(11.3));
}

TEST(Hours, addition) {
	Hours d1(3.9);
	Hours d2(7.4);
	Hours d3 = d1 + d2;

	ASSERT_TRUE(d3 == Hours(11.3));
}

TEST(Dollars, addition) {
	Dollars d1(3.9);
	Dollars d2(7.4);
	Dollars d3 = d1 + d2;

	ASSERT_TRUE(d3 == Dollars(11.3));
}

TEST(ValueTypeInteractions, dist_by_speed) {
	Miles m(25);
	MilesPerHour s(4);
	auto t = m / s;

	ASSERT_TRUE(t == Hours(6.25));
}

TEST(ValueTypeInteractions, speed_mul_time) {
	MilesPerHour s(4);
	Hours t(2.4);
	
	Miles d1 = s * t;
	ASSERT_EQ(9.6, d1.value());

	// To ensure that its commutative
	Miles d2 = t * s;
	ASSERT_EQ(9.6, d2.value());	
}

TEST(ValueTypeInteractions, dist_mul_cost) {
	Miles d(4);
	DollarsPerMile c(3.2);

	Dollars c1 = d * c;
	ASSERT_EQ(12.8, c1.value());

	Dollars c2 = c * d;
	ASSERT_EQ(12.8, c2.value());	
}

TEST(Probability, basic) {
	try {
		Probability p(-2.2);
		ASSERT_TRUE(false);
	} catch (const fwk::RangeException& e) {
		ASSERT_TRUE(true);
	}

	try {
		Probability p(1.2);
		ASSERT_TRUE(false);
	} catch (const fwk::RangeException& e) {
		ASSERT_TRUE(true);
	}

	Probability p1(0.47);
	Probability p2(0.47);

	ASSERT_TRUE(p1 == p2);

	Probability p3 = p1 + p2;
	ASSERT_EQ(0.94, p3.value());
}

TEST(TravelInstanceManager, Stats) {
	const auto manager = TravelInstanceManager::instanceManager();

	const auto res1 = manager->instanceNew("residence_1", "Residence");
	const auto flight1 = manager->instanceNew("flight_1", "Flight");
	
	const auto air1 = manager->instanceNew("airport_1", "Airport");
	const auto air2 = manager->instanceNew("airport_2", "Airport");

	const auto stats = manager->instanceNew("stats_1", "Stats");

	const auto road1 = manager->instanceNew("road_1", "Road");
	const auto flight2 = manager->instanceNew("flight_2", "Flight");
	const auto road2 = manager->instanceNew("road_2", "Road");
	const auto road3 = manager->instanceNew("road_3", "Road");

	ASSERT_EQ(stats->attribute("Residence"), "1");
	ASSERT_EQ(stats->attribute("Airport"), "2");
	ASSERT_EQ(stats->attribute("Flight"), "2");
	ASSERT_EQ(stats->attribute("Road"), "3");

	const auto stats1 = manager->instanceNew("stats_2", "Stats");
	
	ASSERT_EQ(stats1->attribute("Residence"), "1");
	ASSERT_EQ(stats1->attribute("Airport"), "2");
	ASSERT_EQ(stats1->attribute("Flight"), "2");
	ASSERT_EQ(stats1->attribute("Road"), "3");
}

void setVehicleAttributes(const Ptr<Instance>& instance,
						  const string& cost,
						  const string& capacity,
						  const string& speed) {
	instance->attributeIs("cost", cost);
	instance->attributeIs("capacity", capacity);
	instance->attributeIs("speed", speed);
}

void verifyVehicleAttrValues(const Ptr<Instance>& instance,
						  	 const string& cost,
						  	 const string& capacity,
						  	 const string& speed) {
	ASSERT_EQ(instance->attribute("cost"), cost);
	ASSERT_EQ(instance->attribute("capacity"), capacity);
	ASSERT_EQ(instance->attribute("speed"), speed);
}

TEST(TravelInstanceManager, VehicleInstance) {
	const auto manager = TravelInstanceManager::instanceManager();
	const auto plane = manager->instanceNew("plane", "Airplane");

	verifyVehicleAttrValues(plane, "0.000000", "0", "0");

	setVehicleAttributes(plane, "3.45", "7", "16");
	verifyVehicleAttrValues(plane, "3.450000", "7", "16");

	setVehicleAttributes(plane, "-53.8", "-91", "-6");
	verifyVehicleAttrValues(plane, "3.450000", "7", "16");

	setVehicleAttributes(plane, "5.6", "29", "21");
	verifyVehicleAttrValues(plane, "5.600000", "29", "21");

	ASSERT_EQ(plane->attribute("qtwr"), "");
	plane->attributeIs("opow", "");

	verifyVehicleAttrValues(plane, "5.600000", "29", "21");

	const auto car = manager->instanceNew("car", "Car");

	verifyVehicleAttrValues(car, "0.000000", "0", "0");

	setVehicleAttributes(car, "49.352", "58", "124");
	verifyVehicleAttrValues(car, "49.352000", "58", "124");

	setVehicleAttributes(car, "-53.8", "-91", "-6");
	verifyVehicleAttrValues(car, "49.352000", "58", "124");
}

void setSegmentAttributes(const Ptr<Instance>& instance,
						  const string& source,
						  const string& destination,
						  const string& length) {
	instance->attributeIs("source", source);
	instance->attributeIs("destination", destination);
	instance->attributeIs("length", length);
}

void verifySegmentAttrValues(const Ptr<Instance>& instance,
						  	 const string& source,
						  	 const string& destination,
						  	 const string& length) {
	ASSERT_EQ(instance->attribute("source"), source);
	ASSERT_EQ(instance->attribute("destination"), destination);
	ASSERT_EQ(instance->attribute("length"), length);
}

TEST(TravelInstanceManager, SegmentInstance) {
	const auto manager = TravelInstanceManager::instanceManager();
	const auto flight = manager->instanceNew("flight", "Flight");
	const auto res1 = manager->instanceNew("residence-1", "Residence");
	const auto air1 = manager->instanceNew("airport-1", "Airport");
	const auto air2 = manager->instanceNew("airport-2", "Airport");

	verifySegmentAttrValues(flight, "", "", "0.000000");

	setSegmentAttributes(flight, "residence-1", "airport-1", "34");
	verifySegmentAttrValues(flight, "", "airport-1", "34.000000");

	setSegmentAttributes(flight, "airport-1", "airport-2", "4");
	verifySegmentAttrValues(flight, "airport-1", "airport-2", "4.000000");

	setSegmentAttributes(flight, "airport-2", "residence-1", "-23");
	verifySegmentAttrValues(flight, "airport-2", "airport-2", "4.000000");

	const auto road = manager->instanceNew("road", "Road");

	verifySegmentAttrValues(road, "", "", "0.000000");	

	setSegmentAttributes(road, "residence-1", "airport-1", "34");
	verifySegmentAttrValues(road, "residence-1", "airport-1", "34.000000");

	setSegmentAttributes(road, "airport-1", "airport-2", "4");
	verifySegmentAttrValues(road, "airport-1", "airport-2", "4.000000");

	setSegmentAttributes(road, "airport-2", "residence-1", "-23");
	verifySegmentAttrValues(road, "airport-2", "residence-1", "4.000000");

	ASSERT_EQ(res1->attribute("segment1"), "");
	ASSERT_EQ(air1->attribute("segment1"), "");
	ASSERT_EQ(air2->attribute("segment1"), "flight");
	ASSERT_EQ(air2->attribute("segment2"), "road");
	ASSERT_EQ(air2->attribute("segment3"), "");
}

TEST(VehicleManager, availableVehicleCount) {
	const auto manager = TravelNetworkManager::instanceNew("mgr");
	auto sim = TravelSim::instanceNew(manager);
	const auto vehicleManager = sim->vehicleManager();

	const auto car1 = manager->carNew("car-1");
	const auto car2 = manager->carNew("car-2");
	const auto car3 = manager->carNew("car-3");
	const auto car4 = manager->carNew("car-4");
	const auto car5 = manager->carNew("car-5");
	const auto flight1 = manager->airplaneNew("airplane-1");

	ASSERT_EQ(5, vehicleManager->availableVehicleCount());

	manager->vehicleDel("car-3");
	manager->vehicleDel("car-4");
	ASSERT_EQ(3, vehicleManager->availableVehicleCount());	

	car1->statusIs(Vehicle::available);
	ASSERT_EQ(3, vehicleManager->availableVehicleCount());	

	car1->statusIs(Vehicle::assignedForTrip);
	ASSERT_EQ(2, vehicleManager->availableVehicleCount());	

	car5->statusIs(Vehicle::dispatchedForPassengerPickup);
	ASSERT_EQ(1, vehicleManager->availableVehicleCount());	

	car1->statusIs(Vehicle::available);
	ASSERT_EQ(2, vehicleManager->availableVehicleCount());	

	car5->statusIs(Vehicle::transportingPassengers);
	ASSERT_EQ(2, vehicleManager->availableVehicleCount());	

	// car3 has been deleted. It should not affect the count
	car3->statusIs(Vehicle::dispatchedForPassengerPickup);
	ASSERT_EQ(2, vehicleManager->availableVehicleCount());

	// car3 has been deleted. It should not affect the count
	car3->statusIs(Vehicle::available);
	ASSERT_EQ(2, vehicleManager->availableVehicleCount());

	car5->statusIs(Vehicle::available);
	ASSERT_EQ(3, vehicleManager->availableVehicleCount());

	// deleting an airplane should not affect the availableVehicleCount since the simulation currently supports only cars
	manager->vehicleDel("airplane-1");
	ASSERT_EQ(3, vehicleManager->availableVehicleCount());

	sim->activitiesDel();
}

Ptr<Car> createCar(const Ptr<TravelNetworkManager>& travelNetworkManager, 
			   const Ptr<Location>& loc, const string& carName) {
	auto c = travelNetworkManager->carNew(carName);
	c->locationIs(loc);
	c->speedIs(5);

	return c;
}

TEST(VehicleManager, nearestVehicle) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto sim = TravelSim::instanceNew(manager);

	const auto loc1 = manager->residenceNew("loc1");
	const auto loc2 = manager->residenceNew("loc2");
	const auto loc3 = manager->residenceNew("loc3");
	const auto loc4 = manager->residenceNew("loc4");
	const auto loc5 = manager->residenceNew("loc5");
	const auto loc6 = manager->residenceNew("loc6");

	const auto seg12 = createRoadSegment(manager, "road-1", loc1, loc2, 15);
	const auto seg13 = createRoadSegment(manager, "road-2", loc1, loc3, 5); 
	const auto seg14 = createRoadSegment(manager, "road-3", loc1, loc4, 20);
	const auto seg15 = createRoadSegment(manager, "road-4", loc1, loc5, 100);

	const auto seg24 = createRoadSegment(manager, "road-5", loc2, loc4, 30);

	const auto seg31 = createRoadSegment(manager, "road-6", loc3, loc1, 2);
	const auto seg34 = createRoadSegment(manager, "road-7", loc3, loc4, 10);
	const auto seg35 = createRoadSegment(manager, "road-8", loc3, loc5, 60);
	const auto seg36 = createRoadSegment(manager, "road-9", loc3, loc6, 25);

	const auto seg41 = createRoadSegment(manager, "road-10", loc4, loc1, 35);
	const auto seg45 = createRoadSegment(manager, "road-11", loc4, loc5, 120);
	const auto seg46 = createRoadSegment(manager, "road-12", loc4, loc6, 3);

	const auto seg65 = createRoadSegment(manager, "road-13", loc6, loc5, 10);

	const auto vehicleManager = sim->vehicleManager();
	auto v = vehicleManager->nearestVehicle(loc1);
	ASSERT_EQ(v, null);

	const auto car11 = createCar(manager, loc1, "car-11");
	const auto car12 = createCar(manager, loc1, "car-12");
	const auto car2 = createCar(manager, loc2, "car-2");
	const auto car3 = createCar(manager, loc3, "car-3");
	const auto car4 = createCar(manager, loc4, "car-4");

	v = vehicleManager->nearestVehicle(loc1);
	ASSERT_EQ(v, car11);

	car11->locationIs(loc6);
	v = vehicleManager->nearestVehicle(loc1);
	ASSERT_EQ(v, car12);

	car12->statusIs(Vehicle::assignedForTrip);
	v = vehicleManager->nearestVehicle(loc1);
	ASSERT_EQ(v, car3);

	car3->statusIs(Vehicle::dispatchedForPassengerPickup);
	v = vehicleManager->nearestVehicle(loc1);
	ASSERT_EQ(v, car4);

	car4->statusIs(Vehicle::transportingPassengers);
	v = vehicleManager->nearestVehicle(loc1);
	ASSERT_EQ(v, car2);

	car2->locationIs(loc5);
	v = vehicleManager->nearestVehicle(loc2);
	ASSERT_EQ(v, null);
	
	v = vehicleManager->nearestVehicle(loc6);
	ASSERT_EQ(v, car11);
	manager->vehicleDel("car-11");
	v = vehicleManager->nearestVehicle(loc6);
	ASSERT_EQ(v, null);

	ASSERT_EQ(vehicleManager->nearestVehicle(loc5), car2);
	car2->speedIs(0);
	ASSERT_EQ(vehicleManager->nearestVehicle(loc5), null);
}

TEST(TravelNetworkManager, trips) {
	const auto manager = TravelNetworkManager::instanceNew("manager-1");
	const auto stats = manager->stats();

	const auto trip1 = manager->tripNew("trip-1");
	const auto trip2 = manager->tripNew("trip-2");
	const auto trip3 = manager->tripNew("trip-3");
	const auto trip4 = manager->tripNew("trip-4");
	const auto trip5 = manager->tripNew("trip-5");

	ASSERT_EQ(Trip::requested, trip1->status());
	ASSERT_EQ(Trip::requested, trip4->status());

	ASSERT_EQ(5, stats->tripCount());

	manager->tripDel("trip-3");
	manager->tripDel("trip-1");

	ASSERT_EQ(3, stats->tripCount());

	manager->tripNew("trip-3");

	ASSERT_EQ(4, stats->tripCount());

	manager->tripDel("trip-1");

	ASSERT_EQ(4, stats->tripCount());

	ASSERT_EQ(manager->trip("trip-1"), null);
	ASSERT_EQ(trip2, manager->trip("trip-2"));

	ASSERT_EQ(0 ,stats->tripCompletedCount());
	trip4->statusIs(Trip::completed);
	trip1->statusIs(Trip::completed);
	trip3->statusIs(Trip::completed);
	trip5->statusIs(Trip::completed);
	ASSERT_EQ(2 ,stats->tripCompletedCount());

	trip2->statusIs(Trip::vehicleDispatched);
	ASSERT_EQ(2 ,stats->tripCompletedCount());

	trip2->statusIs(Trip::transportingPassenger);
	ASSERT_EQ(2 ,stats->tripCompletedCount());
}

