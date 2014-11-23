#ifndef TRAVELSIM_TRAVELINSTANCEMANAGER_H
#define TRAVELSIM_TRAVELINSTANCEMANAGER_H

#include "CommonLib.h"
#include "InstanceManager.h"
#include "TravelNetworkManager.h"

#include <numeric>
#include <sstream>

using std::cerr;
using std::cout;
using std::endl;
using std::to_string;


//=======================================================
// Helper methods
//=======================================================

const string getEntityName(Ptr<NamedInterface> entity) {
    if (entity != null) {
        return entity->name();
    }

    return "";
} 

double tryConvertStrToNonNegDouble(const string& str) {
    stringstream ss(str);
    double value;
    if (ss >> value) {
        return value;
    }

    return -1; // Indicates erroneous output
}

//=======================================================

/**
 * InstanceManager implementation for the travel simulation.
 */
class TravelInstanceManager : public virtual InstanceManager {
public:

    /**
     * Return an instance manager for the travel simulation.
     */
    static Ptr<InstanceManager> instanceManager() {
        if (instanceManager_ == null) {
            instanceManager_ = new TravelInstanceManager();
            instanceManager_->initialize();
        }

        return instanceManager_;
    }

protected:

    typedef std::invalid_argument InvalidArgumentException;

    class LocationInstance : public Instance {
    public:

        _noinline
        string attribute(const string& name) {
            if (location_ == null) {
                logError(WARNING, "Internal Error in attribute() within LocationInstance. [location_ == null]");
                return "";
            }

            const auto i = segmentNumber(name) - 1;
            if (i >= 0) {
                const auto segment = location_->sourceSegment(i);
                if (segment != null) {
                    return getEntityName(segment);
                }
            }

            logError(WARNING, "Invalid attribute ('" + name + "') specified for Location. Skipping command.");
            return "";
        }

        _noinline
        void attributeIs(const string& name, const string& value) {
            logError(WARNING, "All attributes of Location are read-only.");
        }

    protected:

        Ptr<Location> location() const {
            return location_;
        }

        void locationIs(const Ptr<Location>& location) {
            location_ = location;
        }

        explicit LocationInstance(const string& name) :
            Instance(name)
        {
            location_ = null;
        }

    private:

        friend class TravelInstanceManager;

        static const string segmentStr;

        static const unsigned long segmentStrlen;

        Ptr<Location> location_;

        _noinline
        int segmentNumber(const string& name) {
            if (name.substr(0, segmentStrlen) == segmentStr) {
                auto tmp = name.substr(segmentStrlen, name.length() - segmentStrlen);
                try {
                    return std::stoi(tmp);
                } catch (std::invalid_argument& e) { }
            }

            return -1;
        }
    };


    class SegmentInstance : public Instance {
    public:

        _noinline
        string attribute(const string& name) {
            if (segment_ == null) {
                logError(WARNING, "Internal Error in attributeIs() within SegmentInstance. [segment_ == null]");
                return "";
            }

            if (name == "source") {
                return getEntityName(segment_->source());
            } else if (name == "destination") {
                return getEntityName(segment_->destination());
            } else if (name == "length") {
                return to_string(segment_->length().value());
            }

            logError(WARNING, "Invalid attribute ('" + name + "') specified for Segment. Skipping command.");
            return "";
        }

         _noinline
        void attributeIs(const string& name, const string& value) {
            if (segment_ != null) {
                try {
                    if (name == "source") {
                        const auto location = travelManager_->location(value);
                        if (location != null) {
                            segment_->sourceIs(location);
                            return;
                        }
                        
                        logError(WARNING, "Specified location '" + value + "' does not exist. Skipping command.");
                    } else if (name == "destination") {
                        const auto location = travelManager_->location(value);
                        if (location != null) {
                            segment_->destinationIs(location);
                            return;
                        }

                        logError(WARNING, "Specified location '" + value + "' does not exist. Skipping command.");
                    } else if (name == "length") {
                        auto length = Miles(tryConvertStrToNonNegDouble(value));
                        segment_->lengthIs(length);
                    } else {
                        logError(WARNING, "Invalid attribute ('" + name + "') specified for Segment. Skipping command.");
                    }
                } catch (const fwk::RangeException& e) {
                    logError(WARNING, e.what());
                }
            } else {
                logError(WARNING, "Internal Error in attributeIs() within SegmentInstance. [segment_ == null]");
            }
        }

