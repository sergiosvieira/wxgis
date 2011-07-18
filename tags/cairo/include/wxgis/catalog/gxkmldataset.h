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
	wxGxKMLDataset(CPLString Path, wxString Name, wxGISEnumVectorDatasetType Type);
	virtual ~wxGxKMLDataset(void);
    virtual void SetEncoding(wxFontEncoding Encoding);
	//IGxObject
    virtual void Detach(void);
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void);
	virtual wxString GetCategory(void);
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
    //IGxDataset
    virtual wxGISDatasetSPtr GetDataset(ITrackCancel* pTrackCancel = NULL);
	virtual wxGISEnumDatasetType GetType(void){return enumGISContainer;};
    virtual int GetSubType(void){return (int)m_type;};
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){LoadChildren(); return m_Children.size() > 0 ? true : false;};
	//wxGxKMLDataset
	virtual void LoadChildren(void);
	virtual void EmptyChildren(void);
protected:
	wxString m_sName;
	CPLString m_sPath;
    wxFontEncoding m_Encoding;
	wxGISDatasetSPtr m_pwxGISDataset;
	wxGISEnumVectorDatasetType m_type;
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
	wxGxKMLSubDataset(CPLString Path, wxString sName, wxGISDatasetSPtr pwxGISDataset, wxGISEnumVectorDatasetType nType);
	virtual ~wxGxKMLSubDataset(void);
    virtual void SetEncoding(wxFontEncoding Encoding);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void){return GetName();};
    virtual CPLString GetInternalName(void){return m_sPath;};
	virtual wxString GetCategory(void);
	//IGxDataset
	virtual wxGISDatasetSPtr GetDataset(ITrackCancel* pTrackCancel = NULL);
	virtual wxGISEnumDatasetType GetType(void){return enumGISFeatureDataset;};
    virtual int GetSubType(void){return (int)m_type;};
protected:
	wxString m_sName;
	CPLString m_sPath;
    wxFontEncoding m_Encoding;
	wxGISDatasetSPtr m_pwxGISDataset;
	wxGISEnumVectorDatasetType m_type;
};
