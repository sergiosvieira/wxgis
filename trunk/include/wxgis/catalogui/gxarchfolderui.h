/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchiveUI classes.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/catalogui/gxfolderui.h"
#include "wxgis/catalog/gxarchfolder.h"

/** \class wxGxArchiveUI gxarchfolderui.h
    \brief An archive GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLU wxGxArchiveUI :
	public wxGxArchive,
    public IGxObjectUI,
    public IGxObjectEditUI
{
public:
	wxGxArchiveUI(CPLString Path, wxString Name, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxArchiveUI(void);
    //IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxArchive.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxArchive.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //wxGxArchive
    virtual IGxObject* GetArchiveFolder(CPLString szPath, wxString soName);
protected:
    wxIcon m_oLargeIcon, m_oSmallIcon;
};
*/
/** \class wxGxArchiveFolderUI gxarchfolderui.h
    \brief An archive folder GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLU wxGxArchiveFolderUI :
	public wxGxArchiveFolder,
    public IGxObjectUI,
    public IGxObjectEditUI
{
public:
	wxGxArchiveFolderUI(CPLString Path, wxString Name, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxArchiveFolderUI(void);
    //IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxArchiveFolder.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxArchiveFolder.NewMenu"));};
    //wxGxArchiveFolder
    virtual IGxObject* GetArchiveFolder(CPLString szPath, wxString soName);
protected:
    wxIcon m_oLargeIcon, m_oSmallIcon;
};
*/

