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

#include "ogrsf_frmts.h"
//#include "ogr_api.h"
#include "gdal_priv.h"
#include "gdal_alg_priv.h"

#include "wxgis/framework/framework.h"
#include <wx/datetime.h>

enum wxGISEnumDatasetType
{
	enumGISFeatureDataset = 1, 
	enumGISTableDataset = 2, 
	enumGISRasterDataset = 4
};

enum wxGISEnumShapefileDatasetType
{
	enumESRIShapefile = 1, 
	enumMapinfoTabfile = 2
};

enum wxGISEnumRasterDatasetType
{
	enumRasterUnknown = 0
};

//this class should be in wxGISGeodatabase
class wxGISDataset :
	public IPointer
{
public:	
	wxGISDataset(wxString sPath) : IPointer(), m_sPath(sPath){};
	virtual ~wxGISDataset(void){};
	virtual wxGISEnumDatasetType GetType(void) = 0;
	virtual wxString GetPath(void){return m_sPath;};
protected:
	wxString m_sPath;
    wxCriticalSection m_CritSect;
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
protected:
	std::vector<OGRFeature*> m_OGRFeatureArray;
};

class IQueryFilter
{
public:
	virtual ~IQueryFilter(void){};

};

class wxGISSpatialFilter : public IQueryFilter
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