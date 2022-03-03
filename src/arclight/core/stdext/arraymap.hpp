/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arraymap.hpp
 */

#pragma once

#include "optionalref.hpp"
#include "util/assert.hpp"
#include "util/typetraits.hpp"

#include <unordered_map>



template<class Key, class Value, SizeT DomainSize = 512>
class ArrayMap {

	constexpr static SizeT invalidStorage = -1;

	struct Storage {

		static_assert(std::is_nothrow_constructible_v<Value>, "Value must be nothrow-constructible");

		constexpr Storage() noexcept : next(invalidStorage) {}

		Value value;
		SizeT next;

	};

public:

	template<bool Const>
	class Iterator {

	public:

		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = TT::ConditionalConst<Const, Value>;
		using pointer = value_type*;
		using reference = value_type&;
		using element_type = value_type;

		using StorageT = TT::ConditionalConst<Const, Storage>;
		using ArrayT = TT::ConditionalConst<Const, ArrayMap>;


		constexpr Iterator(ArrayT& array, StorageT* storage, SizeT idx) noexcept : array(array), storage(storage), idx(idx), limit(limitFromIndex(idx)) {}

		template<bool ConstOther> requires (Const >= ConstOther)
		constexpr explicit Iterator(const Iterator<ConstOther>& it) noexcept : Iterator(it.array, it.storage, it.idx) {}

		constexpr reference operator*() const noexcept { return storage->value; }
		constexpr pointer operator->() const noexcept { return **this; }

		constexpr Iterator& operator++() noexcept {

			if (storage->next == invalidStorage) [[unlikely]] {

				storage = nullptr;
				idx = invalidStorage;

			} else {

				if (storage->next >= limit) {

					idx = storage->next;
					storage = &array.storageByIndex(idx);
					limit = limitFromIndex(idx);

				} else {

					SizeT inc = storage->next - idx;
					idx = storage->next;
					storage += inc;

				}

			}

			return *this;

		}

		constexpr Iterator operator++(int) noexcept { auto cpy = *this; ++(*this); return cpy; }

		template<bool ConstOther>
		constexpr bool operator==(const Iterator<ConstOther>& other) const noexcept {
			return idx == other.idx;
		}

		template<bool ConstOther>
		constexpr auto operator<=>(const Iterator<ConstOther>& other) const noexcept {
			return idx <=> other.idx;
		}

	private:

		template<bool ConstOther>
		friend class Iterator;

		constexpr SizeT limitFromIndex(SizeT index) const noexcept {
			return (index / DomainSize + 1) * DomainSize;
		}

		ArrayT& array;
		StorageT* storage;
		SizeT idx;
		SizeT limit;

	};

	using iterator = Iterator<false>;
	using const_iterator = Iterator<true>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;


	ArrayMap() : firstFree(invalidStorage), firstActive(invalidStorage), count(0) {}

	Value& create(Key key) {

		if (contains(key)) {
			return get(key);
		}

		Storage* storage = nullptr;
		SizeT current = firstFree;

		//Add new storages if none left
		if (current == invalidStorage) {

			SizeT baseID = storages.size() * DomainSize;

			auto& container = storages.emplace_back();

			//Subject to C++23 resize_and_overwrite (TODO)
			container.resize(DomainSize);

			for (SizeT i = 0; i < DomainSize - 1; i++) {
				container[i].next = baseID + i + 1;
			}

			container[DomainSize - 1].next = invalidStorage;

			storage = container.data();
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
		indexMap[key] = current;
		count++;

		//Construct and return the new value
		return *new (&storage->value) Value;

	}

	bool contains(Key key) const {
		return indexMap.contains(key);
	}

	void destroy(Key key) {

		if (!contains(key)) {
			return;
		}

		SizeT current = indexMap[key];

		//Destruct
		Storage& storage = storageByIndex(current);
		storage.value.~Value();

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
					for (SizeT i = current - 2; i >= firstFree; i--) {

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
					arc_force_assert("Illegal condition");

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
					for (SizeT i = current - 2; i >= firstActive; i--) {

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
					arc_force_assert("Illegal condition");

				}

			}

		} else {

			//We have the first storage, trivial case
			firstActive = storage.next;
			storage.next = firstFree;
			firstFree = 0;

		}

		//Delete the entry
		indexMap.erase(key);
		count--;

	}

	void clear() {

		indexMap.clear();
		storages.clear();

		firstActive = invalidStorage;
		firstFree = invalidStorage;
		count = 0;

	}

	Value& get(Key key) noexcept {
		return const_cast<Value&>(std::as_const(*this).get(key));
	}

	Value& getOrThrow(Key key) {
		return const_cast<Value&>(std::as_const(*this).getOrThrow(key));
	}

	OptionalRef<Value> getOrNull(Key key) noexcept {

		OptionalRef<const Value> ref = std::as_const(*this).getOrNull(key);

		if (ref) {
			return {const_cast<Value&>(ref.getDirect())};
		} else {
			return {};
		}

	}

	const Value& get(Key key) const noexcept {
		return valueByIndex(indexMap.find(key)->second);
	}

	const Value& getOrThrow(Key key) const {
		return valueByIndex(indexMap.at(key));
	}

	OptionalRef<const Value> getOrNull(Key key) const noexcept {

		auto it = indexMap.find(key);

		if (it != indexMap.end()) {
			return valueByIndex(it->second);
		} else {
			return {};
		}

	}

	iterator begin() noexcept {
		return iterator(*this, size() ? &storageByIndex(firstActive) : nullptr, firstActive);
	}

	const_iterator begin() const noexcept {
		return const_iterator(*this, size() ? &storageByIndex(firstActive) : nullptr, firstActive);
	}

	const_iterator cbegin() const noexcept {
		return begin();
	}

	iterator end() noexcept {
		return iterator(*this, nullptr, -1);
	}

	const_iterator end() const noexcept {
		return const_iterator(*this, nullptr, -1);
	}

	const_iterator cend() const noexcept {
		return end();
	}

	reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}

	reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	const_reverse_iterator crend() const noexcept {
		return rend();
	}

	bool empty() const noexcept {
		return !size();
	}

	SizeT size() const noexcept {
		return count;
	}

private:

	Value& valueByIndex(SizeT idx) noexcept {
		return storageByIndex(idx).value;
	}

	const Value& valueByIndex(SizeT idx) const noexcept {
		return storageByIndex(idx).value;
	}

	Storage& storageByIndex(SizeT idx) noexcept {
		return const_cast<Storage&>(std::as_const(*this).storageByIndex(idx));
	}

	const Storage& storageByIndex(SizeT idx) const noexcept {

		SizeT ctIdx = idx / DomainSize;
		SizeT arIdx = idx % DomainSize;

		return storages[ctIdx][arIdx];

	}


	std::unordered_map<Key, SizeT> indexMap;
	std::vector<std::vector<Storage>> storages;

	SizeT firstActive;
	SizeT firstFree;
	SizeT count;

};