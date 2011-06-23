/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  datasource header.
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

#include "wxgis/framework/framework.h"
#include "wx/datetime.h"

#include "ogrsf_frmts/ogrsf_frmts.h"
#include "gdal_priv.h"
#include "gdalwarper.h"
#include "gdal_alg_priv.h"
#include "cpl_quad_tree.h"
#include "cpl_string.h"

enum wxGISEnumDatasetType
{
    enumGISAny = 0,
	enumGISFeatureDataset = 1,
	enumGISTableDataset = 2,
	enumGISRasterDataset = 3,
	enumGISContainer = 4
};

enum wxGISEnumVectorDatasetType
{
	enumVecUnknown = 0,
	enumVecESRIShapefile,
	enumVecMapinfoTab,
    enumVecMapinfoMif,
    enumVecKML,
    enumVecKMZ,
    enumVecDXF,
	emumVecPostGIS,
	enumVecGML,
    emumVecMAX
};

enum wxGISEnumRasterDatasetType
{
	enumRasterUnknown = 0,
	enumRasterBmp,
	enumRasterTiff,
	enumRasterTil,
	enumRasterImg,
	enumRasterJpeg,
	enumRasterPng,
    enumRasterGif,
    enumRasterMAX
};

enum wxGISEnumTableDatasetType
{
	enumTableUnknown = 0,
    enumTableDBF,
	enumTablePostgres,
    enumTableQueryResult,
    enumTableMapinfoTab,
    enumTableMapinfoMif,
    enumTableCSV,
    emumTableMAX
};

enum wxGISEnumPrjFileType
{
	enumESRIPrjFile = 1,
	enumSPRfile = 2
};

enum wxGISEnumContainerType
{
    enumContUnknown = 0,
	enumContFolder = 1,
	enumContGDBFolder = 2,
    enumCondDataset = 3,
	enumContGDB = 4
};


//GDAL SmartPointers
DEFINE_SHARED_PTR(OGRSpatialReference);
DEFINE_SHARED_PTR(OGRFeature);
DEFINE_SHARED_PTR(OGREnvelope);

static void OGRFeatureDeleter( OGRFeature* pFeature)
{
	OGRFeature::DestroyFeature(pFeature);
}

typedef struct _Limits
{
    double minx, miny, maxx, maxy;
}
LIMITS, *LPLIMITS;

class wxGISDataset;
DEFINE_SHARED_PTR(wxGISDataset);

class wxGISDataset
{
public:
	wxGISDataset(CPLString sPath)
    {
        m_sPath = sPath;
    };
	virtual ~wxGISDataset(void){ };
	virtual wxGISEnumDatasetType GetType(void){return m_nType;};
    virtual int GetSubType(void){return m_nSubType;};
    virtual void SetSubType(int nSubType){m_nSubType = nSubType;};
	virtual CPLString GetPath(void){return m_sPath;};
    virtual size_t GetSubsetsCount(void){return 0;};
    virtual wxGISDatasetSPtr GetSubset(size_t nIndex){return wxGISDatasetSPtr();};
    virtual wxString GetName(void){return wxEmptyString;};
	virtual void Close(void){m_bIsOpened = false;};
	virtual const OGRSpatialReferenceSPtr GetSpatialReference(void){return OGRSpatialReferenceSPtr();};
	virtual bool IsOpened(void){return m_bIsOpened;};
	virtual bool IsReadOnly(void){return m_bIsReadOnly;};
protected:
	CPLString m_sPath;
    wxCriticalSection m_CritSect;
    int m_nSubType;
	wxGISEnumDatasetType m_nType;
	bool m_bIsOpened;
	bool m_bIsReadOnly;
};

class wxGISFeatureSet
{
public:
	wxGISFeatureSet(void)
	{
		m_OGRFeatureArray.reserve(1000);
	}
	wxGISFeatureSet(size_t nReserve)
	{
		m_OGRFeatureArray.reserve(nReserve);
	}
	virtual ~wxGISFeatureSet(void){ };
	virtual void AddFeature(OGRFeature* poFeature){m_OGRFeatureArray.push_back(poFeature);};
	virtual size_t GetSize(void){return m_OGRFeatureArray.size();};
	virtual OGRFeature* GetAt(int nIndex) //const    0 based
	{
		wxASSERT(nIndex >= 0);
		//wxASSERT(nIndex < m_OGRFeatureArray.size());
		return m_OGRFeatureArray[nIndex];
	}
	virtual bool IsEmpty(void){return m_OGRFeatureArray.empty();};
	virtual OGRFeature* operator [](int nIndex) //const    same as GetAt
	{
		return GetAt(nIndex);
	}
    virtual void Clear(void){m_OGRFeatureArray.clear();};
protected:
	std::vector<OGRFeature*> m_OGRFeatureArray;
};

class wxGISGeometrySet :
	public IPointer
{
public:
	wxGISGeometrySet(bool bOwnGeometry = false)
	{
        SetOwnGeometry(bOwnGeometry);
        m_Iterator = m_OGRGeometryMap.begin();
	}
	virtual ~wxGISGeometrySet(void)
    {
        Clear();
    }
	virtual void AddGeometry(OGRGeometry* poGeometry, long nOID){m_OGRGeometryMap[poGeometry] = nOID;};
	virtual size_t GetSize(void){return m_OGRGeometryMap.size();};
	virtual bool IsEmpty(void){return m_OGRGeometryMap.empty();};
    virtual void Clear(void)
    {
        Reset();
        if(m_bOwnGeometry)
        {
            OGRGeometry* pOutGeom;
            while((pOutGeom = Next()) != NULL)
                wxDELETE(pOutGeom);
        }
        m_OGRGeometryMap.clear();
    };
    virtual void Reset(void){m_Iterator = m_OGRGeometryMap.begin();};
    virtual OGRGeometry* Next(void)
    {
        if(GetSize() == 0)
            return NULL;
        if(m_Iterator == m_OGRGeometryMap.end())
            return NULL;
        OGRGeometry* pOutGeom = m_Iterator->first;
        ++m_Iterator;
        return pOutGeom;
    };
    virtual long operator[](OGRGeometry* pGeom){return m_OGRGeometryMap[pGeom];};
    virtual void SetOwnGeometry(bool bOwnGeometry){m_bOwnGeometry = bOwnGeometry;};
protected:
	std::map<OGRGeometry*, long> m_OGRGeometryMap;
    std::map<OGRGeometry*, long>::const_iterator m_Iterator;
    bool m_bOwnGeometry;
};

class wxGISQueryFilter
{
public:
    wxGISQueryFilter(void){};
    wxGISQueryFilter(wxString sWhereClause){m_sWhereClause = sWhereClause;};
	virtual ~wxGISQueryFilter(void){};
    virtual void SetWhereClause(wxString sWhereClause){m_sWhereClause = sWhereClause;};
    virtual wxString GetWhereClause(void){return m_sWhereClause;};
protected:
    wxString m_sWhereClause;
};

class wxGISSpatialFilter : public wxGISQueryFilter
{
public:
	virtual ~wxGISSpatialFilter(void){};
	virtual void SetEnvelope(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY)
	{
		m_Env.MinX = dfMinX;
		m_Env.MaxX = dfMaxX;
		m_Env.MinY = dfMinY;
		m_Env.MaxY = dfMaxY;
	}
	virtual void SetEnvelope(OGREnvelope Env)
	{
		m_Env = Env;
	}
	virtual OGREnvelope GetEnvelope(void){return m_Env;};

protected:
	OGREnvelope m_Env;
};

