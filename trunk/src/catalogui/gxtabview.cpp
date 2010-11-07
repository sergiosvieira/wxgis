/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTabView class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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
#include "wxgis/catalogui/gxtabview.h"
#include "wxgis/catalogui/gxapplication.h"

//-------------------------------------------------------------------
// wxGxTab
//-------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxTab, wxPanel)

BEGIN_EVENT_TABLE(wxGxTab, wxPanel)
	EVT_CHOICE(ID_WNDCHOICE, wxGxTab::OnChoice)
END_EVENT_TABLE()

wxGxTab::wxGxTab(IGxApplication* application, wxXmlNode* pTabDesc, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style), m_bShowChoices(false), m_pCurrentWnd(NULL)
{
	m_sName = wxGetTranslation( pTabDesc->GetPropVal(wxT("name"), NONAME) );
	m_bShowChoices = pTabDesc->GetPropVal(wxT("show_choices"), wxT("f")) == wxT("f") ? false : true;
    m_pApp = dynamic_cast<IApplication*>(application);
    if(!m_pApp)
        return;

	wxXmlNode* pChild = pTabDesc->GetChildren();
	while(pChild)
	{
		wxString sClass = pChild->GetPropVal(wxT("class"), ERR);
		int nPriority = wxAtoi(pChild->GetPropVal(wxT("priority"), wxT("0")));

		wxObject *obj = wxCreateDynamicObject(sClass);
		IGxView *pGxView = dynamic_cast<IGxView*>(obj);
		wxWindow* pWnd = dynamic_cast<wxWindow*>(pGxView);
		if(pGxView && pWnd)
		{
            pGxView->Create(this);
			pWnd->Hide();
			m_pApp->RegisterChildWindow(pWnd);

			pGxView->Activate(application, pChild);
			if(m_pWindows.size() <= nPriority)
			for(size_t i = 0; i < nPriority + 1; i++)
				m_pWindows.push_back(NULL);

			m_pWindows[nPriority] = pWnd;
			wxLogMessage(_("wxGxTab: View class %s in '%s' tab initialise"), sClass.c_str(), m_sName.c_str());
		}
		else
		{
			wxLogError(_("wxGxTab: Error creating view %s"), sClass.c_str());
		    wxDELETE(pGxView);
		}

		pChild = pChild->GetNext();
	}

	m_bSizerMain = new wxBoxSizer( wxVERTICAL );

	//bool bSet = false;
	//for(size_t i = 0; i < m_pWindows.size(); i++)
	//{
	//	if(m_pWindows[i] == NULL)
	//		continue;
	//	wxWindow* pWnd = m_pWindows[i];
	//	pWnd->Show();
	//	m_bSizerMain->Add( pWnd, 1, m_bShowChoices == true ? wxEXPAND | wxALL : wxEXPAND, 5 );
	//	m_pCurrentWnd = pWnd;
	//	bSet = true;
	//	break;
	//}

	//if(!bSet)
	//{
	m_tabwnd = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_bSizerMain->Add( m_tabwnd, 1, m_bShowChoices == true ? wxEXPAND | wxALL : wxEXPAND, 5 );
	m_pCurrentWnd = m_tabwnd;
	//}

	if(m_bShowChoices)
	{

		m_tabselector = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
		m_tabselector->SetMaxSize( wxSize( -1,30 ) );

		wxBoxSizer* bSizerMinor = new wxBoxSizer( wxHORIZONTAL );

		m_staticText = new wxStaticText( m_tabselector, wxID_ANY, _("Preview: "), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText->Wrap( -1 );
		bSizerMinor->Add( m_staticText, 0, wxALIGN_CENTER_VERTICAL, 5 );

		m_choice = new wxChoice( m_tabselector, ID_WNDCHOICE, wxDefaultPosition, wxDefaultSize, 0, 0 );
		m_choice->Disable();
		m_choice->SetMinSize( wxSize( 170,-1 ) );

		bSizerMinor->Add( m_choice, 0, wxALIGN_CENTER_VERTICAL, 5 );

		m_tabselector->SetSizer( bSizerMinor );
		m_tabselector->Layout();
		bSizerMinor->Fit( m_tabselector );
		m_bSizerMain->Add( m_tabselector, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	}
	this->SetSizer( m_bSizerMain );
	this->Layout();

    wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(application->GetCatalog());
	m_pSelection = pGxCatalogUI->GetSelection();
}

wxGxTab::~wxGxTab(void)
{
    while (!m_pWindows.empty())
    {
        wxWindow* poWnd = m_pWindows.back();
        m_pApp->UnRegisterChildWindow(poWnd);
        m_pWindows.pop_back();
    }


	//for(size_t i = 0; i < m_pWindows.size(); i++)
	//{
	//	wxGxView* pView = dynamic_cast<wxGxView*>(m_pWindows[i]);
	//	if(pView != NULL)
 //       {
	//		pView->Deactivate();
            //m_pApp->UnRegisterChildWindow(m_pWindows[i]);
 //       }
	//	//wxDELETE(m_pWindows[i]);//destroy in registerwindows array
 //       m_pWindows[i]->Destroy();
	//}
}

wxString wxGxTab::GetName(void)
{
	return m_sName;
}

wxWindow* wxGxTab::GetWindow(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_pWindows.size())
		return NULL;
	return m_pWindows[iIndex];
}

void wxGxTab::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
    if(!IsShown())
        return;
	if(!Selection)
		return;

	if(nInitiator == GetId())
		return;

    this->Layout();
	//select in tree ctrl
	if(nInitiator != TREECTRLID && nInitiator != LISTCTRLID)
	{
		IGxObject* pGxObj = Selection->GetLastSelectedObject();
		if(pGxObj == NULL)
			return;
		Selection->Select(pGxObj , false, GetId());
		return;
	}

	if(m_bShowChoices)
	{
		m_choice->Clear();

		//let's store no view pointer for default selection
		wxWindow* pNoWnd(NULL);
		//check all windows if applies selection and fill m_choice
		for(size_t i = 0; i < m_pWindows.size(); i++)
		{
			wxWindow* pWnd = m_pWindows[i];
			wxGxView* pView = dynamic_cast<wxGxView*>(pWnd);
			if(pView == NULL)
				continue;
			if(pView->Applies(Selection))
			{
				if(pView->GetViewName() != wxString(_("NoView")))
					m_choice->Append(pView->GetViewName(), pWnd);
				else
					pNoWnd = pWnd;
			}
		}
		if(m_choice->GetCount() > 0)
		{
			m_choice->Enable(true);
			bool bWndPresent(false);

			for(size_t i = 0; i < m_choice->GetCount(); i++)
			{
				wxWindow* pWnd = (wxWindow*) m_choice->GetClientData(i);
				if(m_pCurrentWnd == pWnd)
				{
					m_choice->Select(i);
					bWndPresent = true;
					break;
				}
			}

			if(!bWndPresent)
			{
				m_choice->Select(0);
				wxWindow* pWnd = (wxWindow*) m_choice->GetClientData(0);
				if(pWnd != NULL)
				{
					m_pCurrentWnd->Hide();
					m_bSizerMain->Replace(m_pCurrentWnd, pWnd);
					pWnd->Show();
					m_pCurrentWnd = pWnd;
					this->Layout();
				}
			}
		}
		else
		{
			m_choice->Enable(false);
			if(pNoWnd != NULL)
			{
				m_pCurrentWnd->Hide();
				m_bSizerMain->Replace(m_pCurrentWnd, pNoWnd);
				pNoWnd->Show();
				m_pCurrentWnd = pNoWnd;
				this->Layout();
			}
		}
		goto END;
	}
	else
	{
		//get first apply window
		wxGxView* pCurrView = dynamic_cast<wxGxView*>(m_pCurrentWnd);
		if(pCurrView && pCurrView->GetViewName() != wxString(_("NoView")))
		{
			if(pCurrView->Applies(Selection))
				goto END;
		}
		for(size_t i = 0; i < m_pWindows.size(); i++)
		{
			wxWindow* pWnd = m_pWindows[i];
			wxGxView* pView = dynamic_cast<wxGxView*>(pWnd);
			if(pView == NULL)
				continue;
			if(pView->Applies(Selection))
			{
				if(pCurrView != pView)
				{
					m_pCurrentWnd->Hide();
					m_bSizerMain->Replace(m_pCurrentWnd, pWnd);
					pWnd->Show();
					m_pCurrentWnd = pWnd;
					this->Layout();
				}
				goto END;
			}
		}
	}
END:
	IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(m_pCurrentWnd);
	if(pGxSelectionEvents != NULL)
		pGxSelectionEvents->OnSelectionChanged(Selection, nInitiator);
}

