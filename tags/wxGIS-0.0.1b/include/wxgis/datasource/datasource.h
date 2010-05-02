/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  datasource header.
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

#include "wxgis/framework/framework.h"
#include "wx/datetime.h"

#include "ogrsf_frmts.h"
//#include "ogr_api.h"
#include "gdal_priv.h"
#include "gdalwarper.h"
#include "gdal_alg_priv.h"
#include "cpl_quad_tree.h"
#include "cpl_string.h"

enum wxGISEnumDatasetType
{
	enumGISFeatureDataset = 1, 
	enumGISTableDataset = 2, 
	enumGISRasterDataset = 3,
	enumGISContainer = 4
};

enum wxGISEnumVectorDatasetType
{
	enumVecUnknown = 0, 
	enumVecESRIShapefile = 1, 
	enumVecMapinfoTab = 2,
    enumVecMapinfoMif = 3, 
    enumVecKML = 4,
    enumVecDXF = 5
};

enum wxGISEnumRasterDatasetType
{
	enumRasterUnknown = 0,
	enumRasterBmp = 1,
	enumRasterTiff = 2,
	enumRasterImg = 3,
	enumRasterJpeg = 4,
	enumRasterPng = 5
};

enum wxGISEnumTableDatasetType
{
	enumTableUnknown = 0,
    enumTableDBF = 1
};

enum wxGISEnumPrjFileType
{
	enumESRIPrjFile = 1, 
	enumSPRfile = 2
};

typedef struct _Limits
{
    double minx, miny, maxx, maxy;
}
LIMITS, *LPLIMITS;

//this class should be in wxGISGeodatabase
class wxGISDataset :
	public IPointer
{
public:	
	wxGISDataset(wxString sPath, wxMBConv* pPathEncoding = wxConvCurrent) : IPointer(), m_pPathEncoding(pPathEncoding)
    {
        m_sPath = sPath;
        m_sPath.Replace(wxT("\\"), wxT("/"));
    };
	virtual ~wxGISDataset(void){ };
	virtual wxGISEnumDatasetType GetType(void) = 0;
    virtual int GetSubType(void){return m_nSubType;};
    virtual void SetSubType(int nSubType){m_nSubType = nSubType;};
	virtual wxString GetPath(void){return m_sPath;};
    virtual size_t GetSubsetsCount(void){return 0;};
    virtual wxGISDataset* GetSubset(size_t nIndex){return NULL;};
    virtual wxString GetName(void){return wxEmptyString;};
protected:
	wxString m_sPath;
    wxCriticalSection m_CritSect;
    wxMBConv* m_pPathEncoding;
    int m_nSubType;
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

