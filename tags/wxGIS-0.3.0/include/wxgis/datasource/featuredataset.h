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

#include "wxgis/datasource/datasource.h"

class wxGISFeatureDataset;
DEFINE_SHARED_PTR(wxGISFeatureDataset);

void WXDLLIMPEXP_GIS_DS GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds);
wxGISFeatureDatasetSPtr WXDLLIMPEXP_GIS_DS CreateVectorLayer(CPLString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *poFields, OGRSpatialReference *poSpatialRef = NULL, OGRwkbGeometryType eGType = wkbUnknown, char ** papszDataSourceOptions = NULL, char ** papszLayerOptions = NULL); 

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------

class WXDLLIMPEXP_GIS_DS wxGISFeatureDataset :
	public wxGISDataset
{
public:
	wxGISFeatureDataset(CPLString sPath, wxGISEnumVectorDatasetType nType);
	wxGISFeatureDataset(OGRDataSource *poDS, OGRLayer* poLayer, CPLString sPath, wxGISEnumVectorDatasetType nType);
	virtual ~wxGISFeatureDataset(void);
//wxGISDataset
	virtual wxGISEnumDatasetType GetType(void){return m_nType;};
    virtual size_t GetSubsetsCount(void);
    virtual wxGISDatasetSPtr GetSubset(size_t nIndex);
    virtual wxString GetName(void);
	virtual OGRSpatialReference* GetSpatialReference(void);
	virtual void Close(void);
//wxGISFeatureDataset
	virtual bool Open(int iLayer = 0);
	virtual bool Delete(int iLayer = 0);
	virtual bool Rename(wxString sNewName);
	virtual bool Copy(CPLString szDestPath, ITrackCancel* pTrackCancel);
	virtual bool Move(CPLString szDestPath, ITrackCancel* pTrackCancel);
	virtual OGREnvelope* GetEnvelope(void);
    virtual OGRwkbGeometryType GetGeometryType(void);
    virtual OGRFeatureDefn* GetDefiniton(void);
    virtual OGRDataSource* GetDataSource(void);
    virtual OGRErr SetFilter(wxGISQueryFilter* pQFilter = NULL);
	//virtual void SetSpatialFilter(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY);
	virtual OGRFeature* GetAt(long nIndex);
	virtual OGRFeature* operator [](long nIndex);
	virtual wxString GetAsString(long row, int col);
	//virtual wxGISFeatureSet* GetFeatureSet(IQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
	virtual wxGISGeometrySet* GetGeometrySet(wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
    virtual wxGISGeometrySet* GetGeometries(void);
	virtual size_t GetSize(void);
	virtual OGRLayer* GetLayerRef(int iLayer = 0);
    virtual OGRFeature* Next(void);
    virtual void Reset(void);
    virtual OGRErr CreateFeature(OGRFeature* poFeature);
    virtual wxFontEncoding GetEncoding(void){return m_Encoding;};
    virtual void SetEncoding(wxFontEncoding Encoding){m_Encoding = Encoding;};
    virtual char **GetFileList();
protected:
    virtual void CreateQuadTree(OGREnvelope* pEnv);
    virtual void DeleteQuadTree(void);
    virtual void LoadGeometry(void);
    virtual void UnloadGeometry(void);
protected:
	OGRDataSource *m_poDS;
	OGREnvelope* m_psExtent;
	OGRLayer* m_poLayer;
    bool m_bOLCStringsAsUTF8;
    wxFontEncoding m_Encoding;
    std::map<long, OGRFeature*> m_FeaturesMap;
    std::map<long, OGRFeature*>::iterator m_IT;
    int m_FieldCount;
    wxArrayString m_FeatureStringData;
    //
    bool m_bIsGeometryLoaded;
    wxGISGeometrySet *m_pGeometrySet;
    CPLQuadTree* m_pQuadTree;
};
