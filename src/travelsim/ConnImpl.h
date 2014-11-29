
#ifndef CONN_IMPL_H
#define CONN_IMPL_H

#include <climits>

Ptr<Conn::Path> Conn::shortestPath(const Ptr<Location>& source, 
					   const Ptr<Location>& destination) {
	const auto csp = getCachedShortestPath(source, destination);
	if (csp != null) {
		return csp;
	}

	// TODO: Use a priority heap here in order to save time spent to look up the unvisited node at the least distance
	unordered_map< string, Miles> locsToConsiderNextToMinDist;
	unordered_map< string, Ptr<Path> > locToMinPath;
	std::set< string > locationsCompleted;

	const auto sourceName = source->name();
	const Miles maxPathLength(ULONG_MAX);

	for (auto it = travelNetworkManager_->locationIter(); it != travelNetworkManager_->locationIterEnd(); it++) {
		const auto locName = it->first;
		if (locName == sourceName) {
			locsToConsiderNextToMinDist.insert(LocToMinDistMap::value_type(locName, 0));
		} else {
			locsToConsiderNextToMinDist.insert(LocToMinDistMap::value_type(locName, maxPathLength));
		}

		locToMinPath[locName] = new Path();
	}

	while(locsToConsiderNextToMinDist.size() > 0) {
		const auto locName = findNextLocWithMinDist(locsToConsiderNextToMinDist);
		const auto loc = travelNetworkManager_->location(locName);

		if (loc == destination) {
			return locToMinPath[locName];
		}

		locsToConsiderNextToMinDist.erase(locName);
		locationsCompleted.insert(loc->name());

		for (auto it2 = loc->sourceSegmentIter(); it2 != loc->sourceSegmentIterEnd(); it2++) {
			const auto seg = *it2;
			const auto dst = seg->destination();
			const auto dstName = dst->name();

			if (!isKeyPresent(locToMinPath, dstName)) {
				locToMinPath[dstName] = new Path();
			}

			if (!isElemPresentInSet(locationsCompleted, dstName)) {
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

	return new Path();
}

#endif