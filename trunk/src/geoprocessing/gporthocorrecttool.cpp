/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  ortho correct geoprocessing tools.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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

#include "wxgis/geoprocessing/gporthocorrecttool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/datasource/rasterdataset.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPOrthoCorrectTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPOrthoCorrectTool, wxObject)

wxGISGPOrthoCorrectTool::wxGISGPOrthoCorrectTool(void) : m_pCatalog(NULL)
{
}

wxGISGPOrthoCorrectTool::~wxGISGPOrthoCorrectTool(void)
{
    for(size_t i = 0; i < m_pParamArr.size(); i++)
        wxDELETE(m_pParamArr[i]);
}

wxString wxGISGPOrthoCorrectTool::GetDisplayName(void)
{
    return wxString(_("Create Ortho-Corrected Raster"));
}

wxString wxGISGPOrthoCorrectTool::GetName(void)
{
    return wxString(wxT("create_ortho"));
}

wxString wxGISGPOrthoCorrectTool::GetCategory(void)
{
    return wxString(_("Data Management Tools/Raster"));
}

GPParameters* wxGISGPOrthoCorrectTool::GetParameterInfo(void)
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

        //dst path
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("dst_path"));
        pParam2->SetDisplayName(_("Destination raster"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTPath);
        pParam2->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
        pDomain2->AddFilter(new wxGxTiffFilter());
        pParam2->SetDomain(pDomain2);

        //pParam2->AddParameterDependency(wxT("src_path"));

        m_pParamArr.push_back(pParam2);

        //DEM_raster
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("dem_path"));
        pParam3->SetDisplayName(_("DEM raster"));
        pParam3->SetParameterType(enumGISGPParameterTypeOptional);
        pParam3->SetDataType(enumGISGPParamDTPath);
        pParam3->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain3 = new wxGISGPGxObjectDomain();
        pDomain3->AddFilter(new wxGxTiffFilter());
        pParam3->SetDomain(pDomain3);

        m_pParamArr.push_back(pParam3);

        ////constant_ elevation double
        //wxGISGPParameter* pParam4 = new wxGISGPParameter();
        //pParam4->SetName(wxT("cons_elev"));
        //pParam4->SetDisplayName(_("Constant elevation value"));
        //pParam4->SetParameterType(enumGISGPParameterTypeOptional);
        //pParam4->SetDataType(enumGISGPParamDTDouble);
        //pParam4->SetDirection(enumGISGPParameterDirectionInput);
        //pParam4->SetValue(0.0);

        //m_pParamArr.push_back(pParam4);
    }
    return &m_pParamArr;
}

void wxGISGPOrthoCorrectTool::SetCatalog(IGxCatalog* pCatalog)
{
    m_pCatalog = pCatalog;
}

IGxCatalog* wxGISGPOrthoCorrectTool::GetCatalog(void)
{
    return m_pCatalog;
}

bool wxGISGPOrthoCorrectTool::Validate(void)
{
    if(!m_pParamArr[1]->GetAltered())
    {
        if(m_pParamArr[0]->GetIsValid())
        {
            //generate temp name
            wxString sPath = m_pParamArr[0]->GetValue();
            wxFileName Name(sPath);
            Name.SetName(Name.GetName() + wxT("_ortho"));
            m_pParamArr[1]->SetValue(wxVariant(Name.GetFullPath(), wxT("path")));
            m_pParamArr[1]->SetAltered(true);//??
        }
    }

    ////check if input & output types is same!
    //if(m_pParamArr[0]->GetIsValid())
    //{
    //    if(!m_pParamArr[1]->GetHasBeenValidated())
    //    {
    //        //TODO: Maybe IGxDataset in future?
    //        //IGxDataset* pDset1 = m_pCatalog->SearchChild()
    //        wxFileName Name1(m_pParamArr[0]->GetValue());
    //        wxFileName Name2(m_pParamArr[1]->GetValue());
    //        if(Name1.GetExt() == Name2.GetExt())
    //        {
    //            m_pParamArr[1]->SetIsValid(false);
    //            m_pParamArr[1]->SetMessage(wxGISEnumGPMessageError, _("Cannot export to the same format"));
    //            return false;
    //        }
    //    }
    //}
    return true;
}

