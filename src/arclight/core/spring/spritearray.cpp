/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritearray.cpp
 */

#include "spritearray.hpp"

#include <new>
#include <utility>



SpriteArray::SpriteArray() :  firstFree(invalidStorage), firstActive(invalidStorage), spriteCount(0) {}



Sprite& SpriteArray::create(u64 id) {

	if (contains(id)) {
		return get(id);
	}

	Storage* storage = nullptr;
	u32 current = firstFree;

	//Add new storages if none left
	if (current == invalidStorage) {

		u32 baseID = sprites.size() * spriteDomainSize;

		auto& spriteContainer = sprites.emplace_back();

		//Subject to C++23 resize_and_overwrite (TODO)
		spriteContainer.resize(spriteDomainSize);

		for (SizeT i = 0; i < spriteDomainSize - 1; i++) {
			spriteContainer[i].next = baseID + i + 1;
		}

		spriteContainer[spriteDomainSize - 1].next = invalidStorage;

		storage = spriteContainer.data();
		current = baseID;

	} else {

		storage = &storageByIndex(current);

	}

	//Fix storage links
	if (current != 0) {

		Storage& prevStorage = storageByIndex(current - 1);

		firstFree = storage->next;
		storage->next = prevStorage.next;
		prevStorage.next = current;

	} else {

		firstFree = storage->next;
		storage->next = firstActive;
		firstActive = current;

	}

	//Add storage to mapping
	spriteIDMapping[id] = current;
	spriteCount++;

	//Construct and return the new sprite
	return *new (&storage->sprite) Sprite;

}



bool SpriteArray::contains(u64 id) const {
	return spriteIDMapping.contains(id);
}



void SpriteArray::destroy(u64 id) {

	if (!contains(id)) {
		return;
	}

	u32 current = spriteIDMapping[id];

	//Destruct
	Storage& storage = storageByIndex(current);
	storage.sprite.~Sprite();

	/*
	 *  We have to find the previous free/active storages.
	 *  This is not trivial since we have no information about our predecessors.
	 *  The approach here is: Constrain as much as we can, then iteratively find those storages
	 */
	if (current != 0) {


		//The preceding storage is a good start since it has to be either free or active; we are interested in either case.
		Storage& prevStorage = storageByIndex(current - 1);

		if (prevStorage.next == current) {

			//The previous storage was active, we seek for a free one preceding it
			//Lower limit is firstFree
			if (current < firstFree) {

				//There is no free storage preceding it, current is the first free storage
				prevStorage.next = storage.next;
				storage.next = firstFree;
				firstFree = current;

			} else {

				//Search for a free one backwards
				for (u32 i = current - 2; i >= firstFree; i--) {

					Storage& precStorage = storageByIndex(i);

					//A free storage would point past current since all active ones would only point to less-equal current
					//This can be faster than checking if the storage breaks the active chain since each iteration is independent
					if (precStorage.next > current) {

						prevStorage.next = storage.next;
						storage.next = precStorage.next;
						precStorage.next = current;
						break;

					}

				}

				//Impossible if the array is intact
				arc_force_assert("Illegal sprite array condition");

			}

		} else {

			//Previous storage is free, seek for a preceding active one
			if (current == firstActive) {

				//There is no preceding active storage
				firstActive = storage.next;
				storage.next = prevStorage.next;
				prevStorage.next = current;

			} else {

				//Search for an active one backwards
				for (u32 i = current - 2; i >= firstActive; i--) {

					Storage& precStorage = storageByIndex(i);

					//The preceding active storage would link to current
					if (precStorage.next == current) {

						precStorage.next = storage.next;
						storage.next = prevStorage.next;
						prevStorage.next = current;
						break;

					}

				}

				//Impossible if the array is intact
				arc_force_assert("Illegal sprite array condition");

			}

		}

	} else {

		//We have the first storage, trivial case
		firstActive = storage.next;
		storage.next = firstFree;
		firstFree = 0;

	}

	//Delete the entry
	spriteIDMapping.erase(id);
	spriteCount--;

}



void SpriteArray::clear() {

	spriteIDMapping.clear();
	sprites.clear();

	firstActive = invalidStorage;
	firstFree = invalidStorage;
	spriteCount = 0;

}



Sprite& SpriteArray::get(u64 id) noexcept {
	return const_cast<Sprite&>(std::as_const(*this).get(id));
}



Sprite& SpriteArray::getOrThrow(u64 id) {
	return const_cast<Sprite&>(std::as_const(*this).getOrThrow(id));
}



OptionalRef<Sprite> SpriteArray::getOrNull(u64 id) noexcept {

	OptionalRef<const Sprite> ref = std::as_const(*this).getOrNull(id);

	if (ref) {
		return {const_cast<Sprite&>(ref.getDirect())};
	} else {
		return {};
	}

}



const Sprite& SpriteArray::get(u64 id) const noexcept {
	return spriteByIndex(spriteIDMapping.find(id)->second);
}



const Sprite& SpriteArray::getOrThrow(u64 id) const {
	return spriteByIndex(spriteIDMapping.at(id));
}



OptionalRef<const Sprite> SpriteArray::getOrNull(u64 id) const noexcept {

	auto it = spriteIDMapping.find(id);

	if (it != spriteIDMapping.end()) {
		return spriteByIndex(it->second);
	} else {
		return {};
	}

}



bool SpriteArray::empty() const noexcept {
	return !size();
}



u32 SpriteArray::size() const noexcept {
	return spriteCount;
}



const Sprite& SpriteArray::spriteByIndex(u32 idx) const noexcept {
	return storageByIndex(idx).sprite;
}



SpriteArray::Storage& SpriteArray::storageByIndex(u32 idx) noexcept {
	return const_cast<Storage&>(std::as_const(*this).storageByIndex(idx));
}



const SpriteArray::Storage& SpriteArray::storageByIndex(u32 idx) const noexcept {

	u32 ctIdx = idx / spriteDomainSize;
	u32 arIdx = idx % spriteDomainSize;

	return sprites[ctIdx][arIdx];

}