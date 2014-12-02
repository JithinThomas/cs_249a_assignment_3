
#ifndef CONN_IMPL_H
#define CONN_IMPL_H

#include <climits>

Ptr<Conn::Path> Conn::shortestPath(const Ptr<Location>& source, 
					   const Ptr<Location>& destination) {
	if ( (source == null) || 
		 (destination == null) || 
		 (!isLocationPartOfTravelNetwork(source)) || 
		 (!isLocationPartOfTravelNetwork(destination)) ) {
		return null;
	}

	if (source == destination) {
		return new Path();
	}

	const auto csp = getCachedShortestPath(source, destination);
	if (csp != null) {
		return csp;
	}

	// TODO: Use a priority heap here in order to save time spent to look up the unvisited node at the least distance
	unordered_map< string, Miles> locsToConsiderNextToMinDist;
	unordered_map< string, Ptr<Path> > locToMinPath;
	std::set< string > locationsVisited;

	const auto sourceName = source->name();
	const auto destName = destination->name();
	const Miles maxPathLength(ULONG_MAX);

	for (auto it = travelNetworkManager_->locationIter(); it != travelNetworkManager_->locationIterEnd(); it++) {
		const auto locName = it->first;
		if (locName == sourceName) {
			locsToConsiderNextToMinDist.insert(LocToMinDistMap::value_type(locName, 0));
		} else {
			locsToConsiderNextToMinDist.insert(LocToMinDistMap::value_type(locName, maxPathLength));
		}
	}

	locToMinPath[sourceName] = new Path();

	// Main loop of Djikstra's algorithm
	while(locsToConsiderNextToMinDist.size() > 0) {
		const auto locName = findNextLocWithMinDist(locsToConsiderNextToMinDist);
		if (locName == "__no_loc_found__") {
			return null;
		}

		const auto loc = travelNetworkManager_->location(locName);
		const auto minPathToLoc = locToMinPath[locName];

		// Update path cache
		if (minPathToLoc->segmentCount() > 0) {
			insertIntoPathL1Cache(sourceName, locName, minPathToLoc);
			insertIntoPathL2Cache(minPathToLoc);
		}

		if (loc == destination) {
			return locToMinPath[destName];
		}

		locsToConsiderNextToMinDist.erase(locName);
		locationsVisited.insert(locName);

		for (auto it2 = loc->sourceSegmentIter(); it2 != loc->sourceSegmentIterEnd(); it2++) {
			const auto seg = *it2;
			const auto dst = seg->destination();
			if (dst != null) {
				const auto dstName = dst->name();

				if (!isKeyPresent(locToMinPath, dstName)) {
					locToMinPath[dstName] = new Path();
				}

				if (!isElemPresentInSet(locationsVisited, dstName)) {
					const auto minPathToLoc = locToMinPath[locName];
					const auto tmp = minPathToLoc->length() + seg->length();
					const auto minPathToDst = locToMinPath[dstName];
					if ( (minPathToDst->segmentCount() == 0) || (minPathToDst->length() > tmp) ) {
						Ptr<Path> p = new Path(minPathToLoc);
						p->segmentIs(seg);
						locToMinPath[dstName] = p;
						locsToConsiderNextToMinDist[dstName] = p->length();
					}
				}
			}
		}
	}

	return null;
}

Ptr<Conn::Path> Conn::getCachedShortestPath(const Ptr<Location>& source, const Ptr<Location>& destination) const {
	const auto sourceName = source->name();
	const auto destName = destination->name();

	auto p1 = tryFetchShortestPathFromL1Cache(sourceName, destName);
	if (p1 != null) {
		return p1;
	}

	return tryFetchShortestPathFromL2Cache(sourceName, destName);
}

Ptr<Conn::Path> Conn::tryFetchShortestPathFromL1Cache(const string& sourceName, const string& destName) const {
	if (isKeyPresent(pathL1Cache_, sourceName)) {
		auto d = pathL1Cache_.at(sourceName);
		if (isKeyPresent(d, destName)) {
			return d[destName];
		}
	}

	return null;
}


