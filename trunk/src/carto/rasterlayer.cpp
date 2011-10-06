/******************************************************************************
 * Project:  wxGIS
 * Purpose:  RasterLayer header.
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
#include "wxgis/carto/rasterlayer.h"
#include "wxgis/carto/rasterrenderer.h"
#include "wxgis/display/displaytransformation.h"

wxGISRasterLayer::wxGISRasterLayer(wxGISDatasetSPtr pwxGISDataset) : wxGISLayer()
{
    m_pwxGISRasterDataset = boost::dynamic_pointer_cast<wxGISRasterDataset>(pwxGISDataset);
	if(m_pwxGISRasterDataset)
	{
		m_pSpatialReference = m_pwxGISRasterDataset->GetSpatialReference();
		m_FullEnvelope = m_pwxGISRasterDataset->GetEnvelope();

        //TODO: load or get all renderers and check if i render can draw this dataset. If yes - set it as current
		if(m_pwxGISRasterDataset->GetBandCount() >= 3)
			m_pRasterRenderer = boost::static_pointer_cast<IRasterRenderer>(boost::make_shared<wxGISRasterRGBARenderer>());
        else if(m_pwxGISRasterDataset->GetBandCount() == 1)
        {
		    GDALDataset* poGDALDataset = m_pwxGISRasterDataset->GetMainRaster();
		    if(!poGDALDataset)
			    poGDALDataset = m_pwxGISRasterDataset->GetRaster();
		    if(!poGDALDataset)
			    return;

		    GDALRasterBand* pBand = poGDALDataset->GetRasterBand(1);
            GDALColorInterp eColorInterpretation = pBand->GetColorInterpretation();
            if( eColorInterpretation == GCI_PaletteIndex )
            {
	    	    //RasterColormapRenderer
                m_pRasterRenderer = boost::static_pointer_cast<IRasterRenderer>(boost::make_shared<wxGISRasterRasterColormapRenderer>());
            }
            else if( eColorInterpretation == GCI_GrayIndex )
            {
    		//TODO: else RasterStretchColorRampRenderer
            }
        }
		SetName(m_pwxGISRasterDataset->GetName());
	}

	if(m_pRasterRenderer)
		m_pRasterRenderer->PutRaster(m_pwxGISRasterDataset);
}

wxGISRasterLayer::~wxGISRasterLayer(void)
{
}

bool wxGISRasterLayer::Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
	if(m_pRasterRenderer->CanRender(m_pwxGISRasterDataset))
	{
	    //bool bSetFilter(false);
		////Check if get all features
		//OGREnvelope Env = pDisplay->GetBounds();
		//if(!IsDoubleEquil(m_PreviousEnvelope.MaxX, Env.MaxX) || !IsDoubleEquil(m_PreviousEnvelope.MaxY, Env.MaxY) || !IsDoubleEquil(m_PreviousEnvelope.MinX, Env.MinX) || !IsDoubleEquil(m_PreviousEnvelope.MinY, Env.MinY))
		//{
		//	OGREnvelope TempFullEnv = m_FullEnvelope;
		//	//use angle
		//	if(!IsDoubleEquil(pDisplay->GetRotate(), 0.0))
		//	{
		//		double dCenterX = Env.MinX + (Env.MaxX - Env.MinX) / 2;
		//		double dCenterY = Env.MinY + (Env.MaxY - Env.MinY) / 2;

		//		RotateEnvelope(TempFullEnv, pDisplay->GetRotate(), dCenterX, dCenterY);
		//	}
		//	bSetFilter = TempFullEnv.Contains(Env) != 0;
		//}

		//store envelope
		//m_PreviousEnvelope = Env;

	 //   //Get pixel dataset
	 //   if(bSetFilter)
	 //   {
		//	//const CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
		//	//pCursor = m_pwxGISFeatureDataset->SearchGeometry(&Rect);
		//}
		//else
		//{
		//	//pCursor = m_pwxGISFeatureDataset->SearchGeometry();
		//}

		RAWPIXELDATA stPixelData;
		stPixelData.pPixelData = NULL;
		stPixelData.nOutputWidth = stPixelData.nOutputHeight = -1;
		stPixelData.nPixelDataWidth = stPixelData.nPixelDataHeight = -1;


		if(GetPixelData(stPixelData, pDisplay, pTrackCancel))
			m_pRasterRenderer->Draw(stPixelData, DrawPhase, pDisplay, pTrackCancel);
		if(stPixelData.pPixelData)
			CPLFree (stPixelData.pPixelData);
	}
	return true;

	//IDisplayTransformation* pDisplayTransformation = pDisplay->GetDisplayTransformation();
	//if(!pDisplayTransformation)
	//	return;
	////1. get envelope
 //   OGREnvelope Env = pDisplayTransformation->GetVisibleBounds();
 //   const OGREnvelope* LayerEnv = m_pwxGISRasterDataset->GetEnvelope();
 //   OGRSpatialReference* pEnvSpaRef = pDisplayTransformation->GetSpatialReference();
 //   const OGRSpatialReferenceSPtr pLayerSpaRef = m_pwxGISRasterDataset->GetSpatialReference();

 //   if(pLayerSpaRef && pEnvSpaRef)
 //   {
 //       if(!pLayerSpaRef->IsSame(pEnvSpaRef))
 //       {
 //           OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( pEnvSpaRef, pLayerSpaRef.get() );
 //           poCT->Transform(1, &Env.MaxX, &Env.MaxY);
 //           poCT->Transform(1, &Env.MinX, &Env.MinY);
 //           OCTDestroyCoordinateTransformation(poCT);
 //       }
 //   }

	////2. set spatial filter
	//pDisplay->StartDrawing(GetCacheID());
	//if(m_pRasterRenderer && m_pRasterRenderer->CanRender(m_pwxGISRasterDataset))
	//{
	//	m_pRasterRenderer->Draw(m_pwxGISRasterDataset, DrawPhase, pDisplay, pTrackCancel);
	//////	wxGISFeatureSet* pGISFeatureSet(NULL);
	////	//3. get raster set
	////	wxGISSpatialFilter pFilter;
	////	pFilter.SetEnvelope(Env);
	//////	pGISFeatureSet = m_pwxGISFeatureDataset->GetFeatureSet(&pFilter, pTrackCancel);
	////	wxImage Image = m_pwxGISRasterDataset->GetSubimage(pDisplayTransformation, &pFilter, pTrackCancel);
	////	if(Image.IsOk())
	////	{
	////		pDisplay->DrawBitmap(Image, 0, 0);
	////		//4. send it to renderer
	//////	m_pFeatureRenderer->Draw(pGISFeatureSet, DrawPhase, pDisplay, pTrackCancel);
	//	m_pPreviousDisplayEnv = Env;
	////	}
	//}
	////5. clear a spatial filter		
	//pDisplay->FinishDrawing();
}

OGRSpatialReferenceSPtr wxGISRasterLayer::GetSpatialReference(void)
{
	return m_pSpatialReference;
}

void wxGISRasterLayer::SetSpatialReference(OGRSpatialReferenceSPtr pSpatialReference)
{
    if(NULL == pSpatialReference)
        return;
    if(m_pSpatialReference && m_pSpatialReference->IsSame(pSpatialReference.get()))
        return;
    m_pSpatialReference = pSpatialReference;
	//TODO: перепроецирования растра делать поблочно для его текущего уровня пирамид. После смены уровня оставлять запомненым в массиве - подумать об использовании in memory raster для этих целей.
}

OGREnvelope wxGISRasterLayer::GetEnvelope(void)
{
	return m_FullEnvelope;
}

bool wxGISRasterLayer::IsValid(void)
{
	return m_pwxGISRasterDataset == NULL ? false : true;
}

bool wxGISRasterLayer::GetPixelData(RAWPIXELDATA &stPixelData, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
	if(!pDisplay)
		return false;
	OGREnvelope stRasterExtent = m_pwxGISRasterDataset->GetEnvelope();
	OGREnvelope stDisplayExtent = pDisplay->GetBounds();
	//rotate raster extent
	if(!IsDoubleEquil(pDisplay->GetRotate(), 0.0))
	{
		double dCenterX = stDisplayExtent.MinX + (stDisplayExtent.MaxX - stDisplayExtent.MinX) / 2;
		double dCenterY = stDisplayExtent.MinY + (stDisplayExtent.MaxY - stDisplayExtent.MinY) / 2;

		RotateEnvelope(stRasterExtent, pDisplay->GetRotate(), dCenterX, dCenterY);
	}

	//if envelopes don't intersect exit
    if(!stDisplayExtent.Intersects(stRasterExtent))
        return false;

	//get intersect envelope to fill raster data
	OGREnvelope stDrawBounds = stRasterExtent;
	stDrawBounds.Intersect(stDisplayExtent);
	if(!stDrawBounds.IsInit())
		return false;

	GDALDataset* pRaster = m_pwxGISRasterDataset->GetRaster();
	//create inverse geo transform to get pixel data
	double adfGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	double adfReverseGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	CPLErr err = pRaster->GetGeoTransform(adfGeoTransform);
	bool bNoTransform(false);
	if(err != CE_None)
	{
		bNoTransform = true;
	}
	else
	{
		int nRes = GDALInvGeoTransform( adfGeoTransform, adfReverseGeoTransform );
	}

	//width & height of extent
	double dOutWidth = stDrawBounds.MaxX - stDrawBounds.MinX;
	double dOutHeight = stDrawBounds.MaxY - stDrawBounds.MinY;

	//get width & height in pixels of draw area
	pDisplay->World2DCDist(&dOutWidth, &dOutHeight);
	if(dOutWidth < 0) dOutWidth *= -1;
	if(dOutHeight < 0) dOutHeight *= -1;

	//round float pixel to int using ceil
	int nOutWidth = ceil(dOutWidth);
	int nOutHeight = ceil(dOutHeight);

	//raster size
    int nXSize = m_pwxGISRasterDataset->GetWidth();
    int nYSize = m_pwxGISRasterDataset->GetHeight();
	    
	//transform from world extent to pixel bounds
	OGREnvelope stPixelBounds = stDrawBounds;
    if(bNoTransform)
    {
		//swap min/max
        stPixelBounds.MaxY = nYSize - stDrawBounds.MinY;
        stPixelBounds.MinY = nYSize - stDrawBounds.MaxY;
    }
    else
    {
		GDALApplyGeoTransform( adfReverseGeoTransform, stDrawBounds.MinX, stDrawBounds.MinY, &stPixelBounds.MinX, &stPixelBounds.MaxY );
		GDALApplyGeoTransform( adfReverseGeoTransform, stDrawBounds.MaxX, stDrawBounds.MaxY, &stPixelBounds.MaxX, &stPixelBounds.MinY );
    }

	//get width & height in pixels of raster area
    double dWidth = stPixelBounds.MaxX - stPixelBounds.MinX;
    double dHeight = stPixelBounds.MaxY - stPixelBounds.MinY;

	int nWidth = ceil(dWidth);
	int nHeight = ceil(dHeight);
    int nMinX = floor(stPixelBounds.MinX);
    int nMinY = floor(stPixelBounds.MinY);

	GDALDataType eDT = m_pwxGISRasterDataset->GetDataType();
	int nDataSize = GDALGetDataTypeSize(eDT) / 8;

	void *data = NULL;
	int nBandCount(0);
	int *panBands = m_pRasterRenderer->GetBandsCombination(&nBandCount); 
	
	if( nOutWidth > nWidth && nOutHeight > nHeight ) // not scale
	{
		//increase little in data
		nWidth += 2; nHeight += 2;
		if(nWidth > nXSize) nWidth = nXSize; 
		if(nHeight > nYSize) nHeight = nYSize;

		stPixelData.dPixelDeltaX = stPixelBounds.MinX - double(nMinX);// - 0.5;
		stPixelData.dPixelDeltaY = stPixelBounds.MinY - double(nMinY);// - 0.5;
		stPixelData.dPixelDataWidth = dWidth;
		stPixelData.dPixelDataHeight = dHeight;
		stPixelData.nPixelDataWidth = nWidth;
		stPixelData.nPixelDataHeight = nHeight;
		stPixelData.nOutputWidth = nOutWidth;
		stPixelData.nOutputHeight = nOutHeight;

		data = CPLMalloc (nWidth * nHeight * nDataSize * nBandCount);
		if(!m_pwxGISRasterDataset->GetPixelData(data, nMinX, nMinY, nWidth, nHeight, nWidth, nHeight, eDT, nBandCount, panBands))
			return false;//the data is free outside
	}
	else
	{
		stPixelData.dPixelDeltaX = stPixelData.dPixelDeltaY = 0;
		stPixelData.dPixelDataWidth = nOutWidth;
		stPixelData.dPixelDataHeight = nOutHeight;
		stPixelData.nPixelDataWidth = nOutWidth;
		stPixelData.nPixelDataHeight = nOutHeight;
		stPixelData.nOutputWidth = -1;
		stPixelData.nOutputHeight = -1;

		data = CPLMalloc (nOutWidth * nOutHeight * nDataSize * nBandCount);
		if(!m_pwxGISRasterDataset->GetPixelData(data, nMinX, nMinY, nWidth, nHeight, nOutWidth, nOutHeight, eDT, nBandCount, panBands))
			return false;//the data is free outside
	}

	stPixelData.pPixelData = data;
	stPixelData.stWorldBounds = stDrawBounds;

	return true;

	//check if zoom_in
    //bool bIsZoomIn = stDisplayExtent.MaxX > stExtent.MaxX || stDisplayExtent.MaxY > stExtent.MaxY || stDisplayExtent.MinX < stExtent.MinX || stDisplayExtent.MinY < stExtent.MinY;
	//	1. Raster extent inside display -> get full raster data
	//	2. Raster extent overlap dispaly -> get intersect envelope -> get raster data


    //if(bIsZoomIn)
    //{
	//	//int stride = cairo_format_stride_for_width (CAIRO_FORMAT_RGB24, nOutWidth);// * 3 CAIRO_FORMAT_ARGB32 GDT_UInt32
	//	////CAIRO_FORMAT_A8 GDT_Byte
	//	//if(stride == -1)
	//	//	return;//TODO: ERROR
	//	//void *data = malloc (stride * nOutHeight);//TODO: CPLMalloc(
	//	//
	//	////memset(data, 255, stride * nOutHeight);
	//	//if(!m_pwxGISRasterDataset->GetPixelData(data, nMinX, nMinY, nWidth, nHeight, nOutWidth, nOutHeight, GDT_Byte, 4, bands))
	//	//{
	//	//	free(data);//TODO CPLFree
	//	//	return;
	//	//}
	//	//cairo_surface_t *surface;
	//	//surface = cairo_image_surface_create_for_data ((unsigned char*)data, CAIRO_FORMAT_RGB24, nOutWidth, nOutHeight, stride);
	//	////
	//	//double dX = DrawBounds.MinX + (DrawBounds.MaxX - DrawBounds.MinX) / 2;
	//	//double dY = DrawBounds.MinY + (DrawBounds.MaxY - DrawBounds.MinY) / 2;
	//	//pDisplay->DrawRaster(surface, DrawBounds);

	//	//cairo_surface_destroy(surface);

 //       //scale pTempData to data using interpolation methods
 //       //pDisplay->DrawBitmap(Scale(data, nWidthOut, nHeightOut, rImgWidthOut, rImgHeightOut, nWidth, nHeight, rMinX - nMinX, rMinY - nMinY, enumGISQualityBilinear, pTrackCancel), nDCXOrig, nDCYOrig); //enumGISQualityNearest

	//}
	//else
	//{
	//	    //1. convert newrasterenvelope to DC		
	//	    OGRRawPoint OGRRawPoints[2];
	//	    OGRRawPoints[0].x = RasterEnvelope.MinX;
	//	    OGRRawPoints[0].y = RasterEnvelope.MinY;
	//	    OGRRawPoints[1].x = RasterEnvelope.MaxX;
	//	    OGRRawPoints[1].y = RasterEnvelope.MaxY;
	//	    wxPoint* pDCPoints = pDisplayTransformation->TransformCoordWorld2DC(OGRRawPoints, 2);	

	//	    //2. get image data from raster - buffer size = DC_X and DC_Y - draw full raster
	//	    if(!pDCPoints)
	//	    {
	//		    wxDELETEA(pDCPoints);
	//		    return;
	//	    }
	//	    int nDCXOrig = pDCPoints[0].x;
	//	    int nDCYOrig = pDCPoints[1].y;
	//	    int nWidth = pDCPoints[1].x - pDCPoints[0].x;
	//	    int nHeight = pDCPoints[0].y - pDCPoints[1].y;
	//	    delete[](pDCPoints);

	//	    GDALDataset* pGDALDataset = pRaster->GetRaster();
	//	    int nImgWidth = pGDALDataset->GetRasterXSize();
	//	    int nImgHeight = pGDALDataset->GetRasterYSize();

	//	    //create buffer
	//	    unsigned char* data = new unsigned char[nWidth * nHeight * 3];
	//	    if(IsSpaRefSame)
	//	    {
	//	        CPLErr err = pGDALDataset->AdviseRead(0, 0, nImgWidth, nImgHeight, nWidth, nHeight, GDT_Byte, 3, bands, NULL);
	//	        if(err != CE_None)
	//	        {
 //                   const char* pszerr = CPLGetLastErrorMsg();
 //                   wxLogError(_("AdviseRead failed! GDAL error: %s"), wgMB2WX(pszerr));
	//			    wxDELETEA(data);
	//			    return;
	//	        }

	//		    //read in buffer
	//		    err = pGDALDataset->RasterIO(GF_Read, 0, 0, nImgWidth, nImgHeight, data, nWidth, nHeight, GDT_Byte, 3, bands, sizeof(unsigned char) * 3, 0, sizeof(unsigned char));
	//		    if(err != CE_None)
	//		    {
 //                   const char* pszerr = CPLGetLastErrorMsg();
 //                   wxLogError(_("RasterIO failed! GDAL error: %s"), wgMB2WX(pszerr));
	//			    wxDELETEA(data);
	//			    return;
	//		    }
	//	    }
	//	    else
	//	    {
	//		    //1. calc Width & Height of TempData with same aspect ratio of raster
	//		    //2. create pTempData buffer
	//		    unsigned char* pTempData;
	//		    //3. fill data
	//		    //4. for each pixel of data buffer get pixel from pTempData using OGRCreateCoordinateTransformation
	//		    //delete[](data);
	//	    }
	//	    //3. draw 
	//	    wxImage ResultImage(nWidth, nHeight, data);
	//	    pDisplay->DrawBitmap(ResultImage, nDCXOrig, nDCYOrig);

	//	    //delete[](data);
	//}

	////TODO: reprojection
	////GDALCreateGenImgProjTransformer
	////GDALCreateReprojectionTransformer
	//		
	//return true;
}
