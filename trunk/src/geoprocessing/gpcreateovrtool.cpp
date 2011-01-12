/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  create overviews tools.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

        //overviews interpolation type
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("interpol_type"));
        pParam2->SetDisplayName(_("Pixel interpolation"));
        pParam2->SetParameterType(enumGISGPParameterTypeOptional);
        pParam2->SetDataType(enumGISGPParamDTStringList);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPStringDomain* pDomain2 = new wxGISGPStringDomain();
        //"NEAREST", "GAUSS", "CUBIC", "AVERAGE", "MODE", "AVERAGE_MAGPHASE" or "NONE"
        pDomain2->AddString(_("Nearest"), wxT("NEAREST"));
        pDomain2->AddString(_("Gauss"), wxT("GAUSS"));
        pDomain2->AddString(_("Cubic"), wxT("CUBIC"));
        pDomain2->AddString(_("Average"), wxT("AVERAGE"));
        pDomain2->AddString(_("Mode"), wxT("MODE"));
        pDomain2->AddString(_("Average magphase"), wxT("AVERAGE_MAGPHASE"));//averages complex data in mag/phase space
        pDomain2->AddString(_("None"), wxT("NONE"));
        pParam2->SetDomain(pDomain2);

        pParam2->SetValue(wxT("NEAREST"));

        m_pParamArr.push_back(pParam2);

        //compression
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("compression"));
        pParam3->SetDisplayName(_("Overview compression"));
        pParam3->SetParameterType(enumGISGPParameterTypeOptional);
        pParam3->SetDataType(enumGISGPParamDTStringList);
        pParam3->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPStringDomain* pDomain3 = new wxGISGPStringDomain();
        //"NONE", "LZW", "DEFLATE"
        pDomain3->AddString(_("ZIP"), wxT("DEFLATE"));
        pDomain3->AddString(_("JPEG"), wxT("JPEG"));
        pDomain3->AddString(_("PACKBITS"), wxT("PACKBITS"));
        pDomain3->AddString(_("LZW"), wxT("LZW"));
        pDomain3->AddString(_("None"), wxT("NONE"));
        pParam3->SetDomain(pDomain3);

        pParam3->SetValue(wxT("NONE"));

        m_pParamArr.push_back(pParam3);

        //levels

        //PHOTOMETRIC_OVERVIEW {RGB,YCBCR,MINISBLACK,MINISWHITE,CMYK,CIELAB,ICCLAB,ITULAB}
        //INTERLEAVE_OVERVIEW {PIXEL|BAND}.
        //JPEG_QUALITY_OVERVIEW
        //PREDICTOR_OVERVIEW 1|2|3 - 1.8!
        //BIGTIFF_OVERVIEW {IF_NEEDED|IF_SAFER|YES|NO}

        //USE_RRD {YES|NO}
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
   
    //PHOTOMETRIC_OVERVIEW {RGB,YCBCR,MINISBLACK,MINISWHITE,CMYK,CIELAB,ICCLAB,ITULAB}    
    //INTERLEAVE_OVERVIEW {PIXEL|BAND}
    //JPEG_QUALITY_OVERVIEW

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
    wxGISRasterDataset* pSrcDataSet = dynamic_cast<wxGISRasterDataset*>(pGxDataset->GetDataset(true));
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

    wxString sInterpolation = m_pParamArr[1]->GetValue();
    wxString sCompress = m_pParamArr[2]->GetValue();
    //CPLSetConfigOption( "COMPRESS_OVERVIEW", wgWX2MB(sCompress) );//LZW "DEFLATE" NONE

    //if(pwxGISRasterDataset->GetSubType() == enumRasterImg)
    //{
    //    CPLSetConfigOption( "USE_RRD", "YES" );
    //    CPLSetConfigOption( "HFA_USE_RRD", "YES" );
    //}
    //else
    //    CPLSetConfigOption( "USE_RRD", "NO" );

    //CPLErr eErr = pDSet->BuildOverviews( wgWX2MB(sResampleMethod), nLevelCount, anOverviewList, 0, NULL, ExecToolProgress, (void*)&Data );

    //if(eErr != CE_None)
    //{
    //    const char* pszErr = CPLGetLastErrorMsg();
    //    wxLogError(_("BuildOverviews failed! GDAL error: %s"), wgMB2WX(pszErr));
    //    wxMessageBox(_("Build Overviews failed!"), _("Error"), wxICON_ERROR | wxOK );
    //}
    //else
    //    pwxGISRasterDataset->SetHasOverviews(true);
            

    //GDALClose(poOutputGDALDataset);
    //wsDELETE(pSrcDataSet);

    //if(pGxObjectContainer)-catalog
    //{
    //    IGxObject* pParentLoc = pGxObjectContainer->SearchChild(sPath);
    //    if(pParentLoc)
    //        pParentLoc->Refresh();
    //}

    return true;
}

//TODO: PostExecute