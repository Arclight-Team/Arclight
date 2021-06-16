#include "arcdebug.h"
#include "log.h"
#include "assert.h"


#ifndef ARC_FINAL_BUILD


ArcDebug::~ArcDebug() {
    flush();
}



void ArcDebug::dispatchToken(Token token) {

    switch(token) {

        case ArcSpace:
            buffer << " ";
            break;
        
        case ArcEndl:
            flush();
            break;

        default:
            arc_force_assert("Invalid ArcDebug token");
            break;

    }

}


void ArcDebug::flush() noexcept {

    try {
        Log::info("Debug", buffer.str());
        buffer.str("");
    } catch(const std::exception& e) {
        arc_force_assert(std::string("Exception caught when flushing debug buffer: ") + e.what());
    }

}


#endif