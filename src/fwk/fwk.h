// Copyright (C) 1993-2002 David R. Cheriton.  All rights reserved.
//
// Edited by Mark Linton for CS 249A Fall 2014.
//

#ifndef FWK_FWK_H
#define FWK_FWK_H

//
// Common framework pragmas, types, and includes.
//

#ifdef _MSC_VER

    //
    // Ignore overzealous VC++ warnings.
    //
    // 4100: Unreferenced formal parameter message.
    // 4250: Inherits symbol via dominance.
    // 4355: 'this' : used in base member initializer list
    // 4511: Copy constructor could not be generated.
    // 4512: Assignment operator could not be generated.
    // 4514: Unreferenced inline function has been removed.
    // 4522: Multiple assignment operators of a single type.
    // 4624: Destructor could not be generated because
    //       a base class destructor is inaccessible.
    // 4625: Copy constructor could not be generated because
    //       a base class copy constructor is inaccessible.
    // 4626: Assignment operator could not be generated because
    //       a base class copy constructor is inaccessible.
    //
#   pragma warning( disable : 4100 4250 4355 4511 4512 4514 4522 4624 4625 4626 )

#   define _noinline __declspec(noinline)

#   if _MSC_VER >= 1600
#       define null (nullptr)
#   else
#       define null (0)

        typedef int nullptr_t;
#   endif

#endif

#ifdef __clang__
#   define _noinline __attribute__((noinline))
#   define null (nullptr)
#endif

#ifdef __GNUC__
#   define _noinline __attribute__((noinline))
#   define null (nullptr)
#endif

#ifndef _noinline
#   define _noinline /**/
#endif

#ifndef null
#   define null (0)
#endif


#ifdef _MSC_VER

    typedef __int8 S8;
    typedef unsigned __int8 U8;
    typedef __int16 S16;
    typedef unsigned __int16 U16;
    typedef __int32 S32;
    typedef unsigned __int32 U32;
    typedef __int64 S64;
    typedef unsigned __int64 U64;

#else

#   include <stdint.h>

    typedef int8_t S8;
    typedef uint8_t U8;
    typedef int16_t S16;
    typedef uint16_t U16;
    typedef int32_t S32;
    typedef uint32_t U32;
    typedef int64_t S64;
    typedef uint64_t U64;

#endif

// Support for DateTime stuff
#ifdef _WIN32
#   include <Windows.h>
#   include <sys/timeb.h>
#   include <time.h>
#else
#   include <sys/time.h>
#   include <time.h>
#   include <unistd.h>
#endif

// Used by fwk classes

#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

using std::string;


namespace fwk {

#   include "fwk/Nominal.h"
#   include "fwk/Ordinal.h"
#   include "fwk/SystemTime.h"
#   include "fwk/Time.h"
#   include "fwk/DateTime.h"

#   include "fwk/Ptr.h"
#   include "fwk/PtrInterface.h"
#   include "fwk/ActivityElement.h"
#   include "fwk/RootNotifiee.h"
#   include "fwk/BaseNotifiee.h"
#   include "fwk/NamedInterface.h"
#   include "fwk/Exception.h"
#   include "fwk/Activity.h"
#   include "fwk/ActivityManager.h"
#   include "fwk/NotifierLib.h"
#   include "fwk/SequentialActivity.h"
#   include "fwk/SequentialManager.h"

}

#endif
