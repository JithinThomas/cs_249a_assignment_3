
#ifndef FLIGHT_H
#define FLIGHT_H

#include <stdio.h>

#include "Location.h"
#include "Segment.h"

using fwk::Ptr;

using std::cerr;
using std::endl;

// ==================================================
//  Helper functions
// ==================================================

bool isAirport(const Ptr<Location>& location) {
	if (location != null) {
		return (dynamic_cast<Airport*>(location.ptr()) != null);
	}

	return false;
}

bool isResidence(const Ptr<Location>& location) {
	if (location != null) {
		return (dynamic_cast<Residence*>(location.ptr()) != null);
	}

	return false;
}

// ==================================================

// ==================================================
//  Flight class
// ==================================================

class Flight : public Segment {
public:

	static Ptr<Flight> instanceNew(const string& name) {
		return new Flight(name);
	}

	virtual void sourceIs(const Ptr<Location>& src) {
		if ((src == null) || (isAirport(src))) {
			Segment::sourceIs(src);
			return;
		}

		logError(WARNING, "A Flight segment can have only an Airport as its source. Given location ('" 
			+ src->name() + "') is not an Airport");
			
	}

	virtual void destinationIs(const Ptr<Location>& dst) {
		if ((dst == null) || (isAirport(dst))) {
			Segment::destinationIs(dst);
			return;
		}

		logError(WARNING, "A Flight segment can have only an Airport as its destination. Given location ('" 
			+ dst->name() + "') is not an Airport");
			
	}

protected:

	Flight(const string& name) :
		Segment(name)
	{
		// Nothing to do
	}

	~Flight() { }
};

// ==================================================

#endif