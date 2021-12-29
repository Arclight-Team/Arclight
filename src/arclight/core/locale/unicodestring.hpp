/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 unicodestring.hpp
 */

#pragma once

#include "unicode.hpp"

#include <string>
#include <span>



template<Unicode::Encoding E>
struct UnicodeStringHelper {

    using DistanceContainer = std::vector<SizeT>;
    constexpr static SizeT CPDistance = Unicode::isUTF8<E>() ? 16 : 32;

    constexpr UnicodeStringHelper() noexcept : totalCodepoints(0) {}

    DistanceContainer distances;
    SizeT totalCodepoints;

};

template<>
struct UnicodeStringHelper<Unicode::UTF32> {};



template<Unicode::Encoding E>
class UnicodeString : private UnicodeStringHelper<E> {

public:

    constexpr static Unicode::Encoding EncodingType = E;

    using CharT = typename Unicode::UTFEncodingTraits<E>::Type;

    using value_type = CharT;
    using pointer = value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using const_pointer = const value_type*;

    using Container = std::basic_string<CharT>;
    using Codepoint = Unicode::Codepoint;
    using Base = UnicodeStringHelper<E>;

    static_assert(std::is_nothrow_copy_constructible_v<CharT> && std::is_nothrow_copy_assignable_v<CharT>, "Type cannot throw on copy");

    
    //Bidirectional iterator for UTF-8/16
    //Contiguous iterator for UTF-32
    template<bool Const>
    class Iterator {

    public:
        
        using iterator_concept  = std::conditional_t<Unicode::isUTF32<E>(), std::contiguous_iterator_tag, std::bidirectional_iterator_tag>;
        using iterator_category = iterator_concept;

        using difference_type   = std::ptrdiff_t;
        using value_type        = std::conditional_t<Unicode::isUTF32<E>() && !Const, UnicodeString<E>::value_type, const UnicodeString<E>::value_type>;
        using pointer           = value_type*;
        using reference         = value_type&;


        constexpr Iterator(SizeT cpIndex, pointer p) noexcept : cpIdx(cpIndex), sp(p) {}
        constexpr Iterator(const Iterator<true>& it) noexcept requires (!Const) = delete;

        constexpr reference operator*() const noexcept { return *sp; }
        constexpr pointer operator->() const noexcept { return sp; }

        constexpr Iterator& operator++() noexcept      { advance(); return *this; }
        constexpr Iterator& operator--() noexcept      { retreat(); return *this; }

        constexpr Iterator operator++(int) noexcept    { auto cpy = *this; ++(*this); return cpy; }
        constexpr Iterator operator--(int) noexcept    { auto cpy = *this; --(*this); return cpy; }

        constexpr bool operator==(const Iterator& other) const noexcept {
            return sp == other.sp;
        }

        template<bool ConstOther>
        constexpr auto operator<=>(const Iterator<ConstOther>& other) const noexcept {
            return sp <=> other.sp;
        }

        constexpr Codepoint getCodepoint() const noexcept {
            return Unicode::getCodepoint<E>(sp);
        }

        constexpr SizeT getCodepointIndex() const noexcept {
            return cpIdx;
        }

		constexpr pointer getPointer() const noexcept { return sp; }

        //Contiguous extension for UTF-32
        constexpr Iterator operator+(SizeT n) const noexcept requires (Unicode::isUTF32<E>())    { return Iterator(cpIdx + n, sp + n); }
        constexpr Iterator operator-(SizeT n) const noexcept requires (Unicode::isUTF32<E>())    { return Iterator(cpIdx - n, sp - n); }
        constexpr Iterator& operator+=(SizeT n) noexcept requires (Unicode::isUTF32<E>())        { sp += n; cpIdx += n; return *this; }
        constexpr Iterator& operator-=(SizeT n) noexcept requires (Unicode::isUTF32<E>())        { sp -= n; cpIdx -= n; return *this; }
        constexpr reference operator[](SizeT n) const noexcept requires (Unicode::isUTF32<E>())  { return *(*this + n); }

