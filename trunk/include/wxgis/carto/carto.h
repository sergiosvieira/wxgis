/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISCarto main header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/display/gisdisplay.h"
#include "wxgis/datasource/quadtree.h"
#include "wxgis/datasource/rasterdataset.h"

/** \class wxGISLayer carto.h
    \brief The base class for map layers
*/
class wxGISLayer 
{
public:
	wxGISLayer(void)
	{
		m_sName = wxString(_("new layer"));
		m_dMaxScale = wxNOT_FOUND;
		m_dMinScale = wxNOT_FOUND;
		m_bVisible = true;
		//m_bCached = false;
        m_nCacheID = 0;
	}
	virtual~wxGISLayer(void){};
	//pure virtual
	virtual OGRSpatialReferenceSPtr GetSpatialReference(void) = 0;
	virtual void SetSpatialReference(OGRSpatialReferenceSPtr pSpatialReference) = 0;
	virtual OGREnvelope GetEnvelope(void) = 0;
	virtual bool IsValid(void) = 0;
	virtual bool Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL) = 0;
	//
	virtual void SetMaximumScale(double dMaxScale){m_dMaxScale = dMaxScale;};
	virtual double GetMaximumScale(void){return m_dMaxScale;};
	virtual void SetMinimumScale(double dMinScale){m_dMinScale = dMinScale;};
	virtual double GetMinimumScale(void){return m_dMinScale;};
	virtual bool GetVisible(void){return m_bVisible;};
	virtual void SetVisible(bool bVisible){m_bVisible = bVisible;};
	virtual void SetName(wxString sName){m_sName = sName;};
	virtual wxString GetName(wxString sName){return m_sName;};
	virtual size_t GetCacheID(void){return m_nCacheID;};
	virtual void SetCacheID(size_t nCacheID){m_nCacheID = nCacheID;};
	virtual bool IsCacheNeeded(void){return true;};
	//virtual bool GetCached(void){return m_bCached;};
	//virtual void SetCached(bool bCached){m_bCached = bCached;};
protected:
	double m_dMaxScale, m_dMinScale;
	wxString m_sName;
	bool m_bVisible;//, m_bCached;
	size_t m_nCacheID;
};
DEFINE_SHARED_PTR(wxGISLayer);

/** \class IRenderer carto.h
    \brief The base class for map layer renderer
*/
class IRenderer
{
public:
	virtual ~IRenderer(void){};
	virtual bool CanRender(wxGISDatasetSPtr pDataset) = 0;	
};

/** \class IFeatureRenderer carto.h
    \brief The base class for map vector layer renderer
*/
class IFeatureRenderer : public IRenderer
{
public:
	virtual ~IFeatureRenderer(void){};
	virtual void Draw(wxGISQuadTreeCursorSPtr pCursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL) = 0;
};
DEFINE_SHARED_PTR(IFeatureRenderer);

typedef struct _rawpixeldata
{
	void* pPixelData;
	int nPixelDataWidth, nPixelDataHeight;
	double dPixelDataWidth, dPixelDataHeight;
	double dPixelDeltaX, dPixelDeltaY;
	int nOutputWidth, nOutputHeight;
	OGREnvelope stWorldBounds;
}RAWPIXELDATA;
/** \class IRasterRenderer carto.h
    \brief The base class for map raster layer renderer
*/
class IRasterRenderer : public IRenderer
{
public:
	virtual ~IRasterRenderer(void){};
	virtual void PutRaster(wxGISRasterDatasetSPtr pRaster) = 0;
	virtual int *GetBandsCombination(int *pnBandCount) = 0;
	virtual void Draw(RAWPIXELDATA &stPixelData, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL) = 0;
	//transform pixel from raster value to display unsigned char and store it in pOutputData (ARGB32)
	virtual void FillPixel(unsigned char* pOutputData, void *pSrcValR, void *pSrcValG, void *pSrcValB, void *pSrcValA) = 0;
};
DEFINE_SHARED_PTR(IRasterRenderer);

