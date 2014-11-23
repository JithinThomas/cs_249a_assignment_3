// Root class for notifiees.
//
// Copyright(c) 1993-2006, 2007, David R. Cheriton, all rights reserved.
//
// Edited by Mark Linton for CS 249A Fall 2014.
//

#ifndef FWK_ROOTNOTIFIEE_H
#define FWK_ROOTNOTIFIEE_H

class RootNotifiee : public ActivityElement {
public:

    /**
     * Special notification that an exception occurred
     * delivering a notification.
     */
    virtual void onNotificationException() { }

};

#endif
