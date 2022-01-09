/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcdebug.hpp
 */

#pragma once

#include "arcconfig.hpp"

#ifndef ARC_FINAL_BUILD

#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "math/quaternion.hpp"
#include "locale/unicodestring.hpp"
#include "memory/memory.hpp"
#include "types.hpp"

#include <sstream>
#include <string>
#include <ranges>



template<class T>
concept StringStreamable = requires(T&& t, std::stringstream stream) {
    stream << t;
};

template<class T>
concept Iterable = requires(T&& t) {
	t.begin();
	t.end();
	requires std::input_iterator<decltype(t.begin())>;
	requires Equal<decltype(t.begin()), decltype(t.end())>;
};



class ArcDebug {

public:

    enum class Token {
        ArcEndl,
        ArcSpace,
        ArcDec,
        ArcHex,
        ArcUpper,
        ArcNoUpper
    };

    constexpr static auto maxLineElements = 20;
    constexpr static auto maxContainerElements = 500;

    ArcDebug() = default;
    ~ArcDebug();

    ArcDebug& operator<<(const StringStreamable auto& value) {

        write(value);
        dispatchToken(Token::ArcSpace);

        return *this;

    }

    ArcDebug& operator<<(Char auto value) {

        write(value);
        dispatchToken(Token::ArcSpace);

        return *this;
        
    }

	template<class T>
    ArcDebug& operator<<(const T* p) requires (!StringStreamable<const T*>) {

		write(Memory::address(p));
		dispatchToken(Token::ArcSpace);

		return *this;

    }
    
    template<class T, class U>
    ArcDebug& operator<<(const std::pair<T, U>& pair){
        write(pair);
        return *this;
    }

    template<class T> 
    requires (Iterable<T> && !StringStreamable<T>)
    ArcDebug& operator<<(const T& container){
        write(container);
        return *this;
    }

    template<Vector V>
    ArcDebug& operator<<(const V& v) {
        write(v);
        return *this;
    }

    template<Matrix M>
    ArcDebug& operator<<(const M& m) {
        write(m);
        return *this;
    }

    template<Float F>
    ArcDebug& operator<<(const Quaternion<F>& q) {
        write(q);
        return *this;
    }

    template<Unicode::Encoding E>
    ArcDebug& operator<<(const UnicodeString<E>& us) {
        write(us);
        return *this;
    }

    ArcDebug& operator<<(Token token) {
        dispatchToken(token);
        return *this;
    }


private:

    void write(Char auto value) {
        buffer << static_cast<u32>(value);
    }

    void write(const StringStreamable auto& value) {
        buffer << value;
    }

    void write(u8 value) {
        buffer << +value;
    }

    void write(i8 value) {
        buffer << +value;
    }

    template<class T, class U>
    void write(const std::pair<T, U>& pair){

        buffer << "[";
        write(pair.first);
        dispatchToken(Token::ArcSpace);
        write(pair.second);
        buffer << "]";

    }


    template<class T> 
    requires (Iterable<T> && !StringStreamable<T>)
    void write(const T& container) {

        bool lineStart = true;
        u32 elementIdx = 0;

        if(!container.size()) {
            buffer << "[Container empty]";
            return;
        }

        for(const auto& e : container) {

            if(lineStart) {
                buffer << "[" << elementIdx << "] ";
                lineStart = false;
            }

            write(e);
            dispatchToken(Token::ArcSpace);
            elementIdx++;

            if(!(elementIdx % maxLineElements)) {
                dispatchToken(Token::ArcEndl);
                lineStart = true;
            }

            if(elementIdx >= maxContainerElements) {
                buffer << "... + " << (container.size() - elementIdx) << " more elements";
                lineStart = false;
                break;
            }

        }

        if(!lineStart) {
            dispatchToken(Token::ArcEndl);
        }

    }


    template<Vector V>
    void write(const V& v) {

        buffer << "Vec" << v.Size << "[";
        
        for(u32 i = 0; i < v.Size - 1; i++) {
            buffer << +v[i] <<  ", ";
        }
            
        buffer << +v[v.Size - 1] << "]";
        dispatchToken(Token::ArcSpace);

    }


    template<Matrix M>
    void write(const M& m) {

        buffer << "Mat" << m.Size << "[";
        
        for(u32 i = 0; i < m.Size; i++) {

            if(i) {
                buffer << "     [";
            } else {
                buffer << "[";
            }

            for(u32 j = 0; j < m.Size - 1; j++) {
                buffer << +m[j][i] <<  ", ";
            }

            buffer << +m[m.Size - 1][i];
            
            if(i != m.Size - 1) {

                buffer << "]";
                dispatchToken(Token::ArcEndl);

            } else {

                buffer << "]]";

            }

        }

    }

    
    template<Float F>
    void write(const Quaternion<F>& q) {

        buffer << "Quat" << "[";
        buffer << q.w << ", ";
        buffer << q.x << ", ";
        buffer << q.y << ", ";
        buffer << q.z << "]";
            
        dispatchToken(Token::ArcSpace);

    }


    template<Unicode::Encoding E>
    void write(const UnicodeString<E>& us) {

        dispatchToken(Token::ArcUpper);

        for(auto it = us.begin(); it != us.end(); ++it) {

            write("U+");
            write(it.getCodepoint());
            dispatchToken(Token::ArcSpace);

        }

        dispatchToken(Token::ArcNoUpper);

    }


    void dispatchToken(Token token);
    void flush() noexcept;

    std::stringstream buffer;

};


using enum ArcDebug::Token;

#endif