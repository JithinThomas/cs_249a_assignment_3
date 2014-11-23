#ifndef FWK_SEQUENTIALACTIVITY_H
#define FWK_SEQUENTIALACTIVITY_H

class SequentialActivity : public Activity {
public:

    _noinline
    static Ptr<Activity> instanceNew(
        const string& name, const Ptr<ActivityManager>& mgr
    ) {
        return new SequentialActivity(name, mgr);
    }


    Status status() {
        return status_;
    }

    _noinline
    void statusIs(const Status s) {
        if (status_ != s) {
            status_ = s;

            if (s == running) {
                current_ = this;
                scheduled_ = false;
            }

            NotifierLib::post(this, &Notifiee::onStatus);

            if (s == running) {
                deliverAll();
            }
        }
    }


    Time nextTime() {
        return nextTime_;
    }

    _noinline
    void nextTimeIs(const Time t) {
        scheduled_ = true;
        nextTime_ = t;

        NotifierLib::post(this, &Notifiee::onNextTime);
    }

    _noinline
    void nextTimeIsOffset(const Time offset) {
        nextTimeIs(manager_->now() + offset);
    }


    Ptr<ActivityManager> manager() {
        return manager_;
    }


    Ptr<ActivityElement> main() {
        return main_;
    }

    _noinline
    void mainIs(const Ptr<ActivityElement>& main) {
        main_ = main;

        if (immediateDeliveryFlag_) {
            try {
                main->onMain();
            } catch (const std::exception& e) {
                undeliverable(e);
            } catch (...) {
                undeliverable();
            }
        } else {
            postingNew(main, [=]() { main->onMain(); });
        }
    }


    bool immediateDeliveryFlag() {
        return immediateDeliveryFlag_;
    }

    void immediateDeliveryFlagIs(const bool flag) {
        immediateDeliveryFlag_ = flag;
    }

protected:

    /**
     * The data structure for a reaction must keep a refcount'd pointer
     * to the reactor so it doesn't go away while the notification is still
     * on the queue.
     *
     * It might make sense to remove or ignore the reaction in the queue
     * when its refcount goes to one, but it isn't clear how that behavior
     * would work in a runtime with garbage collection.
     */
    struct Posting {
        Ptr<ActivityElement> reactor;
        Reaction reaction;
    };

    typedef std::deque<Posting> PostingQueue;

public:

    unsigned long postingCount() {
        return postingQueue.size();
    }

    _noinline
    void postingNew(const Ptr<ActivityElement>& r, const Reaction& reaction) {
        Posting posting;
        posting.reactor = r;
        posting.reaction = reaction;
        postingQueue.push_back(posting);

        if (status_ == idle && postingQueue.size() == 1) {
            status_ = ready;
            manager_->activityAdd(this);
        }
    }

protected:

    Status status_;
    bool scheduled_;
    Time nextTime_;
    Ptr<ActivityElement> main_;
    Ptr<ActivityManager> manager_;
    bool immediateDeliveryFlag_;
    PostingQueue postingQueue;


    SequentialActivity(const string& name, const Ptr<ActivityManager>& mgr) :
        Activity(name),
        status_(idle),
        scheduled_(false),
        nextTime_(0.0),
        manager_(mgr),
        immediateDeliveryFlag_(true)
    {
        // Nothing else to do.
    }

    ~SequentialActivity() {
        // Just for debugging.
    }


    /**
     * Deliver all pending notifications.
     */
    void deliverAll() {
        while (deliverOne());
    }

    /**
     * Deliver a notification from the queue.
     *
     * Notifications posted during delivery should be processed
     * before postings already in the queue.
     */
    bool deliverOne() {
        if (postingQueue.empty()) {
            if (scheduled_) {
                status_ = scheduled;
                manager_->activityAdd(this);
            } else {
                status_ = idle;
                nextTime_ = 0;
            }
            return false;
        }

        const auto n = postingQueue.size() - 1;

        tryDeliver(postingQueue.front());
        postingQueue.pop_front();

        const auto nn = postingQueue.size();
        for (auto i = n; i < nn; ++i) {
            postingQueue.push_front(postingQueue.back());
            postingQueue.pop_back();
        }

        return true;
    }

    void tryDeliver(const Posting& posting) {
        try {
            posting.reaction();
        } catch (const std::exception& e) {
            undeliverable(e);
        } catch (...) {
            undeliverable();
        }
    }

    void undeliverable(const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    void undeliverable() {
        std::cerr << "Unknown exception caught" << std::endl;
    }

};

#endif
