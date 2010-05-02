/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  base header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
//#define WXDLLEXPORT WXMAKINGDLL_GIS
//#define WXDLLEXPORT_DATA WXDLLIMPEXP_DATA_GIS


#include <wx/xml/xml.h>

#include <algorithm>
#include <map>
#include <vector>
#include <queue>

#define CONFIG_DIR wxT("wxGIS") 
#define DEF wxT("~def~") 
#define NON wxT("~non~") 
#define ERR wxT("~err~") 
#define NONAME _("no name") 
#define DELTA 0.001
#define CACHE_SIZE 3000
#define ZOOM_FACTOR 0.25
#define PI 3.1415926535897932384626433832795
#define PIDEG 0.017453292519943295769236907684886

#ifndef _DEBUG
#define WXGISPORTABLE
#endif

#define WIN 0
#define LIN 1

static wxString DoubleToString(double Val, bool IsLon)
{
	wxString znak;
	if(Val < 0)
	{
		if(IsLon) znak = _(" W");
		else znak = _(" S");
	}
	else
	{
		if(IsLon) znak = _(" E");
		else znak = _(" N");
	}
	Val = fabs(Val);
	int grad = floor(Val);
	int min = floor((Val - grad) * 60);
	int sec = floor((Val - grad - (double) min / 60) * 3600);
	wxString str;
	if(IsLon)
		str.Printf(wxT("%.3d-%.2d-%.2d%s"), grad, min, sec, znak.c_str());
	else
		str.Printf(wxT("%.2d-%.2d-%.2d%s"), grad, min, sec, znak.c_str());
	return str;
};

static double StringToDouble(wxString Val, wxString asterisk)
{
	wxString buff;
	unsigned char counter = 0;
	int grad, min, sec;
	for(size_t i = 0; i < Val.Len(); i++)
	{
		wxChar ch = Val[i];
		if(ch == '-' || ch == ' ')
		{
			switch(counter)
			{
				case 0:				
				grad = wxAtoi(buff.c_str());
				break;
				case 1:
				min = wxAtoi(buff.c_str());
				break;
				case 2:
				sec = wxAtoi(buff.c_str());
				break;
			}
		}		
	}	
	int mul = -1;
	if(buff == _(" E") || buff == _(" N"))
		mul = 1;
	return ((double) grad + (double)min / 60 + (double)sec / 3600) * mul;
};

static wxString NumberScale(double fScaleRatio)
{
	wxString str = wxString::Format(wxT("%.2f"), fScaleRatio);	
	int pos = str.Find(wxT("."));
	if(pos == wxNOT_FOUND)
		pos = str.Len();
	wxString res = str.Right(str.Len() - pos);
	for(size_t i = 1; i < pos + 1; i++)
	{
		res.Prepend(str[pos - i]);
		if((i % 3) == 0)
			res.Prepend(wxT(" "));
	}
	return res;
};