/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  RasterDataset class.
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

#include "wxgis/datasource/datasource.h"

/** \class wxGISRasterDataset rasterdataset.h
    \brief The GIS RasterDataset class.

    This class stores raster geographic data (imagery & etc.).
*/
class WXDLLIMPEXP_GIS_DS wxGISRasterDataset :
	public wxGISDataset
{
public:
	wxGISRasterDataset(CPLString sPath, wxGISEnumRasterDatasetType nType);
	virtual ~wxGISRasterDataset(void);
    // wxGISDataset
	virtual const OGRSpatialReferenceSPtr GetSpatialReference(void);
    virtual wxString GetName(void);
	virtual void Close(void);
	virtual bool IsCached(void){return false;};
	virtual void Cache(ITrackCancel* pTrackCancel = NULL){};
    // wxGISRasterDataset
	virtual bool Open(bool bReadOnly);
	virtual bool Rename(wxString sNewName);
	virtual bool Copy(CPLString szDestPath, ITrackCancel* pTrackCancel);
	virtual bool Move(CPLString szDestPath, ITrackCancel* pTrackCancel);
    virtual bool Delete(void);
	virtual OGREnvelope GetEnvelope(void);
	virtual GDALDataset* GetRaster(void){return m_poDataset;};
	virtual GDALDataset* GetMainRaster(void){return m_poMainDataset;};
	virtual bool HasOverviews(void){return m_bHasOverviews;};
	virtual void SetHasOverviews(bool bVal){m_bHasOverviews = bVal;};
	virtual bool HasStatistics(void){return m_bHasStats;};
	virtual void SetHasStatistics(bool bVal){m_bHasStats = bVal;};
    virtual int GetWidth(void){return m_nXSize;};
    virtual int GetHeight(void){return m_nYSize;};
    virtual int GetBandCount(void){return m_nBandCount;};
    virtual char **GetFileList();
	virtual bool GetPixelData(void *data, int nXOff, int nYOff, int nXSize, int nYSize, int nBufXSize, int nBufYSize, GDALDataType eDT, int nBandCount, int *panBandList);
protected:
	OGREnvelope m_stExtent;
	GDALDataset  *m_poDataset;
	GDALDataset  *m_poMainDataset;
	OGRSpatialReferenceSPtr m_pSpaRef;
    bool m_bHasOverviews;
    bool m_bHasStats;
	int m_nXSize;
	int m_nYSize;
	int m_nBandCount;
};

DEFINE_SHARED_PTR(wxGISRasterDataset);