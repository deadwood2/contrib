/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _GENERIC_TYPES_H_
#define _GENERIC_TYPES_H_

#include "Generic.h"

/** \file Types.h
 * \brief This file defines types for all four architectures.\n
 * Carries all the type specifications, array macros and other defines.
 */

/*
 * default common types & platform specific includes */
#if defined(__AROS__)
#include <exec/types.h>
#include <stddef.h>

typedef UQUAD              uint64;  /**< @brief unsigned 64bit integer */
typedef ULONG              uint32;  /**< @brief unsigned 32bit integer */
typedef UWORD              uint16;  /**< @brief unsigned 16bit integer */
typedef UBYTE              uint8;   /**< @brief unsigned 8bit integer  */
typedef QUAD               int64;   /**< @brief signed 64bit integer */
typedef LONG               int32;   /**< @brief signed 32bit integer */
typedef WORD               int16;   /**< @brief signed 16bit integer */
typedef BYTE               int8;    /**< @brief signed 8bit integer  */

typedef SIPTR              sint;    /**< @brief architecture specific signed int: sizeof(s_int) = sizeof(void*) */
typedef IPTR               uint;    /**< @brief architecture specific unsigned int: sizeof(u_int) = sizeof(void*) */

#else
typedef unsigned long long uint64;  /**< @brief unsigned 64bit integer */
typedef unsigned long int  uint32;  /**< @brief unsigned 32bit integer */
typedef unsigned short     uint16;  /**< @brief unsigned 16bit integer */
typedef unsigned char      uint8;   /**< @brief unsigned 8bit integer  */
typedef signed long long   int64;   /**< @brief signed 64bit integer */
typedef signed long int    int32;   /**< @brief signed 32bit integer */
typedef signed short       int16;   /**< @brief signed 16bit integer */
typedef signed char        int8;    /**< @brief signed 8bit integer  */

typedef signed long        sint;    /**< @brief architecture specific signed int: sizeof(s_int) = sizeof(void*) */
typedef unsigned long      uint;    /**< @brief architecture specific unsigned int: sizeof(u_int) = sizeof(void*) */

# if defined(__mc68000)
typedef long unsigned int size_t;
typedef long IPTR;
#  elif defined(__AMIGAOS4__)
typedef unsigned int size_t;
typedef long IPTR;
#  elif defined(__MORPHOS__)
typedef unsigned int size_t;
#  else
#error no size_t defined
#  endif
#endif

#if (0)
typedef uint*              sized_iptr; /**< @brief type returned by #SIZEARRAY to differentiate it from normal pointers */
typedef uint*              iptr;       /**< @brief type returned by #ARRAY to differentiate it from normal pointers */
#endif

#define PACKED __attribute__((packed))

/**
 * \enum TriState 
 * \brief Suggested enum for all three-state functions that accept or return
 * true, false or unknown states.
 */
enum TriState
{
   stUnknown   = -1,    /**< Unknown state */
   stNo        = 0,     /**< No = False    */
   stFalse     = 0,     /**< No = False    */
   stYes       = 1,     /**< Yes = True    */
   stTrue      = 1      /**< Yes = True    */
};

#ifdef __cplusplus

#include <cstdint>
#include <array>

template <std::size_t N>
struct IPTRArray
{
    template <typename... Ts>
    constexpr IPTRArray(Ts &&... vs)
        : array_(iptrarray(std::forward<Ts>(vs)...))
    {}

    operator IPTR ()
    {
        return (IPTR)array_.data();
    }

    operator TagItem* ()
    {
        return (TagItem *)(char*)array_.data();
    }

    operator APTR ()
    {
        return (APTR)(char*)array_.data();
    }

private:
    std::array<IPTR, N> array_;

    template <typename... Ts>
    static auto iptrarray(Ts &&... vs)
    {
        return std::array<IPTR, N> { (IPTR)std::forward<Ts>(vs)... };
    }
};

//! Use this template instead of varargs. Maintains compatibility across platforms.
template <typename... Ts>
IPTRArray<sizeof...(Ts)> ARRAY(Ts &&... vs) {
    return { std::forward<Ts>(vs)... };
}

#endif

/**
 * \def OFFSET(type, field)
 * \brief this macro calculates offset of field within structure.
 */
#define OFFSETOF(type, field) \
   ((size_t)(&((type*)1)->field)-1)

/**
 * \def OFFSETWITH(type, field)
 * \brief this macro calculates offset of first element after the selected field
 */
#define OFFSETWITH(type, field) \
   (((size_t)(&((type*)1)->field))+sizeof(type::field)-1)

/**
 * \def SWAP_WORD(data)
 * \brief switches endian of a 16-bit word
 */
#define SWAP_WORD(data) (((data & 0xff) << 8) | ((data >> 8) & 0xff))

/**
 * \def SWAP_LONG(data)
 * \brief switches endian of a 32-bit word
 */
#define SWAP_LONG(data) ({ register uint32 t = (((data & 0xffff) << 16) | ((data >> 16) &0xffff)); \
                           t = ((t >> 8) & 0xff00ff) | ((t & 0xff00ff) << 8); t; })

#if !defined(__AROS__) || AROS_BIG_ENDIAN

#define ENDIAN BIG   /**< @brief Defines endianess for current architecture. Can be either @b BIG or @b LITTLE */

/** 
 * @brief Way to change any word into BigEndian 
 * or change any BigEndian word to current arch. 
 */
#define W2BE(x) (x)
/** 
 * @brief Way to change any word into LittleEndian 
 * or change andy LittleEndian word to current arch.
 */
#define W2LE(x) ((((x)>>8)&255) | (((x)&255)<<8))
/** 
 * @brief Way to change any long into BigEndian 
 * or change any BigEndian long to current arch. 
 */
#define L2BE(x) (x)
/** 
 * @brief Way to change any long into LittleEndian 
 * or change andy LittleEndian long to current arch.
 */
#define L2LE(x) ((((x)>>24)&255) | (((x)>>8)&0xff00) | (((x)<<8)&0xff0000) | (((x)<<24)&0xff000000))

#else /* defined(__AROS__) && !AROS_BIG_ENDIAN */

#define ENDIAN LITTLE
#define W2LE(x)   (x)
#define W2BE(x)   ((((x)>>8)&255) | (((x)&255)<<8))
#define L2LE(x)   (x)
#define L2BE(x)   ((((x)>>24)&255) | (((x)>>8)&0xff00) | (((x)<<8)&0xff0000) | (((x)<<24)&0xff000000))

#endif /* !defined(__AROS__) || AROS_BIG_ENDIAN */

#endif