void wxGxTab::OnChoice(wxCommandEvent& event)
{
	event.Skip();
	int pos = event.GetSelection();
	if(pos < 0)
		return;

	wxWindow* pWnd = (wxWindow*) m_choice->GetClientData(pos);
	if(pWnd && m_pCurrentWnd != pWnd)
	{
		m_pCurrentWnd->Hide();
		m_bSizerMain->Replace(m_pCurrentWnd, pWnd);
		pWnd->Show();
		m_pCurrentWnd = pWnd;

		this->Layout();

		IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(m_pCurrentWnd);
		if(pGxSelectionEvents != NULL)
			pGxSelectionEvents->OnSelectionChanged(m_pSelection, GetId());

	}
}

bool wxGxTab::Show(bool bShow)
{
	if(m_pCurrentWnd)
		m_pCurrentWnd->Show(bShow);
    if(bShow)
        this->Layout();
	return wxWindow::Show(bShow);
}

void wxGxTab::Deactivate(void)
{
	for(size_t i = 0; i < m_pWindows.size(); i++)
	{
		wxGxView* pView = dynamic_cast<wxGxView*>(m_pWindows[i]);
		if(pView != NULL)
        {
			pView->Deactivate();
            //m_pApp->UnRegisterChildWindow(m_pWindows[i]);
        }
		//wxDELETE(m_pWindows[i]);//destroy in registerwindows array
        //if(m_pWindows[i])
        //    m_pWindows[i]->Destroy();
	}
}

