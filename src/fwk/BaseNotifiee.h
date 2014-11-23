// Copyright(c) 1993-2006, 2007, David R. Cheriton, all rights reserved.
//
// Edited by Mark Linton for CS 249A Fall 2014.
//

#ifndef FWK_BASENOTIFIEE_H
#define FWK_BASENOTIFIEE_H

/**
 * BaseNotifiee is a template that implements the connection between
 * a notifiee and a notifier. The notifier class must implement a
 * notifiees attribute that is a collection supporting STL-style
 * push_back(notifiee), begin(), end(), and erase(iterator). The notifier
 * defines a Notifiee subclass that extends BaseNotifiee<Notifier>, and
 * this subclass defines a notifierIs method in addition to the notifications.
 * The notifierIs method simply calls the protected connect method:
 *
 *     void notifierIs(const Ptr<Notifier>& notifier) {
 *         connect(notifier, this);
 *     }
 */
template <class Notifier>
class BaseNotifiee : public RootNotifiee {
public:

    const Ptr<Notifier>& notifier() {
        return notifier_;
    }

protected:

    Ptr<Notifier> notifier_;


    BaseNotifiee() {
        // Nothing else to do.
    }

    ~BaseNotifiee() {
        disconnect();
        notifier_ = null;
    }


    /**
     * Connect a notifier to this notifiee. We must pass the notifiee,
     * which should always be this, because the collection element type
     * is Notifier::Notifiee and therefore we have to pass an instance
     * of that type to push_back.
     */
    _noinline
    void connect(
        const Ptr<Notifier>& notifier, typename Notifier::Notifiee* const notifiee
    ) {
        if (notifier != notifier_) {
            if (notifier_ != null) {
                disconnect();
            }

            notifier_ = notifier;

            if (notifier != null) {
                notifier->notifiees().push_back(notifiee);
            }
        }
    }

    /**
     * Disconnect this notifiee from its notifier.
     */
    _noinline
    void disconnect() {
        if (notifier_ != null) {
            auto& notifiees = notifier_->notifiees();
            for (auto i = notifiees.begin(); i != notifiees.end(); ++i) {
                if (*i == this) {
                    notifiees.erase(i);
                    break;
                }
            }
        }
    }

};

#endif
