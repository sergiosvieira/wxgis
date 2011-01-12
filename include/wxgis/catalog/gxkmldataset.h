/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxKMLDataset class.
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

/** \class wxGxKMLDataset gxkmldataset.h
    \brief A kml GxObject.
*/
class WXDLLIMPEXP_GIS_CLT wxGxKMLDataset :
	public IGxObjectEdit,
    public IGxObjectContainer,
	public IGxDataset
{
public:
	wxGxKMLDataset(wxString Path, wxString Name, wxGISEnumVectorDatasetType Type);
	virtual ~wxGxKMLDataset(void);
    virtual void SetEncoding(wxFontEncoding Encoding);
	//IGxObject
    virtual void Detach(void);
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void);
	virtual wxString GetCategory(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	//IGxDataset
    virtual wxGISDataset* GetDataset(bool bReadOnly);
	virtual wxGISEnumDatasetType GetType(void){return enumGISContainer;};
    virtual void SetPathEncoding(wxMBConv* pPathEncoding){m_pPathEncoding = pPathEncoding;};
    virtual wxMBConv* GetPathEncoding(void){return m_pPathEncoding;};
    virtual int GetSubType(void){return (int)m_type;};
    virtual wxString GetPath(void){return m_sPath;};
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){LoadChildren(); return m_Children.size() > 0 ? true : false;};
	//wxGxKMLDataset
	virtual void LoadChildren(void);
	virtual void EmptyChildren(void);
protected:
	wxString m_sName, m_sPath;
    wxFontEncoding m_Encoding;
	wxGISDataset* m_pwxGISDataset;
	wxGISEnumVectorDatasetType m_type;
    wxMBConv* m_pPathEncoding;
    bool m_bIsChildrenLoaded;
};

/** \class wxGxKMLSubDataset gxkmldataset.h
    \brief A kml layer GxObject.
*/
class WXDLLIMPEXP_GIS_CLT wxGxKMLSubDataset :
	public IGxObject,
	public IGxDataset
{
public:
	wxGxKMLSubDataset(wxString sName, wxGISDataset* pwxGISDataset, wxGISEnumVectorDatasetType nType);
	virtual ~wxGxKMLSubDataset(void);
    virtual void SetEncoding(wxFontEncoding Encoding);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void){return GetName();};
	virtual wxString GetCategory(void);
	//IGxDataset
	virtual wxGISDataset* GetDataset(bool bReadOnly);
	virtual wxGISEnumDatasetType GetType(void){return enumGISFeatureDataset;};
    virtual void SetPathEncoding(wxMBConv* pPathEncoding){m_pPathEncoding = pPathEncoding;};
    virtual wxMBConv* GetPathEncoding(void){return m_pPathEncoding;};
    virtual int GetSubType(void){return (int)m_type;};
    virtual wxString GetPath(void){return wxEmptyString;};
protected:
	wxString m_sName;
    wxFontEncoding m_Encoding;
	wxGISDataset* m_pwxGISDataset;
	wxGISEnumVectorDatasetType m_type;
    wxMBConv* m_pPathEncoding;
};
