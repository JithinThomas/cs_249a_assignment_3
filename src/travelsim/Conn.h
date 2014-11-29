
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

	struct LocDistPair {
		Ptr<Location> loc;
		Miles distance;
	};

	struct LessThanByDist {
		bool operator() (const LocDistPair& p1, const LocDistPair& p2) {
			// return true if 'p1' is ordered before p2
			return p1.distance <= p2.distance;
		}
	};

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

	Ptr<Path> getCachedShortestPath(Ptr<Location> source, Ptr<Location> destination) {
		return null;
	}

	string getSrcDstStr(const Ptr<Location>& source, const Ptr<Location>& destination) {
		return source->name() + "___" + destination->name();
	}

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
};


#endif
