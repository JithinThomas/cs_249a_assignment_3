
#ifndef CONN_H
#define CONN_H

#include <climits>
#include <set>

#include "CommonLib.h"
#include "Location.h"
#include "Segment.h"

using fwk::BaseNotifiee;
using fwk::NamedInterface;
using fwk::Nominal;
using fwk::NotifierLib::post;
using fwk::Ordinal;
using fwk::Ptr;
using fwk::PtrInterface;

using std::set;
using std::to_string;

class TravelNetworkManager;

class Conn : public NamedInterface {
public:

	static Ptr<Conn> instanceNew(const string& name, const Ptr<TravelNetworkManager>& mgr) {
		return new Conn(name, mgr);
	}

	class Path : public PtrInterface {
	public:

		static Ptr<Path> instanceNew() {
			return new Path();
		}

		static Ptr<Path> instanceNew(const Ptr<Path>& p) {
			return new Path(p);
		}

		void segmentIs(const Ptr<Segment> segment) {
			segments_.push_back(segment);
			length_ = length_ + segment->length();
		}

		Ptr<Segment> segment(const U32 id) {
			if (id < segments_.size()) {
				return segments_[id];
			}

			return null;
		}

		vector< Ptr<Segment> > segments() const {
			return segments_;
		}

		unsigned int segmentCount() const {
			return segments_.size();
		}

		Miles length() const {
			return length_;
		}

		Ptr<Location> source() const {
			if (segments_.size() > 0) {
				return segments_[0]->source();
			}

			return null;
		}

		Ptr<Location> destination() const {
			const auto numSegments = segments_.size();
			if (numSegments > 0) {
				return segments_[numSegments - 1]->destination();
			}

			return null;
		}

		string stringRep() const {
            string str = "";
            for (auto it = segments_.cbegin(); it != segments_.cend(); it++) {
                auto seg = *it;
                str += seg->source()->name() + "(" + seg->name() + ":" + to_string(seg->length().value()) + ") ";
            }

            if (segments_.size() > 0) {
	            auto lastSegment = segments_[segments_.size() - 1];
	            str += lastSegment->destination()->name();
	        }

            return str;
        }

    protected:

		Path():
			length_(0)
		{
			// Nothing else to do
		}

		Path(const Ptr<Path>& p) :
			segments_(p->segments()),
			length_(p->length())
		{
			// Nothing else to do
		}

	private:

		vector< Ptr<Segment> > segments_;
		Miles length_;
	};

	class PathCacheStats : public PtrInterface {
	public:

		unsigned int hitCount() const {
			return hitCount_;
		}

		unsigned int missCount() const {
			return missCount_;
		}

		unsigned int requestCount() const {
			return requestCount_;
		}

		PathCacheStats(const PathCacheStats&) = delete;

		void operator =(const PathCacheStats&) = delete;
		void operator ==(const PathCacheStats&) = delete;

	protected:

		friend class Conn;

		static Ptr<PathCacheStats> instanceNew() {
			return new PathCacheStats();
		}

		void hitCountIsIncByOne() {
			hitCount_ += 1;
		}

		void missCountIsIncByOne() {
			missCount_ += 1;
		}

		void requestCountIsIncByOne() {
			requestCount_ += 1;
		}

		PathCacheStats() :
			hitCount_(0),
			missCount_(0),
			requestCount_(0)
		{
			// Nothing else to do
		}

		~PathCacheStats() { }

	private:

		unsigned int hitCount_;
		unsigned int missCount_;
		unsigned int requestCount_;

	};

protected:

	typedef vector< Ptr<Path> > PathVector;
	typedef unordered_map< string, Miles> LocToMinDistMap;
	typedef unordered_map<string, string> LocToSeg;
	typedef unordered_map< string, LocToSeg > ShortestPathCache;

public:
	const PathVector paths(const Ptr<Location>& location, const Miles& maxLength) const {
		set<string> locationsVisited;
		locationsVisited.insert(location->name());

		return getPathsFromLoc(location, Path::instanceNew(), maxLength, locationsVisited);
	}

	Ptr<Path> shortestPath(const Ptr<Location>& source, const Ptr<Location>& destination);