        template<bool ConstOther>
        constexpr difference_type operator-(const Iterator<ConstOther>& other) const noexcept requires (Unicode::isUTF32<E>()) { return sp - other.sp; }

    private:

        template<bool ConstOther>
        friend class Iterator;

        constexpr void advance() noexcept {
            sp += Unicode::getEncodedSize<E>(sp);
            cpIdx++;
        }

        constexpr void retreat() noexcept {
            sp -= Unicode::getEncodedSizeBackwards<E>(sp);
            cpIdx--;
        }

        SizeT cpIdx;
        pointer sp;

    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;


    constexpr UnicodeString() noexcept = default;


    constexpr Codepoint at(SizeT index) {
        return getCodepointOrThrow(index);
    }

    constexpr Codepoint operator[](SizeT index) const noexcept(noexcept(std::declval<Container>().operator[](0))) {
        return getCodepointDirect(index);
    }

    constexpr Codepoint front() noexcept(noexcept(std::declval<decltype(*this)>().operator[](0))) {
        return Unicode::getCodepoint<E>(str.data());
    }

    constexpr Codepoint back() noexcept(noexcept(std::declval<decltype(*this)>().operator[](0))) {

        const CharT* backPtr = str.data() + str.size();
        backPtr -= Unicode::getEncodedSizeBackwards<E>(backPtr);

        return Unicode::getCodepoint<E>(backPtr);

    }

    constexpr CharT* data() noexcept {
        return str.data();
    }

    constexpr const CharT* data() const noexcept {
        return str.data();
    }



    constexpr iterator begin() noexcept {
        return iterator(0, str.data());
    }

    constexpr const_iterator begin() const noexcept {
        return iterator(0, str.data());
    }

    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(0, str.data());
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return reverse_iterator(cbegin());
    }

    constexpr iterator end() noexcept {
        return iterator(size(), str.data() + str.size());
    }

    constexpr const_iterator end() const noexcept {
        return iterator(size(), str.data() + str.size());
    }

    constexpr const_iterator cend() const noexcept {
        return const_iterator(size(), str.data() + str.size());
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return reverse_iterator(cend());
    }



    constexpr bool empty() const noexcept {
        return str.empty();
    }

    constexpr SizeT size() const noexcept {

        if constexpr (Unicode::isUTF32<E>()) {
            return str.size();
        } else {
            return Base::totalCodepoints;
        }

    }

    constexpr SizeT length() const noexcept {
        return size();
    }

    constexpr SizeT maxSize() const noexcept {
        return str.max_size();
    }

    constexpr void reserve(SizeT newCapacity) {
        str.reserve(newCapacity);
    }

    constexpr SizeT rawSize() const noexcept {
        return str.size();
    }

    constexpr SizeT capacity() const noexcept {
        return str.capacity();
    }

    constexpr void shrinkToFit() {
        str.shrink_to_fit();
    }



    constexpr void clear() noexcept {

        str.clear();

        if constexpr (!Unicode::isUTF32<E>()) {

			Base::distances.clear();
            Base::totalCodepoints = 0;

        }

    }

    constexpr UnicodeString& insert(SizeT index, Codepoint codepoint) {
        return insert(index, 1, codepoint);
    }

    constexpr UnicodeString& insert(SizeT index, SizeT count, Codepoint codepoint) {
        insertCodepoint(index, codepoint, count);
        return *this;
    }

    constexpr UnicodeString& insert(SizeT index, const Codepoint* s, SizeT count) {

        std::span<const Codepoint> span {s, count};
        insert(iterator(index, getInsertionOffsetOrThrow(index)), span.begin(), span.end());

        return *this;

    }

    constexpr UnicodeString& insert(SizeT index, const Codepoint* s) {

        const Codepoint* p = s;
        while(*p++);

        return insert(index, s, p - s);

    }

    constexpr UnicodeString& insert(SizeT index, const UnicodeString& ustr) {

        SizeT offset = getInsertionOffsetOrThrow(index);

        str.insert(offset, ustr.str);
        restoreDistanceRange(index, offset, size() + ustr.size());

        return *this;

    }

