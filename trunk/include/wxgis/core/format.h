/******************************************************************************
 * Project:  wxGIS
 * Purpose:  format clases (for format coordinates etc.).
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/core/core.h"

/** \fn wxString DoubleToString(double Val, bool IsLon) format.h
    \brief Format double value as string coordinate
*/
wxString WXDLLIMPEXP_GIS_CORE DoubleToString(double dVal, bool bIsLon);
/** \fn static double StringToDouble(wxString Val, wxString asterisk) format.h
    \brief Format string coordinate as double value 
*/
double WXDLLIMPEXP_GIS_CORE StringToDouble(const wxString &sVal, const wxString &sAsterisk);
/** \fn static wxString NumberScale(double fScaleRatio) format.h
    \brief Format map scale text
*/
WXDLLIMPEXP_GIS_CORE wxString NumberScale(double dScaleRatio);

/** \class wxGISCoordinatesFormat gpshapetotexttool.h
    \brief The class to format coordinates according different masks
*/
class WXDLLIMPEXP_GIS_CORE wxGISCoordinatesFormat
{
public:
    wxGISCoordinatesFormat(void);
    wxGISCoordinatesFormat(const wxString &sMask, bool bSwap = false);
    virtual ~wxGISCoordinatesFormat(void);
	virtual wxString Format(double dX, double dY);
	virtual bool IsOk(void){return m_bIsOk;};
	virtual void Create(const wxString &sMask, bool bSwap = false);
public:
	typedef enum coord_format {  
            D,
            DM,
            DMS,
            M,
            MS,
            S
    } COORD_FORMAT;
protected:
	wxString FormatToken(int nValHigh, int nValLow);
	wxString FormatString(double dCoord, const wxString &sFormat, COORD_FORMAT nCoordFormat, bool bLat);
	wxString ParseString(const wxString &sMask, COORD_FORMAT* pCoordFormat);
protected:
	bool m_bIsOk;
    bool m_bSwaped;
    wxString m_sMask;
    wxString m_sFormatX, m_sFormatY, m_sDivider;
    COORD_FORMAT m_CoordFormatX, m_CoordFormatY;
    bool m_bSign;
};



#define hibyte(a) ((a>>8) & 0xFF)
#define lobyte(a) ((a) & 0xFF)

static wxString Encode(wxString sInput, wxString sPass)
{
    wxString sRes;
  //  size_t pos(0);
  //  for(size_t i = 0; i < sInput.Len(); ++i)
  //  {
  //      if(pos == sPass.Len())
  //          pos = 0;
  //      wxChar Symbol = sInput[i] ^ sPass[pos];
  //      char SymbolHi = hibyte(Symbol);
  //      char SymbolLo = lobyte(Symbol);//(Symbol >> 4) & 0xff;
		//sRes += wxDecToHex(SymbolHi);
		//sRes += wxDecToHex(SymbolLo);
  //      pos++;
  //  }
    return sRes;
}

static wxString Decode(wxString sInput, wxString sPass)
{
    wxString sRes;
  //  size_t pos(0);
  //  for(size_t i = 0; i < sInput.Len(); i += 4)
  //  {
  //      if(pos == sPass.Len())
  //          pos = 0;
		//wxString sHex = sInput[i];
		//sHex += sInput[i + 1]; 
  //      char SymbolHi = wxHexToDec(sHex);
  //      sHex = sInput[i + 2];
		//sHex += sInput[i + 3];
  //      char SymbolLo = wxHexToDec(sHex);
  //      wxChar Symbol = (SymbolHi << 8) + SymbolLo;
  //      Symbol = Symbol ^ sPass[pos];
  //      sRes += Symbol;
  //      pos++;
  //  }
    return sRes;
}