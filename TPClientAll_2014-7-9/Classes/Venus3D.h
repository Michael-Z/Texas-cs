#pragma once

#ifdef VE_PLATFORM_IOS
#define _NEW_
#include "VePower.h"
VE_POWER_API void* operator new(VeSizeT stSize, void* pvMemory) throw();
#endif
#include <VePowerPCH.h>
#define CURL_STATICLIB
#include <curllib/curl.h>

void VenusInit();

void VenusTerm();

void VenusUpdate();