    constexpr UnicodeString& insert(SizeT index, const UnicodeString& ustr, SizeT ssIndex, SizeT ssSize = SizeT(-1)) {

        if(ssIndex > ustr.size()) {
            throw std::out_of_range("Insertion substring index out of range");
        }

        if(ssSize > ustr.size() - ssIndex) {
            ssSize = ustr.size() - ssIndex;
        }

        SizeT offset = getInsertionOffsetOrThrow(index);
        SizeT ssStart = ustr.getInsertionOffsetDirect(ssIndex);
        SizeT ssEnd = ustr.getInsertionOffsetDirect(ssIndex + ssSize);

        str.insert(offset, ustr.str, ssStart, ssEnd - ssStart);
        restoreDistanceRange(index, offset, size() + ssSize);

        return *this;

    }

    constexpr iterator insert(const const_iterator& pos, Codepoint codepoint) {
        return insert(pos, 1, codepoint);
    }

    constexpr iterator insert(const const_iterator& pos, SizeT count, Codepoint codepoint) {

	    SizeT cpIdx = pos.getCodepointIndex();

	    //The container storage might get reallocated so get the distance to the start
	    SizeT distance = pos.getPointer() - str.data();

	    insertCodepoint(cpIdx, codepoint, count);

	    //Construct a new iterator to cpIdx and return it
	    return iterator(cpIdx, str.data() + distance);

	}

    template<std::input_iterator InputIt>
    constexpr iterator insert(const const_iterator& pos, const InputIt& first, const InputIt& last) {
        return insertByRange(pos, first, last);
    }

    constexpr iterator insert(const const_iterator& pos, std::initializer_list<Codepoint> list) {
        return insert(pos, list.begin(), list.end());
    }

    template<class StringView>
    constexpr UnicodeString& insert(SizeT index, const StringView& sv) requires (ImpConvertible<const StringView&, std::basic_string_view<Codepoint>> && !ImpConvertible<const StringView&, const Codepoint*>) {

		std::basic_string_view<Codepoint> view = sv;
		insert(index, view.data(), view.size());

		return *this;
        
    }

	template<class StringView>
	constexpr UnicodeString& insert(SizeT index, const StringView& sv, SizeT svIndex, SizeT svSize = SizeT(-1)) requires (ImpConvertible<const StringView&, std::basic_string_view<Codepoint>> && !ImpConvertible<const StringView&, const Codepoint*>) {

		std::basic_string_view<Codepoint> view = sv;

		if(svIndex > view.size()) {
			throw std::out_of_range("Insertion substring index out of range");
		}

		if(svSize > view.size() - svIndex) {
			svSize = view.size() - svIndex;
		}

		insert(index, view.data() + svIndex, svSize);

		return *this;

	}


	constexpr UnicodeString& erase(SizeT index, SizeT count = SizeT(-1)) {

		SizeT offset = getInsertionOffsetOrThrow(index);

		if(count > size() - index) {
			count = size() - index;
		}

		SizeT end = getInsertionOffsetDirect(index + count);
		str.erase(offset, end - offset);
		restoreDistanceRange(index, offset, size() - count);

		return *this;

	}

	constexpr iterator erase(const const_iterator& pos) {

		SizeT index = pos.getCodepointIndex();
		SizeT offset = pos.getPointer() - str.data();

		erase(index, 1);

		return iterator(index, str.data() + offset);

	}

	constexpr iterator erase(const const_iterator& first, const_iterator last) {

		//Clamp last to first
		if(last < first) {
			last = first;
		}

		SizeT index = first.getCodepointIndex();
		SizeT endIndex = last.getCodepointIndex();
		SizeT offset = first.getPointer() - str.data();

		erase(index, endIndex - index);

		return iterator(index, str.data() + offset);

	}