Ptr<Conn::Path> Conn::tryFetchShortestPathFromL2Cache(const string& sourceName, const string& destName) const {
	pathL2CacheStats_->requestCountIsIncByOne();

	if (isKeyPresent(pathL2Cache_, destName)) {
		const auto d = pathL2Cache_.at(destName);
		if (isKeyPresent(d, sourceName)) {
			auto p = new Path();
			auto currLocName = sourceName;
			const auto shortestPathsToDest = pathL2Cache_.at(destName);
			while(currLocName != destName) {
				if (!isKeyPresent(shortestPathsToDest, currLocName)) {
					pathL2CacheStats_->missCountIsIncByOne();
					return null;
				}

				const auto segName = shortestPathsToDest.at(currLocName).segName;
				const auto seg = travelNetworkManager_->segment(segName);
				p->segmentIs(seg);
				currLocName = seg->destination()->name();
			}

			pathL2CacheStats_->hitCountIsIncByOne();

			return p;
		}
	}

	pathL2CacheStats_->missCountIsIncByOne();

	return null;
}

void Conn::insertIntoPathL1Cache(const string& sourceName, const string& destName, const Ptr<Path>& path) {
	if (!isKeyPresent(pathL1Cache_, sourceName)) {
		LocNameToPath tmp;
		pathL1Cache_.insert(PathL1Cache::value_type(sourceName, tmp));
	}

	auto d = pathL1Cache_.at(sourceName);
	// TODO: Will this throw an exception if the key is already present in the map
	d.insert(LocNameToPath::value_type(destName, path));
}

void Conn::insertIntoPathL2Cache(const Ptr<Path>& path) {
	const auto numSegments = path->segmentCount();
	if (numSegments > 0) {
		const auto destName = path->destination()->name();
		if (!isKeyPresent(pathL2Cache_, destName)) {
			unordered_map<string, PathL2CacheEntry> tmp;
			pathL2Cache_.insert(PathL2Cache::value_type(destName, tmp));
		}

		const auto it = pathL2Cache_.find(destName);
		for (auto i = 0; i < numSegments; i++) {
			const auto seg = path->segment(i);
			const auto source = seg->source();
			it->second.insert(LocToCacheEntry::value_type(source->name(), {seg->name(), 0}));
		}
	}
}

void Conn::onLocationDel(const Ptr<Location>& location) {
	auto it = pathL2Cache_.find(location->name());
	if (it != pathL2Cache_.end()) {
		pathL2Cache_.erase(it);
	}

	std::set<string> destinationSegNames;
	for (auto it = location->destinationSegmentIter(); it != location->destinationSegmentIterEnd(); it++) {
		const auto seg = *it;
		destinationSegNames.insert(seg->name());
	}

	const auto locName = location->name();

	for (auto it1 = pathL2Cache_.begin(); it1 != pathL2Cache_.end(); it1++) {
		const auto destName = it1->first;
		auto srcToSeg = it1->second;
		auto it2 = srcToSeg.begin();

		while(it2 != srcToSeg.end()) {
			const auto srcName = it2->first;
			const auto segName = it2->second.segName;
			if ( (srcName == locName) || (isElemPresentInSet(destinationSegNames, segName)) ) {
				it1->second.erase(srcName);
			}

			it2++;
		}
	}
}

void Conn::onSegmentDel(const Ptr<Segment>& segment) {
	const auto deletedSegName = segment->name();
	for (auto it1 = pathL2Cache_.begin(); it1 != pathL2Cache_.end(); it1++) {
		const auto destName = it1->first;
		auto srcToSeg = it1->second;
		auto it2 = srcToSeg.begin();

		while(it2 != srcToSeg.end()) {
			const auto srcName = it2->first;
			const auto segName = it2->second.segName;
			if (deletedSegName == segName) {
				it1->second.erase(srcName);
			}

			it2++;
		}
	}
}

void Conn::pathCacheIsEmpty() {
	pathL1Cache_.clear();
	pathL2Cache_.clear();
}

bool Conn::isLocationPartOfTravelNetwork(const Ptr<Location>& loc) {
	if (loc != null) {
		const auto locInNetwork = travelNetworkManager_->location(loc->name());
		if (loc == locInNetwork) {
			return true;
		}
	}

	return false;
}

#endif