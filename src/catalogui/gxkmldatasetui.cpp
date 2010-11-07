/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxKMLDatasetUI classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
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
#include "wxgis/catalogui/gxkmldatasetui.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/datasource/featuredataset.h"

#include "../../art/kml_subdset_16.xpm"
#include "../../art/kml_subdset_48.xpm"


//--------------------------------------------------------------
//class wxGxKMLDatasetUI
//--------------------------------------------------------------

wxGxKMLDatasetUI::wxGxKMLDatasetUI(wxString Path, wxString Name, wxGISEnumVectorDatasetType Type, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxKMLDataset(Path, Name, Type)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
    m_LargeSubIcon = wxIcon(kml_subdset_48_xpm);
    m_SmallSubIcon = wxIcon(kml_subdset_16_xpm);
}

wxGxKMLDatasetUI::~wxGxKMLDatasetUI(void)
{
}

wxIcon wxGxKMLDatasetUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxKMLDatasetUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

void wxGxKMLDatasetUI::EditProperties(wxWindow *parent)
{
}

void wxGxKMLDatasetUI::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
        wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pCatalog);
        if(pCatalog)
        {
            IGxSelection* pSel = pCatalog->GetSelection();
            if(pSel)
                pSel->Unselect(m_Children[i], IGxSelection::INIT_ALL);
        }
		m_Children[i]->Detach();
		wxDELETE( m_Children[i] );
	}
    m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxKMLDatasetUI::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

	if(m_pwxGISDataset == NULL)
	{
        wxGISFeatureDataset* pwxGISFeatureDataset = new wxGISFeatureDataset(m_sPath, m_type);

        if(!pwxGISFeatureDataset->Open())
        {
		    const char* err = CPLGetLastErrorMsg();
		    wxString sErr = wxString::Format(_("Open failed! GDAL error: %s"), wgMB2WX(err));
		    wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);

            wxDELETE(pwxGISFeatureDataset);
			return;
        }

		m_pwxGISDataset = static_cast<wxGISDataset*>(pwxGISFeatureDataset);
        m_pwxGISDataset->SetSubType(m_type);
        m_pwxGISDataset->Reference();
	}

    for(size_t i = 0; i < m_pwxGISDataset->GetSubsetsCount(); i++)
    {
        wxGISFeatureDataset* pwxGISFeatureSuDataset = dynamic_cast<wxGISFeatureDataset*>(m_pwxGISDataset->GetSubset(i));//wxGISDataset* pSubSet
        pwxGISFeatureSuDataset->SetSubType(m_type);
        wxGxKMLSubDatasetUI* pGxSubDataset = new wxGxKMLSubDatasetUI(pwxGISFeatureSuDataset->GetName(), pwxGISFeatureSuDataset, m_type, m_LargeSubIcon, m_SmallSubIcon);
		bool ret_code = AddChild(pGxSubDataset);
		if(!ret_code)
			wxDELETE(pGxSubDataset);
	}
	m_bIsChildrenLoaded = true;
}

//--------------------------------------------------------------
//class wxGxKMLSubDatasetUI
//--------------------------------------------------------------

wxGxKMLSubDatasetUI::wxGxKMLSubDatasetUI(wxString sName, wxGISDataset* pwxGISDataset, wxGISEnumVectorDatasetType nType, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxKMLSubDataset(sName, pwxGISDataset, nType)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxKMLSubDatasetUI::~wxGxKMLSubDatasetUI(void)
{
}

wxIcon wxGxKMLSubDatasetUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxKMLSubDatasetUI::GetSmallImage(void)
{
	return m_SmallIcon;
}
