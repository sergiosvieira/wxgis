/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  write shape cordinates to text file
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

#include "wxgis/geoprocessing/gpshapetotexttool.h"
//#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/geoprocessing/gpvector.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/catop.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPShapeToTextTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPShapeToTextTool, wxGISGPTool)

wxGISGPShapeToTextTool::wxGISGPShapeToTextTool(void) : wxGISGPTool()
{
}

wxGISGPShapeToTextTool::~wxGISGPShapeToTextTool(void)
{
}

const wxString wxGISGPShapeToTextTool::GetDisplayName(void)
{
    return wxString(_("Write shapes coordinates to text file"));
}

const wxString wxGISGPShapeToTextTool::GetName(void)
{
    return wxString(wxT("shape_to_text"));
}

const wxString wxGISGPShapeToTextTool::GetCategory(void)
{
    return wxString(_("Conversion Tools/Vector"));
}

GPParameters wxGISGPShapeToTextTool::GetParameterInfo(void)
{
    if(m_paParam.IsEmpty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source table"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISFeatureDataset));
        pParam1->SetDomain(pDomain1);

        m_paParam.Add(pParam1);

		//swap x y
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("swap_xy"));
        pParam2->SetDisplayName(_("Swap XY coordinates"));
        pParam2->SetParameterType(enumGISGPParameterTypeOptional);
        pParam2->SetDataType(enumGISGPParamDTBool);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);
        pParam2->SetValue(false);

        m_paParam.Add(pParam2);

		//dst path
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("dst_path"));
        pParam3->SetDisplayName(_("Destination text file"));
        pParam3->SetParameterType(enumGISGPParameterTypeRequired);
        pParam3->SetDataType(enumGISGPParamDTPath);
        pParam3->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain3 = new wxGISGPGxObjectDomain();
		pDomain3->AddFilter(new wxGxTextFilter(wxString(_("Text file")), wxString(wxT(".txt"))));
        pParam3->SetDomain(pDomain3);

        m_paParam.Add(pParam3);

    }
    return m_paParam;
}

bool wxGISGPShapeToTextTool::Validate(void)
{
    if(!m_paParam[2]->GetAltered())
    {
        if(m_paParam[0]->GetIsValid())
        {
            //generate temp name
            wxString sPath = m_paParam[0]->GetValue();
            wxFileName Name(sPath);
            Name.SetName(Name.GetName() + wxT("_shape_to_text"));
			Name.SetExt(wxT("txt"));
            m_paParam[2]->SetValue(wxVariant(Name.GetFullPath(), wxT("path")));
            m_paParam[2]->SetAltered(true);//??
        }
    }
    return true;
}

bool wxGISGPShapeToTextTool::Execute(ITrackCancel* pTrackCancel)
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

    wxString sSrcPath = m_paParam[0]->GetValue();
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

	bool bSwapXY = m_paParam[1]->GetValue();
    
    wxString sDstPath = m_paParam[2]->GetValue();

	//check overwrite & do it!
	if(!OverWriteGxObject(pGxObjectContainer->SearchChild(sDstPath), pTrackCancel))
		return false;

	IGxObject* pGxDstObject = GetParentGxObjectFromPath(sDstPath, pGxObjectContainer, pTrackCancel);
    if(!pGxDstObject)
        return false;

    CPLString szPath = pGxDstObject->GetInternalName();
    wxFileName sDstFileName(sDstPath);
    wxString sName = sDstFileName.GetFullName();	

	int nAcc = 16;
	CPLString osDiv(",");
	CPLString osFrm(CPLSPrintf("%%.%df%s%%.%df\n", nAcc, osDiv.c_str(), nAcc));
    bool bRes = GeometryVerticesToTextFile(pSrcDataSet, CPLFormFilename(szPath, sName.mb_str(wxConvUTF8), NULL), osFrm, bSwapXY, NULL, pTrackCancel);

    if(pGxDstObject)
		pGxDstObject->Refresh();

    return bRes;

}

