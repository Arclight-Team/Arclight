#pragma once

#include "types.h"
#include <type_traits>



typedef u32 EventID;
typedef u32 EventParamType;

template<typename T>
concept EventData = std::is_pointer<T>::value;

class Event {

public:
	//constexpr explicit Event(EventID id, EventParamType param, EventData auto data) : id(id), param(param), data(static_cast<void*>(data)) {}

private:
	EventID id;
	EventParamType param;
	void* data;

};