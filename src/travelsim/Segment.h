
#ifndef SEGMENT_H
#define SEGMENT_H

#include "CommonLib.h"
#include "ValueTypes.h"

using fwk::BaseNotifiee;
using fwk::NamedInterface;
using fwk::NotifierLib::post;
using fwk::Ordinal;
using fwk::Ptr;


// ==================================================

class Location;

class Segment : public NamedInterface {
public:

	class Notifiee : public BaseNotifiee<Segment> {
	public:
		void notifierIs(const Ptr<Segment>& segment) {
			connect(segment, this);
		}

		/* Notification that the 'source' of this segment has been modified */
		virtual void onSource() { }

		/* Notification that the 'destination' of this segment has been modified */
		virtual void onDestination() { }

		/* Notification that the 'length' of this segment has been modified */
		virtual void onLength() { }
	};

	static Ptr<Segment> instanceNew(const string& name) {
		return new Segment(name);
	}

protected:
	typedef std::list<Notifiee*> NotifieeList;

public:

	Ptr<Location> source() const {
		return source_;
	}

	Ptr<Location> destination() const {
		return destination_;
	}

	Miles length() const {
		return length_;
	}

	void lengthIs(const Miles& length) {
		if (length_ != length) {
			length_ = length;
			post(this, &Notifiee::onLength);
		}
	}

	/* Resets the source attribute to a default value */
	void sourceDel() {
		sourceIs(null);
	}

	/* Resets the destination attribute to a default value */
	void destinationDel() {
		destinationIs(null);
	}

	NotifieeList& notifiees() {
        return notifiees_;
    }

    virtual void sourceIs(const Ptr<Location>& source);
	virtual void destinationIs(const Ptr<Location>& destination);

	Segment(const Segment&) = delete;

	void operator =(const Segment&) = delete;
	void operator ==(const Segment&) = delete;

protected:

	NotifieeList notifiees_;

	explicit Segment(const string& name) :
		NamedInterface(name),
		length_(0)
	{
		source_ = null;
		destination_ = null;
	}

	virtual ~Segment() {
		source_ = null;
		destination_ = null;
	}

private:

	Ptr<Location> source_;
	Ptr<Location> destination_;
	Miles length_;
};

class Road : public Segment {
public:

	static Ptr<Road> instanceNew(const string& name) {
		return new Road(name);
	}

protected:

	Road(const string& name) :
		Segment(name)
	{
		// Nothing to do
	}

	~Road() {}
};

#endif