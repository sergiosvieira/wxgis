/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDataset class.
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

#include "../../art/shape_icon16.xpm"
#include "../../art/shape_icon48.xpm"
#include "../../art/mi_dset_16.xpm"
#include "../../art/mi_dset_48.xpm"
#include "../../art/md_dset_16.xpm"
#include "../../art/md_dset_48.xpm"
#include "../../art/dxf_dset_16.xpm"
#include "../../art/dxf_dset_48.xpm"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/framework/application.h"

#include <wx/busyinfo.h>
#include <wx/utils.h>

//--------------------------------------------------------------
//class wxGxTableDataset
//--------------------------------------------------------------

wxGxTableDataset::wxGxTableDataset(wxString Path, wxString Name, wxGISEnumTableDatasetType nType)
{
	m_ImageListSmall.Create(16, 16);
	m_ImageListSmall.Add(wxBitmap(shape_icon16_xpm));
	m_ImageListLarge.Create(48, 48);
	m_ImageListLarge.Add(wxBitmap(shape_icon48_xpm));

	m_type = nType;

	m_sName = Name;
	m_sPath = Path;

	m_pwxGISDataset = NULL;
    m_pPathEncoding = wxConvCurrent;
}

wxGxTableDataset::~wxGxTableDataset(void)
{
    wsDELETE(m_pwxGISDataset);
}

wxIcon wxGxTableDataset::GetLargeImage(void)
{
////	return wxIcon(m_ImageListLarge.GetIcon(2));
//	switch(m_type)
//	{
//	case enumGISFeatureDataset:
//		return wxIcon(m_ImageListLarge.GetIcon(0));
//	case enumGISRasterDataset:
//		return wxIcon(m_ImageListLarge.GetIcon(2));
//	case enumGISTableDataset:
//	default:
		return wxIcon(m_ImageListLarge.GetIcon(1));
	//}
}

wxIcon wxGxTableDataset::GetSmallImage(void)
{
	return wxIcon(m_ImageListSmall.GetIcon(1));
}

bool wxGxTableDataset::Delete(void)
{
    wxGISFeatureDataset* pDSet = dynamic_cast<wxGISFeatureDataset*>(GetDataset());
    if(!pDSet)
        return false;

    //pDSet->Reference();
    bool bRet = pDSet->Delete();
    if(bRet)
	{
        wsDELETE(pDSet);

		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pParent);
		if(pGxObjectContainer == NULL)
			return false;
		return pGxObjectContainer->DeleteChild(this);		
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
        wxLogError(_("Delete failed! OGR error: %s, file '%s'"), wgMB2WX(err), m_sPath.c_str());
		return false;	
    }
}

bool wxGxTableDataset::Rename(wxString NewName)
{
	m_sName = NewName; 
	return true;
}

void wxGxTableDataset::EditProperties(wxWindow *parent)
{
}

wxGISDataset* wxGxTableDataset::GetDataset(void)
{
	if(m_pwxGISDataset == NULL)
	{		
		m_pwxGISDataset = new wxGISFeatureDataset(m_sPath, enumVecUnknown);
		//for storing internal pointer
		m_pwxGISDataset->Reference();
	}
	//for outer pointer
	m_pwxGISDataset->Reference();
	return m_pwxGISDataset;
}


//--------------------------------------------------------------
//class wxGxFeatureDataset
//--------------------------------------------------------------

wxGxFeatureDataset::wxGxFeatureDataset(wxString Path, wxString Name, wxGISEnumVectorDatasetType Type)
{
	m_ImageListSmall.Create(16, 16);
	m_ImageListSmall.Add(wxBitmap(shape_icon16_xpm));
	m_ImageListLarge.Create(48, 48);
	m_ImageListLarge.Add(wxBitmap(shape_icon48_xpm));

	m_type = Type;

	m_sName = Name;
	m_sPath = Path;

	m_pwxGISDataset = NULL;

    m_pPathEncoding = wxConvCurrent;
}

wxGxFeatureDataset::~wxGxFeatureDataset(void)
{
    wsDELETE(m_pwxGISDataset);
}

wxString wxGxFeatureDataset::GetCategory(void)
{
	return wxString(_("Feature class"));
}

