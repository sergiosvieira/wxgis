/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISRasterRGBARenderer classes.
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
#include "wxgis/carto/rasterrenderer.h"
#include "wxgis/carto/interpolation.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/display/screendisplay.h"
//-----------------------------------
// wxRasterDrawThread
//-----------------------------------

wxRasterDrawThread::wxRasterDrawThread(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, int nBandCount, unsigned char *pTransformData, wxGISEnumDrawQuality nQuality, int nOutXSize, int nOutYSize, int nBegY, int nEndY, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel) : wxThread(wxTHREAD_JOINABLE), m_stPixelData(stPixelData)
{
    m_pTrackCancel = pTrackCancel;
	m_eSrcType = eSrcType;
	m_pTransformData = pTransformData;
	m_nBandCount = nBandCount;
	m_nQuality = nQuality;
	m_nOutXSize = nOutXSize;
	m_nOutYSize = nOutYSize;
	m_nBegY = nBegY;
	m_nEndY = nEndY;
	m_pRasterRenderer = pRasterRenderer;
	m_bNodataNewBehaviour = GetConfig()->
}

void *wxRasterDrawThread::Entry()
{
	//double rWRatio, rHRatio;
	//rWRatio = m_rOrigX / m_nDestX;
	//rHRatio = m_rOrigY / m_nDestY;
	//switch(m_Quality)
	//{
	//case enumGISQualityBilinear:
	//	OnBilinearInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
	//	break;
	//case enumGISQualityHalfBilinear:
	//	OnHalfBilinearInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
	//	break;
	//case enumGISQualityHalfQuadBilinear:
	//	OnHalfQuadBilinearInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
	//	break;
	//case enumGISQualityBicubic:
	//	OnBicubicInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
	//	break;
	//case enumGISQualityNearest:
	//default:
	//	OnNearestNeighbourInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
	//	break;
	//}
	NearestNeighbourInterpolation(m_stPixelData.pPixelData, m_stPixelData.nPixelDataWidth, m_stPixelData.dPixelDataWidth, m_stPixelData.dPixelDataHeight, m_stPixelData.dPixelDeltaX, m_stPixelData.dPixelDeltaY, m_eSrcType, m_pTransformData, m_nOutXSize, m_nOutYSize, m_nBegY, m_nEndY, m_nBandCount, m_pRasterRenderer, m_pTrackCancel);

	return NULL;
}

void wxRasterDrawThread::OnExit()
{
}


//-----------------------------------
// wxGISRasterRGBARenderer
//-----------------------------------

wxGISRasterRGBARenderer::wxGISRasterRGBARenderer(void)
{
	m_nRedBand = 1;
	m_nGreenBand = 2;
	m_nBlueBand = 3;
	m_nAlphaBand = -1;
	m_nQuality = enumGISQualityNearest;

	BANDSTATS stDefault = {0, 255, 127.5, 180.3122292026, -99999.0};
	m_staBandStats[0] = stDefault;
	m_staBandStats[1] = stDefault;
	m_staBandStats[2] = stDefault;

	m_oNoDataColor = wxColor(0,0,0,0);
	//m_oBkColorSet = 
	//m_oBkColorGet = wxNullColour;//TODO: May be array of  ColorGet/ColorSet
}

wxGISRasterRGBARenderer::~wxGISRasterRGBARenderer(void)
{
}

bool wxGISRasterRGBARenderer::CanRender(wxGISDatasetSPtr pDataset)
{
	return pDataset->GetType() == enumGISRasterDataset ? true : false;
}

void wxGISRasterRGBARenderer::PutRaster(wxGISRasterDatasetSPtr pRaster)
{
	m_pwxGISRasterDataset = pRaster;
	OnFillStats();
}	

