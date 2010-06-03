/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxGxTasksView class.
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

#include "wxgis/geoprocessingui/gptasksview.h"
#include "wxgis/framework/progressor.h"

//#include "wx/richtext/richtextctrl.h"

#include "../../art/tool_16.xpm"
#include "../../art/close_16a.xpm"
#include "../../art/expand_16.xpm"
#include "../../art/state_16.xpm"

//////////////////////////////////////////////////////////////////
// wxGxTaskPanel
//////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGxTaskPanel, wxPanel)
    EVT_BUTTON(wxID_MORE, wxGxTaskPanel::OnExpand)
END_EVENT_TABLE()

wxGxTaskPanel::wxGxTaskPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style), m_bExpand(false)
{
    m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(state_16_xpm));

    m_bMainSizer = new wxBoxSizer( wxVERTICAL );
 	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer( 1, 4, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
    wxStaticBitmap* pStateBitmap = new wxStaticBitmap( this, wxID_ANY, m_ImageList.GetIcon(4), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( pStateBitmap, 0, wxALL, 5 );
	
    wxStaticText * title = new wxStaticText(this, wxID_ANY, _("Tool name"));
    wxFont titleFont = this->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    title->Wrap( -1 );
	fgSizer1->Add( title, 1, wxALL | wxEXPAND, 5 );


    m_ExpandBitmap = wxBitmap(expand_16_xpm);
    wxImage bwImage = m_ExpandBitmap.ConvertToImage();
    m_ExpandBitmapRotated = wxBitmap(bwImage.Mirror(false));
    m_ExpandBitmapBW = wxBitmap(bwImage.ConvertToGreyscale());
    m_ExpandBitmapBWRotated = wxBitmap(bwImage.ConvertToGreyscale().Mirror(false));
    m_bpExpandButton = new wxBitmapButton( this, wxID_MORE, m_ExpandBitmapBW, wxDefaultPosition, wxDefaultSize, 0 );
//    m_bpExpandButton->SetBackgroundColour(bgColour);
    m_bpExpandButton->SetBitmapDisabled(m_ExpandBitmapBW);
    m_bpExpandButton->SetBitmapLabel(m_ExpandBitmapBW);
    m_bpExpandButton->SetBitmapSelected(m_ExpandBitmap);
    m_bpExpandButton->SetBitmapHover(m_ExpandBitmap);
    //wxBitmapButton* bpExpandButton = new wxBitmapButton( this, wxID_MORE, wxBitmap(expand_16_xpm), wxDefaultPosition, wxDefaultSize, 0 );
    m_bpExpandButton->SetToolTip(_("Expand"));
	fgSizer1->Add( m_bpExpandButton, 0, wxALL, 5 );

    wxBitmap NormalCBitmap = wxBitmap(close_16a_xpm);
    bwImage = NormalCBitmap.ConvertToImage();
    wxBitmap bwCBitmap = bwImage.ConvertToGreyscale();
    wxBitmapButton* bpCloseButton = new wxBitmapButton( this, wxID_CLOSE, bwCBitmap, wxDefaultPosition, wxDefaultSize, 0 );
//    bpCloseButton->SetBackgroundColour(bgColour);
    bpCloseButton->SetBitmapDisabled(bwCBitmap);
    bpCloseButton->SetBitmapLabel(bwCBitmap);
    bpCloseButton->SetBitmapSelected(NormalCBitmap);
    bpCloseButton->SetBitmapHover(NormalCBitmap);
    //wxBitmapButton* bpCloseButton = new wxBitmapButton( this, wxID_CLOSE, wxBitmap(close_16a_xpm), wxDefaultPosition, wxDefaultSize, 0 );
    bpCloseButton->SetToolTip(_("Close"));
    fgSizer1->Add( bpCloseButton, 0, wxALL, 5 );

    m_bMainSizer->Add(fgSizer1, 0 ,wxEXPAND/* | wxBOTTOM */| wxLEFT | wxRIGHT, 5);

/*  	wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    wxStaticText * text = new wxStaticText(this, wxID_ANY, _("status message"));// very long to fit in control may be and try again status message very long to fit in control may be, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE
    text->Wrap( -1 );
    fgSizer2->Add(text, 0, wxEXPAND | wxALL, 5);
 
    wxGISProgressor* pProgressor = new wxGISProgressor(this);
    pProgressor->SetInitialSize(wxSize(130, 18));
    fgSizer2->Add(pProgressor, 0, wxALL|wxEXPAND, 5);

    bMainSizer->Add(fgSizer2, 0 ,wxEXPAND| wxLEFT | wxRIGHT, 5);

*/
    wxStaticText * text = new wxStaticText(this, wxID_ANY, _("status message"));// very long to fit in control may be and try again status message very long to fit in control may be, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE
    text->Wrap( -1 );
    m_bMainSizer->Add(text, 0, wxEXPAND | wxALL, 5);

    wxGISProgressor* pProgressor = new wxGISProgressor(this);
    pProgressor->SetInitialSize(wxSize(-1, 18));
    m_bMainSizer->Add(pProgressor, 0, wxALL|wxEXPAND, 5);

/*     wxGISProgressor* pProgressor = new wxGISProgressor(this);
    pProgressor->SetInitialSize(wxSize(-1, 18));
    bMainSizer->Add(pProgressor, 0, wxALL|wxEXPAND, 5);
*/
    wxCheckBox* pChkBox = new wxCheckBox(this, wxID_ANY, _("Close when complited successfully"));
    m_bMainSizer->Add(pChkBox, 0, /*wxEXPAND | */wxALL, 5);

//    wxRichTextCtrl* pRichTextCtrl = new wxRichTextCtrl(this, wxID_ANY);
    m_pHtmlWindow = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_DEFAULT_STYLE | wxBORDER_THEME);
    m_pHtmlWindow->SetInitialSize(wxSize(-1, 255));
    m_bMainSizer->Add(m_pHtmlWindow, 0, wxEXPAND | wxALL, 5);
    m_pHtmlWindow->Show(false);

   	this->SetSizer( m_bMainSizer );
	this->Layout();

 	//this->SetSizeHints( wxDefaultSize, wxDefaultSize );
   //m_bMainSizer->SetSizeHints( this );
}

