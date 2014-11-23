#ifndef TRAVELSIM_INSTANCEMANAGER_H
#define TRAVELSIM_INSTANCEMANAGER_H

#include "Instance.h"

using fwk::Ptr;

/**
 * InstanceManager provides an interface to create and destroy
 * named instances of specified types of objects.
 */
class InstanceManager : public fwk::PtrInterface {
public:

    /**
     * Returns a newly-created instance with the specified name.
     * The spec paramater indicates additional information about
     * the instance, such as its type.
     */
    Ptr<Instance> instanceNew(const string& name, const string& spec) {
        const auto iter = instanceMap.find(name);
        if (iter != instanceMap.end()) {
            throw fwk::NameInUseException(name);
        }

        const auto instance = createInstance(name, spec);

        if (instance != null) {
            instanceMap[name] = instance;
        }

        return instance;
    }

    /**
     * Return the instance with the given name or null if there is none.
     */
    Ptr<Instance> instance(const string& name) {
        const auto iter = instanceMap.find(name);
        if (iter != instanceMap.end()) {
            return iter->second;
        }

        return null;
    }

    /**
     * Delete the instance with the given name, if there is one.
     */
    void instanceDel(const string& name) {
        const auto iter = instanceMap.find(name);
        if (iter != instanceMap.end()) {
            instanceMap.erase(iter);
        }
    }

protected:

    std::unordered_map< string, Ptr<Instance> > instanceMap;


    /**
     * Create an instance from a given spec. Specific instance managers
     * should override this method.
     *
     * The instance name is included in case the instance wants
     * to store that itself.
     */
    virtual Ptr<Instance> createInstance(const string& name, const string& spec) {
        return null;
    }

};

#endif
