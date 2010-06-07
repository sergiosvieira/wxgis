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
        pParam2->SetDisplayName(_("Destination feature class"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTPath);
        pParam2->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
        pDomain2->AddFilter(new wxGxTiffFilter());
        pParam2->SetDomain(pDomain2);

        //pParam2->AddParameterDependency(wxT("src_path"));

        m_pParamArr.push_back(pParam2);

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
    //GDALCreateRPCTransformer apszOptions 	Other transformer options (ie. RPC_HEIGHT=<z>).
    //GDALCreateGenImgProjTransformer2
    //RPC_HEIGHT: A fixed height to be used with RPC calculations.
    //RPC_DEM: The name of a DEM file to be used with RPC calculations.

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

    //OGRSpatialReference* pSrcSpaRef = pSrcDataSet->GetSpatialReference();
    //OGRSpatialReference* pNewSpaRef(NULL);

    //if(nNewSubType == enumVecKML)
    //    pNewSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);
    //else
    //    if(pSrcSpaRef)
    //        pNewSpaRef = pSrcSpaRef->Clone();

    //if(!pSrcSpaRef && pNewSpaRef)
    //{
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("The input spatial reference is not defined!"), -1, enumGISMessageErr);

    //    wsDELETE(pSrcDataSet);
    //    wxDELETE(pNewSpaRef);
    //}

    ////set filter
    //wxGISQueryFilter* pQFilter(NULL);

    //bool bHasErrors(false);
    ////check multi geometry
    //OGRwkbGeometryType nGeomType = pSrcDataSet->GetGeometryType();
    //bool bIsMultigeom = nNewSubType == enumVecESRIShapefile && (wkbFlatten(nGeomType) == wkbUnknown || wkbFlatten(nGeomType) == wkbGeometryCollection);
    //if(bIsMultigeom)
    //{
    //    //TODO: combain filters
    //    wxGISQueryFilter Filter(wxString(wxT("OGR_GEOMETRY='POINT'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_point"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbPoint );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='POLYGON'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_polygon"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbPolygon );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='LINESTRING'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_line"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbLineString );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOINT'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_mpoint"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbMultiPoint );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTILINESTRING'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_mline"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbMultiLineString );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOLYGON'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_mpolygon"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbMultiPolygon );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //}
    //else
    //{
    //    if(!OnExport(pSrcDataSet, sPath, sName, sExt, sDriver, pDef->Clone(), pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //        bHasErrors = true;
    //}

    //wsDELETE(pSrcDataSet);
    //wxDELETE(pNewSpaRef);

    //IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    //if(pCont)
    //{
    //    IGxObject* pParentLoc = pCont->SearchChild(sPath);
    //    if(pParentLoc)
    //        pParentLoc->Refresh();
    //}

    //return !bHasErrors;
    return false;
}


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