	constexpr void push_back(Codepoint codepoint) {

		CharT decomposed[Unicode::UTFEncodingTraits<E>::MaxDecomposed];
		SizeT count = Unicode::toUTF<E>(codepoint, decomposed);
		SizeT ssize = str.size();

		str.insert(ssize, decomposed, count);
		restoreDistanceRange(size(), ssize, size() + 1);

	}


	constexpr void pop_back() {
		erase(--end());
	}


	constexpr UnicodeString& append(Codepoint codepoint) {
		return append(1, codepoint);
	}

	constexpr UnicodeString& append(SizeT count, Codepoint codepoint) {
		appendCodepoint(codepoint, count);
		return *this;
	}

	constexpr UnicodeString& append(const UnicodeString& ustr) {

		SizeT oldSize = str.size();
		str.append(ustr.str);

		restoreDistanceRange(size(), oldSize, size() + ustr.size());

		return *this;

	}

	constexpr UnicodeString& append(const UnicodeString& ustr, SizeT ssIndex, SizeT ssSize = SizeT(-1)) {

		if(ssIndex > ustr.size()) {
			throw std::out_of_range("Appending substring index out of range");
		}

		if(ssSize > ustr.size() - ssIndex) {
			ssSize = ustr.size() - ssIndex;
		}

		SizeT oldSize = str.size();
		SizeT ssStart = ustr.getInsertionOffsetDirect(ssIndex);
		SizeT ssEnd = ustr.getInsertionOffsetDirect(ssIndex + ssSize);

		str.append(ustr.str, ssStart, ssEnd - ssStart);
		restoreDistanceRange(size(), oldSize, size() + ssSize);

		return *this;

	}

	constexpr UnicodeString& append(const Codepoint* s, SizeT count) {

		std::span<const Codepoint> span {s, count};
		append(span.begin(), span.end());

		return *this;

	}

	constexpr UnicodeString& append(const Codepoint* s) {

		const Codepoint* p = s;
		while(*p++);

		return append(s, p - s);

	}

	template<std::input_iterator InputIt>
	constexpr UnicodeString& append(const InputIt& first, const InputIt& last) {
		appendByRange(first, last);
		return *this;
	}

	constexpr UnicodeString& append(std::initializer_list<Codepoint> list) {
		return append(list.begin(), list.size());
	}

	template<class StringView>
	constexpr UnicodeString& append(const StringView& sv) requires (ImpConvertible<const StringView&, std::basic_string_view<Codepoint>> && !ImpConvertible<const StringView&, const Codepoint*>) {

		std::basic_string_view<Codepoint> view = sv;
		append(view.data(), view.size());

		return *this;

	}

	template<class StringView>
	constexpr UnicodeString& append(const StringView& sv, SizeT svIndex, SizeT svSize = SizeT(-1)) requires (ImpConvertible<const StringView&, std::basic_string_view<Codepoint>> && !ImpConvertible<const StringView&, const Codepoint*>) {

		std::basic_string_view<Codepoint> view = sv;

		if(svIndex > view.size()) {
			throw std::out_of_range("Appending substring index out of range");
		}

		if(svSize > view.size() - svIndex) {
			svSize = view.size() - svIndex;
		}

		append(view.data() + svIndex, svSize);

		return *this;

	}

	constexpr UnicodeString& operator+=(const UnicodeString& ustr) {
		return append(ustr);
	}

	constexpr UnicodeString& operator+=(Codepoint codepoint) {
		return append(codepoint);
	}

	constexpr UnicodeString& operator+=(const Codepoint* s) {
		return append(s);
	}

	constexpr UnicodeString& operator+=(std::initializer_list<Codepoint> list) {
		return append(list);
	}

	template<class StringView>
	constexpr UnicodeString& operator+=(const StringView& sv) requires (ImpConvertible<const StringView&, std::basic_string_view<Codepoint>> && !ImpConvertible<const StringView&, const Codepoint*>) {
		return append(sv);
	}



private:

    constexpr void verifyCodepointIndex(SizeT index) const {

        if(index >= size()) {
            throw std::out_of_range("Codepoint index out of range");
        }

    }

