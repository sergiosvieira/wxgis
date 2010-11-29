/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  create overviews tools.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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
#include "wxgis/geoprocessing/gpcreateovrtool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/datasource/rasterdataset.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPOrthoCorrectTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPCreateOverviewsTool, wxGISGPTool)

wxGISGPCreateOverviewsTool::wxGISGPCreateOverviewsTool(void) : wxGISGPTool()
{
}

wxGISGPCreateOverviewsTool::~wxGISGPCreateOverviewsTool(void)
{
    for(size_t i = 0; i < m_pParamArr.size(); i++)
        wxDELETE(m_pParamArr[i]);
}

wxString wxGISGPCreateOverviewsTool::GetDisplayName(void)
{
    return wxString(_("Create overviews"));
}

wxString wxGISGPCreateOverviewsTool::GetName(void)
{
    return wxString(wxT("create_ovr"));
}

wxString wxGISGPCreateOverviewsTool::GetCategory(void)
{
    return wxString(_("Data Management Tools/Raster"));
}

GPParameters* wxGISGPCreateOverviewsTool::GetParameterInfo(void)
{
    if(m_pParamArr.empty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source raster"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISRasterDataset));
        pParam1->SetDomain(pDomain1);

        m_pParamArr.push_back(pParam1);

        //elevation interpolation type
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("interpol_type"));
        pParam2->SetDisplayName(_("Pixel interpolation"));
        pParam2->SetParameterType(enumGISGPParameterTypeOptional);
        pParam2->SetDataType(enumGISGPParamDTStringList);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPStringDomain* pDomain2 = new wxGISGPStringDomain();
        //"NEAREST", "GAUSS", "CUBIC", "AVERAGE", "MODE", "AVERAGE_MAGPHASE" or "NONE"
        pDomain2->AddString(_("Nearest"));
        pDomain2->AddString(_("Gauss"));
        pDomain2->AddString(_("Cubic"));
        pDomain2->AddString(_("Average"));
        pDomain2->AddString(_("Mode"));
        pDomain2->AddString(_("Average magphase"));
        pDomain2->AddString(_("None"));
        pParam2->SetDomain(pDomain2);

        pParam2->SetValue(_("Nearest"));

        m_pParamArr.push_back(pParam2);

        //levels
    }
    return &m_pParamArr;
}

bool wxGISGPCreateOverviewsTool::Validate(void)
{

    //autogen levels
         //       int nSize = MIN(pwxGISRasterDataset->GetHeight(), pwxGISRasterDataset->GetWidth());
         //   int anOverviewList[25] = {0};
         //   int nLevel(1);
         //   int nLevelCount(0);
         //   while(1)
         //   {
         //       nSize /= 2;
         //       if(nSize < 20)
         //           break;
         //       nLevel *= 2;
         //       if(nLevel != 2)
         //       {
         //           anOverviewList[nLevelCount] = nLevel;
         //           nLevelCount++;
         //       }
         //   }
	        ////int anOverviewList[8] = { 4, 8, 16, 32, 64, 128, 256, 512 };

    //return m_pParamArr[0]->GetIsValid();
    return true;
}

