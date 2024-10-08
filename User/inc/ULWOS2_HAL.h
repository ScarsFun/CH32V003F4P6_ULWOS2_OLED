#ifndef __ULWOS2_HAL_H
#define __ULWOS2_HAL_H

#include <stdint.h>
#include <stddef.h>

// Define ULWOS target platform:
// ULWOS2_TARGET_LINUX for Linux console
// ULWOS2_TARGET_ARDUINO for Arduino Platform
// ULWOS2_TARGET_PARTICLE for Particle devices
// ULWOS2_TARGET_MSP430 for TI MSP430 devices
// ULWOS2_TARGET_GD32V for GigaDevice GD32V RISC-V MCUs
// ULWOS2_TARGET_UNITTEST for unit testing ULWOS2
// New targets coming soon!
#define ULWOS2_TARGET_LINUX 		1
#define ULWOS2_TARGET_PARTICLE 		2
#define ULWOS2_TARGET_MSP430        3
#define ULWOS2_TARGET_ARDUINO       4
#define ULWOS2_TARGET_GD32V			5
#define ULWOS2_TARGET_CH32V			6
#define ULWOS2_TARGET_UNITTEST		255

#define ULWOS2_TARGET  ULWOS2_TARGET_CH32V
// Platform configuration
#ifdef ARDUINO
    #define ULWOS2_TARGET   ULWOS2_TARGET_ARDUINO
	#define ULWOS2_SIGNALS	// signals enabled by default on Arduinos
#endif
#ifndef ULWOS2_TARGET
    #define ULWOS2_TARGET ULWOS2_TARGET_PARTICLE
#endif

#if ULWOS2_TARGET == ULWOS2_TARGET_LINUX
	typedef uint64_t tULWOS2Timer;
#elif ULWOS2_TARGET == ULWOS2_TARGET_ARDUINO
	typedef uint32_t tULWOS2Timer;
#elif ULWOS2_TARGET == ULWOS2_TARGET_PARTICLE
	typedef uint32_t tULWOS2Timer;
#elif ULWOS2_TARGET == ULWOS2_TARGET_GD32V
	typedef uint32_t tULWOS2Timer;	
#elif ULWOS2_TARGET == ULWOS2_TARGET_MSP430
	typedef uint16_t tULWOS2Timer;
#elif ULWOS2_TARGET == ULWOS2_TARGET_CH32V
	typedef uint32_t tULWOS2Timer;
#else
	// defaults to unit test
	typedef uint64_t tULWOS2Timer;
#endif

tULWOS2Timer ULWOS2_getMilliseconds(void);

#endif
