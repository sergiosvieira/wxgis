/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxGxToolboxView class.
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

#include "wxgis/geoprocessingui/gptoolboxview.h"

//-------------------------------------------------------------------
// wxGxToolboxView
//-------------------------------------------------------------------

//BEGIN_EVENT_TABLE(wxGxToolboxView, wxAuiNotebook)
//	EVT_AUINOTEBOOK_PAGE_CHANGED(TOOLVIEWCTRLID, wxGxToolboxView::OnAUINotebookPageChanged)
//END_EVENT_TABLE()

//IMPLEMENT_DYNAMIC_CLASS(wxGxToolboxView, wxAuiNotebook)

wxGxToolboxView::wxGxToolboxView(void)
{
}

wxGxToolboxView::wxGxToolboxView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxAuiNotebook(parent, id, pos, size, wxAUI_NB_BOTTOM | wxNO_BORDER | wxAUI_NB_TAB_MOVE)
{
    m_sViewName = wxString(_("Tasks pane"));
}

wxGxToolboxView::~wxGxToolboxView(void)
{
}

bool wxGxToolboxView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	if(!application || !pConf)
		return false;
	wxGxView::Activate(application, pConf);

    m_pApp = dynamic_cast<IApplication*>(application);
	//wxXmlNode* pChild = m_pXmlConf->GetChildren();
	wxUint8 count(0);
	//while(pChild)
	//{
	//	wxGxTab* pGxTab = new wxGxTab(application, pChild, this);
	//	//wxWindow* pWnd = pGxTab->GetWindow(0);
	//	//if(pWnd == NULL)
	//	//	pWnd = new wxWindow(this, wxID_ANY);
	//	m_Tabs.push_back(pGxTab);

    //add tasks vindow
    m_pGxTasksView = new wxGxTasksView(this);    
    AddPage(m_pGxTasksView, m_pGxTasksView->GetViewName(), count == 0 ? true : false, m_pGxTasksView->GetViewIcon());
    m_pApp->RegisterChildWindow(m_pGxTasksView);
    count++;

    //add tree tools window

	//AddPage(static_cast<wxWindow*>(pGxTab), pGxTab->GetName(), count == 0 ? true : false/*, m_ImageListSmall.GetBitmap(9)*/);
	//	
	//	count++;

	//	pChild = pChild->GetNext();
	//}

	//m_pSelection = application->GetCatalog()->GetSelection();
	//m_pConnectionPointSelection = dynamic_cast<IConnectionPointContainer*>( m_pSelection );
	//if(m_pConnectionPointSelection != NULL)
	//	m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(this);

	return true;
}

void wxGxToolboxView::Deactivate(void)
{
    //m_pGxTasksView->Deactivate();
    m_pApp->UnRegisterChildWindow(m_pGxTasksView);
    while(GetPageCount() > 0)
        RemovePage(0); //will delete in app destructor

	//if(m_ConnectionPointSelectionCookie != -1)
	//	m_pConnectionPointSelection->Unadvise(m_ConnectionPointSelectionCookie);
}

//void wxGxToolboxView::OnAUINotebookPageChanged(wxAuiNotebookEvent& event)
//{
//	//update view while changing focus of tabs
//	event.Skip();
//	//int nSelTab = event.GetSelection();
// //   if(nSelTab < 0)
// //       return;
//	//wxASSERT(nSelTab >= 0 && nSelTab < m_Tabs.size());
//
//	//wxGxTab* pCurrTab = m_Tabs[nSelTab];
//	//if(pCurrTab != NULL && m_pSelection != NULL)
//	//	pCurrTab->OnSelectionChanged(m_pSelection, IGxSelection::INIT_ALL);
//}

wxWindow* wxGxToolboxView::GetCurrentWnd(void)
{
	//int nSelTab = GetSelection();
	//wxASSERT(nSelTab >= 0 && nSelTab < m_Tabs.size());

	//wxGxTab* pCurrTab = m_Tabs[nSelTab];
	//if(pCurrTab)
	//	return pCurrTab->GetCurrentWindow();
	return NULL;
}
