
#ifndef RANDOM_NUMBER_GENERATORS_H
#define RANDOM_NUMBER_GENERATORS_H

//========================================================
// RandomNumberGenerator class
//========================================================

class RandomNumberGenerator : public PtrInterface {
public:

	virtual double value() = 0;

    double lower() const {
        return lower_;
    }

    double upper() const {
        return upper_;
    }

    void lowerIs(const double lower) {
        if (lower_ != lower) {
            if (lower > upper_) {
                throw fwk::RangeException("The upper bound of the random number generator should be greater than equal to the lower bound");
            }

            lower_ = lower;
        }
    }

    void upperIs(const double upper) {
        if (upper_ != upper) {
            if (upper < lower_) {
                throw fwk::RangeException("The upper bound of the random number generator should be greater than equal to the lower bound");
            }

            upper_ = upper;
        }
    }

protected:

    RandomNumberGenerator(const double lower, const double upper) :
        lower_(lower),
        upper_(upper)
    {
        if (lower > upper) {
            throw fwk::RangeException("The upper bound of the random number generator should be greater than equal to the lower bound");
        }
    }

    ~RandomNumberGenerator() { }

    double lower_;
    double upper_;

};

//========================================================
// NormalDistributionRandom class
//========================================================

class NormalDistributionRandom : public RandomNumberGenerator {
public:

    static Ptr<NormalDistributionRandom> instanceNew(
    					const double mean, const double dev,
						const double lower, const double upper) {
        return new NormalDistributionRandom(mean, dev, lower, upper);
    }

    static Ptr<NormalDistributionRandom> instanceNew(
    					const double seed, const double mean, const double dev,
						const double lower, const double upper) {
        return new NormalDistributionRandom(seed, mean, dev, lower, upper);
    }

    double value() {
        const auto r = normal();

        if (r < lower_) {
            return lower_;
        }

        if (r > upper_) {
            return upper_;
        }

        return r;
    }

    double mean() const {
    	return mean_;
    }

    double dev() const {
    	return dev_;
    }

    void meanIs(const double mean) {
    	if (mean_ != mean) {
    		mean_ = mean;
    	}
    }

    void devIs(const double dev) {
    	if (dev_ != dev) {
    		dev_ = dev;
    	}
    }

protected:

    NormalDistributionRandom(const double mean, const double dev,
    						 const double lower, const double upper) :
        RandomNumberGenerator(lower, upper),
        generator_(U32(SystemTime::now().value() & 0xffffffff)),
        distribution_(0.0, 1.0),
        mean_(mean),
        dev_(dev)
    {
        // Nothing else to do
    }

    NormalDistributionRandom(const double seed, const double mean, const double dev,
    						 const double lower, const double upper) :
        RandomNumberGenerator(lower, upper),
        generator_(seed),
        distribution_(0.0, 1.0),
        mean_(mean),
        dev_(dev)
    {
        if (lower > upper) {
			throw fwk::RangeException("The upper bound of the normal distribution should be greater than equal to the lower bound");
		}
    }

    ~NormalDistributionRandom() { }

private:

    double normal() {
        return dev_ * distribution_(generator_) + mean_;
    }

	std::default_random_engine generator_;
    std::normal_distribution<double> distribution_;
    double mean_;
    double dev_;

};

//========================================================
// UniformDistributionRandom class
//========================================================

class UniformDistributionRandom : public RandomNumberGenerator {
public:

    static Ptr<UniformDistributionRandom> instanceNew(const double lower, const double upper) {
        return new UniformDistributionRandom(lower, upper);
    }

    static Ptr<UniformDistributionRandom> instanceNew(const double seed, const double lower, const double upper) {
        return new UniformDistributionRandom(seed, lower, upper);
    }

    double value() {
    	return lower_ + (upper_ - lower_) * distribution_(generator_);
    }

protected:

    UniformDistributionRandom(const double lower, const double upper) :
        RandomNumberGenerator(lower, upper),
        generator_(U32(SystemTime::now().value() & 0xffffffff)),
        distribution_(0.0, 1.0)
    {
        // Nothing else to do
    }

    UniformDistributionRandom(const double seed, const double lower, const double upper) :
        RandomNumberGenerator(lower, upper),
        generator_(seed),
        distribution_(0.0, 1.0)
    {
        // Nothing else to do
    }

    ~UniformDistributionRandom() { }

private:

	std::default_random_engine generator_;
    std::uniform_real_distribution<double> distribution_;

};

//========================================================
// ConstGenerator class
//========================================================

class ConstGenerator : public RandomNumberGenerator {
public:

	static Ptr<ConstGenerator> instanceNew(const double value) {
		return new ConstGenerator(value);
	}

	double value() {
		return value_;
	}

	void valueIs(const double value) {
		if (value_ != value) {
			value_ = value;
		}
	}

protected:

	ConstGenerator(const double value) :
        RandomNumberGenerator(value, value),
		value_(value)
	{
		// Nothing else to do
	}

	~ConstGenerator() { }

private:

	double value_;
};

#endif