    constexpr void verifyInsertionIndex(SizeT index) const {

        if(index > size()) {
            throw std::out_of_range("Insertion index out of range");
        }

    }

    template<bool Verify>
    constexpr SizeT getCodepointOffset(SizeT index) const noexcept(!Verify) {

        if constexpr (Verify) {
            verifyCodepointIndex(index);
        }

        if constexpr (Unicode::isUTF8<E>()) {

            //Forward iteration for UTF-8 is much faster since the first byte carries information about the whole codepoint size
            SizeT dstIdx = index / Base::CPDistance;

            //Offset to traverse
            SizeT traversalOffset = index % Base::CPDistance;

            //Start pointer to iterate from
            const CharT* startPtr = &str[Base::distances[dstIdx]];

            for(SizeT i = 0; i < traversalOffset; i++) {
                startPtr += Unicode::getEncodedSize<E>(startPtr);
            }

            return startPtr - str.data();

        } else if constexpr (Unicode::isUTF16<E>()) {

            //For UTF-16, bidirectional iteration is equally performant because surrogates are reflected in both code units
            //Therefore, round the index
            SizeT dstIdx = (index + Base::CPDistance / 2) / Base::CPDistance;
            std::make_signed_t<SizeT> offset = index - dstIdx * Base::CPDistance;

            //If the distance index is out of bounds (i.e. at the border with negative offsets), switch to forward traversal
            if(dstIdx >= Base::distances.size()) {
                
                dstIdx--;
                offset += Base::CPDistance;

            }

            const CharT* startPtr = &str[Base::distances[dstIdx]];

            if(offset >= 0) {

                for(imax i = 0; i < offset; i++) {
                    startPtr += Unicode::getEncodedSize<E>(startPtr);
                }

            } else {

                for(imax i = 0; i > offset; i--) {
                    startPtr -= Unicode::getEncodedSize<E>(startPtr);
                }

            }

            return startPtr - str.data();

        } else {

            return index;

        }

    }

    template<bool Verify>
    constexpr SizeT getInsertionOffset(SizeT index) const noexcept(!Verify) {

        if(index == size()) {
            return str.size();
        } else {
            return getCodepointOffset<Verify>(index);
        }

    }

    constexpr SizeT getCodepointOffsetOrThrow(SizeT index) const {
        return getCodepointOffset<true>(index);
    }

    constexpr SizeT getCodepointOffsetDirect(SizeT index) const noexcept {
        return getCodepointOffset<false>(index);
    }

    constexpr SizeT getInsertionOffsetOrThrow(SizeT index) const {
        return getInsertionOffset<true>(index);
    }

    constexpr SizeT getInsertionOffsetDirect(SizeT index) const noexcept {
        return getInsertionOffset<false>(index);
    }

    constexpr Codepoint getCodepointOrThrow(SizeT index) const {
        return Unicode::getCodepoint<E>(&str[getCodepointOffsetOrThrow(index)]);
    }

    constexpr Codepoint getCodepointDirect(SizeT index) const noexcept {
        return Unicode::getCodepoint<E>(&str[getCodepointOffsetDirect(index)]);
    }



    constexpr void restoreDistanceRange(SizeT cpStart, SizeT charStart, SizeT totalCps) {

        if constexpr (!Unicode::isUTF32<E>()) {

            //First, we get the start index into distances to modify
            SizeT dstOffset = cpStart / Base::CPDistance;

            //Resize to fit new distance elements
            Base::totalCodepoints = totalCps;
            Base::distances.resize((totalCps + Base::CPDistance - 1) / Base::CPDistance);

            SizeT x = cpStart;
            const CharT* ptr = str.data() + charStart;

            //Calculate all new offsets from start point onwards
            for(SizeT i = dstOffset + 1; i < Base::distances.size(); i++) {

                for(; x < i * Base::CPDistance; x++) {
                    ptr += Unicode::getEncodedSize<E>(ptr);
                }

                Base::distances[i] = ptr - str.data();

            }

        }

    }

