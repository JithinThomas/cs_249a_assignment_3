/**
 * Simple simulation for a variation on a whackamole game.
 *
 * Only one mole and a player cannot interrupt a swing even if the mole hides
 * during the swing.
 */

#include "fwk/fwk.h"

#include <random>

using namespace fwk;
using NotifierLib::post;


/**
 * Helper class for random number generation.
 */
class Random : public PtrInterface {
public:

    static Ptr<Random> instanceNew() {
        return new Random();
    }


    double normal(const double mean, const double dev) {
        return dev * distribution_(generator_) + mean;
    }

    double normalRange(
        const double mean, const double dev,
        const double lower, const double upper
    ) {
        const auto r = normal(mean, dev);

        if (r < lower) {
            return lower;
        }

        if (r > upper) {
            return upper;
        }

        return r;
    }

protected:

    std::default_random_engine generator_;
    std::normal_distribution<double> distribution_;


    Random() :
        generator_(U32(SystemTime::now().value() & 0xffffffff)),
        distribution_(0.0, 1.0)
    {
        // Nothing else to do.
    }

};


static Ptr<Random> rng = Random::instanceNew();


static void logEntryNew(const Time t, const string& s) {
    //std::cout << timeMilliAsString(t) << " " << s << std::endl;
    std::cout << timeAsString(t) << " " << s << std::endl;
}


/**
 * Mole represents the current state of a mole, either hidden or visible.
 */
class Mole : public PtrInterface {
public:

    enum Status { hidden, visible };

    class Notifiee : public BaseNotifiee<Mole> {
    public:

        void notifierIs(const Ptr<Mole>& mole) {
            connect(mole, this);
        }


        virtual void onStatus() { }

    };

protected:

    typedef std::list<Notifiee*> NotifieeList;

public:

    static Ptr<Mole> instanceNew() {
        return new Mole();
    }


    Status status() {
        return status_;
    }

    NotifieeList& notifiees() {
        return notifiees_;
    }

protected:

    friend class MoleSim;

    Status status_;
    NotifieeList notifiees_;


    Mole() :
        status_(visible)
    {
        // Nothing else to do.
    }


    void statusIs(const Status status) {
        if (status_ != status) {
            status_ = status;

            post(this, &Notifiee::onStatus);
        }
    }

};

/**
 * Common base class for simulation objects.
 *
 * These already have an activity attribute (from ActivityElement) which
 * should always be the same as their notifier.
 */
class Sim : public Activity::Notifiee {
public:

    void activityDel() {
        const auto a = activity();
        a->manager()->activityDel(a->name());
    }

};


/**
 * MoleSim is the simulator logic for a mole.
 */
class MoleSim : public Sim {
public:

    static Ptr<MoleSim> instanceNew(
        const Ptr<ActivityManager>& mgr, const Ptr<Mole>& mole
    ) {
        return new MoleSim(mgr->activityNew("MoleSim"), mole);
    }


    void onStatus() {
        const auto a = activity();
        if (a->status() == Activity::running) {
            if (mole_->status() == Mole::visible) {
                logEntryNew(a->manager()->now(), "Mole becomes hidden");
                mole_->statusIs(Mole::hidden);
                const auto delta = rng->normalRange(3.0, 1.5, 0.5, 5.0);
                a->nextTimeIsOffset(delta);
            } else {
                logEntryNew(a->manager()->now(), "Mole becomes visible");
                mole_->statusIs(Mole::visible);
                a->nextTimeIsOffset(0.25);
            }
        }
    }

protected:

    Ptr<Mole> mole_;


    MoleSim(const Ptr<Activity>& activity, const Ptr<Mole>& mole) :
        mole_(mole)
    {
        activityIs(activity);

        const auto mgr = activity->manager();
        activity->immediateDeliveryFlagIs(false);
        activity->nextTimeIs(mgr->now());
        activity->statusIs(Activity::scheduled);
        mgr->activityAdd(activity);

        notifierIs(activity);
    }

};

/**
 * PlayerSim is the simulator logic for a player.
 */
class PlayerSim : public Sim {
public:

    static Ptr<PlayerSim> instanceNew(
        const Ptr<ActivityManager>& mgr, const Ptr<Mole>& mole
    ) {
        return new PlayerSim(mgr->activityNew("PlayerSim"), mole);
    }


    U64 attempts() {
        return attempts_;
    }

    U64 hits() {
        return hits_;
    }


    void onStatus() {
        const auto a = activity();
        if (a->status() == Activity::running) {
            attempts_ += 1;

            const auto t = a->manager()->now();
            if (reactor_->notifier()->status() == Mole::visible) {
                hits_ += 1;
                logEntryNew(t, "Player hits mole");
            } else {
                logEntryNew(t, "Player misses mole");
            }
        }
    }

    void onMoleStatus(const Ptr<Mole>& mole) {
        if (mole->status() == Mole::visible) {
            const auto a = activity();
            logEntryNew(a->manager()->now(), "Player starts swing");
            const auto delta = rng->normalRange(0.5, 0.3, 0.0, 5);
            a->nextTimeIsOffset(delta);
        }
    }

protected:

    class MoleReactor : public Mole::Notifiee {
    public:

        void onStatus() {
            player_->onMoleStatus(notifier());
        }

    protected:

        friend class PlayerSim;

        MoleReactor(PlayerSim* const player, const Ptr<Mole>& mole) :
            player_(player)
        {
            activityIs(player->activity());
            notifierIs(mole);
        }


        PlayerSim* const player_;

    };


    Ptr<MoleReactor> reactor_;
    U64 attempts_;
    U64 hits_;


    PlayerSim(const Ptr<Activity>& activity, const Ptr<Mole>& mole) :
        reactor_(new MoleReactor(this, mole)),
        attempts_(0),
        hits_(0)
    {
        activity->immediateDeliveryFlagIs(false);
        activityIs(activity);
        notifierIs(activity);
    }

};

/**
 * Main program creates a mole, mole simuulation, and player simulation, and
 * then runs the simulation for a fixed period of time (e.g., 2 minutes).
 */
int main(int argc, char *argv[]) {
    const auto mgr = SequentialManager::instance();
    const auto startTime = time(SystemTime::now());
    mgr->nowIs(startTime);

    const auto mole = Mole::instanceNew();
    const auto molesim = MoleSim::instanceNew(mgr, mole);
    const auto playersim = PlayerSim::instanceNew(mgr, mole);

    logEntryNew(startTime, "starting");
    constexpr auto minutes = 60;
    mgr->nowIs(startTime + 2 * minutes);

    molesim->activityDel();
    playersim->activityDel();

    std::cout << "Player score: ";
    std::cout << playersim->hits() << "/" << playersim->attempts() << std::endl;

    return 0;
}