bool wxGISGPCreateOverviewsTool::Execute(ITrackCancel* pTrackCancel)
{
    if(!Validate())
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Unexpected error occurred"), -1, enumGISMessageErr);
        return false;
    }

    IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(!pGxObjectContainer)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting catalog object"), -1, enumGISMessageErr);
        return false;
    }

    wxString sSrcPath = m_pParamArr[0]->GetValue();
    IGxObject* pGxObject = pGxObjectContainer->SearchChild(sSrcPath);
    if(!pGxObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting source object"), -1, enumGISMessageErr);
        return false;
    }
    IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
    if(!pGxDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source object is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    wxGISRasterDataset* pSrcDataSet = dynamic_cast<wxGISRasterDataset*>(pGxDataset->GetDataset());
    if(!pSrcDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }

    GDALDataset* poGDALDataset = pSrcDataSet->GetRaster();
    if(!poGDALDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting raster"), -1, enumGISMessageErr);
        wsDELETE(pSrcDataSet);
        return false;
    }

    //GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName( wgWX2MB(sDriver) );
    //GDALRasterBand * poGDALRasterBand = poGDALDataset->GetRasterBand(1);

    //if(!poGDALRasterBand)
    //{
    //    //add messages to pTrackCancel
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("The raster has no bands"), -1, enumGISMessageErr);
    //    wsDELETE(pSrcDataSet);
    //    return false;
    //}
    //GDALDataType eDT = poGDALRasterBand->GetRasterDataType();

    //wxString soChoice = m_pParamArr[5]->GetValue();
    //if(soChoice == wxString(_("Cubic")))
    //    CPLSetConfigOption( "GDAL_RPCDEMINTERPOLATION", "CUBIC" ); //BILINEAR


    //CPLString osDSTSRSOpt = "DST_SRS=";
    //osDSTSRSOpt += poGDALDataset->GetProjectionRef();

    //const char *apszOptions[6] = { osDSTSRSOpt.c_str(), "METHOD=RPC", NULL, NULL, NULL, NULL};//, NULL  osSRCSRSOpt.c_str(),
    //wxString soDEMPath = m_pParamArr[2]->GetValue();

    //CPLString soCPLDemPath;
    //if(pGxObjectContainer)
    //{
    //    IGxDataset* pGxDemObj = dynamic_cast<IGxDataset*>(pGxObjectContainer->SearchChild(soDEMPath));
    //    if(pGxDemObj)
    //    {
    //        soCPLDemPath = CPLString((const char *)wgWX2MB(pGxDemObj->GetPath()));
    //    }
    //}

    //CPLString osDEMFileOpt = "RPC_DEM=";
    //osDEMFileOpt += soCPLDemPath;
    //apszOptions[2] = osDEMFileOpt.c_str();

    //wxString soHeight = m_pParamArr[3]->GetValue();
    //CPLString osHeightOpt = "RPC_HEIGHT=";
    //osHeightOpt += wgWX2MB(soHeight);
    //apszOptions[3] = osHeightOpt.c_str();

    //wxString soHeightScale = m_pParamArr[4]->GetValue();
    //CPLString osHeightScaleOpt = "RPC_HEIGHT_SCALE=";
    //osHeightScaleOpt += wgWX2MB(soHeightScale);
    //apszOptions[4] = osHeightScaleOpt.c_str();

    //void *hTransformArg = GDALCreateGenImgProjTransformer2( poGDALDataset, NULL, (char **)apszOptions );
    //if(!hTransformArg)
    //{
    //    const char* pszErr = CPLGetLastErrorMsg();
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(wxString::Format(_("Error CreateGenImgProjTransformer. GDAL Error: %s"), wgMB2WX(pszErr)), -1, enumGISMessageErr);
    //    wsDELETE(pSrcDataSet);
    //    return false;
    //}

    //double adfDstGeoTransform[6] = {0,0,0,0,0,0};
    //int nPixels=0, nLines=0;

    //CPLErr eErr = GDALSuggestedWarpOutput( poGDALDataset, GDALGenImgProjTransform, hTransformArg, adfDstGeoTransform, &nPixels, &nLines );
    //if(eErr != CE_None)
    //{
    //    const char* pszErr = CPLGetLastErrorMsg();
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(wxString::Format(_("Error determining output raster size. GDAL Error: %s"), wgMB2WX(pszErr)), -1, enumGISMessageErr);
    //    wsDELETE(pSrcDataSet);
    //    return false;
    //}


    //GDALDestroyGenImgProjTransformer( hTransformArg );

    //// Create the output file.
    //GDALDataset * poOutputGDALDataset = poDriver->Create( wgWX2MB(sDstPath), nPixels, nLines, poGDALDataset->GetRasterCount(), eDT, NULL );
    //if(poOutputGDALDataset == NULL)
    //{
    //    const char* pszErr = CPLGetLastErrorMsg();
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(wxString::Format(_("Error creating output raster. GDAL Error: %s"), wgMB2WX(pszErr)), -1, enumGISMessageErr);
    //    wsDELETE(pSrcDataSet);
    //    return false;
    //}

    //poOutputGDALDataset->SetProjection(poGDALDataset->GetProjectionRef());
    //poOutputGDALDataset->SetGeoTransform( adfDstGeoTransform );

    //// Copy the color table, if required.
    //GDALColorTableH hCT;

    //hCT = GDALGetRasterColorTable( GDALGetRasterBand(poGDALDataset,1) );
    //if( hCT != NULL )
    //    GDALSetRasterColorTable( GDALGetRasterBand(poOutputGDALDataset,1), hCT );


    //// Setup warp options.

    //GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();

    //psWarpOptions->hSrcDS = poGDALDataset;
    //psWarpOptions->hDstDS = poOutputGDALDataset;

    //psWarpOptions->pfnProgress = OvrProgress;
    //psWarpOptions->pProgressArg = (void*)pTrackCancel;

    //// Establish reprojection transformer.

    //psWarpOptions->pTransformerArg = GDALCreateGenImgProjTransformer2( poGDALDataset, poOutputGDALDataset, (char **)apszOptions );
    //psWarpOptions->pfnTransformer = GDALGenImgProjTransform;
    //
    ////TODO: Add to config memory limit in % of free memory
    //double dfMemLim = wxGetFreeMemory().ToDouble() / wxThread::GetCPUCount();
    //if(dfMemLim > 135000000) //128Mb in bytes
    //{
    //    psWarpOptions->dfWarpMemoryLimit = dfMemLim;
    //    wxLogDebug(wxT("wxGISGPOrthoCorrectTool: The dfWarpMemoryLimit set to %f Mb"), dfMemLim / 1048576);
    //}

    //psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SOURCE_EXTRA", "5" );
    //psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SAMPLE_STEPS", "101" );
    //psWarpOptions->eResampleAlg = GRA_Bilinear;

    //// Initialize and execute the warp operation.

    //GDALWarpOperation oOperation;

    //oOperation.Initialize( psWarpOptions );
    //eErr = oOperation.ChunkAndWarpImage( 0, 0, nPixels, nLines );//ChunkAndWarpMulti( 0, 0, nPixels, nLines );//
    //if(eErr != CE_None)
    //{
    //    const char* pszErr = CPLGetLastErrorMsg();
    //    if(pTrackCancel)
    //    {
    //        wxString sErr = wgMB2WX(pszErr);
    //        pTrackCancel->PutMessage(wxString::Format(_("OrthoCorrect failed! GDAL error: %s"), sErr.c_str()), -1, enumGISMessageErr);
    //    }
    //    GDALClose(poOutputGDALDataset);
    //    wsDELETE(pSrcDataSet);
    //    return false;
    //}

    //GDALDestroyGenImgProjTransformer( psWarpOptions->pTransformerArg );
    //GDALDestroyWarpOptions( psWarpOptions );

    //GDALClose(poOutputGDALDataset);
    //wsDELETE(pSrcDataSet);

    //if(pGxObjectContainer)
    //{
    //    IGxObject* pParentLoc = pGxObjectContainer->SearchChild(sPath);
    //    if(pParentLoc)
    //        pParentLoc->Refresh();
    //}

    return true;
}

