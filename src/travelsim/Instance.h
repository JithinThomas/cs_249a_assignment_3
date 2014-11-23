#ifndef TRAVELSIM_INSTANCE_H
#define TRAVELSIM_INSTANCE_H

#include "fwk/fwk.h"
#include "TravelNetworkManager.h"

/**
 * Instance provides the external interface to entities in our simulation.
 * For flexibility in future extensions, clients always refer to instances and
 * their attributes by name and attribute values are represented as strings.
 */
class Instance : public fwk::NamedInterface {
public:

    /**
     * Returns the value of the attribute with the given name.
     * If an attribute with the given name has not been set explicitly
     * then the return value is the default value for the attribute,
     * if any, or the empty string ("") otherwise.
     */
    virtual string attribute(const string& attributeName) = 0;

    /**
     * Modify the named attribute to have the value specified by
     * the given string.
     *
     * Setting an attribute on an instance can implicitly modify attributes
     * on other instances.
     */
    virtual void attributeIs(const string& name, const string& value) = 0;

protected:

    Ptr<TravelNetworkManager> travelManager() const {
        return travelManager_;
    }

    void travelManagerIs(const Ptr<TravelNetworkManager>& travelManager) {
        travelManager_ = travelManager;
    }

    Instance(const string& name) : NamedInterface(name) {
        // Nothing else to do.
    }

    Ptr<TravelNetworkManager> travelManager_;  

};

#endif
