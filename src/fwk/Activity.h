/**
 * Activity interface for coordinating object interactions.
 *
 * An activity has three attributes:
 *
 *     status -- current status of the activity, e.g., idle or running
 *
 *     nextTime -- next time the activity should run
 *
 *     manager -- the activity manager that manages this activity
 *
 *     main -- main element for the activity, which use the special
 *         onMain notification (defined in ActivityElement) that begins
 *         the logic for the activity. Setting the main attribute
 *         is a shortcut for having a special notifiee that receives
 *         the first status change or initial timer event and then bootstraps
 *         the activity before disconnecting itself.
 *
 *     immediateDeliveryFlag -- indicates whether notifications should be
 *         delivered immediately instead of being deferred
 *
 *     posting -- notification queue for this activity
 */

#ifndef FWK_ACTIVITY_H
#define FWK_ACTIVITY_H

class ActivityManager;

class Activity : public NamedInterface {
public:

    /**
     * Return the current activity.
     */
    static Ptr<Activity> current() {
        return current_;
    }


    typedef std::function<void()> Reaction;


    class Notifiee : public BaseNotifiee<Activity> {
    public:

        void notifierIs(const Ptr<Activity>& notifier) {
            connect(notifier, this);
        }

        /** Notification that the activity's current time has changed. */
        virtual void onNextTime() { }

        /** Notification that an activity's status has changed. */
        virtual void onStatus() {}
    };


    enum Status {

        /** Defensive programming status that should never occur. */
        undefined,

        /** Waiting for a notification. */
        idle,

        /** Waiting to run at a particular time. */
        scheduled,

        /** Ready to run. */
        ready,

        /** Deliving notifications (including timers) */
        running,

        /** Stopped from further processing */
        stopped

    };


    /** Current activity status. */
    virtual Status status() = 0;

    /** Modify the current activity status. */
    virtual void statusIs(Status s) = 0;


    /** Time when the activity next should run. */
    virtual Time nextTime() = 0;

    /** Motify the time when the activity next should run. */
    virtual void nextTimeIs(const Time t) = 0;

    /**
     * Motify the time when the activity next should run relative
     * to the current time.
     */
    virtual void nextTimeIsOffset(const Time offset) = 0;


    /** Manager for this activity. */
    virtual Ptr<ActivityManager> manager() = 0;


    /** Return the activity's main element. */
    virtual Ptr<ActivityElement> main() = 0;

    /** Modify the activity's main element. */
    virtual void mainIs(const Ptr<ActivityElement>& main) = 0;


    /** Flag to deliver notifications immediately instead of deferred. */
    virtual bool immediateDeliveryFlag() = 0;

    /** Modify flag to deliver notifications immediately instead of deferred. */
    virtual void immediateDeliveryFlagIs(const bool flag) = 0;

    /** Return the number of notifications in the activity's queue. */
    virtual unsigned long postingCount() = 0;

    /** Add a notification to the activity's queue. */
    virtual void postingNew(
        const Ptr<ActivityElement>& reactor, const Reaction& reaction
    ) = 0;

protected:

    typedef std::list<Notifiee*> NotifieeList;

public:

    NotifieeList& notifiees() {
        return notifiees_;
    }

protected:

    static Ptr<Activity> current_;


    NotifieeList notifiees_;


    Activity(const string& name) :
        NamedInterface(name)
    {
        // Nothing else to do.
    }

};

Ptr<Activity> Activity::current_;


ActivityElement::ActivityElement() :
    activity_(Activity::current())
{
    // Nothing else to do.
}

#endif
