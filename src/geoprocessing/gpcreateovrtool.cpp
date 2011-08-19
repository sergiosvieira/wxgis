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
#include "wxgis/datasource/rasterop.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPOrthoCorrectTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPCreateOverviewsTool, wxGISGPTool)

wxGISGPCreateOverviewsTool::wxGISGPCreateOverviewsTool(void) : wxGISGPTool()
{
}

wxGISGPCreateOverviewsTool::~wxGISGPCreateOverviewsTool(void)
{
    for(size_t i = 0; i < m_pParamArr.size(); ++i)
        wxDELETE(m_pParamArr[i]);
}

const wxString wxGISGPCreateOverviewsTool::GetDisplayName(void)
{
    return wxString(_("Create overviews"));
}

const wxString wxGISGPCreateOverviewsTool::GetName(void)
{
    return wxString(wxT("create_ovr"));
}

const wxString wxGISGPCreateOverviewsTool::GetCategory(void)
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
        pParam2->SetDataType(enumGISGPParamDTStringChoice);
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
        pParam3->SetDataType(enumGISGPParamDTStringChoice);
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
        wxGISGPParameter* pParam4 = new wxGISGPParameter();
        pParam4->SetName(wxT("ovr_levels"));
        pParam4->SetDisplayName(_("Overview levels"));
        pParam4->SetParameterType(enumGISGPParameterTypeRequired);
        pParam4->SetDataType(enumGISGPParamDTIntegerList);
        pParam4->SetDirection(enumGISGPParameterDirectionInput);

        m_pParamArr.push_back(pParam4);

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
    wxString sPath = m_pParamArr[0]->GetValue();
	if(sPath.CmpNoCase(m_sInputPath) != 0)
    {
        if(m_pParamArr[0]->GetIsValid())
        {
			//fill fields list
			IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
			if(!pGxObjectContainer)
			{
				m_pParamArr[0]->SetIsValid(false);
				m_pParamArr[0]->SetMessage(wxGISEnumGPMessageError, _("The GxCatalog is undefined type (not inherited from IGxObjectContainer)"));
				return false;
			}
			IGxObject* pGxObject = pGxObjectContainer->SearchChild(sPath);
			if(!pGxObject)
			{
				m_pParamArr[0]->SetIsValid(false);
				m_pParamArr[0]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("There is no GxObject for path %s"), sPath.c_str()));
				return false;
			}
			IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
			if(!pGxDataset)
			{
				m_pParamArr[0]->SetIsValid(false);
				m_pParamArr[0]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("The input path is not GxDataset (Path %s)"), sPath.c_str()));
				return false;
			}
           	wxGISRasterDatasetSPtr pSrcRaster = boost::dynamic_pointer_cast<wxGISRasterDataset>(pGxDataset->GetDataset());
			if(!pSrcRaster)
			{
				m_pParamArr[0]->SetIsValid(false);
				m_pParamArr[0]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("The input path is not RasterDataset (Path %s)"), sPath.c_str()));
				return false;
			}
			if(!pSrcRaster->IsOpened())
			{
				if(!pSrcRaster->Open(true))
				{
					m_pParamArr[0]->SetIsValid(false);
					m_pParamArr[0]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("Failed open input path: %s)"), sPath.c_str()));
					return false;
				}
			}

            int anOverviewList[25] = {0};
            int nLevelCount = GetOverviewLevels(pSrcRaster, anOverviewList);
			wxArrayString saLevels;
			for(size_t i = 0; i < nLevelCount; ++i)
				saLevels.Add(wxString::Format(wxT("%d"), anOverviewList[i]));

			m_pParamArr[3]->SetValue(saLevels);
			m_pParamArr[3]->SetAltered(true);

			m_sInputPath = sPath;
		}
    }
    if(m_pParamArr[3]->GetAltered())
    {
		if(m_pParamArr[3]->GetValue().GetArrayString().GetCount() <= 0)
		{
            m_pParamArr[3]->SetIsValid(false);
			m_pParamArr[3]->SetMessage(wxGISEnumGPMessageError, wxString(_("The levels list is empty")));
            return false;
		}
		else
		{
            m_pParamArr[3]->SetIsValid(true);
			m_pParamArr[3]->SetMessage(wxGISEnumGPMessageOk);
		}
	}
    return true;
   
    //PHOTOMETRIC_OVERVIEW {RGB,YCBCR,MINISBLACK,MINISWHITE,CMYK,CIELAB,ICCLAB,ITULAB}    
    //INTERLEAVE_OVERVIEW {PIXEL|BAND}
    //JPEG_QUALITY_OVERVIEW
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
    wxGISRasterDatasetSPtr pSrcDataSet = boost::dynamic_pointer_cast<wxGISRasterDataset>(pGxDataset->GetDataset());
    if(!pSrcDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
	if(!pSrcDataSet->IsOpened())
		if(!pSrcDataSet->Open(true))
			return false;

    GDALDataset* poGDALDataset = pSrcDataSet->GetRaster();
    if(!poGDALDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting raster"), -1, enumGISMessageErr);
        return false;
    }

    wxString sResampleMethod = m_pParamArr[1]->GetValue();
    wxString sCompress = m_pParamArr[2]->GetValue();
    CPLSetConfigOption( "COMPRESS_OVERVIEW", wgWX2MB(sCompress) );

    if(pSrcDataSet->GetSubType() == enumRasterImg)
    {
        CPLSetConfigOption( "USE_RRD", "YES" );
        CPLSetConfigOption( "HFA_USE_RRD", "YES" );
    }
    else
        CPLSetConfigOption( "USE_RRD", "NO" );

	wxArrayString saLevels = m_pParamArr[3]->GetValue().GetArrayString();
    int anOverviewList[25] = {0};
    int nLevelCount = saLevels.GetCount();
	for(size_t i = 0; i < saLevels.GetCount(); ++i)
	{
		int nLevel = wxAtoi(saLevels[i]);
		anOverviewList[i] = nLevel;
	}

    CPLErr eErr = poGDALDataset->BuildOverviews( wgWX2MB(sResampleMethod), nLevelCount, anOverviewList, 0, NULL, ExecToolProgress, (void*)pTrackCancel );
    if(eErr != CE_None)
    {
        if(pTrackCancel)
        {
            const char* pszErr = CPLGetLastErrorMsg();
            pTrackCancel->PutMessage(wxString::Format(_("BuildOverviews failed! GDAL error: %s"), wgMB2WX(pszErr)), -1, enumGISMessageErr);
        }
        return false;
    }

    pSrcDataSet->SetHasOverviews(true);

    return true;
}

//TODO: PostExecute