/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  convert geometry vertices to points
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
#include "wxgis/geoprocessing/gpgeomverttoptstool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/catop.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPExportTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPGeomVerticesToPointsTool, wxGISGPTool)

wxGISGPGeomVerticesToPointsTool::wxGISGPGeomVerticesToPointsTool(void) : wxGISGPTool()
{
}

wxGISGPGeomVerticesToPointsTool::~wxGISGPGeomVerticesToPointsTool(void)
{
    for(size_t i = 0; i < m_pParamArr.size(); ++i)
        wxDELETE(m_pParamArr[i]);
}

wxString wxGISGPGeomVerticesToPointsTool::GetDisplayName(void)
{
    return wxString(_("Geometry Vertices To Points"));
}

wxString wxGISGPGeomVerticesToPointsTool::GetName(void)
{
    return wxString(wxT("gvert_to_pts"));
}

wxString wxGISGPGeomVerticesToPointsTool::GetCategory(void)
{
    return wxString(_("Conversion Tools/Vector"));
}

GPParameters* wxGISGPGeomVerticesToPointsTool::GetParameterInfo(void)
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

        //dst path
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("dst_path"));
        pParam2->SetDisplayName(_("Destination feature class"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTPath);
        pParam2->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
        AddAllVectorFilters(pDomain2);
        pParam2->SetDomain(pDomain2);

        m_pParamArr.push_back(pParam2);

    }
    return &m_pParamArr;
}

bool wxGISGPGeomVerticesToPointsTool::Validate(void)
{
    if(!m_pParamArr[1]->GetAltered())
    {
        if(m_pParamArr[0]->GetIsValid())
        {
            //generate temp name
            wxString sPath = m_pParamArr[0]->GetValue();
            wxFileName Name(sPath);
            Name.SetName(Name.GetName() + wxT("_gvert_to_pts"));
            m_pParamArr[1]->SetValue(wxVariant(Name.GetFullPath(), wxT("path")));
            m_pParamArr[1]->SetAltered(true);//??
        }
    }

    return true;
}

bool wxGISGPGeomVerticesToPointsTool::Execute(ITrackCancel* pTrackCancel)
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
            pTrackCancel->PutMessage(_("Error get source object"), -1, enumGISMessageErr);
        return false;
    }
    IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
    if(!pGxDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The source object is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    wxGISFeatureDatasetSPtr pSrcDataSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(pGxDataset->GetDataset());
    if(!pSrcDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The source dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
	if(!pSrcDataSet->IsOpened())
		if(!pSrcDataSet->Open())
			return false;
    
    OGRFeatureDefn *pDef = pSrcDataSet->GetDefinition();
    if(!pDef)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error reading dataset definition"), -1, enumGISMessageErr);
        //wsDELETE(pSrcDataSet);
        return false;
    }
    
    wxString sDstPath = m_pParamArr[1]->GetValue();

	//check overwrite & do it!
	if(!OverWriteGxObject(pGxObjectContainer->SearchChild(sDstPath), pTrackCancel))
		return false;

    wxFileName sDstFileName(sDstPath);
    wxString sPath = sDstFileName.GetPath();
    IGxObject* pGxDstObject = pGxObjectContainer->SearchChild(sPath);
    if(!pGxDstObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error get destination object"), -1, enumGISMessageErr);
        return false;
    }

    CPLString szPath = pGxDstObject->GetInternalName();
    wxString sName = sDstFileName.GetName();
    
    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParamArr[1]->GetDomain());
	IGxObjectFilter* pFilter = pDomain->GetFilter(m_pParamArr[1]->GetSelDomainValue());
    if(!pFilter)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
        return false;
    }
        
    bool bRes = GeometryVerticesToPoints(pSrcDataSet, szPath, sName, pFilter, NULL, pTrackCancel);

    IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(pCont)
    {
        IGxObject* pParentLoc = pCont->SearchChild(sPath);
        if(pParentLoc)
            pParentLoc->Refresh();
    }

    return bRes;
}