wxIcon wxGxFeatureDataset::GetLargeImage(void)
{
	switch(m_type)
	{
	case enumVecESRIShapefile:
		return wxIcon(m_ImageListLarge.GetIcon(0));
	case enumVecMapinfoTab:
        return wxIcon(mi_dset_48_xpm);
	case enumVecMapinfoMif:
        return wxIcon(md_dset_48_xpm);
	case enumVecKML:
		return wxIcon(m_ImageListLarge.GetIcon(4));
	case enumVecDXF:
		return wxIcon(dxf_dset_48_xpm);
	default:
		return wxNullIcon;
	}
}

wxIcon wxGxFeatureDataset::GetSmallImage(void)
{
	switch(m_type)
	{
	case enumVecESRIShapefile:
		return wxIcon(m_ImageListSmall.GetIcon(0));
	case enumVecMapinfoTab:
        return wxIcon(mi_dset_16_xpm);
	case enumVecMapinfoMif:
        return wxIcon(md_dset_16_xpm);
	case enumVecKML:
		return wxIcon(m_ImageListSmall.GetIcon(4));
	case enumVecDXF:
		return wxIcon(dxf_dset_16_xpm);
	default:
		return wxNullIcon;
	}
}

bool wxGxFeatureDataset::Delete(void)
{
    wxGISFeatureDataset* pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = new wxGISFeatureDataset(m_sPath, m_type);
        m_pwxGISDataset = static_cast<wxGISDataset*>(pDSet);
    }
    else
    {
        pDSet = dynamic_cast<wxGISFeatureDataset*>(m_pwxGISDataset);
    }
    
    if(!pDSet)
        return false;

    bool bRet = pDSet->Delete();
    if(bRet)
	{
        //wsDELETE(pDSet);
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pParent);
		if(pGxObjectContainer == NULL)
			return false;
		return pGxObjectContainer->DeleteChild(this);		
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
        wxLogError(_("Delete failed! OGR error: %s, file '%s'"), wgMB2WX(err), m_sPath.c_str());
		return false;	
    }
}

bool wxGxFeatureDataset::Rename(wxString NewName)
{
	m_sName = NewName; 
	return true;
}

void wxGxFeatureDataset::EditProperties(wxWindow *parent)
{
}

wxGISDataset* wxGxFeatureDataset::GetDataset(void)
{
	if(m_pwxGISDataset == NULL)
	{		
        wxGISFeatureDataset* pwxGISFeatureDataset = new wxGISFeatureDataset(m_sPath, m_type);

        if(!pwxGISFeatureDataset->Open())
        {
		    const char* err = CPLGetLastErrorMsg();
		    wxString sErr = wxString::Format(_("Open failed! OGR error: %s"), wgMB2WX(err));
		    wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);

            wxDELETE(pwxGISFeatureDataset);
			return NULL;
        }

       // //Spatial Index
       // if(!pwxGISFeatureDataset->HasSpatialIndex())
       // {
       // 	bool bAskSpaInd = true;
       //     wxString name, ext;
       //     wxFileName::SplitPath(m_sPath, NULL, NULL, &name, &ext);
       //     wxString sFileName = name + wxT(".") + ext;
       //     IGISConfig*  pConfig = m_pCatalog->GetConfig();
       //     bool bCreateSpaInd = true;
       //     if(pConfig)
       //     {
       //         wxXmlNode* pNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/vector")));
       //         if(pNode)
       //             bAskSpaInd = wxAtoi(pNode->GetPropVal(wxT("create_tree"), wxT("1")));
       //         else
       //         {
       //             pNode = pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/vector")), true);
       //             pNode->AddProperty(wxT("create_tree"), wxT("1"));
       //         }
       //         if(bAskSpaInd)
       //         {
       //             //show ask dialog
       //             wxGISMessageDlg dlg(NULL, wxID_ANY, wxString::Format(_("Create spatial index for %s"), sFileName.c_str()), wxString(_("This vector data source does not have spatial index. Spatial index allow for rapid display at varying resolutions.")), wxString(_("Spatial index buildin may take a few moments.\nWould you like to create spatial index?")), wxDefaultPosition, wxSize( 400,160 ));
       //             if(dlg.ShowModal() == wxID_NO)
       //             {
       //                 bCreateSpaInd = false;
       //             }
       //             if(!dlg.GetShowInFuture())
       //             {
       //                 pNode->DeleteProperty(wxT("create_tree"));
       //                 pNode->AddProperty(wxT("create_tree"), wxT("0"));
       //             }
       //         }
       //     }

	      //  if(bCreateSpaInd)
	      //  {
       //         wxWindowDisabler disableAll;
       //         wxBusyInfo wait(_("Please wait, working..."));

       //         OGRErr err = pwxGISFeatureDataset->CreateSpatialIndex();
			    //if(err != OGRERR_NONE)
			    //{
       //             const char* pszerr = CPLGetLastErrorMsg();
       //             wxLogError(_("Build Spatial Index failed! OGR error: %s"), wgMB2WX(pszerr));
       //             int nRes = wxMessageBox(_("Build Spatial Index! Continue?"), _("Question"), wxICON_QUESTION | wxYES_NO);
       //             if(nRes == wxNO)
       //                 return NULL;
			    //}
	      //  }
       // }

		m_pwxGISDataset = static_cast<wxGISDataset*>(pwxGISFeatureDataset);
        m_pwxGISDataset->SetSubType(m_type);
		//for storing internal pointer
		m_pwxGISDataset->Reference();
	}
	//for outer pointer
	m_pwxGISDataset->Reference();
	return m_pwxGISDataset;
}

