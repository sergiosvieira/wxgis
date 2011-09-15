/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  base header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#pragma once

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if defined(HAVE_VISIBILITY)
#    define WXEXPORT __attribute__ ((visibility("default")))
#    define WXIMPORT __attribute__ ((visibility("default")))
#elif defined(__WINDOWS__)
    /*
       __declspec works in BC++ 5 and later, Watcom C++ 11.0 and later as well
       as VC++.
     */
#    if defined(__VISUALC__) || defined(__BORLANDC__) || defined(__WATCOMC__)
#        define WXEXPORT __declspec(dllexport)
#        define WXIMPORT __declspec(dllimport)
    /*
        While gcc also supports __declspec(dllexport), it creates unusably huge
        DLL files since gcc 4.5 (while taking horribly long amounts of time),
        see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=43601. Because of this
        we rely on binutils auto export/import support which seems to work
        quite well for 4.5+.
     */
#    elif defined(__GNUC__) && !wxCHECK_GCC_VERSION(4, 5)
        /*
            __declspec could be used here too but let's use the native
            __attribute__ instead for clarity.
        */
#       define WXEXPORT __attribute__((dllexport))
#       define WXIMPORT __attribute__((dllimport))
#    endif
#elif defined(__WXPM__)
#    if defined (__WATCOMC__)
#        define WXEXPORT __declspec(dllexport)
        /*
           __declspec(dllimport) prepends __imp to imported symbols. We do NOT
           want that!
         */
#        define WXIMPORT
#    elif defined(__EMX__)
#        define WXEXPORT
#        define WXIMPORT
#    elif (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))
#        define WXEXPORT _Export
#        define WXIMPORT _Export
#    endif
#elif defined(__WXMAC__) || defined(__WXCOCOA__)
#    ifdef __MWERKS__
#        define WXEXPORT __declspec(export)
#        define WXIMPORT __declspec(import)
#    endif
#elif defined(__CYGWIN__)
#    define WXEXPORT __declspec(dllexport)
#    define WXIMPORT __declspec(dllimport)
#endif

/* for other platforms/compilers we don't anything */
#ifndef WXEXPORT
#    define WXEXPORT
#    define WXIMPORT
#endif

#ifdef WXMAKINGDLL_GIS_CORE
#    define WXDLLIMPEXP_GIS_CORE WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_CORE(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_CORE WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_CORE(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_CORE
#    define WXDLLIMPEXP_DATA_GIS_CORE(type) type
#endif

#ifdef WXMAKINGDLL_GIS_FRW
#    define WXDLLIMPEXP_GIS_FRW WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_FRW(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_FRW WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_FRW(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_FRW
#    define WXDLLIMPEXP_DATA_GIS_FRW(type) type
#endif

#ifdef WXMAKINGDLL_GIS_DSP
#    define WXDLLIMPEXP_GIS_DSP WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_DSP(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_DSP WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_DSP(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_DSP
#    define WXDLLIMPEXP_DATA_GIS_DSP(type) type
#endif

#ifdef WXMAKINGDLL_GIS_CRT
#    define WXDLLIMPEXP_GIS_CRT WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_CRT(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_CRT WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_CRT(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_CRT
#    define WXDLLIMPEXP_DATA_GIS_CRT(type) type
#endif

#ifdef WXMAKINGDLL_GIS_CTU
#    define WXDLLIMPEXP_GIS_CTU WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_CTU(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_CTU WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_CTU(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_CTU
#    define WXDLLIMPEXP_DATA_GIS_CTU(type) type
#endif

#ifdef WXMAKINGDLL_GIS_CLT
#    define WXDLLIMPEXP_GIS_CLT WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_CLT(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_CLT WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_CLT(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_CLT
#    define WXDLLIMPEXP_DATA_GIS_CLT(type) type
#endif

#ifdef WXMAKINGDLL_GIS_CLU
#    define WXDLLIMPEXP_GIS_CLU WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_CLU(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_CLU WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_CLU(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_CLU
#    define WXDLLIMPEXP_DATA_GIS_CLU(type) type
#endif

#ifdef WXMAKINGDLL_GIS_GEOM
#    define WXDLLIMPEXP_GIS_GEOM WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_GEOM(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_GEOM WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_GEOM(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_GEOM
#    define WXDLLIMPEXP_DATA_GIS_GEOM(type) type
#endif

#ifdef WXMAKINGDLL_GIS_DS
#    define WXDLLIMPEXP_GIS_DS WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_DS(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_DS WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_DS(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_DS
#    define WXDLLIMPEXP_DATA_GIS_DS(type) type
#endif

#ifdef WXMAKINGDLL_GIS_GP
#    define WXDLLIMPEXP_GIS_GP WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_GP(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_GP WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_GP(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_GP
#    define WXDLLIMPEXP_DATA_GIS_GP(type) type
#endif

#ifdef WXMAKINGDLL_GIS_GPU
#    define WXDLLIMPEXP_GIS_GPU WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_GPU(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_GPU WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_GPU(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_GPU
#    define WXDLLIMPEXP_DATA_GIS_GPU(type) type
#endif

#ifdef WXMAKINGDLL_GIS_RS
#    define WXDLLIMPEXP_GIS_RS WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_RS(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_RS WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_RS(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_RS
#    define WXDLLIMPEXP_DATA_GIS_RS(type) type
#endif