void wxGISRasterRGBARenderer::OnFillStats(void)
{
	//set min/max values
	//set nodata color for each band
	if(m_pwxGISRasterDataset->HasStatistics())
	{
		GDALDataset* poGDALDataset = m_pwxGISRasterDataset->GetMainRaster();
		if(!poGDALDataset)
			poGDALDataset = m_pwxGISRasterDataset->GetRaster();
		if(!poGDALDataset)
			return;
		GDALRasterBand* pRedBand = poGDALDataset->GetRasterBand(m_nRedBand);
        pRedBand->GetStatistics(FALSE, FALSE, &m_staBandStats[0].dfMin, &m_staBandStats[0].dfMax, &m_staBandStats[0].dfMean, &m_staBandStats[0].dfStdDev);
	    m_staBandStats[0].dfNoData = pRedBand->GetNoDataValue();

		GDALRasterBand* pGreenBand = poGDALDataset->GetRasterBand(m_nGreenBand);
        pGreenBand->GetStatistics(FALSE, FALSE, &m_staBandStats[1].dfMin, &m_staBandStats[1].dfMax, &m_staBandStats[1].dfMean, &m_staBandStats[1].dfStdDev);
	    m_staBandStats[1].dfNoData = pGreenBand->GetNoDataValue();

		GDALRasterBand* pBlueBand = poGDALDataset->GetRasterBand(m_nBlueBand);
        pBlueBand->GetStatistics(FALSE, FALSE, &m_staBandStats[2].dfMin, &m_staBandStats[2].dfMax, &m_staBandStats[2].dfMean, &m_staBandStats[2].dfStdDev);
	    m_staBandStats[2].dfNoData = pBlueBand->GetNoDataValue();
	}
}


int *wxGISRasterRGBARenderer::GetBandsCombination(int *pnBandCount)
{
	int *pBands = NULL;
	if(m_nAlphaBand == -1)
	{
		*pnBandCount = 3;
		pBands = new int[3];
		pBands[0] = m_nRedBand;
		pBands[1] = m_nGreenBand;
		pBands[2] = m_nBlueBand;
	}
	else
	{
		*pnBandCount = 4;
		pBands = new int[4];
		pBands[0] = m_nRedBand;
		pBands[1] = m_nGreenBand;
		pBands[2] = m_nBlueBand;
		pBands[3] = m_nAlphaBand;
	}
	return pBands;
}

bool wxGISRasterRGBARenderer::OnPixelProceed(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, unsigned char *pTransformData, ITrackCancel *pTrackCancel )
{
	int nOutXSize, nOutYSize;
	bool bScale = false;
	if(stPixelData.nOutputHeight == -1 || stPixelData.nOutputWidth == -1)
	{
		bScale = true;
		nOutXSize = stPixelData.nPixelDataWidth;
		nOutYSize = stPixelData.nPixelDataHeight;
	}
	else
	{
		nOutXSize = stPixelData.nOutputWidth;
		nOutYSize = stPixelData.nOutputHeight;
	}

    //multithreaded
    int CPUCount = wxThread::GetCPUCount();//1;//
    std::vector<wxRasterDrawThread*> threadarray;
    int nPartSize = nOutYSize / CPUCount;
    int nBegY(0), nEndY;
    for(int i = 0; i < CPUCount; ++i)
    {        
        if(i == CPUCount - 1)
            nEndY = nOutYSize;
        else
            nEndY = nPartSize * (i + 1);

		unsigned char* pDestInputData = pTransformData + (nBegY * 4 * nOutXSize);
		wxRasterDrawThread *thread = new wxRasterDrawThread(stPixelData, eSrcType, m_nAlphaBand == -1 ? 3 : 4, pDestInputData, bScale == true ? m_nQuality : enumGISQualityNearest, nOutXSize, nOutYSize, nBegY, nEndY, static_cast<IRasterRenderer*>(this), pTrackCancel);
		if(CreateAndRunThread(thread, wxT("wxRasterDrawThread"), wxT("RasterDrawThread")))
	       threadarray.push_back(thread);

        nBegY = nEndY;
    }

    for(size_t i = 0; i < threadarray.size(); ++i)
    {
        wgDELETE(threadarray[i], Wait());
    }
	return true;
}

