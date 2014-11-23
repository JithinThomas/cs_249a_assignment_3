#ifndef TRAVEL_SIM_H
#define TRAVEL_SIM_H

#include "Sim.h"
#include "TripSim.h"

using std::unordered_map;

class TravelSim;

class TripGenerator : public Sim {
public:
	static Ptr<TripGenerator> instanceNew(const Ptr<TravelSim>& travelSim);

	void onStatus();

protected:

	TripGenerator(const Ptr<TravelSim>& travelSim):
		travelSim_(travelSim)
	{
		// Nothing else to do
	}

private:

	Ptr<TravelSim> travelSim_;
	unsigned int nextTripId_ = 0;
};

class TravelSim : public Sim {
public:

	static Ptr<TravelSim> instanceNew(const Ptr<TravelNetworkManager> travelNetworkManager) {
		const Ptr<TravelSim> sim = new TravelSim(travelNetworkManager);
		const auto mgr = sim->activityManager();
		const auto a = mgr->activityNew("TravelSim");
		a->nextTimeIs(mgr->now());
		a->statusIs(Activity::scheduled);
		mgr->activityAdd(a);
		sim->notifierIs(a);
		return sim;
	}

	void simulationEndTimeIsOffset(const Time offset) {
		const auto startTime = time(SystemTime::now());
		
		logEntryNew(startTime, "Starting travel simulation");
	    activityManager_->nowIs(startTime);

	    activityManager_->nowIs(startTime + offset);
	    logEntryNew(startTime + offset, "Stopping travel simulation");

	    tripGenerator_->activityDel();
	}

	Ptr<TripSim> tripNew(const string& name, 
					     const Ptr<Location>& startLocation, 
					     const Ptr<Location>& destination) {
		const Ptr<TripSim> tripSim = TripSim::instanceNew(name, startLocation, destination, activityManager_);
		tripSimMap_.insert(TripSimMap::value_type(name, tripSim));
		return tripSim;
	}

	Ptr<ActivityManager> activityManager() {
		return activityManager_;
	}

	Ptr<TravelNetworkManager> travelNetworkManager() {
		return travelNetworkManager_;
	}

protected:

	typedef unordered_map< string, Ptr<TripSim> > TripSimMap;

	TravelSim(const Ptr<TravelNetworkManager> travelNetworkManager) :
		activityManager_(SequentialManager::instance()),
		travelNetworkManager_(travelNetworkManager)
	{
		activityManager_->nowIs(time(SystemTime::now()));
		tripGenerator_ = TripGenerator::instanceNew(this);
	}

private:

	TripSimMap tripSimMap_;

	Ptr<ActivityManager> activityManager_;
	Ptr<TripGenerator> tripGenerator_;	
	Ptr<TravelNetworkManager> travelNetworkManager_;
};

Ptr<TripGenerator> TripGenerator::instanceNew(const Ptr<TravelSim>& travelSim) {
	const Ptr<TripGenerator> sim = new TripGenerator(travelSim);
	const auto mgr = travelSim->activityManager();
	const auto a = mgr->activityNew("TripGenerator");
	a->nextTimeIs(mgr->now());
	a->statusIs(Activity::scheduled);
	mgr->activityAdd(a);
	sim->notifierIs(a);
	return sim;
}

void TripGenerator::onStatus() {
	const auto a = notifier();
	if (a->status() == Activity::running) {
		logEntryNew(a->manager()->now(), "Requesting for a trip");
		const auto travelNetworkMgr = travelSim_->travelNetworkManager();
		travelSim_->tripNew("TripSim-" + std::to_string(nextTripId_), 
							travelNetworkMgr->location("sfo"), 
							travelNetworkMgr->location("stanford"));
		a->nextTimeIsOffset(5);
		nextTripId_++;
	}
}

#endif