/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  export geoprocessing tools.
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

#include "wxgis/geoprocessing/gpprojecttool.h"
//#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/catalog/gxfilters.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPExportTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPProjectVectorTool, wxGISGPTool)

wxGISGPProjectVectorTool::wxGISGPProjectVectorTool(void) : wxGISGPTool()
{
}

wxGISGPProjectVectorTool::~wxGISGPProjectVectorTool(void)
{
    for(size_t i = 0; i < m_pParamArr.size(); i++)
        wxDELETE(m_pParamArr[i]);
}

wxString wxGISGPProjectVectorTool::GetDisplayName(void)
{
    return wxString(_("Project vector file (single)"));
}

wxString wxGISGPProjectVectorTool::GetName(void)
{
    return wxString(wxT("vproj_single"));
}

wxString wxGISGPProjectVectorTool::GetCategory(void)
{
    return wxString(_("Projections and Transformations/Vector"));
}

GPParameters* wxGISGPProjectVectorTool::GetParameterInfo(void)
{
    if(m_pParamArr.empty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source feature class"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISFeatureDataset));
        pParam1->SetDomain(pDomain1);

        m_pParamArr.push_back(pParam1);

        //proj
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("proj"));
        pParam2->SetDisplayName(_("Spatial reference"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTSpatRef);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);

        m_pParamArr.push_back(pParam2);

        //dst path
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("dst_path"));
        pParam3->SetDisplayName(_("Destination feature class"));
        pParam3->SetParameterType(enumGISGPParameterTypeRequired);
        pParam3->SetDataType(enumGISGPParamDTPath);
        pParam3->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain3 = new wxGISGPGxObjectDomain();
        pDomain3->AddFilter(new wxGxDatasetFilter(enumGISFeatureDataset));
        pParam3->SetDomain(pDomain3);

        m_pParamArr.push_back(pParam3);
    }
    return &m_pParamArr;
}

bool wxGISGPProjectVectorTool::Validate(void)
{
    //if(!m_pParamArr[1]->GetAltered())
    //{
    //    if(m_pParamArr[0]->GetIsValid())
    //    {
    //        //generate temp name
    //        wxString sPath = m_pParamArr[0]->GetValue();
    //        m_pParamArr[1]->SetValue(wxVariant(sPath, wxT("path")));
    //        m_pParamArr[1]->SetAltered(true);//??
    //    }
    //}

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

bool wxGISGPProjectVectorTool::Execute(ITrackCancel* pTrackCancel)
{
    //if(!Validate())
    //{
    //    //add messages to pTrackCancel
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("Unexpected error occurred"), -1, enumGISMessageErr);
    //    return false;
    //}

    //IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    //if(!pGxObjectContainer)
    //{
    //    //add messages to pTrackCancel
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("Error getting catalog object"), -1, enumGISMessageErr);
    //    return false;
    //}

    //wxString sSrcPath = m_pParamArr[0]->GetValue();
    //IGxObject* pGxObject = pGxObjectContainer->SearchChild(sSrcPath);
    //if(!pGxObject)
    //{
    //    //add messages to pTrackCancel
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("Error get source object"), -1, enumGISMessageErr);
    //    return false;
    //}
    //IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
    //if(!pGxDataset)
    //{
    //    //add messages to pTrackCancel
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("The source object is of incompatible type"), -1, enumGISMessageErr);
    //    return false;
    //}
    //wxGISFeatureDatasetSPtr pSrcDataSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(pGxDataset->GetDataset(true));
    //if(!pSrcDataSet)
    //{
    //    //add messages to pTrackCancel
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("The source dataset is of incompatible type"), -1, enumGISMessageErr);
    //    return false;
    //}
    //
    //OGRFeatureDefn *pDef = pSrcDataSet->GetDefiniton();
    //if(!pDef)
    //{
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("Error reading dataset definition"), -1, enumGISMessageErr);
    //    //wsDELETE(pSrcDataSet);
    //    return false;
    //}
    //
    //wxString sDstPath = m_pParamArr[1]->GetValue();
    //wxFileName sDstFileName(sDstPath);
    //wxString sPath = sDstFileName.GetPath();
    //IGxObject* pGxDstObject = pGxObjectContainer->SearchChild(sPath);
    //if(!pGxDstObject)
    //{
    //    //add messages to pTrackCancel
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("Error get destination object"), -1, enumGISMessageErr);
    //    return false;
    //}

    //CPLString szPath = pGxDstObject->GetInternalName();
    //wxString sName = sDstFileName.GetName();
    //
    //wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParamArr[1]->GetDomain());
    //IGxObjectFilter* pFilter = pDomain->GetFilter(pDomain->GetSelFilter());
    //if(!pFilter)
    //{
    //    //add messages to pTrackCancel
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
    //    return false;
    //}
    //    
    //bool bHasErrors = ExportFormat(pSrcDataSet, szPath, sName, pFilter, NULL, pTrackCancel);

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
