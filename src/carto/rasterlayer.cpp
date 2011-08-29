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

		m_pRasterRenderer = boost::static_pointer_cast<IRasterRenderer>(boost::make_shared<wxGISRasterRGBRenderer>());

		SetName(m_pwxGISRasterDataset->GetName());
	}
}

wxGISRasterLayer::~wxGISRasterLayer(void)
{
}

bool wxGISRasterLayer::Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
	if(m_pRasterRenderer->CanRender(m_pwxGISRasterDataset))
	{
	    bool bSetFilter(false);
		//Check if get all features
		OGREnvelope Env = pDisplay->GetBounds();
		if(!IsDoubleEquil(m_PreviousEnvelope.MaxX, Env.MaxX) || !IsDoubleEquil(m_PreviousEnvelope.MaxY, Env.MaxY) || !IsDoubleEquil(m_PreviousEnvelope.MinX, Env.MinX) || !IsDoubleEquil(m_PreviousEnvelope.MinY, Env.MinY))
		{
			OGREnvelope TempFullEnv = m_FullEnvelope;
			//use angle
			if(!IsDoubleEquil(pDisplay->GetRotate(), 0.0))
			{
				double dCenterX = Env.MinX + (Env.MaxX - Env.MinX) / 2;
				double dCenterY = Env.MinY + (Env.MaxY - Env.MinY) / 2;

				RotateEnvelope(TempFullEnv, pDisplay->GetRotate(), dCenterX, dCenterY);
			}
			bSetFilter = TempFullEnv.Contains(Env) != 0;
		}

		//store envelope
		m_PreviousEnvelope = Env;

	    //Get pixel dataset
	    if(bSetFilter)
	    {
			//const CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
			//pCursor = m_pwxGISFeatureDataset->SearchGeometry(&Rect);
		}
		else
		{
			//pCursor = m_pwxGISFeatureDataset->SearchGeometry();
		}
		//m_pFeatureRenderer->Draw(pCursor, DrawPhase, pDisplay, pTrackCancel);
		GetSubRaster(Env, pDisplay, pTrackCancel);
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

void wxGISRasterLayer::GetSubRaster(OGREnvelope& Envelope, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
	if(!pDisplay)
		return;
	
	OGREnvelope stRasterExtent = m_pwxGISRasterDataset->GetEnvelope();
    if(!Envelope.Intersects(stRasterExtent))
        return;

	//create band combination
	int nBandCount = m_pwxGISRasterDataset->GetBandCount();
	int bands[4] = {1,1,1,1};

	//TODO: band combination get from user
	if(nBandCount >= 3)
	{
		bands[0] = 3;
		bands[1] = 2;
		bands[2] = 1;		
		bands[3] = 1;	//alpha	
	}

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

	//check if zoom_in
    //bool bIsZoomIn = stDisplayExtent.MaxX > Envelope.MaxX || stDisplayExtent.MaxY > Envelope.MaxY || stDisplayExtent.MinX < Envelope.MinX || stDisplayExtent.MinY < Envelope.MinY;

    //if(bIsZoomIn)
    //{
		//intersect display & raster bounds
	    OGREnvelope DrawBounds = stRasterExtent;
		DrawBounds.Intersect(Envelope);
		if(!DrawBounds.IsInit())
			return;
		//DrawBounds.MinX = std::max(stRasterExtent.MinX, Envelope.MinX);
	 //   DrawBounds.MinY = std::max(stRasterExtent.MinY, Envelope.MinY);
	 //   DrawBounds.MaxX = std::min(stRasterExtent.MaxX, Envelope.MaxX);
	 //   DrawBounds.MaxY = std::min(stRasterExtent.MaxY, Envelope.MaxY);

		double dOutWidth = DrawBounds.MaxX - DrawBounds.MinX;
		double dOutHeight = DrawBounds.MaxY - DrawBounds.MinY;

		//get width & height in pixels
		pDisplay->World2DCDist(&dOutWidth, &dOutHeight);
		if(dOutWidth < 0) 
			dOutWidth *= -1;
		if(dOutHeight < 0) 
			dOutHeight *= -1;

		int nOutWidth = int(dOutWidth);// + 0.5);//ceil(dOutWidth) + 1;
		int nOutHeight = int(dOutHeight);// + 0.5);//ceil(dOutHeight) + 1;

	    //get image data from raster
        int nXSize = m_pwxGISRasterDataset->GetWidth();
        int nYSize = m_pwxGISRasterDataset->GetHeight();
	    OGREnvelope PixelBounds = DrawBounds;
        if(bNoTransform)
        {
            PixelBounds.MaxY = nYSize - DrawBounds.MinY;
            PixelBounds.MinY = nYSize - DrawBounds.MaxY;
        }
        else
        {
		    GDALApplyGeoTransform( adfReverseGeoTransform, DrawBounds.MinX, DrawBounds.MinY, &PixelBounds.MinX, &PixelBounds.MaxY );
		    GDALApplyGeoTransform( adfReverseGeoTransform, DrawBounds.MaxX, DrawBounds.MaxY, &PixelBounds.MaxX, &PixelBounds.MinY );
        }

        double dWidth = PixelBounds.MaxX - PixelBounds.MinX;
        double dHeight = PixelBounds.MaxY - PixelBounds.MinY;

	    int nWidth = int(dWidth);// + 0.5);//ceil(dWidth) + 1;
	    int nHeight = int(dHeight);// + 0.5);//ceil(dHeight) + 1;
        int nMinX = int(PixelBounds.MinX);//floor
        int nMinY = int(PixelBounds.MinY);//floor

		int stride = cairo_format_stride_for_width (CAIRO_FORMAT_RGB24, nOutWidth);// * 3 CAIRO_FORMAT_ARGB32 GDT_UInt32
		//CAIRO_FORMAT_A8 GDT_Byte
		if(stride == -1)
			return;//TODO: ERROR
		void *data = malloc (stride * nOutHeight);
		
		//memset(data, 255, stride * nOutHeight);
		if(!m_pwxGISRasterDataset->GetPixelData(data, nMinX, nMinY, nWidth, nHeight, nOutWidth, nOutHeight, GDT_Byte, 4, bands))
		{
			free(data);
			return;
		}
		cairo_surface_t *surface;
		surface = cairo_image_surface_create_for_data ((unsigned char*)data, CAIRO_FORMAT_RGB24, nOutWidth, nOutHeight, stride);
		//
		double dX = DrawBounds.MinX + (DrawBounds.MaxX - DrawBounds.MinX) / 2;
		double dY = DrawBounds.MinY + (DrawBounds.MaxY - DrawBounds.MinY) / 2;
		pDisplay->DrawRaster(surface, DrawBounds);

		cairo_surface_destroy(surface);

        //scale pTempData to data using interpolation methods
        //pDisplay->DrawBitmap(Scale(data, nWidthOut, nHeightOut, rImgWidthOut, rImgHeightOut, nWidth, nHeight, rMinX - nMinX, rMinY - nMinY, enumGISQualityBilinear, pTrackCancel), nDCXOrig, nDCYOrig); //enumGISQualityNearest

/*	}
	else
	{
		    //1. convert newrasterenvelope to DC		
		    OGRRawPoint OGRRawPoints[2];
		    OGRRawPoints[0].x = RasterEnvelope.MinX;
		    OGRRawPoints[0].y = RasterEnvelope.MinY;
		    OGRRawPoints[1].x = RasterEnvelope.MaxX;
		    OGRRawPoints[1].y = RasterEnvelope.MaxY;
		    wxPoint* pDCPoints = pDisplayTransformation->TransformCoordWorld2DC(OGRRawPoints, 2);	

		    //2. get image data from raster - buffer size = DC_X and DC_Y - draw full raster
		    if(!pDCPoints)
		    {
			    wxDELETEA(pDCPoints);
			    return;
		    }
		    int nDCXOrig = pDCPoints[0].x;
		    int nDCYOrig = pDCPoints[1].y;
		    int nWidth = pDCPoints[1].x - pDCPoints[0].x;
		    int nHeight = pDCPoints[0].y - pDCPoints[1].y;
		    delete[](pDCPoints);

		    GDALDataset* pGDALDataset = pRaster->GetRaster();
		    int nImgWidth = pGDALDataset->GetRasterXSize();
		    int nImgHeight = pGDALDataset->GetRasterYSize();

		    //create buffer
		    unsigned char* data = new unsigned char[nWidth * nHeight * 3];
		    if(IsSpaRefSame)
		    {
		        CPLErr err = pGDALDataset->AdviseRead(0, 0, nImgWidth, nImgHeight, nWidth, nHeight, GDT_Byte, 3, bands, NULL);
		        if(err != CE_None)
		        {
                    const char* pszerr = CPLGetLastErrorMsg();
                    wxLogError(_("AdviseRead failed! GDAL error: %s"), wgMB2WX(pszerr));
				    wxDELETEA(data);
				    return;
		        }

			    //read in buffer
			    err = pGDALDataset->RasterIO(GF_Read, 0, 0, nImgWidth, nImgHeight, data, nWidth, nHeight, GDT_Byte, 3, bands, sizeof(unsigned char) * 3, 0, sizeof(unsigned char));
			    if(err != CE_None)
			    {
                    const char* pszerr = CPLGetLastErrorMsg();
                    wxLogError(_("RasterIO failed! GDAL error: %s"), wgMB2WX(pszerr));
				    wxDELETEA(data);
				    return;
			    }
		    }
		    else
		    {
			    //1. calc Width & Height of TempData with same aspect ratio of raster
			    //2. create pTempData buffer
			    unsigned char* pTempData;
			    //3. fill data
			    //4. for each pixel of data buffer get pixel from pTempData using OGRCreateCoordinateTransformation
			    //delete[](data);
		    }
		    //3. draw 
		    wxImage ResultImage(nWidth, nHeight, data);
		    pDisplay->DrawBitmap(ResultImage, nDCXOrig, nDCYOrig);

		    //delete[](data);
	}

	//TODO: reprojection
	//GDALCreateGenImgProjTransformer
	//GDALCreateReprojectionTransformer

			*/
}
