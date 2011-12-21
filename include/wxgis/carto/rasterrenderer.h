/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISRasterRGBARenderer classes.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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

#include "wxgis/carto/carto.h"
#include "wxgis/carto/stretch.h"

/** \class wxGISRasterRGBARenderer rasterrenderer.h
    \brief The raster layer renderer for RGB data and Alpha channel
*/
class wxRasterDrawThread : public wxThread
{
public:
	wxRasterDrawThread(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, int nBandCount, unsigned char *pTransformData, wxGISEnumDrawQuality eQuality, int nOutXSize, int nOutYSize, int nBegY, int nEndY, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel = NULL);
    virtual void *Entry();
    virtual void OnExit();
private:
    ITrackCancel* m_pTrackCancel;
	IRasterRenderer *m_pRasterRenderer;
	RAWPIXELDATA &m_stPixelData;
	GDALDataType m_eSrcType;
	unsigned char *m_pTransformData;
	wxGISEnumDrawQuality m_eQuality;
	int m_nOutXSize;
	int m_nOutYSize;
	int m_nBegY;
	int m_nEndY;
	int m_nBandCount;
};

/** \class wxGISRasterRenderer rasterrenderer.h
    \brief The base class for renderers
*/
class wxGISRasterRenderer :
	public IRasterRenderer
{
public:
	wxGISRasterRenderer(void);
	virtual ~wxGISRasterRenderer(void);
//IRasterRenderer
	virtual bool CanRender(wxGISDatasetSPtr pDataset);
	virtual void PutRaster(wxGISRasterDatasetSPtr pRaster);
	virtual int *GetBandsCombination(int *pnBandCount) = 0;
	virtual void Draw(RAWPIXELDATA &stPixelData, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL);
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA) = 0;
    virtual wxGISColorTable GetColorTable(void){return m_mColorTable;};
protected:
	virtual bool OnPixelProceed(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, unsigned char *pTransformData, ITrackCancel *pTrackCancel ) = 0;
protected:
	wxColour m_oNoDataColor;
	//statistics - current display extent, each raster dataset, custom settings
	wxGISRasterDatasetSPtr m_pwxGISRasterDataset;
	wxGISEnumDrawQuality m_eQuality;
    wxGISColorTable m_mColorTable;
};

/** \class wxGISRasterRGBARenderer rasterrenderer.h
    \brief The raster layer renderer for RGB data and Alpha channel
*/
class wxGISRasterRGBARenderer :
	public wxGISRasterRenderer
{
public:
	wxGISRasterRGBARenderer(void);
	virtual ~wxGISRasterRGBARenderer(void);
//IRasterRenderer
	virtual bool CanRender(wxGISDatasetSPtr pDataset);
	virtual void PutRaster(wxGISRasterDatasetSPtr pRaster);
	virtual int *GetBandsCombination(int *pnBandCount);
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA);
    virtual wxGISEnumRendererType GetDataType(void){return enumGISRenderTypeRGBA;};
protected:
	virtual bool OnPixelProceed(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, unsigned char *pTransformData, ITrackCancel *pTrackCancel );
	virtual void OnFillStats(void);
protected:
	int m_nRedBand, m_nGreenBand, m_nBlueBand, m_nAlphaBand;
	//wxColour m_oBkColorGet, m_oBkColorSet, 
	//statistics - current display extent, each raster dataset, custom settings
    wxGISStretch *m_paStretch[4];
	bool m_bNodataNewBehaviour;
};

/** \class wxGISRasterRasterColormapRenderer rasterrenderer.h
    \brief The raster layer renderer for Palette Index data
*/
class wxGISRasterRasterColormapRenderer :
	public wxGISRasterRenderer
{
public:
	wxGISRasterRasterColormapRenderer(void);
	virtual ~wxGISRasterRasterColormapRenderer(void);
//IRasterRenderer
	virtual bool CanRender(wxGISDatasetSPtr pDataset);
	virtual void PutRaster(wxGISRasterDatasetSPtr pRaster);
	virtual int *GetBandsCombination(int *pnBandCount);
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA);
    virtual wxGISEnumRendererType GetDataType(void){return enumGISRenderTypeIndexed;};
    //virtual const wxColor *GetColorByIndex(long nIndex);
protected:
	virtual bool OnPixelProceed(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, unsigned char *pTransformData, ITrackCancel *pTrackCancel );
	virtual void OnFillColorTable(void);
	wxColor HSVtoRGB( const short &h, const short &s, const short &v, const short &alpha );
	wxColor CMYKtoRGB( const short &c, const short &m, const short &y, const short &k );
protected:
	int m_nBandNumber;
    bool m_bHasNoData;
    short m_nNoDataIndex;
};

/** \class wxGISRasterGreyScaleRenderer rasterrenderer.h
    \brief The raster layer renderer for grey scale data
*/
class wxGISRasterGreyScaleRenderer :
	public wxGISRasterRenderer
{
public:
	wxGISRasterGreyScaleRenderer(void);
	virtual ~wxGISRasterGreyScaleRenderer(void);
//IRasterRenderer
	virtual bool CanRender(wxGISDatasetSPtr pDataset);
	virtual void PutRaster(wxGISRasterDatasetSPtr pRaster);
	virtual int *GetBandsCombination(int *pnBandCount);
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA);
    virtual wxGISEnumRendererType GetDataType(void){return enumGISRenderTypeRGBA;};
protected:
	virtual bool OnPixelProceed(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, unsigned char *pTransformData, ITrackCancel *pTrackCancel );
	virtual void OnFillStats(void);
protected:
	int m_nBand;
    wxGISStretch m_oStretch;
};

/** \class wxGISRasterPackedRGBARenderer rasterrenderer.h
    \brief The raster layer renderer for packed RGB data and Alpha channel
*/
class wxGISRasterPackedRGBARenderer :
	public wxGISRasterGreyScaleRenderer
{
public:
	wxGISRasterPackedRGBARenderer(void);
	virtual ~wxGISRasterPackedRGBARenderer(void);
//IRasterRenderer
	virtual bool CanRender(wxGISDatasetSPtr pDataset);
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA);
    virtual wxGISEnumRendererType GetDataType(void){return enumGISRenderTypePackedRGBA;};
protected:
	virtual void OnFillStats(void);
protected:
	bool m_bNodataNewBehaviour;
};