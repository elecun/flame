/**
 * @file version.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief FLAME Version Header
 * @version 0.1
 * @date 2024-04-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_VERSION_INCLUDED
#define FLAME_VERSION_INCLUDED

#ifdef __MAJOR__
#define _FLAME_MAJOR_	__MAJOR__
#else
#define _FLAME_MAJOR_ 0
#endif

#ifdef __MINOR__
#define _FLAME_MINOR_	__MINOR__
#else
#define _FLAME_MINOR_	0
#endif

#ifdef __REV__
#define _FLAME_REV_	__REV__
#else
#define _FLAME_REV_	0
#endif


#define _VER_STR_(x) #x
#define _FLAME_VER_SET_(major, minor, rev)	_VER_STR_(major) "." _VER_STR_(minor) "." _VER_STR_(rev)
#define _FLAME_VER_	_FLAME_VER_SET_(_FLAME_MAJOR_, _FLAME_MINOR_, _FLAME_REV_)


#endif