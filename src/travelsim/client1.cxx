#include "TravelInstanceManager.h"

#include <ostream>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

Ptr<Instance> segmentNew(
    const Ptr<InstanceManager>& manager, const string& name, const string& spec, 
    const string& source, const string& dest, const double length
) {
    const auto seg = manager->instanceNew(name, spec);
    seg->attributeIs("source", source);
    seg->attributeIs("destination", dest);
    seg->attributeIs("length", std::to_string(length));
    return seg;
}

int main(const int argc, const char* const argv[]) {
    const auto manager = TravelInstanceManager::instanceManager();

    const auto plane = manager->instanceNew("plane", "Airplane");
    plane->attributeIs("speed", "500");
    plane->attributeIs("capacity", "200");
    plane->attributeIs("cost", "40");

    const auto car = manager->instanceNew("car", "Car");
    car->attributeIs("speed", "70");
    car->attributeIs("capacity", "5");
    car->attributeIs("cost", "0.75");

    manager->instanceNew("stanford", "Residence");  
    manager->instanceNew("menlopark", "Residence");  

    manager->instanceNew("sfo", "Airport");  
    manager->instanceNew("lax", "Airport");  

    segmentNew(manager, "carSeg1", "Road", "stanford", "sfo", 20);
    segmentNew(manager, "carSeg2", "Road", "sfo", "stanford", 20);
    segmentNew(manager, "carSeg3", "Road", "menlopark", "stanford", 20);
    segmentNew(manager, "carSeg4", "Road", "sfo", "menlopark", 20);
    segmentNew(manager, "carSeg5", "Road", "stanford", "menlopark", 5);
    segmentNew(manager, "carSeg6", "Road", "menlopark", "stanford", 5);
    segmentNew(manager, "flightSeg1", "Flight", "sfo", "lax", 350);

    manager->instance("carSeg2")->attributeIs("length", "40");

    cout << "carSeg1.source: ";
    cout << manager->instance("carSeg1")->attribute("source") << endl;

    cout << "segment1 of sfo: ";
    cout << manager->instance("sfo")->attribute("segment1") << endl; 

    cout << "segment2 of sfo: ";
    cout << manager->instance("sfo")->attribute("segment2") << endl; 

    cout << "segment3 of sfo: ";
    cout << manager->instance("sfo")->attribute("segment3") << endl; 

    cout << "segment4 of sfo: ";
    cout << manager->instance("sfo")->attribute("segment4") << endl; 

    const auto stats = manager->instanceNew("myStats", "Stats");
    cout << "Stats:" << endl;
    cout << "# Residences: " << stats->attribute("Residence") << endl;
    cout << "# Airports: " << stats->attribute("Airport") << endl;
    cout << "# Road segments: " << stats->attribute("Road") << endl;
    cout << "# Flight segments: " << stats->attribute("Flight") << endl;

    const auto stats1 = manager->instanceNew("myStats1", "Stats");
    cout << "Stats:" << endl;
    cout << "# Residences: " << stats1->attribute("Residence") << endl;
    cout << "# Airports: " << stats1->attribute("Airport") << endl;
    cout << "# Road segments: " << stats1->attribute("Road") << endl;
    cout << "# Flight segments: " << stats1->attribute("Flight") << endl;

    const auto stats2 = manager->instance("myStats");
    cout << "Stats:" << endl;
    cout << "# Residences: " << stats2->attribute("Residence") << endl;
    cout << "# Airports: " << stats2->attribute("Airport") << endl;
    cout << "# Road segments: " << stats2->attribute("Road") << endl;
    cout << "# Flight segments: " << stats2->attribute("Flight") << endl;

    const auto stats3 = manager->instance("myStats1");
    cout << "Stats:" << endl;
    cout << "# Residences: " << stats3->attribute("Residence") << endl;
    cout << "# Airports: " << stats3->attribute("Airport") << endl;
    cout << "# Road segments: " << stats3->attribute("Road") << endl;
    cout << "# Flight segments: " << stats3->attribute("Flight") << endl;

    const auto conn = manager->instanceNew("myConn", "Conn");

    const auto query1 = "explore sfo distance 500";
    const auto query2 = "explore sfo distance 20";
    const auto query3 = "explore sfo distance 10";
    const auto query4 = "explore lax distance 1000";

    cout << "**** " << query1 << " ****" << endl;
    cout << conn->attribute(query1) << endl;

    cout << "**** " << query2 << " ****" << endl;
    cout << conn->attribute(query2) << endl;

    cout << "**** " << query3 << " ****" << endl;
    cout << conn->attribute(query3) << endl;

    cout << "**** " << query4 << " ****" << endl;
    cout << conn->attribute(query4) << endl;

    // =====================================================
    //  Instructions with errorneous inputs
    // =====================================================

    // Segment attributes
    auto seg = manager->instance("carSeg2");
    seg->attributeIs("length", "40ert");
    seg->attributeIs("length", "a40sd");
    seg->attributeIs("length", "jkl40");
    seg->attributeIs("source", "jfk");
    seg->attributeIs("destination", "wret");
    seg->attributeIs("length", "-23");
    seg->attributeIs("length", "23");
    seg->attributeIs("length", "-17.234");
    seg->attributeIs("length", "17.234");
    seg->attributeIs("jklse", "17.234");
    seg->attribute("qwerty");

    cout << "Attributes of 'carSeg2':" << endl;
    cout << "Source: " <<  seg->attribute("source") << ", "
         << "Dest: " << seg->attribute("destination") << ", "
         << "Length: " << seg->attribute("length") << endl;

    seg = manager->instance("flightSeg1");
    seg->attributeIs("source", "stanford");
    seg->attributeIs("destination", "menlopark");
    seg->attribute("source");
    seg->attribute("destination");

    cout << "Attributes of 'flightSeg1':" << endl;
    cout << "Source: " <<  seg->attribute("source") << ", "
         << "Dest: " << seg->attribute("destination") << ", "
         << "Length: " << seg->attribute("length") << endl;

    // Vehicle attributes
    car->attributeIs("speed", "-12.802");
    car->attributeIs("capacity", "-49");
    car->attributeIs("cost", "-57.19");
    car->attributeIs("upoi", "-57.19");
    car->attribute("qwerty");

    // Location attributes
    manager->instance("stanford")->attribute("uiuuo");
    manager->instance("stanford")->attribute("segment0");
    manager->instance("stanford")->attribute("segment-1");
    manager->instance("sfo")->attribute("wqer");

    // Conn attributes
    conn->attribute("bnml");

    // Stats attributes
    stats->attribute("plsdf");

    return 0;
}
