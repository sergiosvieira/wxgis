/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTableView class.
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
#include "wxgis/carto/tableview.h"
#include "../../art/db2.xpm"

#include "wx/imaglist.h"
#include "wx/renderer.h"

//------------------------------------------------------------------
// wxGISTable
//------------------------------------------------------------------

wxGISTable::wxGISTable(wxGISDataset* pwxGISDataset) : m_pwxGISDataset(NULL)
{
	m_pwxGISDataset = dynamic_cast<wxGISFeatureDataset*>(pwxGISDataset);
    //m_pwxGISDataset->Reference();
	OGRLayer* pLayer = m_pwxGISDataset->GetLayerRef(0);
    if(pLayer)
    {
	    m_sFIDKeyName = wgMB2WX(pLayer->GetFIDColumn());

	    m_pOGRFeatureDefn = pLayer->GetLayerDefn();
	    nCols = m_pOGRFeatureDefn->GetFieldCount();
	    nRows = m_pwxGISDataset->GetSize();
    }
    else
    {
        nCols = 0;
        nRows = 0;
    }
}

wxGISTable::~wxGISTable()
{
	wsDELETE(m_pwxGISDataset);
}

int wxGISTable::GetNumberCols()
{
	return nCols;
}


int wxGISTable::GetNumberRows()
{
    return nRows;
}

wxString wxGISTable::GetValue(int row, int col)
{
	if(GetNumberCols() <= col || GetNumberRows() <= row)
		return wxEmptyString;

	//fetch more data
	wxBusyCursor wait;
	return m_pwxGISDataset->GetAsString(row, col);
}

void wxGISTable::SetValue(int row, int col, const wxString &value)
{
}

wxString wxGISTable::GetColLabelValue(int col)
{
    wxString label;
	OGRFieldDefn* pOGRFieldDefn = m_pOGRFeatureDefn->GetFieldDefn(col);
	label = wgMB2WX(pOGRFieldDefn->GetNameRef());
	if(!m_sFIDKeyName.IsEmpty())
	{
		if(label == m_sFIDKeyName);
			label += _(" [*]");
	}
    return label;
}

wxString wxGISTable::GetRowLabelValue(int row)
{
	return wxEmptyString;
}

//-------------------------------------
// wxGridCtrl
//-------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGridCtrl, wxGrid);

BEGIN_EVENT_TABLE(wxGridCtrl, wxGrid)
    EVT_GRID_LABEL_LEFT_CLICK(wxGridCtrl::OnLabelLeftClick)
    EVT_GRID_SELECT_CELL(wxGridCtrl::OnSelectCell)
END_EVENT_TABLE();

wxGridCtrl::wxGridCtrl()
{
	m_pImageList = new wxImageList(16, 16);
	m_pImageList->Add(wxBitmap(db2_xpm));
}

wxGridCtrl::wxGridCtrl(wxWindow* parent, const long& id) :
    wxGrid(parent,id,wxDefaultPosition,wxDefaultSize)
{
	m_pImageList = new wxImageList(16, 16);
	m_pImageList->Add(wxBitmap(db2_xpm));
}

wxGridCtrl::~wxGridCtrl(void)
{
	wxDELETE(m_pImageList);
}

void wxGridCtrl::DrawRowLabel(wxDC& dc, int row)
{
    if (GetRowHeight(row) <= 0 || m_rowLabelWidth <= 0)
        return;
    wxRect rect;
#ifdef __WXGTK20__
    rect.SetX(1);
    rect.SetY(GetRowTop(row) + 1);
    rect.SetWidth(m_rowLabelWidth - 2);
    rect.SetHeight(GetRowHeight(row) - 2);
    CalcScrolledPosition(0, rect.y, NULL, &rect.y);
    wxWindowDC *win_dc = (wxWindowDC*)&dc;
    wxRendererNative::Get().DrawHeaderButton(win_dc->m_owner, dc, rect, 0);
#else
    int rowTop = GetRowTop(row), rowBottom = GetRowBottom(row) - 1;
    dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID));
    dc.DrawLine(m_rowLabelWidth - 1, rowTop, m_rowLabelWidth - 1, rowBottom);
    dc.DrawLine(0, rowTop, 0, rowBottom);
    dc.DrawLine(0, rowBottom, m_rowLabelWidth, rowBottom);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(1, rowTop, 1, rowBottom);
    dc.DrawLine(1, rowTop, m_rowLabelWidth - 1, rowTop);
#endif
    if (row == GetGridCursorRow())
	{
		dc.DrawBitmap(m_pImageList->GetBitmap(4), 0, GetRowTop(row), true);
    }
}

void wxGridCtrl::OnLabelLeftClick(wxGridEvent& event)
{
    if (event.GetRow() != -1)
	{
        SetGridCursor(event.GetRow(),0);
    }
    event.Skip();
}

void wxGridCtrl::OnSelectCell(wxGridEvent& event)
{
    GetGridRowLabelWindow()->Refresh();
    event.Skip();
}

//-------------------------------------
// wxGISTableView
//-------------------------------------

#define BITBUTTONSIZE 18

BEGIN_EVENT_TABLE(wxGISTableView, wxPanel)
    EVT_GRID_LABEL_LEFT_CLICK(wxGISTableView::OnLabelLeftClick)
    EVT_GRID_SELECT_CELL(wxGISTableView::OnSelectCell)
	EVT_BUTTON(wxGISTableView::ID_FIRST, wxGISTableView::OnBtnFirst)
	EVT_BUTTON(wxGISTableView::ID_NEXT, wxGISTableView::OnBtnNext)
	EVT_BUTTON(wxGISTableView::ID_PREV, wxGISTableView::OnBtnPrev)
	EVT_BUTTON(wxGISTableView::ID_LAST, wxGISTableView::OnBtnLast)
	EVT_TEXT_ENTER(wxGISTableView::ID_POS, wxGISTableView::OnSetPos)