void wxGISRasterRGBARenderer::Draw(RAWPIXELDATA &stPixelData, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
	int nOutXSize, nOutYSize;
	if(stPixelData.nOutputHeight == -1 || stPixelData.nOutputWidth == -1)
	{
		nOutXSize = stPixelData.nPixelDataWidth;
		nOutYSize = stPixelData.nPixelDataHeight;
	}
	else
	{
		nOutXSize = stPixelData.nOutputWidth;
		nOutYSize = stPixelData.nOutputHeight;
	}

	int stride = cairo_format_stride_for_width (CAIRO_FORMAT_ARGB32, nOutXSize);
	if(stride == -1)
		return;//TODO: ERROR
	unsigned char *pTransformPixelData = (unsigned char *)CPLMalloc (stride * nOutYSize);

	if(!OnPixelProceed(stPixelData, m_pwxGISRasterDataset->GetDataType(), pTransformPixelData, pTrackCancel))
		return;//TODO: ERROR
	
	cairo_surface_t *surface;
	surface = cairo_image_surface_create_for_data (pTransformPixelData, CAIRO_FORMAT_ARGB32, nOutXSize, nOutYSize, stride);
	//TODO: Put UpperLeft coord to DrawRaster not bounds???
	//double dX = DrawBounds.MinX + (DrawBounds.MaxX - DrawBounds.MinX) / 2;
	//double dY = DrawBounds.MinY + (DrawBounds.MaxY - DrawBounds.MinY) / 2;
	pDisplay->DrawRaster(surface, stPixelData.stWorldBounds);

	cairo_surface_destroy(surface);

	CPLFree((void*)pTransformPixelData);
}

unsigned char wxGISRasterRGBARenderer::GetPixel(unsigned char nBand, void *pSrcVal)
{
	//get result from function
	switch(m_pwxGISRasterDataset->GetDataType())
	{
	case GDT_Byte:
		return (void*)(pDataByte + nIndex * sizeof(GByte));
	case GDT_Float32:
		return (void*)(pDataByte + nIndex * sizeof(float));
	case GDT_Float64:
		return (void*)(pDataByte + nIndex * sizeof(double));
	case GDT_Int32:
		return (void*)(pDataByte + nIndex * sizeof(GInt32));
	case GDT_UInt16:
		return (void*)(pDataByte + nIndex * sizeof(GUInt16));
	case GDT_Int16:
		return (void*)(pDataByte + nIndex * sizeof(GInt16));
	case GDT_UInt32:
		return (void*)(pDataByte + nIndex * sizeof(GUInt32));
	case GDT_CInt16:
	case GDT_CInt32:
	case GDT_CFloat32:
	case GDT_CFloat64:
	default:
		return 0;
	}

	//http://www.calc.ru/103.html

	return (unsigned char)SRCVAL(pSrcVal, m_pwxGISRasterDataset->GetDataType(), 0);//TODO: transform from pixel type to unsigned char
}

