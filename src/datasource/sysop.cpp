/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  system operations.
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

#include "wxgis/datasource/sysop.h"

bool DeleteFile(wxString sPath, wxMBConv* conv)
{
    int result = VSIUnlink((const char*) sPath.mb_str(*conv));
    if (result == -1)
        return false;
    return true;
}

wxFontEncoding GetEncodingFromCpg(wxString sPath, wxMBConv* conv)
{
    sPath += wxT(".cpg");
    char **papszLines = CSLLoad( (const char*) sPath.mb_str(*conv));
    if(papszLines == NULL)
        return wxFONTENCODING_DEFAULT;

    char *pszWKT = CPLStrdup(papszLines[0]);
    if(CPLStrnlen(pszWKT, 10) <= 0)
        return wxFONTENCODING_DEFAULT;

    wxString sCP = wgMB2WX(pszWKT);
    int pos;
    if((pos = sCP.Find('\n')) != wxNOT_FOUND)
        sCP = sCP.Left(pos);
    if((pos = sCP.Find('/')) != wxNOT_FOUND)
        sCP = sCP.Right(sCP.Len() - pos - 1);
    int nCP = wxAtoi(sCP);
    //check encoding from code http://en.wikipedia.org/wiki/Code_page
    switch(nCP)
    {
    case 28591: return wxFONTENCODING_ISO8859_1;
    case 28592: return wxFONTENCODING_ISO8859_2;
    case 28593: return wxFONTENCODING_ISO8859_3;
    case 28594: return wxFONTENCODING_ISO8859_4;
    case 28595: return wxFONTENCODING_ISO8859_5;
    case 28596: return wxFONTENCODING_ISO8859_6;
    case 28597: return wxFONTENCODING_ISO8859_7;
    case 28598: return wxFONTENCODING_ISO8859_8;
    case 28599: return wxFONTENCODING_ISO8859_9;
    case 28600: return wxFONTENCODING_ISO8859_10;
    //case 874: return wxFONTENCODING_ISO8859_11;
    // case 28602: return wxFONTENCODING_ISO8859_12;      // doesn't exist currently, but put it
    case 28603: return wxFONTENCODING_ISO8859_13;
    // case 28604: return wxFONTENCODING_ISO8859_14:     ret = 28604; break; // no correspondence on Windows
    case 28605: return wxFONTENCODING_ISO8859_15;
    case 20866: return wxFONTENCODING_KOI8;
    case 21866: return wxFONTENCODING_KOI8_U;
    case 437: return wxFONTENCODING_CP437;
    case 850: return wxFONTENCODING_CP850;
    case 852: return wxFONTENCODING_CP852;
    case 855: return wxFONTENCODING_CP855;
    case 866: return wxFONTENCODING_CP866;
    case 874: return wxFONTENCODING_CP874;
    case 932: return wxFONTENCODING_CP932;
    case 936: return wxFONTENCODING_CP936;
    case 949: return wxFONTENCODING_CP949;
    case 950: return wxFONTENCODING_CP950;
    case 1250: return wxFONTENCODING_CP1250;
    case 1251: return wxFONTENCODING_CP1251;
    case 1252: return wxFONTENCODING_CP1252;
    case 1253: return wxFONTENCODING_CP1253;
    case 1254: return wxFONTENCODING_CP1254;
    case 1255: return wxFONTENCODING_CP1255;
    case 1256: return wxFONTENCODING_CP1256;
    case 1257: return wxFONTENCODING_CP1257;
    case 20932: return wxFONTENCODING_EUC_JP;
    case 10000: return wxFONTENCODING_MACROMAN;
    case 10001: return wxFONTENCODING_MACJAPANESE;
    case 10002: return wxFONTENCODING_MACCHINESETRAD;
    case 10003: return wxFONTENCODING_MACKOREAN;
    case 10004: return wxFONTENCODING_MACARABIC;
    case 10005: return wxFONTENCODING_MACHEBREW;
    case 10006: return wxFONTENCODING_MACGREEK;
    case 10007: return wxFONTENCODING_MACCYRILLIC;
    case 10021: return wxFONTENCODING_MACTHAI;
    case 10008: return wxFONTENCODING_MACCHINESESIMP;
    case 10029: return wxFONTENCODING_MACCENTRALEUR;
    case 10082: return wxFONTENCODING_MACCROATIAN;
    case 10079: return wxFONTENCODING_MACICELANDIC;
    case 10009: return wxFONTENCODING_MACROMANIAN;
    case 65000: return wxFONTENCODING_UTF7;
    case 65001: return wxFONTENCODING_UTF8;
    default: return wxFONTENCODING_DEFAULT;
    }
    return wxFONTENCODING_DEFAULT;
}