/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDataset class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
#include <wx/imaglist.h>

//--------------------------------------------------------------
//class wxGxTableDataset
//--------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxTableDataset :
	public IGxObject,
	public IGxObjectUI,
	public IGxObjectEdit,
	public IGxDataset
{
public:
	wxGxTableDataset(wxString Path, wxString Name, wxGISEnumTableDatasetType nType);
	virtual ~wxGxTableDataset(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
	virtual wxString GetCategory(void){return wxString(_("Table"));};
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxDataset.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxDataset.NewMenu"));};
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	virtual void EditProperties(wxWindow *parent);
	//IGxDataset
	virtual wxGISDataset* GetDataset(void);
	virtual wxGISEnumDatasetType GetType(void){return enumGISTableDataset;};
    virtual int GetSubType(void){return m_type;};
    virtual wxString GetPath(void){return m_sPath;};
protected:
	wxString m_sName, m_sPath;
	wxImageList m_ImageListSmall, m_ImageListLarge;
	wxGISDataset* m_pwxGISDataset;
	wxGISEnumTableDatasetType m_type;
    wxMBConv* m_pPathEncoding;
};

//--------------------------------------------------------------
//class wxGxFeatureDataset
//--------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxFeatureDataset :
	public IGxObject,
	public IGxObjectUI,
	public IGxObjectEdit,
	public IGxDataset
{
public:
	wxGxFeatureDataset(wxString Path, wxString Name, wxGISEnumVectorDatasetType Type, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxFeatureDataset(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
	virtual wxString GetCategory(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxShapefileDataset.ContextMenu"));};
	virtual wxString NewMenu(void){return wxEmptyString;};//wxString(wxT("wxGxShapefileDataset.NewMenu"))
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	virtual void EditProperties(wxWindow *parent);
	//IGxDataset
	virtual wxGISDataset* GetDataset(void);
	virtual wxGISEnumDatasetType GetType(void){return enumGISFeatureDataset;};
    virtual int GetSubType(void){return (int)m_type;};
    virtual wxString GetPath(void){return m_sPath;};
protected:
	wxString m_sName, m_sPath;
    wxFontEncoding m_Encoding;
	//wxImageList m_ImageListSmall, m_ImageListLarge;
	wxGISDataset* m_pwxGISDataset;
	wxGISEnumVectorDatasetType m_type;
    wxMBConv* m_pPathEncoding;
    wxIcon m_LargeIcon, m_SmallIcon;
};

//--------------------------------------------------------------
//class wxGxRasterDataset
//--------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxRasterDataset :
	public IGxObject,
	public IGxObjectUI,
	public IGxObjectEdit,
	public IGxDataset
{
public:
	wxGxRasterDataset(wxString Path, wxString Name, wxGISEnumRasterDatasetType Type);
	virtual ~wxGxRasterDataset(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
	virtual wxString GetCategory(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxRasterDataset.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxRasterDataset.NewMenu"));};
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	virtual void EditProperties(wxWindow *parent);
	//IGxDataset
	virtual wxGISDataset* GetDataset(void);
	virtual wxGISEnumDatasetType GetType(void){return enumGISRasterDataset;};
    virtual int GetSubType(void){return (int)m_type;};
    virtual wxString GetPath(void){return m_sPath;};
protected:
	wxString m_sName, m_sPath;
	wxImageList m_ImageListSmall, m_ImageListLarge;
	wxGISDataset* m_pwxGISDataset;
	wxGISEnumRasterDatasetType m_type;
};