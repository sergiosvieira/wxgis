/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog filters of GxObjects to show.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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

//------------------------------------------------------------
// wxGxObjectFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxObjectFilter : public IGxObjectFilter
{
public:
	wxGxObjectFilter(void);
	virtual ~wxGxObjectFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObject* pLocation, wxString sName );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
    virtual wxGISEnumDatasetType GetType(void);
};

//------------------------------------------------------------
// wxGxPrjFileFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxPrjFileFilter : public wxGxObjectFilter
{
public:
	wxGxPrjFileFilter(void);
	virtual ~wxGxPrjFileFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
};

//------------------------------------------------------------
// wxGxDatasetFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxDatasetFilter : public wxGxObjectFilter
{
public:
	wxGxDatasetFilter(wxGISEnumDatasetType nType, int nSubType = wxNOT_FOUND);
	virtual ~wxGxDatasetFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxGISEnumDatasetType GetType(void){return m_nType;};
    virtual int GetSubType(void){return m_nSubType;};
protected:
    wxGISEnumDatasetType m_nType;
	int m_nSubType;
};

//------------------------------------------------------------
// wxGxFeatureFileFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxFeatureFileFilter : public wxGxObjectFilter
{
public:
	wxGxFeatureFileFilter(wxGISEnumVectorDatasetType nSubType);
	virtual ~wxGxFeatureFileFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
    virtual wxGISEnumDatasetType GetType(void){return enumGISFeatureDataset;};
protected:
    wxGISEnumVectorDatasetType m_nSubType;
};

//------------------------------------------------------------
// wxGxFolderFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxFolderFilter : public wxGxObjectFilter
{
public:
	wxGxFolderFilter(void);
	virtual ~wxGxFolderFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
};

//------------------------------------------------------------
// wxGxRasterFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxRasterFilter : public wxGxObjectFilter
{
public:
	wxGxRasterFilter(wxGISEnumRasterDatasetType nSubType);
	virtual ~wxGxRasterFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
    virtual wxGISEnumDatasetType GetType(void){return enumGISRasterDataset;};
protected:
    wxGISEnumRasterDatasetType m_nSubType;
};

//------------------------------------------------------------
// wxGxTextFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxTextFilter : public wxGxObjectFilter
{
public:
	wxGxTextFilter(const wxString &soName, const wxString &soExt);
	virtual ~wxGxTextFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
protected:
    wxString m_soName;
    wxString m_soExt, m_soExtCmp;
};


//------------------------------------------------------------
// wxGxTableFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxTableFilter : public wxGxObjectFilter
{
public:
	wxGxTableFilter(wxGISEnumTableDatasetType nSubType);
	virtual ~wxGxTableFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
    virtual wxGISEnumDatasetType GetType(void){return enumGISTableDataset;};
protected:
    wxGISEnumTableDatasetType m_nSubType;
};

