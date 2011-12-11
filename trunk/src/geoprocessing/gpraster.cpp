/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  raster dataset functions.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/geoprocessing/gpraster.h"

GDALDataset* CreateOutputDataset()
{
	return NULL;
}

bool SubrasterByVector(wxGISFeatureDatasetSPtr pSrcFeatureDataSet, wxGISRasterDatasetSPtr pSrcRasterDataSet, CPLString &szDstFolderPath, ITrackCancel* pTrackCancel)
{
	//check if openned or/and open dataset
	if(!pSrcFeatureDataSet->IsOpened())
	{
		if(!pSrcFeatureDataSet->Open(true))
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(_("Source vector dataset open failed"), -1, enumGISMessageErr);
			return false;
		}
	}

	if(!pSrcRasterDataSet->IsOpened())
	{
		if(!pSrcRasterDataSet->Open(true))
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(_("Source raster dataset open failed"), -1, enumGISMessageErr);
			return false;
		}
	}

    const OGRSpatialReferenceSPtr pSrsSRS = pSrcFeatureDataSet->GetSpatialReference();
    const OGRSpatialReferenceSPtr pDstSRS = pSrcRasterDataSet->GetSpatialReference();
    OGRCoordinateTransformation *poCT(NULL);
    bool bSame = pSrsSRS == NULL || pDstSRS == NULL || pSrsSRS->IsSame(pDstSRS.get());
    if( !bSame )
        poCT = OGRCreateCoordinateTransformation( pSrsSRS.get(), pDstSRS.get() );

    IProgressor* pProgressor(NULL);
    if(pTrackCancel)
    {
       pProgressor = pTrackCancel->GetProgressor();
       pTrackCancel->PutMessage(wxString::Format(_("Start clip '%s' by geometry from '%s'"), wxString(pSrcRasterDataSet->GetPath(), wxConvUTF8).c_str(), wxString(pSrcFeatureDataSet->GetPath(), wxConvUTF8).c_str()), -1, enumGISMessageNorm);
    }
    int nCounter(0);
    if(pProgressor)
        pProgressor->SetRange(pSrcFeatureDataSet->GetFeatureCount());

    pSrcFeatureDataSet->Reset();
    OGRFeatureSPtr pFeature;
    while((pFeature = pSrcFeatureDataSet->Next()) != NULL)
    {

        if(pTrackCancel && !pTrackCancel->Continue())
        {
            wxString sErr(_("Interrupted by user"));
            CPLString sFullErr(sErr.mb_str());
            CPLError( CE_Warning, CPLE_AppDefined, sFullErr );

            if(pTrackCancel)
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), -1, enumGISMessageErr);
            return false;
        }

        OGRGeometry *pGeom = pFeature->GetGeometryRef();
		if(wkbFlatten(pSrcFeatureDataSet->GetGeometryType()) != wkbUnknown && !pGeom)
            continue;

		OGRGeometry *pNewGeom(NULL);
        if( !bSame && poCT )
        {
            if(pGeom)
            {
				pNewGeom = pGeom->clone();
                OGRErr eErr = pNewGeom->transform(poCT);
                if(eErr != OGRERR_NONE)
	                wxDELETE(pNewGeom);
            }
        }
		else
			pNewGeom = pGeom->clone();

		//TODO: do main work

        nCounter++;
        if(pProgressor)
            pProgressor->SetValue(nCounter);
    }

    if(poCT)
        OCTDestroyCoordinateTransformation(poCT);

	return true;
}

 //   wxString sName = sDstFileName.GetName();

 //   wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_paParam[1]->GetDomain());
	//IGxObjectFilter* pFilter = pDomain->GetFilter(m_paParam[1]->GetSelDomainValue());
 //   if(!pFilter)
 //   {
 //       //add messages to pTrackCancel
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
 //       return false;
 //   }

 //   wxString sDriver = pFilter->GetDriver();
 //   wxString sExt = pFilter->GetExt();
 //   int nNewSubType = pFilter->GetSubType();

 //   GDALDataset* poGDALDataset = pSrcDataSet->GetRaster();
 //   if(!poGDALDataset)
 //   {
 //       //add messages to pTrackCancel
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(_("Error getting raster"), -1, enumGISMessageErr);
 //       return false;
 //   }

	//GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName( sDriver.mb_str() );
 //   GDALRasterBand * poGDALRasterBand = poGDALDataset->GetRasterBand(1);

 //   if(!poGDALRasterBand)
 //   {
 //       //add messages to pTrackCancel
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(_("The raster has no bands"), -1, enumGISMessageErr);
 //       return false;
 //   }
 //   GDALDataType eDT = poGDALRasterBand->GetRasterDataType();

 //   wxString soChoice = m_paParam[5]->GetValue();
 //   if(soChoice == wxString(_("Cubic")))
 //       CPLSetConfigOption( "GDAL_RPCDEMINTERPOLATION", "CUBIC" ); //BILINEAR


 //   //CPLString osSRCSRSOpt = "SRC_SRS=";
 //   //osSRCSRSOpt += poGDALDataset->GetProjectionRef();
 //   CPLString osDSTSRSOpt = "DST_SRS=";
 //   osDSTSRSOpt += poGDALDataset->GetProjectionRef();

 //   const char *apszOptions[6] = { osDSTSRSOpt.c_str(), "METHOD=RPC", NULL, NULL, NULL, NULL};//, NULL  osSRCSRSOpt.c_str(),
 //   wxString soDEMPath = m_paParam[2]->GetValue();

 //   CPLString soCPLDemPath;
 //   if(pGxObjectContainer)
 //   {
 //       IGxObject* pGxDemObj = pGxObjectContainer->SearchChild(soDEMPath);
 //       if(pGxDemObj)
 //       {
 //           soCPLDemPath = pGxDemObj->GetInternalName();
 //       }
 //   }

 //   CPLString osDEMFileOpt = "RPC_DEM=";
 //   osDEMFileOpt += soCPLDemPath;
 //   apszOptions[2] = osDEMFileOpt.c_str();

 //   wxString soHeight = m_paParam[3]->GetValue();
 //   CPLString osHeightOpt = "RPC_HEIGHT=";
 //   osHeightOpt += soHeight.mb_str();
 //   apszOptions[3] = osHeightOpt.c_str();

 //   wxString soHeightScale = m_paParam[4]->GetValue();
 //   CPLString osHeightScaleOpt = "RPC_HEIGHT_SCALE=";
	//osHeightScaleOpt += soHeightScale.mb_str();
 //   apszOptions[4] = osHeightScaleOpt.c_str();

 //   //double dfPixErrThreshold = MIN(adfDstGeoTransform[1], adfDstGeoTransform[5]);

 //   void *hTransformArg = GDALCreateGenImgProjTransformer2( poGDALDataset, NULL, (char **)apszOptions );
 //   if(!hTransformArg)
 //   {
 //       const char* pszErr = CPLGetLastErrorMsg();
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(wxString::Format(_("Error CreateGenImgProjTransformer. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
 //       return false;
 //   }

 //   double adfDstGeoTransform[6] = {0,0,0,0,0,0};
 //   int nPixels=0, nLines=0;

 //   CPLErr eErr = GDALSuggestedWarpOutput( poGDALDataset, GDALGenImgProjTransform, hTransformArg, adfDstGeoTransform, &nPixels, &nLines );
 //   if(eErr != CE_None)
 //   {
 //       const char* pszErr = CPLGetLastErrorMsg();
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(wxString::Format(_("Error determining output raster size. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
 //       return false;
 //   }


 //   GDALDestroyGenImgProjTransformer( hTransformArg );

 //   // Create the output file.
	//CPLString sFullPath = CPLFormFilename(szPath, sName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
 //   GDALDataset * poOutputGDALDataset = poDriver->Create( sFullPath, nPixels, nLines, poGDALDataset->GetRasterCount(), eDT, NULL );
 //   if(poOutputGDALDataset == NULL)
 //   {
 //       const char* pszErr = CPLGetLastErrorMsg();
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(wxString::Format(_("Error creating output raster. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
 //       return false;
 //   }

 //   poOutputGDALDataset->SetProjection(poGDALDataset->GetProjectionRef());
 //   poOutputGDALDataset->SetGeoTransform( adfDstGeoTransform );

 //   // Copy the color table, if required.
 //   GDALColorTableH hCT;

 //   hCT = GDALGetRasterColorTable( GDALGetRasterBand(poGDALDataset,1) );
 //   if( hCT != NULL )
 //       GDALSetRasterColorTable( GDALGetRasterBand(poOutputGDALDataset,1), hCT );


 //   // Setup warp options.

 //   GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();

 //   psWarpOptions->hSrcDS = poGDALDataset;
 //   psWarpOptions->hDstDS = poOutputGDALDataset;

 //   //psWarpOptions->nBandCount = 1;
 //   //psWarpOptions->panSrcBands =
 //   //    (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
 //   //psWarpOptions->panSrcBands[0] = 1;
 //   //psWarpOptions->panDstBands =
 //   //    (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
 //   //psWarpOptions->panDstBands[0] = 1;

 //   psWarpOptions->pfnProgress = ExecToolProgress;
 //   psWarpOptions->pProgressArg = (void*)pTrackCancel;

 //   // Establish reprojection transformer.

 //   psWarpOptions->pTransformerArg = GDALCreateGenImgProjTransformer2( poGDALDataset, poOutputGDALDataset, (char **)apszOptions );
 //   psWarpOptions->pfnTransformer = GDALGenImgProjTransform;
 //   
 //   //TODO: Add to config memory limit in % of free memory
 //   double dfMemLim = wxMemorySize(wxGetFreeMemory() / wxThread::GetCPUCount()).ToDouble();
 //   if(dfMemLim > 135000000) //128Mb in bytes
 //   {
 //       psWarpOptions->dfWarpMemoryLimit = dfMemLim;
 //       wxLogDebug(wxT("wxGISGPOrthoCorrectTool: The dfWarpMemoryLimit set to %f Mb"), dfMemLim / 1048576);
 //   }

 //   psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SOURCE_EXTRA", "5" );
 //   psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SAMPLE_STEPS", "101" );
 //   psWarpOptions->eResampleAlg = GRA_Bilinear;

 //   // Initialize and execute the warp operation.

 //   GDALWarpOperation oOperation;

 //   oOperation.Initialize( psWarpOptions );
 //   eErr = oOperation.ChunkAndWarpImage( 0, 0, nPixels, nLines );//ChunkAndWarpMulti( 0, 0, nPixels, nLines );//
 //   if(eErr != CE_None)
 //   {
 //       const char* pszErr = CPLGetLastErrorMsg();
 //       if(pTrackCancel)
 //       {
 //           pTrackCancel->PutMessage(wxString::Format(_("OrthoCorrect failed! GDAL error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
 //       }
 //       GDALClose(poOutputGDALDataset);
 //       //wsDELETE(pSrcDataSet);
 //       return false;
 //   }

 //   GDALDestroyGenImgProjTransformer( psWarpOptions->pTransformerArg );
 //   GDALDestroyWarpOptions( psWarpOptions );

 //   GDALClose(poOutputGDALDataset);
 //   //wsDELETE(pSrcDataSet);

