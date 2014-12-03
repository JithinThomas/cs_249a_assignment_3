
#ifndef COMMONLIB_H
#define COMMONLIB_H

#include <ctype.h>
#include <ostream>
#include <iostream>
#include <stdio.h>
#include <utility> 

#include "fwk/fwk.h"

using namespace fwk;
using fwk::Ordinal;
using fwk::Ptr;

using namespace std;
using std::cerr;
using std::endl;

enum ErrorLevel {
    WARNING,
    ERROR
};

template<class T, class V>
bool isInstanceOf(const Ptr<T> p) {
    if (p != null) {
        return (dynamic_cast<V*>(p.ptr()) != null);
    }

    return false;
}

void logError(ErrorLevel errorLevel, const string& err) {
    switch(errorLevel) {
        case WARNING: 
            cerr << "[WARNING]: " << err << endl;
            break;
        case ERROR:
            cerr << "[ERROR]: " << err << endl;
            break;
        default:
            cerr << "[INTERNAL ERROR]: Unexpected error level: " << errorLevel << endl;
    }
}

static void logEntryNew(const Time t, const string& s) {
    std::cout << timeAsString(t) << " " << s << std::endl;
}

bool isNumber(const string& str) {
    for (auto i = 0u; i < str.length(); i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }

    return true;
}

template<typename K, typename V>
bool isKeyPresent(const unordered_map<K,V>& map, const K& key) {
    return (map.find(key) != map.end());
}

template<typename K, typename V>
void tryInsertKeyValuePair(const unordered_map<K,V>& map, const K& key, const V& value) {
    if (!isKeyPresent(map, key)) {
        insertKeyValuePair(map, key, value);
    }
}

template<typename K, typename V>
void insertKeyValuePair(const unordered_map<K,V>& map, const K& key, const V& value) {
    const auto p = std::make_pair(key, value);
    map.insert(p);
}

#endif
