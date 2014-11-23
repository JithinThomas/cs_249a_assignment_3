/**
 * Abstract base class for objects associated with activities,
 * including notifiees.
 */

#ifndef FWK_ACTIVITYELEMENT_H
#define FWK_ACTIVITYELEMENT_H

class Activity;

class ActivityElement : public PtrInterface {
public:

    /**
     * Return the activity that controls this element.
     */
    Ptr<Activity> activity() {
        return activity_;
    }


    /**
     * Notification from that activity that this element is the main element
     * of the activity.
     *
     * Defined here instead of Activity::Notifiee to simplify
     * the class hierarchy.
     */
    virtual void onMain() { }

protected:

    Ptr<Activity> activity_;


    // Defined in Activity to handle cross-reference.
    ActivityElement();

    ActivityElement(const Ptr<Activity>& activity) :
        activity_(activity)
    {
        // Nothing else to do.
    }

};

#endif
