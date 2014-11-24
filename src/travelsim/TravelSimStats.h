
#ifndef TRAVELSIM_STATS_H
#define TRAVELSIM_STATS_H

#include "fwk/fwk.h"
#include "TravelNetworkManager.h"

class TravelSimStats;

class TripTracker : public Trip::Notifiee {
public:

	static TripTracker* instanceNew(const Ptr<Trip> trip, const Ptr<TravelSimStats> stats) {
		const auto tracker = new TripTracker();
		tracker->notifierIs(trip);
		tracker->travelSimStatsIs(stats);
		return tracker;
	}

	void travelSimStatsIs(const Ptr<TravelSimStats> stats) {
		if (travelSimStats_ != stats) {
			travelSimStats_ = stats;
		}
	}

	void onStatus();

protected:

	explicit TripTracker() :
		travelSimStats_(null)
	{
		// Nothing else to do
	}

	~TripTracker() { }

private:

	friend class TravelSimStats;

	Ptr<TravelSimStats> travelSimStats_;

};

class TravelSimStats : public TravelNetworkManager::Notifiee {
public:

	static Ptr<TravelSimStats> instanceNew(const string& name) {
		return new TravelSimStats(name);
	}

	void onTripNew(const Ptr<Trip>& trip) {
		TripTracker* tracker = TripTracker::instanceNew(trip, this);
		tripToTracker_[trip->name()] = tracker;
		tripsCreatedCount_++;
		tripsInTheNetwork_++;
	}

	void onTripDel(const Ptr<Trip>& trip) {
		auto tripTracker = tripToTracker_[trip->name()];
        delete tripTracker;
        tripToTracker_.erase(trip->name());
        tripsInTheNetwork_--;
	}

	void onTripStatus(const Ptr<Trip> trip) {
		if (trip->status() == Trip::completed) {
			tripsCompletedCount_++;
		}
	}

	string name() const {
		return name_;
	}

	// TODO: Rename this method. Its an attribute. Should be noun-based.
	void printStats() const {
		cout << "# of trips created == " << tripsCreatedCount_ << endl;
		cout << "# of trips completed == " << tripsCompletedCount_ << endl;
		cout << "# of trips in the network == " << tripsInTheNetwork_ << endl;
		cout << "Avg passenger wait time == " << avgPassengerWaitTime_ << endl;
	}

protected:

	explicit TravelSimStats(const string& name) :
		name_(name),
		tripsCreatedCount_(0),
		tripsCompletedCount_(0),
		avgPassengerWaitTime_(0)
	{
		// Nothing else to do
	} 

private:

	string name_;
	unsigned int tripsCreatedCount_;
	unsigned int tripsCompletedCount_;
	unsigned int tripsInTheNetwork_;
	Time avgPassengerWaitTime_;

	unordered_map< string, TripTracker* > tripToTracker_;

};

void TripTracker::onStatus() {
	travelSimStats_->onTripStatus(notifier());
}

#endif