#ifdef WXMAKINGDLL_GIS_RSU
#    define WXDLLIMPEXP_GIS_RSU WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_RSU(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_RSU WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_RSU(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_RSU
#    define WXDLLIMPEXP_DATA_GIS_RSU(type) type
#endif

#ifdef WXMAKINGDLL_GIS_NET
#    define WXDLLIMPEXP_GIS_NET WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_NET(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_NET WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_NET(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_NET
#    define WXDLLIMPEXP_DATA_GIS_NET(type) type
#endif

#ifdef WXMAKINGDLL_GIS_MAPU
#    define WXDLLIMPEXP_GIS_MAPU WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_MAPU(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_MAPU WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_MAPU(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_MAPU
#    define WXDLLIMPEXP_DATA_GIS_MAPU(type) type
#endif

#ifdef WXMAKINGDLL_GIS_MAP
#    define WXDLLIMPEXP_GIS_MAP WXEXPORT
#    define WXDLLIMPEXP_DATA_GIS_MAP(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#    define WXDLLIMPEXP_GIS_MAP WXIMPORT
#    define WXDLLIMPEXP_DATA_GIS_MAP(type) WXIMPORT type
#else /* not making nor using DLL */
#    define WXDLLIMPEXP_GIS_MAP
#    define WXDLLIMPEXP_DATA_GIS_MAP(type) type
#endif

#if defined(HAVE_VISIBILITY) || (defined(__WINDOWS__) && defined(__GNUC__))
    #define WXDLLIMPEXP_FWD_GIS_CORE
    #define WXDLLIMPEXP_FWD_GIS_FRW
    #define WXDLLIMPEXP_FWD_GIS_DSP
    #define WXDLLIMPEXP_FWD_GIS_CRT
    #define WXDLLIMPEXP_FWD_GIS_CTU
    #define WXDLLIMPEXP_FWD_GIS_CLT
    #define WXDLLIMPEXP_FWD_GIS_CLU
    #define WXDLLIMPEXP_FWD_GIS_GEOM
    #define WXDLLIMPEXP_FWD_GIS_DS
    #define WXDLLIMPEXP_FWD_GIS_GP
    #define WXDLLIMPEXP_FWD_GIS_GPU
    #define WXDLLIMPEXP_FWD_GIS_RS
    #define WXDLLIMPEXP_FWD_GIS_RSU
    #define WXDLLIMPEXP_FWD_GIS_NET
    #define WXDLLIMPEXP_FWD_GIS_MAPU
    #define WXDLLIMPEXP_FWD_GIS_MAP
#else
    #define WXDLLIMPEXP_FWD_GIS_CORE	WXDLLIMPEXP_GIS_CORE
    #define WXDLLIMPEXP_FWD_GIS_FRW 	WXDLLIMPEXP_GIS_FRW
    #define WXDLLIMPEXP_FWD_GIS_DSP 	WXDLLIMPEXP_GIS_DSP
    #define WXDLLIMPEXP_FWD_GIS_CRT 	WXDLLIMPEXP_GIS_CRT
    #define WXDLLIMPEXP_FWD_GIS_CTU 	WXDLLIMPEXP_GIS_CTU
    #define WXDLLIMPEXP_FWD_GIS_CLT 	WXDLLIMPEXP_GIS_CLT
    #define WXDLLIMPEXP_FWD_GIS_CLU 	WXDLLIMPEXP_GIS_CLU
    #define WXDLLIMPEXP_FWD_GIS_GEOM	WXDLLIMPEXP_GIS_GEOM
    #define WXDLLIMPEXP_FWD_GIS_DS  	WXDLLIMPEXP_GIS_DS
    #define WXDLLIMPEXP_FWD_GIS_GP  	WXDLLIMPEXP_GIS_GP
    #define WXDLLIMPEXP_FWD_GIS_GPU 	WXDLLIMPEXP_GIS_GPU
    #define WXDLLIMPEXP_FWD_GIS_RS  	WXDLLIMPEXP_GIS_RS
    #define WXDLLIMPEXP_FWD_GIS_RSU 	WXDLLIMPEXP_GIS_RSU
    #define WXDLLIMPEXP_FWD_GIS_NET 	WXDLLIMPEXP_GIS_NET
    #define WXDLLIMPEXP_FWD_GIS_MAPU	WXDLLIMPEXP_GIS_MAPU
    #define WXDLLIMPEXP_FWD_GIS_MAP		WXDLLIMPEXP_GIS_MAP
#endif

#include <wx/xml/xml.h>

#include <algorithm>
#include <map>
#include <vector>
#include <queue>

#define DEF wxT("~def~")
#define NON wxT("~non~")
#define ERR wxT("~err~")
#define NONAME _("no name")
#define CACHE_SIZE 3000
#define DELTA (FLT_EPSILON * 16)
//M_PI
#define PI 3.1415926535897932384626433832795
#define DOUBLEPI 6.283185307179586476925286766558
#define PIDEG 0.017453292519943295769236907684886
#define DEGPI 57.295779513082320876798155633941

#define APP_VER wxT("0.4.0") //global for all apps

#ifndef _DEBUG
   #define WXGISPORTABLE
#endif

#ifndef WXDWORD
    #define WXDWORD unsigned long
#endif

