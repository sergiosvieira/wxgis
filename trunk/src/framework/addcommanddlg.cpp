/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  add command in command bar dialog.
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
#include "wxgis/framework/addcommanddlg.h"
#include <wx/accel.h>
#include <wx/utils.h>


BEGIN_EVENT_TABLE(wxGISAddCommandDlg, wxDialog)
	EVT_LISTBOX(wxGISAddCommandDlg::ID_LSTBX, wxGISAddCommandDlg::OnListboxSelect)
	EVT_SPLITTER_DCLICK(wxID_ANY, wxGISAddCommandDlg::OnDoubleClickSash)
	EVT_LIST_ITEM_ACTIVATED(wxGISAddCommandDlg::ID_LSTCTRL, wxGISAddCommandDlg::OnListctrlActivated)
	EVT_UPDATE_UI(wxID_OK, wxGISAddCommandDlg::OnUpdateOKUI)
	EVT_BUTTON(wxID_OK, wxGISAddCommandDlg::OnOk)
END_EVENT_TABLE()

wxGISAddCommandDlg::wxGISAddCommandDlg( wxGISApplication* pGxApp, wxWindow* parent,  wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_pGxApp = pGxApp;
	if(!m_pGxApp)
		return;

	this->SetSizeHints( wxSize( 540,400 ), wxDefaultSize );
	wxBoxSizer* bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_Splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxNO_BORDER );
	m_Splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISAddCommandDlg::SplitterOnIdle ), NULL, this );
	bSizer5->Add( m_Splitter, 1, wxEXPAND, 5 );

	//fill m_listBox1
	COMMANDARRAY* pArr = m_pGxApp->GetCommands();
	if(pArr)
	{
		for(size_t i = 0; i < pArr->size(); i++)
		{
			wxString sCat = pArr->at(i)->GetCategory();
			if(m_CategoryMap[sCat] == NULL)
				m_CategoryMap[sCat] = new COMMANDARRAY;
			m_CategoryMap[sCat]->push_back(pArr->at(i));
		}
	}

	wxArrayString CatArray;
	for(CATEGORYMAP::iterator IT = m_CategoryMap.begin(); IT != m_CategoryMap.end(); ++IT)
	{
		CatArray.Add(IT->first);
	}

	m_ListBox = new wxListBox( m_Splitter, wxGISAddCommandDlg::ID_LSTBX, wxDefaultPosition, wxDefaultSize, CatArray, wxLC_SINGLE_SEL|wxLC_SORT_ASCENDING  |wxNO_BORDER);

	m_ListCtrl = new wxListView( m_Splitter, wxGISAddCommandDlg::ID_LSTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SORT_ASCENDING | wxNO_BORDER );
	m_ListCtrl->InsertColumn(0, _("Command Name"), wxLIST_FORMAT_LEFT, 90);
	m_ListCtrl->InsertColumn(1, _("Description"), wxLIST_FORMAT_LEFT, 120);
	m_ListCtrl->InsertColumn(2, _("KeyCode"), wxLIST_FORMAT_LEFT, 60);
	m_ImageList.Create(16, 16);
	m_ListCtrl->SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	m_Splitter->SetSashGravity(0.5);
	m_Splitter->SplitVertically(m_ListBox, m_ListCtrl, 150);

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK, wxString(_("OK")) );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL, wxString(_("Cancel")));
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();

	bSizer5->Add( m_sdbSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( bSizer5 );
	this->Layout();

	m_ListBox->Select(0);
	wxCommandEvent event;
	event.SetString(m_ListBox->GetString(0));//CatArray[0]
	OnListboxSelect(event);
}

wxGISAddCommandDlg::~wxGISAddCommandDlg()
{
	for(CATEGORYMAP::iterator IT = m_CategoryMap.begin(); IT != m_CategoryMap.end(); ++IT)
	{
		wxDELETE(IT->second);
	}
}

void wxGISAddCommandDlg::OnListboxSelect(wxCommandEvent& event)
{
	int selpos = m_ListBox->GetSelection();
	if(selpos == wxNOT_FOUND)
		return;

	wxString selName = m_ListBox->GetString(selpos);
	COMMANDARRAY* pArr = m_CategoryMap[selName];
	if(pArr != NULL)
	{
		m_ListCtrl->DeleteAllItems();
		m_ImageList.RemoveAll();
		for(size_t i = 0; i < pArr->size(); i++)
		{
			wxString sName = wxStripMenuCodes(pArr->at(i)->GetCaption());
			wxString sMessage = pArr->at(i)->GetMessage();
			wxString sKeyCode = m_pGxApp->GetGISAcceleratorTable()->GetText(pArr->at(i)->GetID());
			int nIndex = m_ImageList.Add(pArr->at(i)->GetBitmap());
			long pos = m_ListCtrl->InsertItem(i, sName, nIndex);
			m_ListCtrl->SetItem(pos, 1, sMessage);
			m_ListCtrl->SetItem(pos, 2, sKeyCode);
			m_ListCtrl->SetItemData(pos,  pArr->at(i)->GetID());
		}
		m_ListCtrl->Update();
	}
}

void wxGISAddCommandDlg::OnDoubleClickSash(wxSplitterEvent& event)
{
	event.Veto();
}

void wxGISAddCommandDlg::OnListctrlActivated(wxListEvent& event)
{
	FillIDArray();
	EndModal(wxID_OK);
}

void wxGISAddCommandDlg::OnUpdateOKUI(wxUpdateUIEvent& event)
{
	if(m_ListCtrl->GetSelectedItemCount() > 0)
		event.Enable(true);
	else
		event.Enable(false);
}

void wxGISAddCommandDlg::OnOk(wxCommandEvent& event)
{
	FillIDArray();
	EndModal(wxID_OK);
}

void wxGISAddCommandDlg::FillIDArray(void)
{
	long selpos = m_ListCtrl->GetFirstSelected();
	while(selpos != wxNOT_FOUND)
	{
		long ID = m_ListCtrl->GetItemData(selpos);
		m_IDArray.push_back(ID);
		selpos = m_ListCtrl->GetNextSelected(selpos);
	}
}