    protected:

        Ptr<Segment> segment() const {
            return segment_;
        }

        void segmentIs(const Ptr<Segment>& segment) {
            segment_ = segment;
        }

        explicit SegmentInstance(const string& name) :
            Instance(name)
        {
            segment_ = null;
        }

    private:

        friend class TravelInstanceManager;

        Ptr<Segment> segment_;

    };


    class VehicleInstance : public Instance {
    public:

        _noinline
        string attribute(const string& name) {
            if (vehicle_ == null) {
                logError(WARNING, "Internal Error in attribute() within VehicleInstance. [vehicle_ == null]");
                return "";
            }

            if (name == "capacity") {
                return to_string(vehicle_->capacity().value());
            } else if (name == "speed") {
                return to_string(vehicle_->speed().value());
            } else if (name == "cost") {
                return to_string(vehicle_->cost().value());
            }

            logError(WARNING, "Invalid attribute ('" + name + "') specified for Vehicle. Skipping command.");
            return "";
        }

        _noinline
        void attributeIs(const string& name, const string& value) {
            if (vehicle_ != null) {
                try {
                   if (name == "capacity") {
                        //vehicle_->capacityIs(strToInt(value));
                        vehicle_->capacityIs(std::stoi(value));
                    } else if (name == "cost") {
                        vehicle_->costIs(tryConvertStrToNonNegDouble(value));
                    } else if (name == "speed") {
                        vehicle_->speedIs(tryConvertStrToNonNegDouble(value));
                    } else {
                        logError(WARNING, "Invalid attribute ('" + name + "') specified for Vehicle. Skipping command.");
                    } 
                } catch (const fwk::RangeException& e) {
                    logError(WARNING, e.what());
                }
            } else {
                logError(WARNING, "Internal Error in attributeIs() within VehicleInstance. [vehicle_ == null]");
            }
        }

    protected:

        Ptr<Vehicle> vehicle() const {
            return vehicle_;
        }

        void vehicleIs(const Ptr<Vehicle>& vehicle) {
            vehicle_ = vehicle;
        }

        explicit VehicleInstance(const string& name) :
            Instance(name)
        {
            vehicle_ = null;
        }

    private:

        friend class TravelInstanceManager;

        Ptr<Vehicle> vehicle_;

    };


    class ConnInstance : public Instance {
    public:

        _noinline
        string attribute(const string& name) {
            using std::stringstream;

            stringstream ss(name);

            string cmd;
            ss >> cmd;
            if (cmd != "explore") {
                logError(WARNING, "Invalid attribute ('" + name + "') specified for Conn. Skipping command.");
                return "";
            }

            string locName;
            ss >> locName;

            unsigned int maxLength;

            ss >> cmd >> maxLength;
            if (cmd != "distance") {
                logError(WARNING, "Invalid attribute ('" + name + "') specified for Conn. Skipping command.");
                return "";
            }

            auto location = travelManager_->location(locName);
            auto paths = conn_->paths(location, Miles(maxLength));
            string allPathsStr = "";
            for (auto p : paths) {
                allPathsStr += toString(p) + "\n";
            }

            return allPathsStr;
        }

        _noinline
        void attributeIs(const string& name, const string& value) {
            logError(WARNING, "All attributes of Conn are read-only.");
        }

    protected:

        Ptr<Conn> conn() const {
            return conn_;
        }

        void connIs(const Ptr<Conn>& conn) {
            conn_ = conn;
        }

        explicit ConnInstance(const string& name) :
            Instance(name)
        {
           conn_ = null; 
        }

    private:

        friend class TravelInstanceManager;

        string toString(const Ptr<Conn::Path>& p) const {
            string str = "";
            auto segments = p->segments();
            for (auto it = segments.cbegin(); it != segments.cend(); it++) {
                auto seg = *it;
                str += seg->source()->name() + "(" + seg->name() + ":" + to_string(seg->length().value()) + ") ";
            }

            auto lastSegment = p->segment(p->segmentCount() - 1);
            if (lastSegment != null) {
                str += lastSegment->destination()->name();
            }

            return str;
        }

        Ptr<Conn> conn_;

    };

    class StatsInstance : public Instance {
    public:

