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
#pragma once

#include "wxgis/datasource/datasource.h"

bool WXDLLIMPEXP_GIS_DS DeleteDir(CPLString sPath);
bool WXDLLIMPEXP_GIS_DS CreateDir(CPLString sPath, long mode = 0777); 
bool WXDLLIMPEXP_GIS_DS DeleteFile(CPLString sPath);
bool WXDLLIMPEXP_GIS_DS RenameFile(CPLString sOldPath, CPLString sNewPath);
bool WXDLLIMPEXP_GIS_DS CopyFile(CPLString sDestPath, CPLString sSrcPath, ITrackCancel* pTrackCancel);
bool WXDLLIMPEXP_GIS_DS MoveFile(CPLString sDestPath, CPLString sSrcPath, ITrackCancel* pTrackCancel);
wxFontEncoding WXDLLIMPEXP_GIS_DS GetEncodingFromCpg(CPLString sPath);
wxString WXDLLIMPEXP_GIS_DS ClearExt(wxString sPath);
bool WXDLLIMPEXP_GIS_DS IsFileHidden(CPLString sPath);
wxString  WXDLLIMPEXP_GIS_DS CheckUniqName(CPLString sPath, wxString sName, wxString sExt, int nCounter = 0);
CPLString WXDLLIMPEXP_GIS_DS CheckUniqPath(CPLString sPath, int nCounter = 0);
CPLString WXDLLIMPEXP_GIS_DS Transliterate(const char* str);



