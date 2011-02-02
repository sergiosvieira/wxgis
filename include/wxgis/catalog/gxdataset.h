/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDataset class.
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

/** \class wxGxTableDataset gxdataset.h
    \brief A Table Dataset GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxTableDataset :
	public IGxObject,
	public IGxObjectEdit,
	public IGxDataset
{
public:
	wxGxTableDataset(wxString Path, wxString Name, wxGISEnumTableDatasetType nType);
	virtual ~wxGxTableDataset(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
	virtual wxString GetBaseName(void);
    virtual CPLString GetInternalName(void){return m_sPath;};
	virtual wxString GetCategory(void){return wxString(_("Table"));};
	virtual void Detach(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	//IGxDataset
	virtual wxGISDatasetSPtr GetDataset(bool bReadOnly);
	virtual wxGISEnumDatasetType GetType(void){return enumGISTableDataset;};
    virtual int GetSubType(void){return m_type;};
protected:
	wxString m_sName, m_sPath;
	wxGISDatasetSPtr m_pwxGISDataset;
	wxGISEnumTableDatasetType m_type;
};

/** \class wxGxFeatureDataset gxdataset.h
    \brief A Feature Dataset GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxFeatureDataset :
	public IGxObject,
	public IGxObjectEdit,
	public IGxDataset
{
public:
	wxGxFeatureDataset(wxString Path, wxString Name, wxGISEnumVectorDatasetType nType);
	virtual ~wxGxFeatureDataset(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
	virtual wxString GetBaseName(void);
    virtual wxString GetInternalName(void){return m_sPath;};
	virtual wxString GetCategory(void);
	virtual void Detach(void);
	//IGxObjectUI
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	//IGxDataset
	virtual wxGISDatasetSPtr GetDataset(bool bReadOnly);
	virtual wxGISEnumDatasetType GetType(void){return enumGISFeatureDataset;};
    virtual int GetSubType(void){return (int)m_type;};
protected:
	wxString m_sName, m_sPath;
    wxFontEncoding m_Encoding;
	wxGISDatasetSPtr m_pwxGISDataset;
	wxGISEnumVectorDatasetType m_type;
};

/** \class wxGxRasterDataset gxdataset.h
    \brief A Raster Dataset GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxRasterDataset :
	public IGxObject,
	public IGxObjectEdit,
	public IGxDataset
{
public:
	wxGxRasterDataset(wxString Path, wxString Name, wxGISEnumRasterDatasetType nType);
	virtual ~wxGxRasterDataset(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
	virtual wxString GetBaseName(void);
    virtual CPLString GetInternalName(void){return m_sPath;};
	virtual wxString GetCategory(void);
	virtual void Detach(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	//IGxDataset
	virtual wxGISDatasetSPtr GetDataset(bool bReadOnly);
	virtual wxGISEnumDatasetType GetType(void){return enumGISRasterDataset;};
    virtual int GetSubType(void){return (int)m_type;};
protected:
	wxString m_sName, m_sPath;
    wxGISEnumRasterDatasetType m_type;
	wxGISDatasetSPtr m_pwxGISDataset;
};