void wxGISRasterRGBARenderer::FillPixel(unsigned char* pOutputData, void *pSrcValR, void *pSrcValG, void *pSrcValB, void *pSrcValA)
{
	if(pSrcValR == NULL)
		return;
	//wxBYTE_ORDER
	//pOutputData[0] = 0;	//	A	B
	//pOutputData[1] = 0;	//	R	G
	//pOutputData[2] = 255;	//	G	R
	//pOutputData[3] = 255;	//	B	A

	if(pSrcValA == NULL)
		pOutputData[3] = 255;
	else
		pOutputData[3] = GetPixel(3, pSrcValA);

	unsigned char RPixVal = GetPixel(0, pSrcValR);
	pOutputData[2] = RPixVal;

	if(pSrcValG == NULL || pSrcValB == NULL)
	{
		pOutputData[1] = RPixVal;
		pOutputData[0] = RPixVal;
	}
	else
	{
		pOutputData[1] = GetPixel(1, pSrcValG);
		pOutputData[0] = GetPixel(2, pSrcValB);
	}

	//check for nodata
	bool bIsChanged(false);
	if(m_bNodataNewBehaviour)
	{
		if(IsDoubleEquil(double(pOutputData[2]), m_staBandStats[0].dfNoData) && IsDoubleEquil(double(pOutputData[1]), m_staBandStats[1].dfNoData) && IsDoubleEquil(double(pOutputData[0]), m_staBandStats[2].dfNoData))
		{
			bIsChanged = true;
			pOutputData[3] = m_oNoDataColor.Alpha();
			pOutputData[2] = m_oNoDataColor.Red();
			pOutputData[1] = m_oNoDataColor.Green();
			pOutputData[0] = m_oNoDataColor.Blue();
		}
	}
	else
	{
		if(IsDoubleEquil(double(pOutputData[2]), m_staBandStats[0].dfNoData) || IsDoubleEquil(double(pOutputData[1]), m_staBandStats[1].dfNoData) || IsDoubleEquil(double(pOutputData[0]), m_staBandStats[2].dfNoData))
		{
			bIsChanged = true;
			pOutputData[3] = m_oNoDataColor.Alpha();
			pOutputData[2] = m_oNoDataColor.Red();
			pOutputData[1] = m_oNoDataColor.Green();
			pOutputData[0] = m_oNoDataColor.Blue();
		}
	}

	if(bIsChanged)
		return;

	//check for background data

	//if(RPixVal > 128)
	//{
	//	pOutputData[0] = 0;
	//	pOutputData[1] = 0;
	//	pOutputData[2] = 0;
	//	pOutputData[3] = 0;
	//	return;
	//}
}
/*
void wxGISRasterRGBRenderer::Draw(wxGISDatasetSPtr pRasterDataset, wxGISEnumDrawPhase DrawPhase, IDisplay* pDisplay, ITrackCancel* pTrackCancel)
{
    wxGISRasterDatasetSPtr pRaster = boost::dynamic_pointer_cast<wxGISRasterDataset>(pRasterDataset);
	if(!pRaster)
		return;
    pRaster->Open(true);
	IDisplayTransformation* pDisplayTransformation = pDisplay->GetDisplayTransformation();
    if(!pDisplayTransformation)
        return;

    OGRSpatialReference* pDisplaySpatialReference = pDisplayTransformation->GetSpatialReference();
	const OGRSpatialReferenceSPtr pRasterSpatialReference = pRaster->GetSpatialReference();
	bool IsSpaRefSame(true);
	if(pDisplaySpatialReference && pRasterSpatialReference)
		IsSpaRefSame = pDisplaySpatialReference->IsSame(pRasterSpatialReference.get());
	
    //OGREnvelope VisibleBounds = pDisplayTransformation->GetVisibleBounds();
    RECTARARRAY* pInvalidrectArray = pDisplay->GetInvalidRect();
    OGREnvelope Envs[10];
    size_t EnvCount = pInvalidrectArray->size();
    if(EnvCount > 10)
        EnvCount = 10;

    if(EnvCount == 0)
    {
        Envs[0] = pDisplayTransformation->GetVisibleBounds();
        EnvCount = 1;
    }
    else
    {
        for(size_t i = 0; i < EnvCount; ++i)
        {
            wxRect Rect = pInvalidrectArray->operator[](i);   
            Rect.Inflate(1,1);
            Envs[i] = pDisplayTransformation->TransformRect(Rect);
        }
    }

	const OGREnvelope* pRasterExtent = pRaster->GetEnvelope();
    //calc envelopes
	OGREnvelope RasterEnvelope;
	if(!IsSpaRefSame)
	{
		RasterEnvelope = TransformEnvelope(pRasterExtent, pRasterSpatialReference.get(), pDisplaySpatialReference);
	}
	else
	{
		RasterEnvelope = *pRasterExtent;
	}

    for(size_t i = 0; i < EnvCount; ++i)
    {
	    //1. get envelope
	    OGREnvelope VisibleBounds = Envs[i];//pDisplayTransformation->GetVisibleBounds();

        if(!VisibleBounds.Intersects(RasterEnvelope))
            continue;

	    bool IsZoomIn(false);
	    //IsZoomIn = RasterEnvelope.Contains(VisibleBounds);
	    IsZoomIn = RasterEnvelope.MaxX > VisibleBounds.MaxX || RasterEnvelope.MaxY > VisibleBounds.MaxY || RasterEnvelope.MinX < VisibleBounds.MinX || RasterEnvelope.MinY < VisibleBounds.MinY;
	    if(IsZoomIn)
	    {
		    //intersect bounds
		    OGREnvelope DrawBounds = RasterEnvelope;
//            DrawBounds.Intersect(VisibleBounds);
		    DrawBounds.MinX = MAX(RasterEnvelope.MinX, VisibleBounds.MinX);
		    DrawBounds.MinY = MAX(RasterEnvelope.MinY, VisibleBounds.MinY);
		    DrawBounds.MaxX = MIN(RasterEnvelope.MaxX, VisibleBounds.MaxX);
		    DrawBounds.MaxY = MIN(RasterEnvelope.MaxY, VisibleBounds.MaxY);
            //convert draw bounds to DC coords
		    OGRRawPoint OGRRawPoints[2];
		    OGRRawPoints[0].x = DrawBounds.MinX;
		    OGRRawPoints[0].y = DrawBounds.MinY;
		    OGRRawPoints[1].x = DrawBounds.MaxX;
		    OGRRawPoints[1].y = DrawBounds.MaxY;
		    wxPoint* pDCPoints = pDisplayTransformation->TransformCoordWorld2DC(OGRRawPoints, 2);	

		    if(!pDCPoints)
		    {
			    wxDELETEA(pDCPoints);
			    return;
		    }

		    int nDCXOrig = pDCPoints[0].x;
		    int nDCYOrig = pDCPoints[1].y;
		    int nWidth = pDCPoints[1].x - pDCPoints[0].x;
		    int nHeight = pDCPoints[0].y - pDCPoints[1].y;
		    wxDELETEA(pDCPoints);

       //     if(nWidth <= 20 || nHeight <= 20)
			    //return;

		    GDALDataset* pGDALDataset = pRaster->GetRaster();

		    int nBandCount = pGDALDataset->GetRasterCount();
		    //hack! should check user configuration
		    int bands[3];
		    if(nBandCount < 3)
		    {
			    bands[0] = 1;
			    bands[1] = 1;
			    bands[2] = 1;	
		    }
		    else
		    {
			    bands[0] = 1;
			    bands[1] = 2;
			    bands[2] = 3;		
		    }

		    double adfGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
            double adfReverseGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
		    CPLErr err = pGDALDataset->GetGeoTransform(adfGeoTransform);
            bool bNoTransform(false);
            if(err != CE_None)
            {
                bNoTransform = true;
            }
            else
            {
                int nRes = GDALInvGeoTransform( adfGeoTransform, adfReverseGeoTransform );
            }

		    //2. get image data from raster - draw part of the raster
		    if(IsSpaRefSame)
		    {
			    double rMinX, rMinY, rMaxX, rMaxY;

                int nXSize = pGDALDataset->GetRasterXSize();
                int nYSize = pGDALDataset->GetRasterYSize();

                if(bNoTransform)
                {
                    rMinX = DrawBounds.MinX;
                    rMaxX = DrawBounds.MaxX;
                    rMaxY = nYSize - DrawBounds.MinY;
                    rMinY = nYSize - DrawBounds.MaxY;
                }
                else
                {
    			    GDALApplyGeoTransform( adfReverseGeoTransform, DrawBounds.MinX, DrawBounds.MinY, &rMinX, &rMaxY );
	    		    GDALApplyGeoTransform( adfReverseGeoTransform, DrawBounds.MaxX, DrawBounds.MaxY, &rMaxX, &rMinY );
                }

                double rImgWidth = rMaxX - rMinX;
                double rImgHeight = rMaxY - rMinY;
			    int nImgWidth = ceil(rImgWidth) + 1;
			    int nImgHeight = ceil(rImgHeight) + 1;
                
			    //read in buffer
                int nMinX = int(rMinX);//floor
                int nMinY = int(rMinY);//floor
			    if(nMinX < 0) nMinX = 0;
			    if(nMinY < 0) nMinY = 0;

                if(nImgWidth > nXSize - nMinX) nImgWidth = nXSize - nMinX;
                if(nImgHeight > nYSize - nMinY) nImgHeight = nYSize - nMinY;

		        //create buffer
                int nWidthOut, nHeightOut;
                double rImgWidthOut, rImgHeightOut;
                if(nWidth > nImgWidth)
                {
			        nWidthOut = nImgWidth;
			        rImgWidthOut = rImgWidth;
                }
                else
                {
			        nWidthOut = nWidth;
			        rImgWidthOut = (double)nWidthOut * rImgWidth / nImgWidth;
			        //nWidthOut++;
                }
                
                if(nHeight > nImgHeight)
                {
			        nHeightOut = nImgHeight;
			        rImgHeightOut = rImgHeight;
                }
                else
                {
			        nHeightOut = nHeight;
			        rImgHeightOut = (double)nHeightOut * rImgHeight / nImgHeight;
                    //nHeightOut++;
                }


		        unsigned char* data = new unsigned char[nWidthOut * nHeightOut * 3];

		        err = pGDALDataset->AdviseRead(nMinX, nMinY, nImgWidth, nImgHeight, nWidthOut, nHeightOut, GDT_Byte, 3, bands, NULL);
		        if(err != CE_None)
		        {
                    const char* pszerr = CPLGetLastErrorMsg();
                    wxLogError(_("AdviseRead failed! GDAL error: %s"), wgMB2WX(pszerr));

			        wxDELETEA(data);
			        return;
		        }

		        err = pGDALDataset->RasterIO(GF_Read, nMinX, nMinY, nImgWidth, nImgHeight, data, nWidthOut, nHeightOut, GDT_Byte, 3, bands, sizeof(unsigned char) * 3, 0, sizeof(unsigned char));
		        if(err != CE_None)
		        {
                    const char* pszerr = CPLGetLastErrorMsg();
                    wxLogError(_("RasterIO failed! GDAL error: %s"), wgMB2WX(pszerr));

			        wxDELETEA(data);
			        return;
		        }
		        //scale pTempData to data using interpolation methods
		        pDisplay->DrawBitmap(Scale(data, nWidthOut, nHeightOut, rImgWidthOut, rImgHeightOut, nWidth, nHeight, rMinX - nMinX, rMinY - nMinY, enumGISQualityBilinear, pTrackCancel), nDCXOrig, nDCYOrig); //enumGISQualityNearest

                wxDELETEA(data);
		    }
		    else
		    {
		    //void *hTransformArg = GDALCreateGenImgProjTransformer( hSrcDS, pszSrcWKT, NULL, pszDstWKT, FALSE, 0, 1 );
		    //GDALDestroyGenImgProjTransformer( hTransformArg );
	    ////		//get new envelope - it may rotate
	    ////		OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( pDisplaySpatialReference, pRasterSpatialReference);
	    //////		get real envelope
	    //////		poCT->Transform(1, &pRasterExtent->MaxX, &pRasterExtent->MaxY);
	    //////		poCT->Transform(1, &pRasterExtent->MinX, &pRasterExtent->MinY);
	    //////		poCT->Transform(1, &pRasterExtent->MaxX, &pRasterExtent->MinY);
	    //////		poCT->Transform(1, &pRasterExtent->MinX, &pRasterExtent->MaxY);
	    ////		OCTDestroyCoordinateTransformation(poCT);
		    }
	    }
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

		    int nBandCount = pGDALDataset->GetRasterCount();
		    //hack!
		    int bands[3];
		    if(nBandCount < 3)
		    {
			    bands[0] = 1;
			    bands[1] = 1;
			    bands[2] = 1;	
		    }
		    else
		    {
			    bands[0] = 1;
			    bands[1] = 2;
			    bands[2] = 3;		
		    }

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
		    //3. draw //think about transparancy!
		    wxImage ResultImage(nWidth, nHeight, data);
		    pDisplay->DrawBitmap(ResultImage, nDCXOrig, nDCYOrig);

		    //delete[](data);
	    }
    }
}

//wxImage wxGISRasterDataset::GetSubimage(IDisplayTransformation* pDisplayTransformation, IQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
//{
//	if(!m_bIsOpened)
//		if(!Open())
//			return wxImage();
//	//pyramids!!! in scale and descale	
//	wxGISSpatialFilter* pSpaFil = dynamic_cast<wxGISSpatialFilter*>(pQFilter);
//	if(!pSpaFil)
//	{
//		wxLogError(_("The spatial filter is null!"));
//		return wxImage();
//	}
//	OGREnvelope Env = pSpaFil->GetEnvelope();
//	bool bDownsample = m_psExtent->Contains(Env);
//	//if(!bDownsample)
//	//	bDownsample = m_psExtent->MaxX == Env.MaxX && m_psExtent->MaxY == Env.MaxY && m_psExtent->MinX == Env.MinX && m_psExtent->MinY == Env.MinY;
//
//	if(!bDownsample)
//	{
//		//input size - whole image
//		//TransformCoordWorld2DC - output size		
//		OGRRawPoint world_points[2];
//		world_points[0].x = Env.MinX;
//		world_points[0].y = Env.MinY;
//		world_points[1].x = Env.MaxX;
//		world_points[1].y = Env.MaxY;
//		wxPoint* dc_points = pDisplayTransformation->TransformCoordWorld2DC(world_points, 2);
//		if(!dc_points)
//			return wxImage();
//		int nWidth = dc_points[1].x - dc_points[0].x;
//		int nHeight = dc_points[0].y - dc_points[1].y;
//		int nImgWidth = m_poDataset->GetRasterXSize();
//		int nImgHeight = m_poDataset->GetRasterYSize();
//
//		int nTestHeight = double(nWidth) / nImgWidth * nImgHeight;
//		if(nTestHeight > nHeight)
//			nWidth = double(nHeight) / nImgHeight * nImgWidth;
//		else
//			nHeight = nTestHeight;
//
//		delete [] dc_points;
//		int nBandCount = m_poDataset->GetRasterCount();
//		//hack!
//		if(nBandCount > 3)
//			nBandCount = 3; 
//		//create buffer
//		unsigned char* data = new unsigned char[nWidth * nHeight * nBandCount];
//		//read in buffer
//		int bands[3];
//		bands[0] = 1;
//		bands[1] = 1;
//		bands[2] = 1;
//
//		CPLErr err = m_poDataset->RasterIO(GF_Read, 0, 0, nImgWidth, nImgHeight, data, nWidth, nHeight, GDT_Byte, nBandCount, NULL, sizeof(unsigned char) * nBandCount, 0, sizeof(unsigned char));//bands
//		if(err != CE_None)
//			return wxImage();
//
//		//transfer to image
//		wxImage ResultImage(nWidth, nHeight, data);
//		////display pixel size -> image size -> downsample
//		return ResultImage;
//	}
//	else
//	{
//		//convert from geo coord to pixel coord (adfGeoTransform)
//		//read pixels to image
//		//resample image to display extent
//
//	}
//
//	//m_poDataset->RasterIO(GF_Read, 
//	return wxImage();
//}

OGREnvelope wxGISRasterRGBRenderer::TransformEnvelope(const OGREnvelope* pEnvelope, OGRSpatialReference* pSrsSpatialReference, OGRSpatialReference* pDstSpatialReference)
{
	//get new envelope - it may rotate
	OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( pSrsSpatialReference, pDstSpatialReference);
	double pointsx[4];
	double pointsy[4];
	pointsx[0] = pEnvelope->MaxX;
	pointsy[0] = pEnvelope->MaxY;
	pointsx[1] = pEnvelope->MinX;
	pointsy[1] = pEnvelope->MinY;
	pointsx[2] = pEnvelope->MaxX;
	pointsy[2] = pEnvelope->MinY;
	pointsx[3] = pEnvelope->MinX;
	pointsy[3] = pEnvelope->MaxY;
	//get real envelope
	poCT->Transform(4, pointsx, pointsy);
	OCTDestroyCoordinateTransformation(poCT);
	OGREnvelope out;
	out.MinX = MIN(pointsx[0], MIN(pointsx[1], MIN(pointsx[2], pointsx[3])));
	out.MaxX = MAX(pointsx[0], MAX(pointsx[1], MAX(pointsx[2], pointsx[3])));
	out.MinY = MIN(pointsy[0], MIN(pointsy[1], MIN(pointsy[2], pointsy[3])));
	out.MaxY = MAX(pointsy[0], MAX(pointsy[1], MAX(pointsy[2], pointsy[3])));
	return out;
}

wxImage wxGISRasterRGBRenderer::Scale(unsigned char* pData, int nOrigX, int nOrigY, double rOrigX, double rOrigY, int nDestX, int nDestY, double rDeltaX, double rDeltaY, wxGISEnumDrawQuality Quality, ITrackCancel* pTrackCancel)
{
    ////simple way
    //wxImage ResultImage1(nOrigX, nOrigY, pData);
    //ResultImage1 = ResultImage1.Scale(nDestX, nDestY);
    //return ResultImage1;

    wxImage ResultImage(nDestX, nDestY, false);
    unsigned char* pDestData = ResultImage.GetData();
    //multithreaded
    int CPUCount = wxThread::GetCPUCount();
    std::vector<wxRasterDrawThread*> threadarray;
    int nPartSize = nDestY / CPUCount;
    int nBegY(0), nEndY;
    for(int i = 0; i < CPUCount; ++i)
    {        
        if(i == CPUCount - 1)
            nEndY = nDestY;
        else
            nEndY = nPartSize * (i + 1);

        unsigned char* pDestInputData = pDestData  + (nDestX * nBegY * 3);
        wxRasterDrawThread *thread = new wxRasterDrawThread(pData, pDestInputData, nOrigX, nOrigY, rOrigX, rOrigY, nDestX, nDestY, rDeltaX, rDeltaY, Quality, pTrackCancel, nBegY, nEndY);
        thread->Create();
        thread->Run();
        threadarray.push_back(thread);
        nBegY = nEndY;
    }

    for(size_t i = 0; i < threadarray.size(); ++i)
    {
        wgDELETE(threadarray[i], Wait());
    }
    return ResultImage;
}
*/
