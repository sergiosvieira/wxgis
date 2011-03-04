/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFileUI classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxfile.h"

/** \class wxGxFile gxfileui.h
    \brief A file GxObject.
*/
class WXDLLIMPEXP_GIS_CLU wxGxFileUI :
	public wxGxFile,
	public IGxObjectUI,
    public IGxObjectEditUI
{
public:
	wxGxFileUI(CPLString Path, wxString Name);
	virtual ~wxGxFileUI(void);
};

/** \class wxGxPrjFileUI gxfileui.h
    \brief A proj file GxObject.
*/
class WXDLLIMPEXP_GIS_CLU wxGxPrjFileUI :
    public wxGxPrjFile,
	public IGxObjectUI,
    public IGxObjectEditUI,
    public IGxObjectWizard
{
public:
	wxGxPrjFileUI(CPLString Path, wxString Name, wxGISEnumPrjFileType nType, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxPrjFileUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxPrjFile.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxPrjFile.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
protected:
    wxIcon m_oLargeIcon;
    wxIcon m_oSmallIcon;
};

/** \class wxGxTextFileUI gxfileui.h
    \brief A text file GxObject.
*/
class WXDLLIMPEXP_GIS_CLU wxGxTextFileUI :
    public wxGxTextFile
{
public:
	wxGxTextFileUI(CPLString Path, wxString Name, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxTextFileUI(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Text file"));};
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxTextFile.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxTextFile.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_oLargeIcon;
    wxIcon m_oSmallIcon;
};