bool wxGISGPOrthoCorrectTool::Execute(ITrackCancel* pTrackCancel)
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
            pTrackCancel->PutMessage(_("The source object is incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    wxGISRasterDataset* pSrcDataSet = dynamic_cast<wxGISRasterDataset*>(pGxDataset->GetDataset());
    if(!pSrcDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The source dataset is incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    
    wxString sDstPath = m_pParamArr[1]->GetValue();
    wxFileName sDstFileName(sDstPath);
    wxString sPath = sDstFileName.GetPath();
    wxString sName = sDstFileName.GetName();
    
    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParamArr[1]->GetDomain());
    IGxObjectFilter* pFilter = pDomain->GetFilter(pDomain->GetSelFilter());
    if(!pFilter)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
        return false;
    }
        
    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();
    int nNewSubType = pFilter->GetSubType();

    if(!pSrcDataSet->Open())
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error opening raster"), -1, enumGISMessageErr);
        return false;
    }

    GDALDataset* poGDALDataset = pSrcDataSet->GetRaster();
    if(!poGDALDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting raster"), -1, enumGISMessageErr);
        return false;
    }

    GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName( wgWX2MB(sDriver) );
    GDALRasterBand * poGDALRasterBand = poGDALDataset->GetRasterBand(1);

    if(!poGDALRasterBand)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The raster has no bands"), -1, enumGISMessageErr);
        return false;
    }
    GDALDataType eDT = poGDALRasterBand->GetRasterDataType();
    const char *apszOptions[3] = { "METHOD=RPC", NULL, NULL };//
    wxString soDEMPath = m_pParamArr[2]->GetValue();

    if(pGxObjectContainer)
    {
        IGxObject* pGxDemObj = pGxObjectContainer->SearchChild(soDEMPath);
        if(pGxDemObj)
        {
            //pGxDemObj-> ?? get internal path
        }
    }

    //double dfDEMHeight = m_pParamArr[3]->GetValue();
    //if(soDEMPath.IsEmpty())
    //{
    //    CPLString osDEMFileOpt = "RPC_HEIGHT=";
    //    osDEMFileOpt += wgWX2MB(wxString::Format(wxT("%f"), dfDEMHeight));
    //    apszOptions[1] = osDEMFileOpt.c_str();
    //}
    //else
    //{
    //    CPLString osDEMFileOpt = "RPC_DEM=";
    //    osDEMFileOpt += wgWX2MB(soDEMPath);
    //    apszOptions[1] = osDEMFileOpt.c_str();
    //}
    CPLString osDEMFileOpt = "RPC_DEM=0";
    apszOptions[1] = osDEMFileOpt.c_str();

    void *hTransformArg = GDALCreateGenImgProjTransformer2( poGDALDataset, NULL, (char **)apszOptions );

    double adfDstGeoTransform[6];
    int nPixels=0, nLines=0;

    CPLErr eErr = GDALSuggestedWarpOutput( poGDALDataset, GDALGenImgProjTransform, hTransformArg, adfDstGeoTransform, &nPixels, &nLines );
    if(eErr != CE_None)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error determine output raster size"), -1, enumGISMessageErr);
        return false;
    }


    GDALDestroyGenImgProjTransformer( hTransformArg );

    // Create the output file.  
    GDALDataset * poOutputGDALDataset = poDriver->Create( wgWX2MB(sDstPath), nPixels, nLines, poGDALDataset->GetRasterCount(), eDT, NULL );
    if(poOutputGDALDataset == NULL)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error creating output raster"), -1, enumGISMessageErr);
        return false;
    }

    poOutputGDALDataset->SetProjection(poGDALDataset->GetProjectionRef());
    poOutputGDALDataset->SetGeoTransform( adfDstGeoTransform );

    if(poOutputGDALDataset)
        GDALClose(poOutputGDALDataset);


    //hDstDS = GDALCreate( hDriver, "out.tif", nPixels, nLines, 
    //                     GDALGetRasterCount(hSrcDS), eDT, NULL );
    //
    //CPLAssert( hDstDS != NULL );

    //// Write out the projection definition. 

    //GDALSetProjection( hDstDS, pszDstWKT );
    //GDALSetGeoTransform( hDstDS, adfDstGeoTransform );

    //// Copy the color table, if required.

    //GDALColorTableH hCT;

    //hCT = GDALGetRasterColorTable( GDALGetRasterBand(hSrcDS,1) );
    //if( hCT != NULL )
    //    GDALSetRasterColorTable( GDALGetRasterBand(hDstDS,1), hCT );

    //wsDELETE(pSrcDataSet);
    //wxDELETE(pNewSpaRef);

    //IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(pGxObjectContainer)
    {
        IGxObject* pParentLoc = pGxObjectContainer->SearchChild(sPath);
        if(pParentLoc)
            pParentLoc->Refresh();
    }

    //return !bHasErrors;
    return false;
}

    //GDALCreateRPCTransformer apszOptions 	Other transformer options (ie. RPC_HEIGHT=<z>).
    //GDALCreateGenImgProjTransformer2
    //RPC_HEIGHT: A fixed height to be used with RPC calculations.
    //RPC_DEM: The name of a DEM file to be used with RPC calculations.