	void pathCacheIsEmpty();

	Ptr<PathCacheStats> shortestPathCacheStats() const {
		return shortestPathCacheStats_;
	}

	void shortestPathCacheIsEnabledIs(bool b) {
		if (shortestPathCacheIsEnabled_ != b) {
			shortestPathCacheIsEnabled_ = b;
		}
	}

	// TODO: Delete this method. Its for test purposes alone.
	/*
	void printShortestPathCache() {
		for (auto it1 = shortestPathCache_.begin(); it1 != shortestPathCache_.end(); it1++) {
			const auto destName = it1->first;
			const auto srcToCacheEntry = it1->second;
			cout << endl;
			cout << "===========================================" << endl;
			cout << "Destination: " << destName << endl;
			cout << "===========================================" << endl;
			for (auto it2 = srcToCacheEntry.begin(); it2 != srcToCacheEntry.end(); it2++) {
				const auto srcName = it2->first;
				const auto segName = it2->second;
				cout << "	Source: " << srcName << "   Seg: " << segName << endl;
			}
			cout << "===========================================" << endl;
		}
	}
	*/

	// TODO: Delete this method. Its for testing purposes alone
	ShortestPathCache& shortestPathCache() {
		return shortestPathCache_;
	}

	Conn(const Conn&) = delete;

	void operator =(const Conn&) = delete;
	void operator ==(const Conn&) = delete;

protected:

	friend class TravelNetworkManager;

	void onLocationDel(const Ptr<Location>& location);

	void onSegmentDel(const Ptr<Segment>& segment);

	Conn(const string& name, const Ptr<TravelNetworkManager>& mgr):
		NamedInterface(name),
		travelNetworkManager_(mgr),
		shortestPathCacheStats_(PathCacheStats::instanceNew()),
		shortestPathCacheIsEnabled_(true)
	{
		// Nothing else to do
	}

	~Conn() { }

private:

	PathVector getPathsFromLoc(const Ptr<Location>& location,
							   const Ptr<Path>& pathFromStartLocation,
							   const Miles& maxLength, 
							   set<string> locationsVisited
							  ) const {
		PathVector validPaths;

		if (location != null) {
			for (auto i = 0u; i < location->sourceSegmentCount(); i++) {		
				auto segment = location->sourceSegment(i);
				Miles totalLengthOfPath = pathFromStartLocation->length() + segment->length();
				auto destination = segment->destination();

				if ((totalLengthOfPath <= maxLength) && 
					(destination != null) &&
					(locationsVisited.find(destination->name()) == locationsVisited.end())) {
						Ptr<Path> p = Path::instanceNew(pathFromStartLocation);
						p->segmentIs(segment);
						validPaths.push_back(p);

						locationsVisited.insert(destination->name());
						auto otherPaths = getPathsFromLoc(destination, p, maxLength, locationsVisited);
						locationsVisited.erase(destination->name());

						for (auto p : otherPaths) {
							validPaths.push_back(p);
						}
				}
			}
		}

		return validPaths;
	}

	Ptr<Path> getCachedShortestPath(const Ptr<Location>& source, const Ptr<Location>& destination) const;

	void insertIntoShortestPathCache(const Ptr<Path>& path);

	string findNextLocWithMinDist(unordered_map<string, Miles> locsToConsiderNextToMinDist) {
		string minDistLocName = "__no_loc_found__";

		if (locsToConsiderNextToMinDist.size() > 0) {
			Miles minDist = Miles(ULONG_MAX);

			for (auto it = locsToConsiderNextToMinDist.begin(); it != locsToConsiderNextToMinDist.end(); it++) {
				const auto dist = it->second;
				if (dist < minDist) {
					minDistLocName = it->first;
					minDist = dist;
				}
			}

			return minDistLocName;
		}

		return minDistLocName;
	}

	bool isLocationPartOfTravelNetwork(const Ptr<Location>& loc);

	Ptr<TravelNetworkManager> travelNetworkManager_;
	ShortestPathCache shortestPathCache_;
	Ptr<PathCacheStats> shortestPathCacheStats_;
	bool shortestPathCacheIsEnabled_;
};


#endif
