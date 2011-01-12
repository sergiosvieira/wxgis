/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTableView class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/datasource/featuredataset.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxTableView, wxGISTableView)

wxGxTableView::wxGxTableView(void)
{
}

wxGxTableView::wxGxTableView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISTableView(parent, id, pos, size)
{
	m_sViewName = wxString(_("Table View"));
	SetReadOnly(true);
}

wxGxTableView::~wxGxTableView(void)
{
}

bool wxGxTableView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	m_sViewName = wxString(_("Table View"));
    wxGISTableView::Create(parent, TABLECTRLID, pos, size, style, name);
	SetReadOnly(true);
    return true;
}

bool wxGxTableView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	wxGxView::Activate(application, pConf);
	//Serialize(m_pXmlConf, false);

	//m_pConnectionPointCatalog = dynamic_cast<IConnectionPointContainer*>( m_pCatalog );
	//if(m_pConnectionPointCatalog != NULL)
	//	m_ConnectionPointCatalogCookie = m_pConnectionPointCatalog->Advise(this);

    wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(application->GetCatalog());
	m_pSelection = pGxCatalogUI->GetSelection();
	return true;
}

void wxGxTableView::Deactivate(void)
{
	//if(m_ConnectionPointSelectionCookie != -1)
	//	m_pConnectionPointSelection->Unadvise(m_ConnectionPointSelectionCookie);
	//if(m_ConnectionPointCatalogCookie != -1)
	//	m_pConnectionPointCatalog->Unadvise(m_ConnectionPointCatalogCookie);

	//Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}

bool wxGxTableView::Applies(IGxSelection* Selection)
{
	if(Selection == NULL)
		return false;

	for(size_t i = 0; i < Selection->GetCount(); i++)
	{
		IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>( Selection->GetSelectedObjects(i) );
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

void wxGxTableView::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
	if(nInitiator == GetId())
		return;

	if(m_pSelection->GetCount() == 0)
		return;
	IGxObject* pGxObj = m_pSelection->GetLastSelectedObject();	
	if(m_pParentGxObject == pGxObj)
		return;

	IGxDataset* pGxDataset =  dynamic_cast<IGxDataset*>(pGxObj);
	if(pGxDataset == NULL)
		return;

    wxBusyCursor wait;
	wxGISDataset* pwxGISDataset = pGxDataset->GetDataset(true);
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

	wxGISTable* pTable = new wxGISTable(pwxGISDataset);
	wxGISTableView::SetTable(pTable, true);
	////reset 
	//ResetContents();

	//IGxObjectContainer* pObjContainer =  dynamic_cast<IGxObjectContainer*>(pGxObj);
	//if(pObjContainer == NULL || !pObjContainer->HasChildren())
	//	return;
	//GxObjectArray* pArr = pObjContainer->GetChildren();
	//for(size_t i = 0; i < pArr->size(); i++)
	//{
	//	AddObject(pArr->at(i));
	//}

	//SortItems(MyCompareFunction, m_bSortAsc);
 //   SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);

	m_pParentGxObject = pGxObj;
	wxWindow::Refresh();
}