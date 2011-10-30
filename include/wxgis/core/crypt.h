/******************************************************************************
 * Project:  wxGIS
 * Purpose:  base crypt functions.
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

//
//
//#define hibyte(a) ((a>>8) & 0xFF)
//#define lobyte(a) ((a) & 0xFF)
//
//static wxString Encode(wxString sInput, wxString sPass)
//{
//    wxString sRes;
//  //  size_t pos(0);
//  //  for(size_t i = 0; i < sInput.Len(); ++i)
//  //  {
//  //      if(pos == sPass.Len())
//  //          pos = 0;
//  //      wxChar Symbol = sInput[i] ^ sPass[pos];
//  //      char SymbolHi = hibyte(Symbol);
//  //      char SymbolLo = lobyte(Symbol);//(Symbol >> 4) & 0xff;
//		//sRes += wxDecToHex(SymbolHi);
//		//sRes += wxDecToHex(SymbolLo);
//  //      pos++;
//  //  }
//    return sRes;
//}
//
//static wxString Decode(wxString sInput, wxString sPass)
//{
//    wxString sRes;
//  //  size_t pos(0);
//  //  for(size_t i = 0; i < sInput.Len(); i += 4)
//  //  {
//  //      if(pos == sPass.Len())
//  //          pos = 0;
//		//wxString sHex = sInput[i];
//		//sHex += sInput[i + 1]; 
//  //      char SymbolHi = wxHexToDec(sHex);
//  //      sHex = sInput[i + 2];
//		//sHex += sInput[i + 3];
//  //      char SymbolLo = wxHexToDec(sHex);
//  //      wxChar Symbol = (SymbolHi << 8) + SymbolLo;
//  //      Symbol = Symbol ^ sPass[pos];
//  //      sRes += Symbol;
//  //      pos++;
//  //  }
//    return sRes;
//}

#define EVP_KEY_SIZE	32
#define EVP_IV_SIZE		8
#define BUFSIZE			1024

WXDLLIMPEXP_GIS_CORE bool CreateRandomData(void);
WXDLLIMPEXP_GIS_CORE bool Crypt(const wxString &sText, wxString &sCryptText);
WXDLLIMPEXP_GIS_CORE bool Decrypt(const wxString &sText, wxString &sDecryptText);

