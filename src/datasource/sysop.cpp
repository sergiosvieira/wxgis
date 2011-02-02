/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  system operations.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010 Bishop
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

#include "wx/filename.h"

bool DeleteDir(CPLString sPath)
{
    int result = CPLUnlinkTree(sPath);
    //int result = VSIRmdir(sPath);
    if (result == -1)
        return false;
    return true;
}

bool CreateDir(CPLString sPath, long mode )
{
    if( VSIMkdir( sPath, mode ) != 0 )
        return false;
    return true;
}

bool DeleteFile(CPLString sPath)
{
    int result = VSIUnlink(sPath);
    if (result == -1)
        return false;
    return true;
}

bool RenameFile(CPLString sOldPath, CPLString sNewPath)
{
    int result = VSIRename(sOldPath, sNewPath);
    if (result == -1)
        return false;
    return true;
}

wxString ClearExt(wxString sPath)
{
    wxStripExtension(sPath);
    return sPath;
}

wxString CheckUniqName(wxString sPath, wxString sName, wxString sExt, int nCounter)
{
    wxString sResultName = sName + (nCounter > 0 ? wxString::Format(wxT("_%d"), nCounter) : wxString(wxEmptyString));
    wxString sBaseName = sPath + wxFileName::GetPathSeparator() + sResultName + wxT(".") + sExt;
    if(wxFileName::FileExists(sBaseName))
        return CheckUniqName(sPath, sName, sExt, nCounter + 1);
    else
        return sResultName;
}

wxFontEncoding GetEncodingFromCpg(CPLString sPath)
{
	const char* szCPGPath = CPLResetExtension(sPath, "cpg");
    char **papszLines = CSLLoad( szCPGPath);
    if(papszLines == NULL)
        return wxFONTENCODING_DEFAULT;

    char *pszWKT = CPLStrdup(papszLines[0]);
    if(CPLStrnlen(pszWKT, 10) <= 0)
	{
		CSLDestroy( papszLines );
        return wxFONTENCODING_DEFAULT;
	}

    wxString sCP = wgMB2WX(pszWKT);
	
	CSLDestroy( papszLines );
    int pos;
    if((pos = sCP.Find('\n')) != wxNOT_FOUND)
        sCP = sCP.Left(pos);
    if((pos = sCP.Find('/')) != wxNOT_FOUND)
        sCP = sCP.Right(sCP.Len() - pos - 1);
    int nCP = wxAtoi(sCP);
    //check encoding from code http://en.wikipedia.org/wiki/Code_page
    if(nCP < 255)
    {
        //http://www.autopark.ru/ASBProgrammerGuide/DBFSTRUC.HTM
        switch(nCP)
        {
            case 1: nCP = 437;      break;
            case 2: nCP = 850;      break;
            case 3: nCP = 1252;     break;
            case 4: nCP = 10000;    break;
            case 8: nCP = 865;      break;
            case 10: nCP = 850;     break;
            case 11: nCP = 437;     break;
            case 13: nCP = 437;     break;
            case 14: nCP = 850;     break;
            case 15: nCP = 437;     break;
            case 16: nCP = 850;     break;
            case 17: nCP = 437;     break;
            case 18: nCP = 850;     break;
            case 19: nCP = 932;     break;
            case 20: nCP = 850;     break;
            case 21: nCP = 437;     break;
            case 22: nCP = 850;     break;
            case 23: nCP = 865;     break;
            case 24: nCP = 437;     break;
            case 25: nCP = 437;     break;
            case 26: nCP = 850;     break;
            case 27: nCP = 437;     break;
            case 28: nCP = 863;     break;
            case 29: nCP = 850;     break;
            case 31: nCP = 852;     break;
            case 34: nCP = 852;     break;
            case 35: nCP = 852;     break;
            case 36: nCP = 860;     break;
            case 37: nCP = 850;     break;
            case 38: nCP = 866;     break;
            case 55: nCP = 850;     break;
            case 64: nCP = 852;     break;
            case 77: nCP = 936;     break;
            case 78: nCP = 949;     break;
            case 79: nCP = 950;     break;
            case 80: nCP = 874;     break;
            case 87: nCP = 874;     return wxFONTENCODING_DEFAULT;
            case 88: nCP = 1252;     break;
            case 89: nCP = 1252;     break;
            case 100: nCP = 852;     break;
            case 101: nCP = 866;     break;
            case 102: nCP = 865;     break;
            case 103: nCP = 861;     break;
            case 104: nCP = 895;     break;
            case 105: nCP = 620;     break;
            case 106: nCP = 737;     break;
            case 107: nCP = 857;     break;
            case 108: nCP = 863;     break;
            case 120: nCP = 950;     break;
            case 121: nCP = 949;     break;
            case 122: nCP = 936;     break;
            case 123: nCP = 932;     break;
            case 124: nCP = 874;     break;
            case 134: nCP = 737;     break;
            case 135: nCP = 852;     break;
            case 136: nCP = 857;     break;
            case 150: nCP = 10007;   break;
            case 151: nCP = 10029;   break;
            case 200: nCP = 1250;    break;
            case 201: nCP = 1251;    break;
            case 202: nCP = 1254;    break;
            case 203: nCP = 1253;    break;
            case 204: nCP = 1257;    break;
            default: break;
        }
    }
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

bool IsFileHidden(CPLString sPath)
{
#ifdef __WXMSW__
    wxString sTestPath(sPath, wxConvUTF8);
	DWORD dwAttrs = GetFileAttributes(sTestPath); 
    if (dwAttrs != INVALID_FILE_ATTRIBUTES)
		return dwAttrs & FILE_ATTRIBUTE_HIDDEN;
#endif
	return EQUALN(CPLGetFilename(sPath), ".", 1);
}