#ifndef HARDWARE_H
#define HARDWARE_H

#ifndef NON_NATIVE_TEST
#include <cc1110.h>  // /usr/share/sdcc/include/mcs51/cc1110.h
#include "ioCCxx10_bitdef.h"
#endif

#ifdef RILEYLINK
#define GREEN_LED P0_0
#define BLUE_LED P0_1
#endif

#endif
