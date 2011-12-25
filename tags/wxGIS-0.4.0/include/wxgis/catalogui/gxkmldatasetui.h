/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxKMLDatasetUI classes.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalog/gxkmldataset.h"

/** \class wxGxKMLDatasetUI gxkmldatasetui.h
    \brief A kml dataset GxObject.
*/
class WXDLLIMPEXP_GIS_CLU wxGxKMLDatasetUI :
    public wxGxKMLDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
public:
	wxGxKMLDatasetUI(CPLString Path, wxString Name, wxGISEnumVectorDatasetType Type, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon, wxIcon SubLargeIcon = wxNullIcon, wxIcon SubSmallIcon = wxNullIcon);
	virtual ~wxGxKMLDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxShapefileDataset.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxKMLDataset.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
	//wxGxKMLDataset
	virtual void LoadChildren(void);
	virtual void EmptyChildren(void);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
    wxIcon m_LargeSubIcon, m_SmallSubIcon;
};

/** \class wxGxKMLSubDatasetUI gxkmldatasetui.h
    \brief A kml layer GxObject.
*/
class WXDLLIMPEXP_GIS_CLU wxGxKMLSubDatasetUI :
	public wxGxKMLSubDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
public:
	wxGxKMLSubDatasetUI(CPLString Path, wxString sName, wxGISDatasetSPtr pwxGISDataset, wxGISEnumVectorDatasetType nType, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxKMLSubDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxKMLSubDataset.ContextMenu"));};
	virtual wxString NewMenu(void){return wxEmptyString;};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};
