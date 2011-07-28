/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDatasetUI classes.
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
#include "wxgis/catalog/gxdataset.h"

/** \class wxGxTableDataset gxdataset.h
    \brief A Table Dataset GxObject.
*/

class WXDLLIMPEXP_GIS_CLU wxGxTableDatasetUI :
	public wxGxTableDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
public:
	wxGxTableDatasetUI(CPLString Path, wxString Name, wxGISEnumTableDatasetType nType, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxTableDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxDataset.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxDataset.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

/** \class wxGxFeatureDataset gxdataset.h
    \brief A Feature Dataset GxObject.
*/

class WXDLLIMPEXP_GIS_CLU wxGxFeatureDatasetUI :
	public wxGxFeatureDataset,
	public IGxObjectUI,
	public IGxObjectEditUI,
    public IGxObjectWizard
{
public:
	wxGxFeatureDatasetUI(CPLString Path, wxString Name, wxGISEnumVectorDatasetType nType, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxFeatureDatasetUI(void);
    //wxGxFeatureDataset
    virtual wxGISDatasetSPtr GetDataset(bool bCache = true, ITrackCancel* pTrackCancel = NULL);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxShapefileDataset.ContextMenu"));};
	virtual wxString NewMenu(void){return wxEmptyString;};//wxString(wxT("wxGxShapefileDataset.NewMenu"))
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

/** \class wxGxRasterDataset gxdataset.h
    \brief A Raster Dataset GxObject.
*/

class WXDLLIMPEXP_GIS_CLU wxGxRasterDatasetUI :
	public wxGxRasterDataset,
	public IGxObjectUI,
	public IGxObjectEditUI,
    public IGxObjectWizard
{
public:
	wxGxRasterDatasetUI(CPLString Path, wxString Name, wxGISEnumRasterDatasetType nType, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxRasterDatasetUI(void);
    //wxGxRasterDataset
    virtual wxGISDatasetSPtr GetDataset(bool bCached = true, ITrackCancel* pTrackCancel = NULL);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxRasterDataset.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxRasterDataset.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};