#include "TravelInstanceManager.h"

#include <ostream>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

void segmentNew(
    const Ptr<InstanceManager>& manager, const string& name, const string& spec, 
    const string& source, const string& dest, const double length
) {
    const auto seg = manager->instanceNew(name, spec);
    seg->attributeIs("source", source);
    seg->attributeIs("destination", dest);
    seg->attributeIs("length", std::to_string(length));
    return;
}

void twoWaySegmentNew(
	const Ptr<InstanceManager>& manager, const string& name1, const string& name2, const string& spec, 
    const string& loc1, const string& loc2, const double length
) {
    segmentNew(manager, name1, spec, loc1, loc2, length);
    segmentNew(manager, name2, spec, loc2, loc1, length);
    return;
}

void query(const Ptr<Instance>& conn, const string& queryStr) {
    cout << "**** " << queryStr << " ****" << endl;
    cout << conn->attribute(queryStr) << endl;
}

int main(const int argc, const char* const argv[]) {
    const auto manager = TravelInstanceManager::instanceManager();
    
    manager->instanceNew("R1", "Residence"); 
    manager->instanceNew("R2", "Residence"); 
    manager->instanceNew("R3", "Residence"); 

    manager->instanceNew("A1", "Airport");
    manager->instanceNew("A2", "Airport");
    manager->instanceNew("A3", "Airport");
    manager->instanceNew("A4", "Airport");

    twoWaySegmentNew(manager, "carSeg1-1", "carSeg1-2", "Road", "R1", "R2", 20); 
    twoWaySegmentNew(manager, "carSeg2-1", "carSeg2-2", "Road", "R2", "R3", 100);
    //twoWaySegmentNew(manager, "carSeg3-1", "carSeg3-2", "Road", "R3", "R1", 5);
    segmentNew(manager, "carSeg3-1", "Road", "R3", "R1", 10);
    segmentNew(manager, "carSeg3-2", "Road", "R1", "", 10);
    twoWaySegmentNew(manager, "carSeg4-1", "carSeg4-2", "Road", "R1", "A2", 40);

    segmentNew(manager, "carSeg5", "Road", "R1", "A1", 10);
    segmentNew(manager, "carSeg6", "Road", "A4", "R2", 50);

    twoWaySegmentNew(manager, "flightSeg1-1", "flightSeg1-2", "Flight", "A1", "A2", 10);
    twoWaySegmentNew(manager, "flightSeg2-1", "flightSeg2-2", "Flight", "A2", "A3", 50);
    twoWaySegmentNew(manager, "flightSeg3-1", "flightSeg3-2", "Flight", "A3", "A1", 1000);
    twoWaySegmentNew(manager, "flightSeg4-1", "flightSeg4-2", "Flight", "A1", "A4", 20);
	segmentNew(manager, "flightSeg5", "Flight", "A4", "A3", 30);

    // Error: Two airports can be connected only by Flight segments
    twoWaySegmentNew(manager, "flightSeg5-1", "flightSeg5-2", "Flight", "A1", "R1", 300);

    const auto conn = manager->instanceNew("myConn", "Conn");

    query(conn, "explore R1 distance 1000");
    query(conn, "explore R2 distance 1000");

    query(conn, "explore A1 distance 1000");
    query(conn, "explore A2 distance 1000");
    query(conn, "explore A3 distance 1000");
    query(conn, "explore A4 distance 1000");
}