// Copyright (c) 1993-2007 David R. Cheriton, all rights reserved.
// PtrInterface.h
//
// Edited by Mark Linton for CS 249A Fall 2014.
//

#ifndef FWK_PTRINTERFACE_H
#define FWK_PTRINTERFACE_H

class PtrInterface {
public:

    PtrInterface() :
        ref_(0)
    {
        // Nothing else to do.
    }


    unsigned long references() const {
        return ref_;
    }

    void newRef() {
        ref_ += 1;
    }

    void deleteRef() {
        ref_ -= 1;
        if (ref_ == 0) {
            onZeroReferences();
        }
    }

protected:

    virtual ~PtrInterface() {
        // Nothing to do.
    }

    virtual void onZeroReferences() {
        delete this;
    }

private:

    long unsigned ref_;

};

#endif
