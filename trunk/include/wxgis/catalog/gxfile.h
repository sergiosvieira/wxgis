/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFile classes.
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
#include "wxgis/catalog/catalog.h"
#include "wxgis/carto/carto.h"

/** \class wxGxFile gxfileui.h
    \brief A file GxObject.
*/
class WXDLLIMPEXP_GIS_CLT wxGxFile :
	public IGxObject,
    public IGxFile, 
    public IGxObjectEdit
{
public:
	wxGxFile(CPLString Path, wxString Name);
	virtual ~wxGxFile(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void);
    virtual CPLString GetInternalName(void){return m_sPath;};
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	virtual bool Copy(CPLString szDestPath, ITrackCancel* pTrackCancel);
	virtual bool CanCopy(CPLString szDestPath){return true;};
	virtual bool Move(CPLString szDestPath, ITrackCancel* pTrackCancel);
	virtual bool CanMove(CPLString szDestPath){return CanCopy(szDestPath) & CanDelete();};
protected:
	wxString m_sName;
    CPLString m_sPath;
};

/** \class wxGxPrjFile gxfileui.h
    \brief A proj file GxObject.
*/
class WXDLLIMPEXP_GIS_CLT wxGxPrjFile :
    public wxGxFile
{
public:
	wxGxPrjFile(CPLString Path, wxString Name, wxGISEnumPrjFileType nType);
	virtual ~wxGxPrjFile(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Coordinate System"));};
	//wxGxPrjFile
	virtual OGRSpatialReference* GetSpatialReference(void);
protected:
    wxGISEnumPrjFileType m_Type;
	OGRSpatialReference m_OGRSpatialReference;
};

/** \class wxGxTextFile gxfileui.h
    \brief A text file GxObject.
*/
class WXDLLIMPEXP_GIS_CLT wxGxTextFile :
    public wxGxFile
{
public:
	wxGxTextFile(CPLString Path, wxString Name);
	virtual ~wxGxTextFile(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Text file"));};
};