/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTableView class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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
#include "wxgis/catalogui/gxtableview.h"
#include "wxgis/datasource/featuredataset.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxTableView, wxGISTableView)

BEGIN_EVENT_TABLE(wxGxTableView, wxGISTableView)
	EVT_GXSELECTION_CHANGED(wxGxTableView::OnSelectionChanged)
END_EVENT_TABLE()


wxGxTableView::wxGxTableView(void)
{
}

wxGxTableView::wxGxTableView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISTableView(parent, id, pos, size)
{
	m_nParentGxObjectID = wxNOT_FOUND;
	m_sViewName = wxString(_("Table View"));
	SetReadOnly(true);
}

wxGxTableView::~wxGxTableView(void)
{
}

bool wxGxTableView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	m_nParentGxObjectID = wxNOT_FOUND;
	m_sViewName = wxString(_("Table View"));
    wxGISTableView::Create(parent, TABLECTRLID, pos, size, style, name);
	SetReadOnly(true);
    return true;
}

bool wxGxTableView::Activate(IFrameApplication* application, wxXmlNode* pConf)
{
	if(!wxGxView::Activate(application, pConf))
		return false;
	//Serialize(m_pXmlConf, false);

    m_pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pGxApplication->GetCatalog());
	if(m_pCatalog)
		m_pSelection = m_pCatalog->GetSelection();

	return true;
}

void wxGxTableView::Deactivate(void)
{
	//Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}

bool wxGxTableView::Applies(IGxSelection* Selection)
{
	if(Selection == NULL)
		return false;

	for(size_t i = 0; i < Selection->GetCount(); ++i)
	{
		IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject( Selection->GetSelectedObjectID(i) );
		IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>( pGxObject.get() );
		if(pGxDataset != NULL)
		{			
			wxGISEnumDatasetType type = pGxDataset->GetType();
			switch(type)
			{
			case enumGISTableDataset:
			case enumGISFeatureDataset:
				return true;
			case enumGISRasterDataset:
				break;
			}

		}
	}
	return false;
}

void wxGxTableView::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(event.GetInitiator() == GetId())
		return;

	if(m_pSelection->GetCount() == 0)
		return;
    long nSelID = m_pSelection->GetLastSelectedObjectID();
	if(m_nParentGxObjectID == nSelID)
		return;

    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(nSelID);
	IGxDataset* pGxDataset =  dynamic_cast<IGxDataset*>(pGxObject.get());
	if(pGxDataset == NULL)
		return;

    wxBusyCursor wait;
	wxGISDatasetSPtr pwxGISDataset = pGxDataset->GetDataset();
	if(pwxGISDataset == NULL)
		return;

	//wxGISEnumDatasetType type = pwxGISDataset->GetType();
	//OGRLayer* pOGRLayer(NULL);

	//switch(type)
	//{
	//case enumGISTableDataset:
	//case enumGISFeatureDataset:
	//	{
	//	wxGISFeatureDataset* pwxGISFeatureDataset = dynamic_cast<wxGISFeatureDataset*>(pwxGISDataset);
	//	if(pwxGISFeatureDataset)
	//		pOGRLayer = pwxGISFeatureDataset->GetLayer();
	//	}
	//	break;
	//default:
	//case enumGISRasterDataset:
	//	break;
	//}

	////the pOGRLayer will live while IGxObject live. IGxObject( from IGxSelection ) store IwxGISDataset, and destroy it then catalog destroyed 
	//pwxGISDataset->Release();

	//if(pOGRLayer == NULL)
	//	return;

	wxGISGridTable* pTable = new wxGISGridTable(pwxGISDataset);
	wxGISTableView::SetTable(pTable, true);

	////reset 
	//ResetContents();

	//IGxObjectContainer* pObjContainer =  dynamic_cast<IGxObjectContainer*>(pGxObj);
	//if(pObjContainer == NULL || !pObjContainer->HasChildren())
	//	return;
	//GxObjectArray* pArr = pObjContainer->GetChildren();
	//for(size_t i = 0; i < pArr->size(); ++i)
	//{
	//	AddObject(pArr->at(i));
	//}

	//SortItems(MyCompareFunction, m_bSortAsc);
 //   SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
	m_nParentGxObjectID = nSelID;

	wxWindow::Refresh();
}