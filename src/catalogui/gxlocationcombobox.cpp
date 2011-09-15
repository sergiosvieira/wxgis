/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxLocationComboBox class
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
#include "wxgis/catalogui/gxlocationcombobox.h"
#include "wxgis/catalogui/gxcatalogui.h"

BEGIN_EVENT_TABLE(wxGxLocationComboBox, wxComboBox)
	EVT_TEXT_ENTER(wxID_ANY, wxGxLocationComboBox::OnTextEnter)
	EVT_COMBOBOX(wxID_ANY, wxGxLocationComboBox::OnTextEnter)
	EVT_GXSELECTION_CHANGED(wxGxLocationComboBox::OnSelectionChanged)
END_EVENT_TABLE()

wxGxLocationComboBox::wxGxLocationComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name) : wxComboBox(parent, id, value, pos, size, choices, style, validator, name), m_pApp(NULL), m_pGxCatalogUI(NULL), m_pConnectionPointSelection(NULL), m_ConnectionPointSelectionCookie(wxNOT_FOUND)
{
}

wxGxLocationComboBox::~wxGxLocationComboBox(void)
{
}

void wxGxLocationComboBox::OnTextEnter(wxCommandEvent& event)
{
	wxGxApplication* pGxApp = dynamic_cast<wxGxApplication*>(m_pApp);
	if(pGxApp)
	{
		wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
		if(pCatalog)
		{
			pCatalog->SetLocation(GetValue());
			for(size_t i = 0; i < m_ValuesArr.size(); ++i)
				if(m_ValuesArr[i] == GetValue())
					return;
			Append(GetValue());
			m_ValuesArr.push_back(GetValue());
		}
	}
}

void wxGxLocationComboBox::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(event.GetInitiator() != TREECTRLID || !event.GetSelection())
		return;
    IGxObjectSPtr pGxObject = m_pGxCatalogUI->GetRegisterObject(event.GetSelection()->GetLastSelectedObjectID());
    if(!pGxObject)
        return;
	wxString sPath = pGxObject->GetFullName();
	if(sPath.IsEmpty())
		sPath = pGxObject->GetName();
	SetValue(sPath);
    wxFrame* pFrame = dynamic_cast<wxFrame*>(m_pApp);
    if(pFrame)
    {
        pFrame->SetTitle(m_pApp->GetAppName() + wxT(" - [") + sPath + wxT("]"));
    }
}

void wxGxLocationComboBox::Activate(IFrameApplication* pApp)
{
	m_pApp = pApp;
	wxGxApplication* pGxApp = dynamic_cast<wxGxApplication*>(pApp);
    m_pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
	if(m_pGxCatalogUI)
	{
		m_pConnectionPointSelection = dynamic_cast<wxGISConnectionPointContainer*>( m_pGxCatalogUI->GetSelection() );
		if(m_pConnectionPointSelection)
			m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(this);
	}
}

void wxGxLocationComboBox::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != wxNOT_FOUND)
		m_pConnectionPointSelection->Unadvise(m_ConnectionPointSelectionCookie);
}

