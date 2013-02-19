/******************************************************************************
 * Project:  wxGIS
 * Purpose:  system operations.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Bishop
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

#include "wxgis/datasource/datasource.h"
#include "wxgis/datasource/gdalinh.h"

bool WXDLLIMPEXP_GIS_DS DeleteDir(const CPLString &sPath, ITrackCancel* const pTrackCancel = NULL);
bool WXDLLIMPEXP_GIS_DS CreateDir(const CPLString &sPath, long mode = 0777, ITrackCancel* const pTrackCancel = NULL); 
bool WXDLLIMPEXP_GIS_DS DeleteFile(const CPLString &sPath, ITrackCancel* const pTrackCancel = NULL);
bool WXDLLIMPEXP_GIS_DS RenameFile(const CPLString &sOldPath, const CPLString &sNewPath, ITrackCancel* const pTrackCancel = NULL);
bool WXDLLIMPEXP_GIS_DS CopyFile(const CPLString &sDestPath, const CPLString &sSrcPath, ITrackCancel* const pTrackCance = NULL);
bool WXDLLIMPEXP_GIS_DS MoveFile(const CPLString &sDestPath, const CPLString &sSrcPath, ITrackCancel* const pTrackCancel = NULL);
wxFontEncoding WXDLLIMPEXP_GIS_DS GetEncodingFromCpg(const CPLString &sPath);
//CPLString WXDLLIMPEXP_GIS_DS GetEncodingName(wxFontEncoding eEncoding);
wxString WXDLLIMPEXP_GIS_DS ClearExt(const wxString &sPath);
bool WXDLLIMPEXP_GIS_DS IsFileHidden(const CPLString &sPath);
wxString  WXDLLIMPEXP_GIS_DS CheckUniqName(const CPLString &sPath, const wxString &sName, const wxString &sExt, int nCounter = 0);
CPLString WXDLLIMPEXP_GIS_DS CheckUniqPath(const CPLString &sPath, const CPLString &sName, const CPLString &sAdd = "_copy", int nCounter = 0);
CPLString WXDLLIMPEXP_GIS_DS GetUniqPath(const CPLString &szOriginalFullPath, const CPLString &szNewPath, const CPLString &szNewName);
CPLString WXDLLIMPEXP_GIS_DS Transliterate(const char* str);
CPLString WXDLLIMPEXP_GIS_DS GetExtension(const CPLString &sPath, const CPLString &sName = "");
wxString WXDLLIMPEXP_GIS_DS GetConvName(const CPLString &szPath, bool bIsPath = true);

