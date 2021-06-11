#pragma once

#include <sstream>
#include <string>
#include <ranges>
#include "types.h"


template<class T>
concept StringStreamable = requires(T&& t, std::stringstream stream) {
    stream << t;
};

template<class T>
concept Iterable = std::ranges::range<T>;

template<class T, template<class> class U>
concept Vectorizable = requires (U<T>&& v) {
    v.Size;
    v[0] -> T;
};


class ArcDebug {

public:

    enum class Token {
        ArcEndl,
        ArcSpace
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
/*
    template<class T, Vectorizable<T> V>
    ArcDebug& operator<<(const V& v) {
        write(v);
        return *this;
    }
*/
    ArcDebug& operator<<(Token token) {
        dispatchToken(token);
        return *this;
    }

private:

    void write(const StringStreamable auto& value) {
        buffer << value;
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

/*
    template<class T, Vectorizable<T> V>
    void write(const V& v) {

        buffer << "Vec" << v.Size << "[";
        
        for(u32 i = 0; i < v.Size - 1; i++) {
            buffer << v[i] <<  ", ";
        }
            
        buffer << v[v.Size - 1] << "]" << Token::ArcSpace;

    }
*/

    void dispatchToken(Token token);
    void flush() noexcept;

    std::stringstream buffer;

};


using enum ArcDebug::Token;