        _noinline
        string attribute(const string& name) {
            if (stats_ == null) {
                logError(WARNING, "Internal Error in attribute() within StatsInstance. [stats_ == null]");
                return "";
            }

            if (name == "Residence") {
                return to_string(stats_->residenceCount());
            } else if (name == "Airport") {
                return to_string(stats_->airportCount());
            } else if (name == "Flight") {
                return to_string(stats_->flightCount());
            } else if (name == "Road") {
                return to_string(stats_->roadCount());
            }

            logError(WARNING, "Invalid attribute ('" + name + "') specified for Stats. Skipping command.");
            return "";
        }

        _noinline
        void attributeIs(const string& name, const string& value) {
            logError(WARNING, "All attributes of Stats are read-only.");
        }

    protected:

        Ptr<TravelNetworkTracker> stats() const {
            return stats_;
        }

        void statsIs(const Ptr<TravelNetworkTracker>& stats) {
            stats_ = stats;
        }

        explicit StatsInstance(const string& name) :
            Instance(name)
        {
           stats_ = null; 
        }

    private:

        friend class TravelInstanceManager;

        Ptr<TravelNetworkTracker> stats_;

    };


    static Ptr<TravelInstanceManager> instanceManager_;


    /**
     * Initialize any state in the manager.
     */
    void initialize() {
        travelManager_ = TravelNetworkManager::instanceNew("manager");
    }

    /**
     * Returns a newly-created instance with the specified name.
     * The spec parameter indicates additional information about
     * the instance, such as its type.
     */
    Ptr<Instance> createInstance(const string& name, const string& spec) {
        if (spec == "Residence") {
            auto residence = travelManager_->residenceNew(name);
            return createLocationInstance(name, residence);
        } else if (spec == "Airport") {
            auto airport = travelManager_->airportNew(name);
            return createLocationInstance(name, airport);
        } else if (spec == "Flight") {
            auto flight = travelManager_->flightNew(name);
            return createSegmentInstance(name, flight);
        } else if (spec == "Road") {
            auto road = travelManager_->roadNew(name);
            return createSegmentInstance(name, road);
        }  else if (spec == "Airplane") {
            auto airplane = travelManager_->airplaneNew(name);
            return createVehicleInstance(name, airplane);
        }  else if (spec == "Car") {
            auto car = travelManager_->carNew(name);
            return createVehicleInstance(name, car);
        }  else if (spec == "Conn") {
            return createConnInstance(name);
        }  else if (spec == "Stats") {
            return createStatsInstance(name);
        } 

        logError(WARNING, "Unexpected spec for createInstance(): '" + spec + "'");

        return null;
    }

private:

    Ptr<LocationInstance> createLocationInstance(const string& name, const Ptr<Location> location) {
        if (location != null) {
            auto instance = new LocationInstance(name);
            instance->locationIs(location);
            instance->travelManagerIs(travelManager_);
            return instance;
        }

        return null;
    }

    Ptr<SegmentInstance> createSegmentInstance(const string& name, const Ptr<Segment> segment) {
        if (segment != null) {
            auto instance = new SegmentInstance(name);
            instance->segmentIs(segment);
            instance->travelManagerIs(travelManager_);
            return instance;
        }

        return null;
    }

    Ptr<VehicleInstance> createVehicleInstance(const string& name, const Ptr<Vehicle> vehicle) {
        if (vehicle != null) {
            auto instance = new VehicleInstance(name);
            instance->vehicleIs(vehicle);
            instance->travelManagerIs(travelManager_);
            return instance;
        }

        return null;
    }

    Ptr<StatsInstance> createStatsInstance(const string& name) {
        auto instance = new StatsInstance(name);
        instance->statsIs(travelManager_->stats());
        instance->travelManagerIs(travelManager_);
        return instance;
    }

    Ptr<ConnInstance> createConnInstance(const string& name) {
        auto instance = new ConnInstance(name);
        instance->connIs(travelManager_->conn());
        instance->travelManagerIs(travelManager_);
        return instance;
    }

    Ptr<TravelNetworkManager> travelManager_;
};

const string TravelInstanceManager::LocationInstance::segmentStr = "segment";
const unsigned long TravelInstanceManager::LocationInstance::segmentStrlen =
    segmentStr.length();

Ptr<TravelInstanceManager> TravelInstanceManager::instanceManager_;

#endif
