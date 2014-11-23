/**
 * SequentialManager implements ActivityManager to run all activities
 * sequentially--that is, no two activities ever execute in parallel.
 */

#ifndef FWK_SEQUENTIALMANAGER_H
#define FWK_SEQUENTIALMANAGER_H

class SequentialManager : public ActivityManager {
public:

    static Ptr<ActivityManager> instance() {
        if (instance_ == null) {
            instance_ = new SequentialManager();
        }

        return instance_;
    }


    bool verbose() {
        return verbose_;
    }

    void verboseIs(const bool verbose) {
        verbose_ = verbose;
    }


    _noinline
    Ptr<Activity> activity(const string& name) {
        const auto i = activities_.find(name);
        if (i != activities_.end()) {
            return i->second;
        }

        return null;
    }

    _noinline
    Ptr<Activity> activityNew(const string& name) {
        if (activities_[name] != null) {
            throw NameInUseException(name);
        }

        const Ptr<Activity> a = SequentialActivity::instanceNew(name, this);

        activities_[name] = a;

        return a;
    }

    _noinline
    void activityDel(const string& name) {
        const auto i = activities_.find(name);
        if (i != activities_.end()) {
            activities_.erase(i);
        }
    }

    _noinline
    void activityAdd(const Ptr<Activity>& activity) {
        scheduledActivities_.push(activity);
    }


    Time now() {
        return now_;
    }

    /**
     * Move to the given time, running any scheduled activites with nextTime
     * less than or equal to given time (and run them in chronological order).
     */
    _noinline
    void nowIs(const Time& t) {
        while (!scheduledActivities_.empty()) {
            const auto nextToRun = scheduledActivities_.top();
            const auto nextTimeToRun = nextToRun->nextTime();

            if (nextTimeToRun > t) {
                // Finished running everything before or at time t.
                break;
            }

            if (nextTimeToRun > now_) {
                now_ = nextTimeToRun;
            }

            scheduledActivities_.pop();

            if (verbose_) {
                std::cout << timeAsString(nextTimeToRun) << " ";
                std::cout << "Activity: " << nextToRun->name() << std::endl;
            }

            nextToRun->statusIs(Activity::running);
        }

        //
        // Move the time up to specified time in case the last scheduled
        // activity ran before t and the next one runs after t.
        //
        now_ = t;
    }

protected:

    typedef std::unordered_map< string, Ptr<Activity> > ActivityMap;


    typedef std::vector< Ptr<Activity> > Container;

    class Cmp : public std::binary_function<Ptr<Activity>, Ptr<Activity>, bool> {
    public:

        Cmp() {
            // Nothing to do.
        }


        bool operator()(const Ptr<Activity>& a1, const Ptr<Activity>& a2) const {
            return a1->nextTime() > a2->nextTime();
        }

    };

    typedef std::priority_queue<Ptr<Activity>, Container, Cmp> ActivityQueue;

protected:

    bool verbose_;
    Time now_;
    ActivityMap activities_;
    ActivityQueue scheduledActivities_;


    SequentialManager() :
        verbose_(false),
        now_(0.0)
    {
        // Nothing else to do.
    }

};

#endif
