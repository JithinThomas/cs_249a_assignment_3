
#ifndef SEGMENT_IMPL_H
#define SEGMENT_IMPL_H

#include "Segment.h"
#include "Location.h"

void Segment::sourceIs(const Ptr<Location>& source) {
	if (source_ != source) {
		if (source_ != null) {
			source_->sourceSegmentDel(this);
		}

		source_ = source;

		if (source_ != null) {
			source_->sourceSegmentIs(this);
		}

		post(this, &Notifiee::onSource);
	}
}

void Segment::destinationIs(const Ptr<Location>& destination) {
	if (destination_ != destination) {
		if (destination_ != null) {
			destination_->destinationSegmentDel(this);
		}

		destination_ = destination;

		if (destination_ != null) {
			destination_->destinationSegmentIs(this);
		}
		
		post(this, &Notifiee::onDestination);
	}
}

#endif