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

#ifndef FLAME_VERSION_HPP_INCLUDED
#define FLAME_VERSION_HPP_INCLUDED

#ifdef __MAJOR__
#define FLAME_MAJOR	__MAJOR__
#else
#define FLAME_MAJOR 0
#endif

#ifdef __MINOR__
#define FLAME_MINOR	__MINOR__
#else
#define FLAME_MINOR	0
#endif

#ifdef __REV__
#define FLAME_REV	__REV__
#else
#define FLAME_REV	0
#endif


#define VER_STR(x) #x
#define FLAME_VER_SET(major, minor, rev)	VER_STR(major) "." VER_STR(minor) "." VER_STR(rev)
#define FLAME_VER	FLAME_VER_SET(FLAME_MAJOR, FLAME_MINOR, FLAME_REV)


#endif