//    GDALDriverH hDriver;
//    GDALDataType eDT;
//    GDALDatasetH hDstDS;
//    GDALDatasetH hSrcDS;
//
//    // Open the source file. 
//
//    hSrcDS = GDALOpen( "in.tif", GA_ReadOnly );
//    CPLAssert( hSrcDS != NULL );
//    
//    // Create output with same datatype as first input band. 
//
//    eDT = GDALGetRasterDataType(GDALGetRasterBand(hSrcDS,1));
//
//    // Get output driver (GeoTIFF format)
//
//    hDriver = GDALGetDriverByName( "GTiff" );
//    CPLAssert( hDriver != NULL );
//
//    // Get Source coordinate system. 
//
//    const char *pszSrcWKT, *pszDstWKT = NULL;
//
//    pszSrcWKT = GDALGetProjectionRef( hSrcDS );
//    CPLAssert( pszSrcWKT != NULL && strlen(pszSrcWKT) > 0 );
//
//    // Setup output coordinate system that is UTM 11 WGS84. 
//
//    OGRSpatialReference oSRS;
//
//    oSRS.SetUTM( 11, TRUE );
//    oSRS.SetWellKnownGeogCS( "WGS84" );
//
//    oSRS.exportToWkt( &pszDstWKT );
//
//    // Create a transformer that maps from source pixel/line coordinates
//    // to destination georeferenced coordinates (not destination 
//    // pixel line).  We do that by omitting the destination dataset
//    // handle (setting it to NULL). 
//
//    void *hTransformArg;
//
//    hTransformArg = 
//        GDALCreateGenImgProjTransformer( hSrcDS, pszSrcWKT, NULL, pszDstWKT, 
//                                         FALSE, 0, 1 );
//    CPLAssert( hTransformArg != NULL );
//
//    // Get approximate output georeferenced bounds and resolution for file. 
//
//    double adfDstGeoTransform[6];
//    int nPixels=0, nLines=0;
//    CPLErr eErr;
//
//    eErr = GDALSuggestedWarpOutput( hSrcDS, 
//                                    GDALGenImgProjTransform, hTransformArg, 
//                                    adfDstGeoTransform, &nPixels, &nLines );
//    CPLAssert( eErr == CE_None );
//
//    GDALDestroyGenImgProjTransformer( hTransformArg );
//
//    // Create the output file.  
//
//    hDstDS = GDALCreate( hDriver, "out.tif", nPixels, nLines, 
//                         GDALGetRasterCount(hSrcDS), eDT, NULL );
//    
//    CPLAssert( hDstDS != NULL );
//
//    // Write out the projection definition. 
//
//    GDALSetProjection( hDstDS, pszDstWKT );
//    GDALSetGeoTransform( hDstDS, adfDstGeoTransform );
//
//    // Copy the color table, if required.
//
//    GDALColorTableH hCT;
//
//    hCT = GDALGetRasterColorTable( GDALGetRasterBand(hSrcDS,1) );
//    if( hCT != NULL )
//        GDALSetRasterColorTable( GDALGetRasterBand(hDstDS,1), hCT );
//
//    ... proceed with warp as before ...
//
//int main()
//{
//    GDALDatasetH  hSrcDS, hDstDS;
//
//    // Open input and output files. 
//
//    GDALAllRegister();
//
//    hSrcDS = GDALOpen( "in.tif", GA_ReadOnly );
//    hDstDS = GDALOpen( "out.tif", GA_Update );
//
//    // Setup warp options. 
//    
//    GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();
//
//    psWarpOptions->hSrcDS = hSrcDS;
//    psWarpOptions->hDstDS = hDstDS;
//
//    psWarpOptions->nBandCount = 1;
//    psWarpOptions->panSrcBands = 
//        (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
//    psWarpOptions->panSrcBands[0] = 1;
//    psWarpOptions->panDstBands = 
//        (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
//    psWarpOptions->panDstBands[0] = 1;
//
//    psWarpOptions->pfnProgress = GDALTermProgress;   
//
//    // Establish reprojection transformer. 
//
//    psWarpOptions->pTransformerArg = 
//        GDALCreateGenImgProjTransformer( hSrcDS, 
//                                         GDALGetProjectionRef(hSrcDS), 
//                                         hDstDS,
//                                         GDALGetProjectionRef(hDstDS), 
//                                         FALSE, 0.0, 1 );
//    psWarpOptions->pfnTransformer = GDALGenImgProjTransform;
//
//    // Initialize and execute the warp operation. 
//
//    GDALWarpOperation oOperation;
//
//    oOperation.Initialize( psWarpOptions );
//    oOperation.ChunkAndWarpImage( 0, 0, 
//                                  GDALGetRasterXSize( hDstDS ), 
//                                  GDALGetRasterYSize( hDstDS ) );
//
//    GDALDestroyGenImgProjTransformer( psWarpOptions->pTransformerArg );
//    GDALDestroyWarpOptions( psWarpOptions );
//
//    GDALClose( hDstDS );
//    GDALClose( hSrcDS );
//
//    return 0;
//}
