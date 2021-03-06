/*
 *  File: debug.h
 *  Created: Thursday, April 29, 2004
 *  Copyright: blind assist authors
 *  Email:
 */

/************************************************************************
 *                                                                      *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ************************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <string>

#include "octlog.h"

#ifndef __FUNCSIG__
#	ifdef __GNUC__
#		define __FUNCSIG__ __PRETTY_FUNCTION__
#	else
#		define __FUNCSIG__ ""
#	endif
#endif

#ifndef __DEPRECATED__
#	ifdef __GNUC__
#		define __DEPRECATED__ __attribute__ ((deprecated))
#	else
#		define __DEPRECATED__
#	endif
#endif

#ifdef OCT_DEBUG

#	define octInfo(msg) { octopus::Octstream ss; ss << msg; octopus::Octlog::getInstance() << ss.str(); }

#	ifdef __GNUC__
#		define OCT_BREAK __asm__ __volatile__ ("int $3")
#		define octErrorOutput(T,E,F,L,M) octInfo((F) << ":" << (L) << ": error: " << (T) << ": \"" << (E) << "\" in \"" << (M) << "\"\n");
#		define octWarningOutput(M) octInfo(__FILE__ << ":" << __LINE__ << ": warning: " << (M) << " in \"" << (__FUNCSIG__) << "\"\n");
#	else
#		define OCT_BREAK {__asm int 3}
#		define octErrorOutput(T,E,F,L,M) octInfo((F) << "(" << (L) << ") : " << (T) << "(" << (E) << ") in " << (M) << "\n");
#       define octWarningOutput(M)
#	endif

#	ifdef OCT_BREAK_ASSERT
#		define octAssert(x) {if(!(x)) {octErrorOutput("ASSERT",#x,__FILE__,__LINE__,__FUNCSIG__); OCT_BREAK;}}
#	else
#		define octAssert(x) {if(!(x)) {octErrorOutput("ASSERT",#x,__FILE__,__LINE__,__FUNCSIG__);}}
#	endif

#	define octCheck(x) {if(!(x)) {octErrorOutput("CHECK",#x,__FILE__,__LINE__,__FUNCSIG__);}}

#else // OCT_DEBUG

#	define octInfo(msg)
#	define octAssert(expr)
#	define octCheck(expr)
#	define OCT_BREAK

#endif // OCT_DEBUG

#endif
