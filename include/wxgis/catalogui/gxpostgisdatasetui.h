/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDatasetUI classes.
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxpostgisdataset.h"

/** \class wxGxPostGISTableDatasetUI gxpostgisdatasetui.h
    \brief A PostGIS Table Dataset GxObjectUI.
*/
class WXDLLIMPEXP_GIS_CLU wxGxPostGISTableDatasetUI :
	public wxGxPostGISTableDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
public:
	wxGxPostGISTableDatasetUI(CPLString soPath, wxGISDatasetSPtr pwxGISDataset, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxPostGISTableDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxPostGISTableDatasetUI.ContextMenu"));};
	virtual wxString NewMenu(void){return wxEmptyString;};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

/** \class wxGxPostGISFeatureDatasetUI gxpostgisdatasetui.h
    \brief A PostGIS Table Dataset GxObjectUI.
*/
class WXDLLIMPEXP_GIS_CLU wxGxPostGISFeatureDatasetUI :
	public wxGxPostGISFeatureDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
public:
	wxGxPostGISFeatureDatasetUI(CPLString soPath, wxGISDatasetSPtr pwxGISDataset, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxPostGISFeatureDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxPostGISFeatureDatasetUI.ContextMenu"));};
	virtual wxString NewMenu(void){return wxEmptyString;};
	virtual wxDataFormat GetDataFormat(void){return wxDataFormat(wxT("application/x-vnd.qgis.qgis.uri"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};