//-------------------------------------------------------------------
// wxGxTabView
//-------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxTabView, wxAuiNotebook)

BEGIN_EVENT_TABLE(wxGxTabView, wxAuiNotebook)
	EVT_AUINOTEBOOK_PAGE_CHANGED(TABCTRLID, wxGxTabView::OnAUINotebookPageChanged)
	EVT_BUTTON(ID_SELCHANGED, wxGxTabView::OnSelChanged)
END_EVENT_TABLE()

wxGxTabView::wxGxTabView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxAuiNotebook(parent, id, pos, size, wxAUI_NB_TOP | wxNO_BORDER | wxAUI_NB_TAB_MOVE), m_pSelection(NULL)
{
}

wxGxTabView::~wxGxTabView(void)
{
}

bool wxGxTabView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_pSelection = NULL;
    return wxAuiNotebook::Create(parent, TABCTRLID, pos, size, wxAUI_NB_TOP | wxNO_BORDER | wxAUI_NB_TAB_MOVE);
}

bool wxGxTabView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	if(!application || !pConf)
		return false;
	wxGxView::Activate(application, pConf);

	wxXmlNode* pChild = m_pXmlConf->GetChildren();
	wxUint8 count(0);
	while(pChild)
	{
		wxGxTab* pGxTab = new wxGxTab(application, pChild, this);
		//wxWindow* pWnd = pGxTab->GetWindow(0);
		//if(pWnd == NULL)
		//	pWnd = new wxWindow(this, wxID_ANY);
		m_Tabs.push_back(pGxTab);

		AddPage(static_cast<wxWindow*>(pGxTab), pGxTab->GetName(), count == 0 ? true : false/*, m_ImageListSmall.GetBitmap(9)*/);

		count++;

		pChild = pChild->GetNext();
	}

    wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(application->GetCatalog());
	m_pSelection = pGxCatalogUI->GetSelection();
	m_pConnectionPointSelection = dynamic_cast<IConnectionPointContainer*>( m_pSelection );
	if(m_pConnectionPointSelection != NULL)
		m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(this);

	return true;
}

