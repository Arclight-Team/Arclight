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

    constexpr UnicodeString& insert(SizeT index, const CharT* s) {

        const CharT* p = s;
        while(*p++);

        return insert(index, s, p - s);

    }

    constexpr UnicodeString& insert(SizeT index, const CharT* s, SizeT count) {

        std::span<const CharT> span {s, count};
        insert(iterator(index, getInsertionOffsetOrThrow(index)), span.begin(), span.end());

        return *this;

    }

    constexpr UnicodeString& insert(SizeT index, const UnicodeString& ustr) {

        SizeT offset = getInsertionOffsetOrThrow(index);

        str.insert(offset, ustr.str);
        restoreDistanceRange(index, offset, size() + ustr.size());

        return *this;

    }

    constexpr UnicodeString& insert(SizeT index, const UnicodeString& ustr, SizeT ssIndex, SizeT ssSize = -1) {

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
        return insertByIterator(pos, codepoint, count);
    }

    template<std::input_iterator InputIt>
    constexpr iterator insert(const const_iterator& pos, const InputIt& first, const InputIt& last) {
        return insertByRange(pos, first, last);
    }

    constexpr iterator insert(const const_iterator& pos, std::initializer_list<CharT> list) {
        return insert(pos, list.begin(), list.end());
    }
/*
    template<class StringView>
    constexpr basic_string& insert( size_type pos, const StringView& sv) {


        
    }
*/
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

    constexpr CharT getCodepointOrThrow(SizeT index) const {
        return Unicode::getCodepoint<E>(&str[getCodepointOffsetOrThrow(index)]);
    }

    constexpr CharT getCodepointDirect(SizeT index) const noexcept {
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


    constexpr iterator insertByIterator(const const_iterator& pos, Codepoint codepoint, SizeT n) {

        SizeT cpIdx = pos.getCodepointIndex();

        //The container storage might get reallocated so get the distance to the start
        SizeT distance = &*pos - str.data();

        insertCodepoint(cpIdx, codepoint, n);

        //Construct a new iterator to cpIdx and return it
        return iterator(cpIdx, str.data() + distance);

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
            SizeT offset = &*pos - str.data();
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

    Container str;

};