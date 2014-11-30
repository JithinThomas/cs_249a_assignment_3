
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

protected:

	typedef vector< Ptr<Path> > PathVector;
	typedef unordered_map< string, Miles> LocToMinDistMap;
	typedef unordered_map< string, Ptr<Path> > LocNameToPath;
	typedef unordered_map< string, LocNameToPath > PathL1Cache;
	typedef unordered_map<string, string> LocToSegName;
	typedef unordered_map< string, LocToSegName > PathL2Cache;

public:
	const PathVector paths(const Ptr<Location>& location, const Miles& maxLength) const {
		set<string> locationsVisited;
		locationsVisited.insert(location->name());

		return getPathsFromLoc(location, new Path(), maxLength, locationsVisited);
	}

	// TODO: The shortest paths should not be stored as complete paths - too much memory would be required
	//  	 Instead, compress the data by storing just the previous pointers.
	//  	 eg: http://rosettacode.org/wiki/Dijkstra%27s_algorithm#C.2B.2B
	Ptr<Path> shortestPath(const Ptr<Location>& source, const Ptr<Location>& destination);

	// TODO: Delete this method. Its for test purposes alone.
	void printPathL2Cache() {
		for (auto it1 = pathL2Cache_.begin(); it1 != pathL2Cache_.end(); it1++) {
			const auto destName = it1->first;
			const auto srcToSeg = it1->second;
			cout << endl;
			cout << "===========================================" << endl;
			cout << "Destination: " << destName << endl;
			cout << "===========================================" << endl;
			for (auto it2 = srcToSeg.begin(); it2 != srcToSeg.end(); it2++) {
				const auto srcName = it2->first;
				const auto segName = it2->second;
				cout << "	Source: " << srcName << "   Seg: " << segName << endl;
			}
			cout << "===========================================" << endl;
		}
	}

	// TODO: Delete this method. Its for testing purposes alone
	PathL2Cache& pathL2Cache() {
		return pathL2Cache_;
	}

	Conn(const Conn&) = delete;

	void operator =(const Conn&) = delete;
	void operator ==(const Conn&) = delete;

protected:

	Conn(const string& name, const Ptr<TravelNetworkManager>& mgr):
		NamedInterface(name),
		travelNetworkManager_(mgr)
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
						Ptr<Path> p = new Path(pathFromStartLocation);
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

	Ptr<Path> tryFetchShortestPathFromL1Cache(const string& sourceName, const string& destName) const;

	Ptr<Path> tryFetchShortestPathFromL2Cache(const string& sourceName, const string& destName) const;

	// Precondition: The L2 cache does have the shortest path info from sourceName to destName
	Ptr<Path> fetchShortestPathFromL2Cache(const string& sourceName, const string& destName) const;

	void insertIntoPathL1Cache(const string& sourceName, const string& destName, const Ptr<Path>& path);

	void insertIntoPathL2Cache(const string& sourceName, const string& destName, const string& segName);

	string findNextLocWithMinDist(unordered_map<string, Miles> locsToConsiderNextToMinDist) {
		if (locsToConsiderNextToMinDist.size() > 0) {
			string minDistLocName;
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

		// TODO: Change this default string?
		return "__no_loc_found__";
	}

	template<typename T>
	bool isElemPresentInSet(const std::set<T>& s, const T& elem) {
		return (s.find(elem) != s.end());
	}

	Ptr<TravelNetworkManager> travelNetworkManager_;
	PathL1Cache pathL1Cache_;
	PathL2Cache pathL2Cache_;
};


#endif