wxGxTaskPanel::~wxGxTaskPanel(void)
{
}

void wxGxTaskPanel::OnExpand(wxCommandEvent & event)
{
    m_bExpand = !m_bExpand;

    m_bpExpandButton->SetBitmapDisabled(m_bExpand == false ? m_ExpandBitmapBW : m_ExpandBitmapBWRotated);
    m_bpExpandButton->SetBitmapLabel(m_bExpand == false ? m_ExpandBitmapBW : m_ExpandBitmapBWRotated);
    m_bpExpandButton->SetBitmapSelected(m_bExpand == false ? m_ExpandBitmap : m_ExpandBitmapRotated);
    m_bpExpandButton->SetBitmapHover(m_bExpand == false ? m_ExpandBitmap : m_ExpandBitmapRotated);

    m_pHtmlWindow->Show(m_bExpand);
    wxSize Size = GetSize();
    Fit();
    wxSize NewSize = GetSize();
    SetSize(wxSize(Size.x, NewSize.y));
    GetParent()->FitInside();
}

//////////////////////////////////////////////////////////////////
// wxGxTasksView
//////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGxTasksView, wxScrolledWindow)
END_EVENT_TABLE()

wxGxTasksView::wxGxTasksView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : 
wxScrolledWindow(parent, id, pos, size, style)
{
    m_sViewName = wxString(_("Tasks"));
    m_Icon = wxIcon(tool_16_xpm);

    wxBoxSizer* bMainSizer = new wxBoxSizer( wxVERTICAL );
	wxGxTaskPanel* pTaskPanel1 = new wxGxTaskPanel( this, wxID_ANY, wxDefaultPosition, wxSize(size.x, 100));//wxDefaultSize
    //pTaskPanel->SetBackgroundColour(*wxWHITE);
    bMainSizer->Add( pTaskPanel1, 0, wxEXPAND, 5 );
	wxGxTaskPanel* pTaskPanel2 = new wxGxTaskPanel( this, wxID_ANY, wxDefaultPosition, wxSize(size.x, 100));
    bMainSizer->Add( pTaskPanel2, 0, wxEXPAND, 5 );

    SetScrollbars(20, 20, 50, 50);

  	this->SetSizer( bMainSizer );
	this->Layout();
}

wxGxTasksView::~wxGxTasksView(void)
{
}

bool wxGxTasksView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	wxGxView::Activate(application, pConf);

	return true;
}

void wxGxTasksView::Deactivate(void)
{
	wxGxView::Deactivate();
}