void wxGxTabView::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != -1)
		m_pConnectionPointSelection->Unadvise(m_ConnectionPointSelectionCookie);

	for(size_t i = 0; i < m_Tabs.size(); i++)
	{
		//RemovePage(0);
		//wxDELETE(m_Tabs[i]);
        m_Tabs[i]->Deactivate();
	}
}

void wxGxTabView::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
	if(nInitiator == GetId())
		return;

    int nSelTab = GetSelection();
    if(nSelTab < 0)
        return;
	wxASSERT(nSelTab >= 0 && nSelTab < m_Tabs.size());

	wxGxTab* pCurrTab = m_Tabs[nSelTab];
	//if(pCurrTab)
	//	pCurrTab->OnSelectionChanged(Selection, nInitiator);

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, ID_SELCHANGED);
    event.SetClientData(pCurrTab);
    event.SetExtraLong(nInitiator);
    ::wxPostEvent(this, event);

	//wxWindow* pCurrWnd = GetPage(nSelTab);
	//IGxView* pCurrView = dynamic_cast<IGxView*>(pCurrWnd);
	//if(pCurrView != NULL && pCurrView->GetName() != _("NoView"))
	//{
	//	if(pCurrView->Applies(Selection))
	//		return;
	//}

	//wxGxTab* pCurrTab = m_Tabs[nSelTab];
	//if(pCurrTab == NULL)
	//	return;
	//for(size_t i = 0; i < pCurrTab->GetCount(); i++)
	//{
	//	wxWindow* pWnd = pCurrTab->GetWindow(i);
	//	IGxView* pView = dynamic_cast<IGxView*>(pWnd);
	//	if(pView == NULL)
	//		continue;
	//	if(pView->Applies(Selection))
	//	{
	//		if(pCurrView != pView)
	//		{
	//			pCurrWnd->Hide();
	//			RemovePage(nSelTab);
	//			InsertPage(nSelTab, pWnd, pCurrTab->GetName(), true);
	//			pWnd->Show();
	//		}
	//		return;
	//	}
	//}
	////IConnectionPointContainer* pConnectionPointContainer = dynamic_cast<IConnectionPointContainer*>( m_pSelection );
}

void wxGxTabView::OnAUINotebookPageChanged(wxAuiNotebookEvent& event)
{
	event.Skip();
    SetFocus();
	//update view while changing focus of tabs
	int nSelTab = event.GetSelection();
    if(nSelTab < 0)
        return;
	wxASSERT(nSelTab >= 0 && nSelTab < m_Tabs.size());

	wxGxTab* pCurrTab = m_Tabs[nSelTab];
	//if(pCurrTab && m_pSelection)
	//	pCurrTab->OnSelectionChanged(m_pSelection, IGxSelection::INIT_ALL);
    wxCommandEvent cevent(wxEVT_COMMAND_BUTTON_CLICKED, ID_SELCHANGED);
    cevent.SetExtraLong(IGxSelection::INIT_ALL);
    cevent.SetClientData(pCurrTab);
    ::wxPostEvent(this, cevent);
}

wxWindow* wxGxTabView::GetCurrentWnd(void)
{
	int nSelTab = GetSelection();
	wxASSERT(nSelTab >= 0 && nSelTab < m_Tabs.size());

	wxGxTab* pCurrTab = m_Tabs[nSelTab];
	if(pCurrTab)
		return pCurrTab->GetCurrentWindow();
	return NULL;
}

void wxGxTabView::OnSelChanged(wxCommandEvent & event)
{
	//int nSelTab = GetSelection();
 //   if(nSelTab < 0)
 //       return;
	//wxASSERT(nSelTab >= 0 && nSelTab < m_Tabs.size());

	//wxGxTab* pCurrTab = m_Tabs[nSelTab];
	//if(pCurrTab)
	//	pCurrTab->OnSelectionChanged(m_pSelection, event.GetExtraLong());
    
    wxGxTab* pCurrTab = (wxGxTab*)event.GetClientData();
    //for(size_t i = 0; i < m_Tabs.size(); i++)
    //    if(m_Tabs[i])
    //        m_Tabs[i]->OnSelectionChanged(m_pSelection, event.GetExtraLong());
	if(pCurrTab)
		pCurrTab->OnSelectionChanged(m_pSelection, event.GetExtraLong());
}