END_EVENT_TABLE();

wxGISTableView::wxGISTableView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_grid = new wxGridCtrl( this, wxID_ANY);

	// Grid
	m_grid->CreateGrid( 5, 5 );
	m_grid->EnableEditing( false );
	m_grid->EnableGridLines( true );
	m_grid->EnableDragGridSize( false );
	m_grid->SetMargins( 0, 0 );

	// Columns
	m_grid->EnableDragColMove( false );
	m_grid->EnableDragColSize( true );
	m_grid->SetColLabelSize( 20 );
	m_grid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

	// Rows
	m_grid->EnableDragRowSize( true );
	m_grid->SetRowLabelSize( 15 );
	m_grid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

	// Label Appearance

	// Cell Defaults
	m_grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	//m_grid->SetDefaultCellBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	bSizerMain->Add( m_grid, 1, wxALL|wxEXPAND, 0 );

	wxBoxSizer* bSizerLow;
	bSizerLow = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Record:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizerLow->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpFirst = new wxBitmapButton( this, wxGISTableView::ID_FIRST, m_grid->m_pImageList->GetBitmap(0), wxDefaultPosition, wxSize( BITBUTTONSIZE,BITBUTTONSIZE ), wxBU_AUTODRAW );
	m_bpFirst->SetMinSize( wxSize( BITBUTTONSIZE,BITBUTTONSIZE ) );
	m_bpFirst->SetMaxSize( wxSize( BITBUTTONSIZE,BITBUTTONSIZE ) );

	bSizerLow->Add( m_bpFirst, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );

	m_bpPrev = new wxBitmapButton( this, wxGISTableView::ID_PREV, m_grid->m_pImageList->GetBitmap(1), wxDefaultPosition, wxSize( BITBUTTONSIZE,BITBUTTONSIZE ), wxBU_AUTODRAW );
	m_bpPrev->SetMinSize( wxSize( BITBUTTONSIZE,BITBUTTONSIZE ) );
	m_bpPrev->SetMaxSize( wxSize( BITBUTTONSIZE,BITBUTTONSIZE ) );

	bSizerLow->Add( m_bpPrev, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );

	m_position = new wxTextCtrl( this, wxGISTableView::ID_POS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_PROCESS_ENTER  );
	bSizerLow->Add( m_position, 0, wxALL, 5 );

	m_bpNext = new wxBitmapButton( this, wxGISTableView::ID_NEXT, m_grid->m_pImageList->GetBitmap(2), wxDefaultPosition, wxSize( BITBUTTONSIZE,BITBUTTONSIZE ), wxBU_AUTODRAW );
	m_bpNext->SetMinSize( wxSize( BITBUTTONSIZE,BITBUTTONSIZE ) );
	m_bpNext->SetMaxSize( wxSize( BITBUTTONSIZE,BITBUTTONSIZE ) );

	bSizerLow->Add( m_bpNext, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );

	m_bpLast = new wxBitmapButton( this, wxGISTableView::ID_LAST, m_grid->m_pImageList->GetBitmap(3), wxDefaultPosition, wxSize( BITBUTTONSIZE,BITBUTTONSIZE ), wxBU_AUTODRAW );
	m_bpLast->SetMinSize( wxSize( BITBUTTONSIZE,BITBUTTONSIZE ) );
	m_bpLast->SetMaxSize( wxSize( BITBUTTONSIZE,BITBUTTONSIZE ) );

	bSizerLow->Add( m_bpLast, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("of"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizerLow->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizerMain->Add( bSizerLow, 0, wxEXPAND, 5 );

	this->SetSizer( bSizerMain );

	(*m_position) << 1;

	this->Layout();
}

wxGISTableView::~wxGISTableView(void)
{
}

void wxGISTableView::OnLabelLeftClick(wxGridEvent& event)
{
    event.Skip();
}

void wxGISTableView::OnSelectCell(wxGridEvent& event)
{
	m_position->Clear();
	(*m_position) << event.GetRow() + 1;

    event.Skip();
}

void wxGISTableView::OnBtnFirst(wxCommandEvent& event)
{
	m_grid->SetGridCursor(0,0);
	m_grid->MakeCellVisible(0,0);
	m_position->Clear();
	(*m_position) << 1;
}

void wxGISTableView::OnBtnNext(wxCommandEvent& event)
{
	m_grid->MoveCursorDown(false);
	m_position->Clear();
	(*m_position) << m_grid->GetGridCursorRow() + 1;
}

void wxGISTableView::OnBtnPrev(wxCommandEvent& event)
{
	m_grid->MoveCursorUp(false);
	m_position->Clear();
	(*m_position) << m_grid->GetGridCursorRow() + 1;
}

void wxGISTableView::OnBtnLast(wxCommandEvent& event)
{
	m_grid->SetGridCursor(m_grid->GetNumberRows() - 1,0);
	m_grid->MakeCellVisible(m_grid->GetNumberRows() - 1,0);
	m_position->Clear();
	(*m_position) << m_grid->GetNumberRows();
}

void wxGISTableView::OnSetPos(wxCommandEvent& event)
{
	long pos = wxAtol(event.GetString());
	m_grid->SetGridCursor(pos - 1,0);
	m_grid->MakeCellVisible(pos - 1,0);
}
