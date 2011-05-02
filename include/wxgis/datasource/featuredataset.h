/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  FeatureDataset class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010 Bishop
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

#include "wxgis/datasource/table.h"

void WXDLLIMPEXP_GIS_DS GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds);

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------

class WXDLLIMPEXP_GIS_DS wxGISFeatureDataset :
	public wxGISTable
{
public:
	wxGISFeatureDataset(CPLString sPath, int nSubType, OGRLayer* poLayer = NULL, OGRDataSource* poDS = NULL);
	virtual ~wxGISFeatureDataset(void);
//wxGISTable
    virtual size_t GetSubsetsCount(void);
    virtual wxGISDatasetSPtr GetSubset(size_t nIndex);
	virtual const OGRSpatialReferenceSPtr GetSpatialReference(void);
	virtual void Close(void);
//wxGISFeatureDataset
	virtual bool Open(int iLayer = 0, int bUpdate = 0, ITrackCancel* pTrackCancel = NULL);
	virtual const OGREnvelopeSPtr GetEnvelope(void);
    virtual const OGRwkbGeometryType GetGeometryType(void);
    virtual OGRErr SetFilter(wxGISQueryFilter* pQFilter = NULL);
//	//virtual void SetSpatialFilter(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY);
//	//virtual wxGISFeatureSet* GetFeatureSet(IQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
	virtual wxGISGeometrySet* GetGeometrySet(wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
    virtual wxGISGeometrySet* GetGeometries(void);
//    virtual OGRErr CreateFeature(OGRFeature* poFeature);
    virtual char **GetFileList();
	//
    virtual OGRDataSource* GetDataSource(void);
	virtual OGRLayer* GetLayerRef(int iLayer = 0);
protected:
    virtual void CreateQuadTree(const OGREnvelopeSPtr pEnv);
    virtual void DeleteQuadTree(void);
    virtual void LoadGeometry(void);
    virtual void UnloadGeometry(void);
protected:
	OGREnvelopeSPtr m_psExtent;
	OGRSpatialReferenceSPtr m_pSpatialReference;

    bool m_bIsGeometryLoaded;
    wxGISGeometrySet *m_pGeometrySet;
    CPLQuadTree* m_pQuadTree;
//    wxArrayString m_FeatureStringData;
};

DEFINE_SHARED_PTR(wxGISFeatureDataset);
