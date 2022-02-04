/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritearray.hpp
 */

#pragma once

#include "sprite.hpp"
#include "stdext/optionalref.hpp"
#include "util/uuid.hpp"

#include <unordered_map>



class SpriteArray {


	constexpr static u32 spriteDomainSize = 512;
	constexpr static u32 invalidStorage = -1;

	struct Storage {

		static_assert(std::is_nothrow_constructible_v<Sprite>, "Sprite must be nothrow-constructible");

		constexpr Storage() noexcept : next(invalidStorage) {}

		Sprite sprite;
		u32 next;

	};

public:

	template<bool Const>
	class Iterator {

	public:

		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = TT::ConditionalConst<Const, Sprite>;
		using pointer = value_type*;
		using reference = value_type&;
		using element_type = value_type;

		using StorageT = TT::ConditionalConst<Const, Storage>;
		using ArrayT = TT::ConditionalConst<Const, SpriteArray>;


		constexpr Iterator(ArrayT& array, StorageT* storage, u32 idx) noexcept : array(array), storage(storage), idx(idx), limit(limitFromIndex(idx)) {}

		template<bool ConstOther> requires (Const >= ConstOther)
		constexpr explicit Iterator(const Iterator<ConstOther>& it) noexcept : Iterator(it.array, it.storage, it.idx) {}

		constexpr reference operator*() const noexcept { return storage->sprite; }
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

					u32 inc = storage->next - idx;
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

		constexpr u32 limitFromIndex(u32 index) const noexcept {
			return (index / spriteDomainSize + 1) * spriteDomainSize;
		}

		ArrayT& array;
		StorageT* storage;
		u32 idx;
		u32 limit;

	};

	using iterator = Iterator<false>;
	using const_iterator = Iterator<true>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;


	SpriteArray();

	Sprite& create(u64 id);
	bool contains(u64 id) const;
	void destroy(u64 id);
	void clear();

	Sprite& get(u64 id) noexcept;
	Sprite& getOrThrow(u64 id);
	OptionalRef<Sprite> getOrNull(u64 id) noexcept;

	const Sprite& get(u64 id) const noexcept;
	const Sprite& getOrThrow(u64 id) const;
	OptionalRef<const Sprite> getOrNull(u64 id) const noexcept;

	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;

	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

	reverse_iterator rbegin() noexcept;
	const_reverse_iterator rbegin() const noexcept;
	const_reverse_iterator crbegin() const noexcept;

	reverse_iterator rend() noexcept;
	const_reverse_iterator rend() const noexcept;
	const_reverse_iterator crend() const noexcept;

	bool empty() const noexcept;
	u32 size() const noexcept;

private:

	Sprite& spriteByIndex(u32 idx) noexcept;
	const Sprite& spriteByIndex(u32 idx) const noexcept;

	Storage& storageByIndex(u32 idx) noexcept;
	const Storage& storageByIndex(u32 idx) const noexcept;


	std::unordered_map<u64, u32> spriteIDMapping;
	std::vector<std::vector<Storage>> sprites;

	u32 firstActive;
	u32 firstFree;
	u32 spriteCount;

};