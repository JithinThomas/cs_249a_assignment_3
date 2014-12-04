
#ifndef VALUE_TYPES_H
#define VALUE_TYPES_H


//===============================================================
// DollarsPerMile Type
//===============================================================

class Vehicle;
class DollarsPerMile : public Ordinal<Vehicle, double> {
public:

	static constexpr double tolerance = 1e-4;

	DollarsPerMile(const double value = 0) :
		Ordinal(value)
	{
		if (value < 0) {
			throw fwk::RangeException("DollarsPerMile cannot be negative");
		}
	}

	DollarsPerMile(const Ordinal<Vehicle, double>& c) :
		DollarsPerMile(c.value()) 
	{
		// Nothing else to do
	}

	DollarsPerMile(const DollarsPerMile& cost) :
		DollarsPerMile(cost.value_)
	{
		// Nothing else to do.
	}

	double value() const {
		return value_;
	}

	/** Test for equality using a builtin tolerance. */
	virtual bool operator ==(const DollarsPerMile& cost) {
		return (value_ < cost.value_ + tolerance) && (value_ > cost.value_ - tolerance);
	}

	/** Test for inequality using a builtin tolerance. */
    virtual bool operator !=(const DollarsPerMile& cost) const {
        return (value_ >= cost.value_ + tolerance) || (value_ <= cost.value_ - tolerance);
    }
};

//===============================================================
// Miles Type
//===============================================================

class Length {};
class Miles : public Ordinal<Length, double> {
public:

	static constexpr double tolerance = 1e-4;

	Miles(const double value = 0) :
		Ordinal(value)
	{
		if (value < 0) {
			throw fwk::RangeException("Miles cannot be negative");
		}
	}

	Miles(const Ordinal<Length, double>& m) :
		Miles(m.value()) 
	{
		// Nothing else to do
	}

	Miles(const Miles& m) :
		Miles(m.value_)
	{
		// Nothing else to do.
	}

	double value() const {
		return value_;
	}

	/** Test for equality using a builtin tolerance. */
	virtual bool operator ==(const Miles& m) {
		return (value_ < m.value_ + tolerance) && (value_ > m.value_ - tolerance);
	}

	/** Test for inequality using a builtin tolerance. */
    virtual bool operator !=(const Miles& m) const {
        return (value_ >= m.value_ + tolerance) || (value_ <= m.value_ - tolerance);
    }
};

//===============================================================
// MilesPerHour Type
//===============================================================

class Speed {};
class MilesPerHour : public Ordinal<Speed, int> {
public:

	MilesPerHour(const int value = 0) :
		Ordinal(value)
	{
		if (value < 0) {
			throw fwk::RangeException("MilesPerHour cannot be negative ('" + std::to_string(value) + "')");
		}
	}

	MilesPerHour(const Ordinal<Speed, int>& c) :
		MilesPerHour(c.value()) 
	{
		// Nothing else to do
	}

	MilesPerHour(const MilesPerHour& m):
		MilesPerHour(m.value_)
	{
		// Nothing else to do
	}	
};

//===============================================================
// PassengerCount Type
//===============================================================

class Capacity {};
class PassengerCount : public Ordinal<Capacity, int> {
public:

	PassengerCount(const int value = 0) :
		Ordinal(value)
	{
		if (value < 0) {
			throw fwk::RangeException("PassengerCount cannot be negative ('" + std::to_string(value) + "')");
		}
	}

	PassengerCount(const Ordinal<Capacity, int>& c) :
		PassengerCount(c.value()) 
	{
		// Nothing else to do
	}

	PassengerCount(const PassengerCount& c):
		PassengerCount(c.value_)
	{
		// Nothing else to do
	}	
};

//===============================================================
// SegmentId Type
//===============================================================

class Segment;
class SegmentId : public Ordinal<Segment, int> {
public:

	SegmentId(const int value = 0) :
		Ordinal(value)
	{
		if (value < 0) {
			throw fwk::RangeException("SegmentId cannot be negative ('" + std::to_string(value) + "')");
		}
	}

	SegmentId(const Ordinal<Segment, int>& c) :
		SegmentId(c.value()) 
	{
		// Nothing else to do
	}

	SegmentId(const SegmentId& id):
		SegmentId(id.value_)
	{
		// Nothing else to do
	}	
};

//===============================================================
// Hours Type
//===============================================================

class TTime {};
class Hours : public Ordinal<TTime, double> {
public:

	static constexpr double tolerance = 1e-4;

	Hours(const double value = 0) :
		Ordinal(value)
	{
		if (value < 0) {
			throw fwk::RangeException("Hours cannot be negative");
		}
	}

	Hours(const Ordinal<TTime, double>& m) :
		Hours(m.value()) 
	{
		// Nothing else to do
	}

	Hours(const Hours& m) :
		Hours(m.value_)
	{
		// Nothing else to do.
	}

	double value() const {
		return value_;
	}

	/** Test for equality using a builtin tolerance. */
	virtual bool operator ==(const Hours& m) {
		return (value_ < m.value_ + tolerance) && (value_ > m.value_ - tolerance);
	}

	/** Test for inequality using a builtin tolerance. */
    virtual bool operator !=(const Hours& m) const {
        return (value_ >= m.value_ + tolerance) || (value_ <= m.value_ - tolerance);
    }
};

//===============================================================
// Dollars Type
//===============================================================

class Currency {};
class Dollars : public Ordinal<Currency, double> {
public:

	static constexpr double tolerance = 1e-4;

	Dollars(const double value = 0) :
		Ordinal(value)
	{
		if (value < 0) {
			throw fwk::RangeException("Dollars cannot be negative");
		}
	}

	Dollars(const Ordinal<Currency, double>& m) :
		Dollars(m.value()) 
	{
		// Nothing else to do
	}

	Dollars(const Dollars& m) :
		Dollars(m.value_)
	{
		// Nothing else to do.
	}

	double value() const {
		return value_;
	}

	/** Test for equality using a builtin tolerance. */
	virtual bool operator ==(const Dollars& m) {
		return (value_ < m.value_ + tolerance) && (value_ > m.value_ - tolerance);
	}

	/** Test for inequality using a builtin tolerance. */
    virtual bool operator !=(const Dollars& m) const {
        return (value_ >= m.value_ + tolerance) || (value_ <= m.value_ - tolerance);
    }
};

//===============================================================
// Operations defined between various units
//===============================================================

Hours operator/(const Miles& distance, const MilesPerHour& speed) {
	Hours h(distance.value() / speed.value());
	return h;
}

Miles operator*(const MilesPerHour& speed, const Hours& t) {
	Miles m(speed.value() * t.value());
	return m;
}

Miles operator*(const Hours& t, const MilesPerHour& speed) {
	Miles m(speed.value() * t.value());
	return m;
}

Dollars operator*(const DollarsPerMile& cost, const Miles& m) {
	Dollars d(cost.value() * m.value());
	return d;
}

Dollars operator*(const Miles& m, const DollarsPerMile& cost) {
	Dollars d(cost.value() * m.value());
	return d;
}

#endif