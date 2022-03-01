/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritegroup.cpp
 */

#include "spritegroup.hpp"
#include "spring.hpp"



SpriteBatch& SpriteGroup::getBatch() {
	return batch;
}



const SpriteBatch& SpriteGroup::getBatch() const {
	return batch;
}



const CTAllocationTable& SpriteGroup::getCTAllocationTable() const {
	return ctAllocationTable;
}



const CTBindings& SpriteGroup::getCTBindings() const {
	return ctBindings;
}



void SpriteGroup::addCTReference(u32 ctID) {

	auto it = ctReferences.find(ctID);

	if (it == ctReferences.end()) {

		if (ctID >= ctGroupTableBuffer.getTableSize()) {
			ctGroupTableBuffer.setTableSize(ctID + 1);
		}

		ctReferences.try_emplace(ctID, 1);
		ctUpdate = true;

	} else {

		it->second++;

	}

}



void SpriteGroup::removeCTReference(u32 ctID) {

	u32& refCount = ctReferences[ctID];

	if (refCount <= 1) {

		ctReferences.erase(ctID);
		ctUpdate = true;

	} else {

		refCount--;

	}

}



void SpriteGroup::prepareCTTable(const CTAllocationTable& prev, bool forceRecalculate) {

	if (!ctUpdate && !forceRecalculate) {
		return;
	}

	if (ctReferences.size() > prev.size()) {
		Log::warn("Sprite Group", "Cannot allocate %d group CTs, requested %d", ctReferences.size(), prev.size());
	}

	ctBindings.clear();
	ctAllocationTable.resize(prev.size());
	std::fill(ctAllocationTable.begin(), ctAllocationTable.end(), Spring::unusedCTSlot);

	std::vector<u32> remainingCTs;

	for (const auto& [ctID, refCount] : ctReferences) {

		bool added = false;

		for (SizeT i = 0; i < prev.size(); i++) {

			if (prev[i] == ctID) {

				ctGroupTableBuffer.setSlot(ctID, i);
				ctAllocationTable[i] = ctID;
				added = true;
				break;

			}

		}

		if (!added) {
			remainingCTs.push_back(ctID);
		}

	}

	SizeT searchStartIndex = 0;

	for (u32 remCT : remainingCTs) {

		for (SizeT i = searchStartIndex; i < ctAllocationTable.size(); i++) {

			if (ctAllocationTable[i] == Spring::unusedCTSlot) {

				ctGroupTableBuffer.setSlot(remCT, i);
				ctAllocationTable[i] = remCT;
				ctBindings.emplace_back(remCT, static_cast<u32>(i));
				searchStartIndex = i + 1;

				break;

			}

		}

	}

	ctUpdate = false;

}



void SpriteGroup::syncData() {

	batch.synchronize();
	ctGroupTableBuffer.update();

}



void SpriteGroup::render() {

	ctGroupTableBuffer.bind();
	batch.render();

}