
#ifndef CONN_H
#define CONN_H

#include <set>

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

class Conn : public NamedInterface {
public:

	static Ptr<Conn> instanceNew(const string& name) {
		return new Conn(name);
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

public:
	const PathVector paths(const Ptr<Location>& location, const Miles& maxLength) const {
		set<string> locationsVisited;
		locationsVisited.insert(location->name());

		return getPathsFromLoc(location, new Path(), maxLength, locationsVisited);
	}

	Conn(const Conn&) = delete;

	void operator =(const Conn&) = delete;
	void operator ==(const Conn&) = delete;

protected:

	Conn(const string& name):
		NamedInterface(name)
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

};


#endif