    constexpr void insertCodepoint(SizeT index, Codepoint codepoint, SizeT n) {

        if constexpr (!Unicode::isUTF32<E>()) {

            verifyInsertionIndex(index);

            //Decompose codepoint
            CharT decomposed[Unicode::UTFEncodingTraits<E>::MaxDecomposed];
            SizeT count = Unicode::toUTF<E>(codepoint, decomposed);

            //Resize array
            SizeT offset = getInsertionOffsetDirect(index);
            SizeT prevSize = str.size();
            str.resize(prevSize + count * n);

            //Copy elements to the new end
            std::copy_backward(str.begin() + offset, str.begin() + prevSize, str.end());

            //Now insert the new decomposed codepoint(s)
            for(SizeT i = 0; i < n; i++) {
                std::copy_n(decomposed, count, str.begin() + offset + i * count);
            }

            restoreDistanceRange(index, offset, Base::totalCodepoints + n);

        } else {

            str.insert(index, n, codepoint);

        }

    }

    template<std::input_iterator InputIt>
    constexpr iterator insertByRange(const const_iterator& pos, InputIt first, const InputIt& last) {

        if constexpr (!Unicode::isUTF32<E>()) {

            SizeT index = pos.getCodepointIndex();

            //Precalculate the total insertion size
            SizeT totalSegments = 0;

            for(InputIt cpyIt = first; cpyIt != last; cpyIt++) {
                totalSegments += Unicode::getEncodedSize<E>(&*cpyIt);
            }

            //Resize array
            SizeT offset = pos.getPointer() - str.data();
            SizeT prevSize = str.size();
            str.resize(prevSize + totalSegments);

            //Copy elements to the new end
            std::copy_backward(str.begin() + offset, str.begin() + prevSize, str.end());

            //Now insert the new decomposed codepoint(s) over the range
            const CharT* ptr = str.data() + offset;
            SizeT totalCodepoints = size();

            for(; first != last; first++, totalCodepoints++) {
                ptr += Unicode::toUTF<E>(*first, ptr);
            }

            restoreDistanceRange(index, offset, totalCodepoints);

            return iterator(index, ptr);

        } else {

            return iterator(pos.getCodepointIndex(), &*str.insert(pos, first, last));

        }

    }


	template<std::input_iterator InputIt>
	constexpr void appendCodepoint(Codepoint codepoint, SizeT n) {

		if constexpr (!Unicode::isUTF32<E>()) {

			//Decompose codepoint
			CharT decomposed[Unicode::UTFEncodingTraits<E>::MaxDecomposed];
			SizeT count = Unicode::toUTF<E>(codepoint, decomposed);

			//Resize array
			SizeT prevSize = str.size();
			str.resize(prevSize + count * n);

			//Now insert the new decomposed codepoint(s)
			for(SizeT i = 0; i < n; i++) {
				std::copy_n(decomposed, count, str.begin() + prevSize + i * count);
			}

			SizeT prevCpCount = size();
			restoreDistanceRange(prevCpCount, prevSize, prevCpCount + n);

		} else {

			str.append(n, codepoint);

		}

	}

	template<std::input_iterator InputIt>
	constexpr void appendByRange(InputIt first, const InputIt& last) {

		if constexpr (!Unicode::isUTF32<E>()) {

			//Precalculate the total insertion size
			SizeT totalSegments = 0;

			for(InputIt cpyIt = first; cpyIt != last; cpyIt++) {
				totalSegments += Unicode::getEncodedSize<E>(&*cpyIt);
			}

			//Resize array
			SizeT prevSize = str.size();
			str.resize(prevSize + totalSegments);

			//Now insert the new decomposed codepoint(s) over the range
			const CharT* ptr = str.data() + prevSize;
			SizeT totalCodepoints = size();

			for(; first != last; first++, totalCodepoints++) {
				ptr += Unicode::toUTF<E>(*first, ptr);
			}

			restoreDistanceRange(size(), prevSize, totalCodepoints);

		} else {

			str.append(first, last);

		}

	}




    Container str;

};