//--------------------------------------------------------------
//class wxGxRasterDataset
//--------------------------------------------------------------

wxGxRasterDataset::wxGxRasterDataset(wxString Path, wxString Name, wxGISEnumRasterDatasetType Type)
{
	m_ImageListSmall.Create(16, 16);
	m_ImageListSmall.Add(wxBitmap(shape_icon16_xpm));
	m_ImageListLarge.Create(48, 48);
	m_ImageListLarge.Add(wxBitmap(shape_icon48_xpm));

	m_type = Type;

	m_sName = Name;
	m_sPath = Path;

	m_pwxGISDataset = NULL;
}

wxGxRasterDataset::~wxGxRasterDataset(void)
{
	wsDELETE(m_pwxGISDataset);
}

wxIcon wxGxRasterDataset::GetLargeImage(void)
{
	switch(m_type)
	{
	case enumRasterUnknown:
	default:
		return wxIcon(m_ImageListLarge.GetIcon(2));
	}
}

wxIcon wxGxRasterDataset::GetSmallImage(void)
{
	switch(m_type)
	{
	case enumRasterUnknown:
	default:
		return wxIcon(m_ImageListSmall.GetIcon(2));
	}
}

bool wxGxRasterDataset::Delete(void)
{
    wxGISRasterDataset* pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = new wxGISRasterDataset(m_sPath, m_type);
        m_pwxGISDataset = static_cast<wxGISDataset*>(pDSet);
    }
    else
    {
        pDSet = dynamic_cast<wxGISRasterDataset*>(m_pwxGISDataset);
    }
    
    if(!pDSet)
        return false;

    bool bRet = pDSet->Delete();
    if(bRet)
	{
        //wsDELETE(pDSet);
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pParent);
		if(pGxObjectContainer == NULL)
			return false;
		return pGxObjectContainer->DeleteChild(this);		
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
        wxLogError(_("Delete failed! OGR error: %s, file '%s'"), wgMB2WX(err), m_sPath.c_str());
		return false;	
    }
}

bool wxGxRasterDataset::Rename(wxString NewName)
{
	m_sName = NewName; 
	return true;
}

void wxGxRasterDataset::EditProperties(wxWindow *parent)
{
}

wxString wxGxRasterDataset::GetCategory(void)
{
	switch(m_type)
	{
	case enumRasterUnknown:
	default:
		return wxString(_("Raster"));
	}
}

wxGISDataset* wxGxRasterDataset::GetDataset(void)
{
	if(m_pwxGISDataset == NULL)
	{	
        wxGISRasterDataset* pwxGISRasterDataset = new wxGISRasterDataset(m_sPath, m_type);

        //open (ask for overviews)
        if(!pwxGISRasterDataset->Open())
        {
		    const char* err = CPLGetLastErrorMsg();
		    wxString sErr = wxString::Format(_("Open failed! OGR error: %s"), wgMB2WX(err));
		    wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);

            wxDELETE(pwxGISRasterDataset);
			return NULL;
        }
        
        m_pwxGISDataset = static_cast<wxGISDataset*>(pwxGISRasterDataset);
		//for storing internal pointer
		m_pwxGISDataset->Reference();
	}
	//for outer pointer
	m_pwxGISDataset->Reference();
	return m_pwxGISDataset;
}
