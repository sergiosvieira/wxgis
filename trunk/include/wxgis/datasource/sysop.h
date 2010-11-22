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

bool WXDLLIMPEXP_GIS_DS DeleteFile(wxString sPath);
bool WXDLLIMPEXP_GIS_DS DeleteDir(wxString sPath);
bool WXDLLIMPEXP_GIS_DS RenameFile(wxString sOldPath, wxString sNewPath);
wxString WXDLLIMPEXP_GIS_DS CheckUniqName(wxString sPath, wxString sName, wxString sExt, int nCounter);
wxFontEncoding WXDLLIMPEXP_GIS_DS GetEncodingFromCpg(wxString sPath);
wxString WXDLLIMPEXP_GIS_DS ClearExt(wxString sPath);
