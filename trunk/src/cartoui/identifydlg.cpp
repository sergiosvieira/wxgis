/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISIdentifyDlg class - dialog/dock window with the results of identify.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/cartoui/identifydlg.h"
#include "wxgis/datasource/vectorop.h"
#include "wxgis/core/config.h"

#include "ogr_api.h"

#include <wx/clipbrd.h> 

#include "../../art/splitter_switch.xpm"
#include "../../art/layers.xpm"
#include "../../art/layer16.xpm"
#include "../../art/id.xpm"
#include "../../art/small_arrow.xpm"

int wxCALLBACK FieldValueCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
    LPFIELDSORTDATA psortdata = (LPFIELDSORTDATA)sortData;
    if(psortdata->nSortAsc == 0)
    {
        return item1 - item2;
    }
    else
    {
        CPLString str1, str2;
        if(psortdata->currentSortCol == 0)
        {
            str1 = CPLString(psortdata->pFeature->GetFieldDefnRef(item1)->GetNameRef());
            str2 = CPLString(psortdata->pFeature->GetFieldDefnRef(item2)->GetNameRef());
        }
        else
        {
            str1 = CPLString(psortdata->pFeature->GetFieldAsString(item1));
            str2 = CPLString(psortdata->pFeature->GetFieldAsString(item2));
        }
        return str1.compare(str2) * psortdata->nSortAsc;
    }
}
//-------------------------------------------------------------------
// wxGISFeatureDetailsPanel
//-------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISFeatureDetailsPanel, wxPanel)
    EVT_CONTEXT_MENU(wxGISFeatureDetailsPanel::OnContextMenu)
	EVT_MENU_RANGE(ID_WG_COPY_NAME, ID_WG_RESET_SORT, wxGISFeatureDetailsPanel::OnMenu)
 	EVT_UPDATE_UI_RANGE(ID_WG_COPY_NAME, ID_WG_RESET_SORT, wxGISFeatureDetailsPanel::OnMenuUpdateUI)
    EVT_LIST_COL_CLICK(ID_LISTCTRL, wxGISFeatureDetailsPanel::OnColClick)
	EVT_BUTTON(ID_MASKBTN, wxGISFeatureDetailsPanel::OnMaskMenu)
END_EVENT_TABLE()

wxGISFeatureDetailsPanel::wxGISFeatureDetailsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxImage img = wxBitmap(small_arrow_xpm).ConvertToImage();
	wxBitmap oDownArrow = img.Rotate90();
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Location:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_textCtrl = new wxTextCtrl(  this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxGenericValidator( &m_sLocation ));
	//m_textCtrl->Enable( false );

	fgSizer1->Add( m_textCtrl, 1, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_bpSelStyleButton = new wxBitmapButton( this, ID_MASKBTN, oDownArrow, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpSelStyleButton->SetToolTip( _("Select location text style") );
	
	fgSizer1->Add( m_bpSelStyleButton, 0, wxRIGHT, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_listCtrl = new wxListCtrl( this, ID_LISTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES );
	m_listCtrl->InsertColumn(0, _("Field"), wxLIST_FORMAT_LEFT, 90);
	m_listCtrl->InsertColumn(1, _("Value"), wxLIST_FORMAT_LEFT, 120);

	m_ImageListSmall.Create(16, 16);

    wxBitmap SmallA(small_arrow_xpm);// > arrow
    wxImage SmallImg = SmallA.ConvertToImage();
    SmallImg = SmallImg.Rotate90();
    wxBitmap SmallDown(SmallImg); 
    SmallImg = SmallImg.Mirror(false);
    wxBitmap SmallUp(SmallImg); 

	m_ImageListSmall.Add(wxBitmap(SmallDown));
	m_ImageListSmall.Add(wxBitmap(SmallUp));

	m_listCtrl->SetImageList(&m_ImageListSmall, wxIMAGE_LIST_SMALL);

	bSizer1->Add( m_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	m_pCFormat = new wxGISCoordinatesFormatMenu();
	m_pCFormat->Create(wxString(wxT("X: dd.dddd[ ]Y: dd.dddd")));//TODO: get/store from/in config, set from property page

	m_pMenu = new wxMenu;
	m_pMenu->Append(ID_WG_COPY_NAME, wxString::Format(_("Copy %s"), _("Field")), wxString::Format(_("Copy '%s' value"), _("Field")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_COPY_VALUE, wxString::Format(_("Copy %s"), _("Value")), wxString::Format(_("Copy '%s' value"), _("Value")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_COPY, wxString(_("Copy")), wxString(_("Copy")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_HIDE, wxString(_("Hide")), wxString(_("Hide rows")), wxITEM_NORMAL);
	m_pMenu->AppendSeparator();
	m_pMenu->Append(ID_WG_RESET_SORT, wxString(_("Remove sort")), wxString(_("Remove sort")), wxITEM_NORMAL);

    m_currentSortCol = 0;
    m_nSortAsc = 0;
	m_dfX = 0;
	m_dfY =	0;
}

wxGISFeatureDetailsPanel::~wxGISFeatureDetailsPanel()
{
	wxDELETE(m_pMenu);
	wxDELETE(m_pCFormat);
}

void wxGISFeatureDetailsPanel::FillPanel(const OGRPoint &pt1)
{
	m_dfX = pt1.getX();
	m_dfY = pt1.getY();
	m_sLocation = m_pCFormat->Format(m_dfX, m_dfY);
	TransferDataToWindow();
}

void wxGISFeatureDetailsPanel::FillPanel(const OGRFeatureSPtr &pFeature)
{
	m_pFeature = pFeature;
	Clear();
	for(int i = 0; i < pFeature->GetFieldCount(); ++i)
	{
		if(m_anExcludeFields.Index(i) != wxNOT_FOUND)
			continue;
		OGRFieldDefn* pFieldDefn = pFeature->GetFieldDefnRef(i);
		if(!pFieldDefn)
			continue;
		wxString sName(pFieldDefn->GetNameRef(), wxConvLocal);
		wxString sValue(pFeature->GetFieldAsString(i), wxConvLocal);
		long pos = m_listCtrl->InsertItem(i, sName, wxNOT_FOUND);
		m_listCtrl->SetItem(pos, 1, sValue);
		m_listCtrl->SetItemData(pos, i);
	}
	//m_listCtrl->Update();
    FIELDSORTDATA sortdata = {m_nSortAsc, m_currentSortCol, m_pFeature};
	m_listCtrl->SortItems(FieldValueCompareFunction, (long)&sortdata);
}

void wxGISFeatureDetailsPanel::Clear(bool bFull)
{
	m_listCtrl->DeleteAllItems();
	if(bFull)
		m_anExcludeFields.Clear();
}

void wxGISFeatureDetailsPanel::OnContextMenu(wxContextMenuEvent& event)
{
	wxRect rc = m_listCtrl->GetRect();
	wxPoint point = event.GetPosition();
    // If from keyboard
    if (point.x == -1 && point.y == -1)
	{
        wxSize size = GetSize();
		point.x = rc.GetLeft() + rc.GetWidth() / 2;
		point.y = rc.GetTop() + rc.GetHeight() / 2;
    }
	else
	{
        point = m_listCtrl->ScreenToClient(point);
    }
	if(!rc.Contains(point))
	{
		event.Skip();
		return;
	}
    PopupMenu(m_pMenu, point.x, point.y);
}

void wxGISFeatureDetailsPanel::OnMaskMenu(wxCommandEvent& event)
{
	wxRect rc = m_bpSelStyleButton->GetRect();
	m_pCFormat->PrepareMenu();//fill new masks from config
	PopupMenu(m_pCFormat, rc.GetBottomLeft());
	m_sLocation = m_pCFormat->Format(m_dfX, m_dfY);
	TransferDataToWindow();
}

void wxGISFeatureDetailsPanel::WriteStringToClipboard(const wxString &sData)
{
	// Write some text to the clipboard
    if (wxTheClipboard->Open())
    {
        // This data objects are held by the clipboard,
        // so do not delete them in the app.
        wxTheClipboard->SetData( new wxTextDataObject(sData) );
        wxTheClipboard->Close();
    }
}

void wxGISFeatureDetailsPanel::OnMenu(wxCommandEvent& event)
{
    long nItem = wxNOT_FOUND;
	wxString sOutput;
	switch(event.GetId())
	{
	case ID_WG_COPY_NAME:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
			sOutput += m_listCtrl->GetItemText(nItem, 0);
			sOutput += wxT("\n");
		}
		WriteStringToClipboard(sOutput);
		break;
	case ID_WG_COPY_VALUE:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
			sOutput += m_listCtrl->GetItemText(nItem, 1);
			sOutput += wxT("\n");
		}
		WriteStringToClipboard(sOutput);
		break;
	case ID_WG_COPY:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
			sOutput += m_listCtrl->GetItemText(nItem, 0);
			sOutput += wxT("\t");
			sOutput += m_listCtrl->GetItemText(nItem, 1);
			sOutput += wxT("\n");
		}
		WriteStringToClipboard(sOutput);
		break;
	case ID_WG_HIDE:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
	        int nFieldNo = (int)m_listCtrl->GetItemData(nItem);
			m_anExcludeFields.Add(nFieldNo);
		}
		FillPanel(m_pFeature);
		break;
	case ID_WG_RESET_SORT:
        {
            m_nSortAsc = 0;

            wxListItem item;
            item.SetMask(wxLIST_MASK_IMAGE);

            //reset image
            item.SetImage(wxNOT_FOUND);
            for(size_t i = 0; i < m_listCtrl->GetColumnCount(); ++i)
                m_listCtrl->SetColumn(i, item);

            FIELDSORTDATA sortdata = {m_nSortAsc, m_currentSortCol, m_pFeature};
	        m_listCtrl->SortItems(FieldValueCompareFunction, (long)&sortdata);
        }
		break;
	default:
		break;
	}
}

void wxGISFeatureDetailsPanel::OnMenuUpdateUI(wxUpdateUIEvent& event)
{
	if(event.GetId() == ID_WG_RESET_SORT)
		return;
	if(m_listCtrl->GetSelectedItemCount() == 0)
		event.Enable(false);
	else
		event.Enable(true);
}

void wxGISFeatureDetailsPanel::OnColClick(wxListEvent& event)
{
    //event.Skip();
    m_currentSortCol = event.GetColumn();
    if(m_nSortAsc == 0)
        m_nSortAsc = 1;
    else
        m_nSortAsc *= -1;

    FIELDSORTDATA sortdata = {m_nSortAsc, m_currentSortCol, m_pFeature};
	m_listCtrl->SortItems(FieldValueCompareFunction, (long)&sortdata);

    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);

    //reset image
    item.SetImage(wxNOT_FOUND);
    for(size_t i = 0; i < m_listCtrl->GetColumnCount(); ++i)
        m_listCtrl->SetColumn(i, item);

    item.SetImage(m_nSortAsc == 1 ? 0 : 1);
    m_listCtrl->SetColumn(m_currentSortCol, item);
}


//-------------------------------------------------------------------
// wxGISIdentifyDlg
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISIdentifyDlg, wxPanel)

BEGIN_EVENT_TABLE(wxGISIdentifyDlg, wxPanel)
	EVT_BUTTON(wxGISIdentifyDlg::ID_SWITCHSPLIT, wxGISIdentifyDlg::OnSwitchSplit)
	EVT_TREE_SEL_CHANGED(wxGISIdentifyDlg::ID_WXGISTREECTRL, wxGISIdentifyDlg::OnSelChanged)
	EVT_MENU_RANGE(ID_WGMENU_FLASH, ID_WGMENU_ZOOM, wxGISIdentifyDlg::OnMenu)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_WXGISTREECTRL, wxGISIdentifyDlg::OnItemRightClick)
END_EVENT_TABLE()


wxGISIdentifyDlg::wxGISIdentifyDlg( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, pos, size, style);
}

wxGISIdentifyDlg::wxGISIdentifyDlg(void)
{
}

bool wxGISIdentifyDlg::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if(!wxPanel::Create( parent, id, pos, size, style, name ))
		return false;

	m_bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_fgTopSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	m_fgTopSizer->AddGrowableCol( 1 );
	m_fgTopSizer->SetFlexibleDirection( wxBOTH );
	m_fgTopSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Identify from:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_fgTopSizer->Add( m_staticText1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_LayerChoiceChoices;
	m_LayerChoiceChoices.Add(_("<Top layer>"));
	m_LayerChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_LayerChoiceChoices, 0 );
	m_LayerChoice->SetSelection( 0 );
	m_LayerChoice->Enable(false);
	m_fgTopSizer->Add( m_LayerChoice, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_BmpVert = wxBitmap(splitter_switch_xpm);
	wxImage oImg = m_BmpVert.ConvertToImage().Rotate90();
	m_BmpHorz = wxBitmap(oImg);
	//get splitter from conf
	m_bpSplitButton = new wxBitmapButton( this, ID_SWITCHSPLIT, m_BmpVert, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_fgTopSizer->Add( m_bpSplitButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_bMainSizer->Add( m_fgTopSizer, 0, wxEXPAND, 5 );

	m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISIdentifyDlg::SplitterOnIdle ), NULL, this );
	
	m_bMainSizer->Add( m_splitter, 1, wxEXPAND, 5 );

	m_TreeImageList.Create(16, 16);
	m_TreeImageList.Add(wxBitmap(layers_xpm));
	m_TreeImageList.Add(wxBitmap(layer16_xpm));
	m_TreeImageList.Add(wxBitmap(id_xpm));
	m_pTreeCtrl = new wxTreeCtrl( m_splitter, ID_WXGISTREECTRL, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT | wxSTATIC_BORDER );
	m_pTreeCtrl->SetImageList(&m_TreeImageList);

	m_splitter->SetSashGravity(0.5);
	m_pFeatureDetailsPanel = new wxGISFeatureDetailsPanel(m_splitter);
	m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, 100);

	this->SetSizer( m_bMainSizer );
	this->Layout();

	m_pMenu = new wxMenu;
	m_pMenu->Append(ID_WGMENU_FLASH, wxString(_("Flash")), wxString(_("Flash geometry")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WGMENU_PAN, wxString(_("Pan")), wxString(_("Pan to geometry center")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WGMENU_ZOOM, wxString(_("Zoom")), wxString(_("Zoom to geometry")), wxITEM_NORMAL);

    return true;
}

wxGISIdentifyDlg::~wxGISIdentifyDlg()
{
	wxDELETE(m_pMenu);
}

void wxGISIdentifyDlg::OnSwitchSplit(wxCommandEvent& event)
{
	wxSplitMode eMode = m_splitter->GetSplitMode();
	int nSplitPos = m_splitter->GetSashPosition();
	m_splitter->Unsplit(m_pTreeCtrl);
	m_splitter->Unsplit(m_pFeatureDetailsPanel);
	if(eMode == wxSPLIT_HORIZONTAL)
		eMode = wxSPLIT_VERTICAL;
	else
		eMode = wxSPLIT_HORIZONTAL;

	switch(eMode)
	{
	case wxSPLIT_HORIZONTAL:
		m_bpSplitButton->SetBitmap(m_BmpHorz);
		m_splitter->SplitHorizontally(m_pTreeCtrl, m_pFeatureDetailsPanel, nSplitPos);
		break;
	case wxSPLIT_VERTICAL:
		m_bpSplitButton->SetBitmap(m_BmpVert);
		m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, nSplitPos);
		break;
	};
}

void wxGISIdentifyDlg::OnSelChanged(wxTreeEvent& event)
{
    wxTreeItemId TreeItemId = event.GetItem();
    if(TreeItemId.IsOk())
    {
        wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
        if(pData == nullptr)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
		if(pData->m_nOID == wxNOT_FOUND)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
		m_pFeatureDetailsPanel->FillPanel(pData->m_pDataset->GetFeature(pData->m_nOID));
    }
}

void wxGISIdentifyDlg::OnMenu(wxCommandEvent& event)
{
}

void wxGISIdentifyDlg::OnItemRightClick(wxTreeEvent& event)
{
 	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
		return;
    m_pTreeCtrl->SelectItem(item);
    PopupMenu(m_pMenu, event.GetPoint());
}

//-------------------------------------------------------------------
// wxAxToolboxView
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxAxIdentifyView, wxGISIdentifyDlg)

wxAxIdentifyView::wxAxIdentifyView(void)
{
}

wxAxIdentifyView::wxAxIdentifyView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISIdentifyDlg(parent, id, pos, size, wxNO_BORDER | wxTAB_TRAVERSAL)
{
    Create(parent, id, pos, size);
}

wxAxIdentifyView::~wxAxIdentifyView(void)
{
}

bool wxAxIdentifyView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	m_pMapView = nullptr;
    m_sViewName = wxString(_("Identify"));
    return wxGISIdentifyDlg::Create(parent, id, pos, size, style, name);
}

bool wxAxIdentifyView::Activate(IFrameApplication* application, wxXmlNode* pConf)
{
    m_pApp = application;

	//get split from config and apply it
	m_pConf = pConf;
	wxSplitMode eMode = (wxSplitMode)wxAtoi(m_pConf->GetAttribute(wxT("split_mode"), wxT("2")));//wxSPLIT_VERTICAL = 2
	int nSplitPos = wxAtoi(m_pConf->GetAttribute(wxT("split_pos"), wxT("100")));

	//int w = wxAtoi(m_pConf->GetAttribute(wxT("width"), wxT("-1")));
	//int h = wxAtoi(m_pConf->GetAttribute(wxT("height"), wxT("-1")));
	//SetClientSize(w, h);

	m_splitter->Unsplit(m_pTreeCtrl);
	m_splitter->Unsplit(m_pFeatureDetailsPanel);
	switch(eMode)
	{
	case wxSPLIT_HORIZONTAL:
		m_bpSplitButton->SetBitmap(m_BmpHorz);
		m_splitter->SplitHorizontally(m_pTreeCtrl, m_pFeatureDetailsPanel, nSplitPos);
		break;
	case wxSPLIT_VERTICAL:
		m_bpSplitButton->SetBitmap(m_BmpVert);
		m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, nSplitPos);
		break;
	};
	return true;
}

void wxAxIdentifyView::Deactivate(void)
{
	//set split to config
	//int w, h;
	//GetClientSize(&w, &h);
	//if(m_pConf->HasAttribute(wxT("width")))
	//	m_pConf->DeleteAttribute(wxT("width"));
	//m_pConf->AddAttribute(wxT("width"), wxString::Format(wxT("%d"), w));
	//if(m_pConf->HasAttribute(wxT("height")))
	//	m_pConf->DeleteAttribute(wxT("height"));
	//m_pConf->AddAttribute(wxT("height"), wxString::Format(wxT("%d"), h));

	wxSplitMode eMode = m_splitter->GetSplitMode();
	int nSplitPos = m_splitter->GetSashPosition();
	if(m_pConf->HasAttribute(wxT("split_mode")))
		m_pConf->DeleteAttribute(wxT("split_mode"));
	m_pConf->AddAttribute(wxT("split_mode"), wxString::Format(wxT("%d"), eMode));
	if(m_pConf->HasAttribute(wxT("split_pos")))
		m_pConf->DeleteAttribute(wxT("split_pos"));
	m_pConf->AddAttribute(wxT("split_pos"), wxString::Format(wxT("%d"), nSplitPos));
}


void wxAxIdentifyView::Identify(OGRGeometrySPtr pGeometryBounds)
{
	wxBusyCursor wait;
	if(!m_pMapView)//TODO: add/remove layer map events connection point
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); ++i)
			{
				wxGISMapView* pMapView = dynamic_cast<wxGISMapView*>(pWinArr->at(i));
				if(pMapView)					
				{
					m_pMapView = pMapView;
                    break;
				}
			}
		}
	}
	if(!m_pMapView)
        return;	
	OGRSpatialReferenceSPtr pSpaRef = m_pMapView->GetSpatialReference();
	OGRPolygon* pRgn = (OGRPolygon*)pGeometryBounds.get();
	if(!pRgn)
		return;
	OGRLinearRing* pRing = pRgn->getExteriorRing();
	if(!pRing)
		return;
	OGRPoint pt1, pt2;
	pRing->getPoint(0, &pt1);
	pRing->getPoint(3, &pt2);
	//OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
    if(IsDoubleEquil(pt1.getX(), pt2.getX()) && IsDoubleEquil(pt1.getY(), pt2.getY()))
	{
		OGREnvelope Env;
 		wxGISAppConfigSPtr pConfig = GetConfig();
        double dfDelta = pConfig->ReadDouble(enumGISHKCU, wxString(wxT("wxGISCommon/math/delta")), 0.0000001);//EPSILON * 10000
		Env.MinX = pt1.getX() - dfDelta;
		Env.MinY = pt1.getY() - dfDelta;
		Env.MaxX = pt1.getX() + dfDelta;
		Env.MaxY = pt1.getY() + dfDelta;
		pGeometryBounds = EnvelopeToGeometry(Env, pSpaRef);
	}
	else
	{
		if(pSpaRef)
			pGeometryBounds->assignSpatialReference(pSpaRef->Clone());
	}
	//set spatial reference from mapview to geometry and/or pt1
	if(pSpaRef)
		pt1.assignSpatialReference(pSpaRef->Clone());
	//get top layer
	wxGISLayerSPtr pTopLayer = m_pMapView->GetLayer(m_pMapView->GetLayerCount() - 1);
	if(!pTopLayer)
		return;
	wxGISEnumDatasetType eType = pTopLayer->GetType();
	switch(eType)
	{
	case enumGISFeatureDataset:
		{
			wxGISFeatureLayerSPtr pFLayer = boost::dynamic_pointer_cast<wxGISFeatureLayer>(pTopLayer);
			if(!pFLayer)
				return;
			wxGISQuadTreeCursorSPtr pCursor = pFLayer->Idetify(pGeometryBounds);
			//flash on map
            GeometryArray Arr;
            for(size_t i = 0; i < pCursor->GetCount(); ++i)
            {
                wxGISQuadTreeItem* pItem = pCursor->at(i);
                if(!pItem)
                    continue;
                Arr.Add(pItem->GetGeometry());
            }
            m_pMapView->FlashGeometry(Arr);
            //fill IdentifyDlg
			m_pFeatureDetailsPanel->Clear(true);
			m_pFeatureDetailsPanel->FillPanel(pt1);
			FillTree(pFLayer, pCursor);
		}
		break;
	default:
		break;
	};
}

void wxAxIdentifyView::FillTree(wxGISFeatureLayerSPtr pFLayer, wxGISQuadTreeCursorSPtr pCursor)
{
	m_pTreeCtrl->DeleteAllItems();
	m_pFeatureDetailsPanel->Clear();
	if(pCursor->GetCount() < 1)
		return;

	//add root
	wxTreeItemId nRootId = m_pTreeCtrl->AddRoot(wxT("Layers"), 0);
	m_pTreeCtrl->SetItemBold(nRootId);
	//add layers
	wxTreeItemId nLayerId = m_pTreeCtrl->AppendItem(nRootId, pFLayer->GetName(), 1);
	m_pTreeCtrl->SetItemData(nLayerId, new wxIdentifyTreeItemData(pFLayer->GetDataset()));
	wxTreeItemId nFirstFeatureId = nLayerId;
    for(size_t i = 0; i < pCursor->GetCount(); ++i)
    {
        wxGISQuadTreeItem* pItem = pCursor->at(i);
        if(!pItem)
            continue;
		wxTreeItemId nFeatureId = m_pTreeCtrl->AppendItem(nLayerId, wxString::Format(wxT("%d"), pItem->GetOID()), 2);
		m_pTreeCtrl->SetItemData(nFeatureId, new wxIdentifyTreeItemData(pFLayer->GetDataset(), pItem->GetOID(), pItem->GetGeometry()));		
		if(i == 0)
			nFirstFeatureId = nFeatureId;
    }
	m_pTreeCtrl->ExpandAllChildren(nLayerId);
	m_pTreeCtrl->SelectItem(nFirstFeatureId);
}

void wxAxIdentifyView::OnSelChanged(wxTreeEvent& event)
{
    wxTreeItemId TreeItemId = event.GetItem();
    if(TreeItemId.IsOk())
    {
        wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
        if(pData == nullptr)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
		if(pData->m_nOID == wxNOT_FOUND)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
        GeometryArray Arr;
		Arr.Add(pData->m_pGeometry);
		m_pMapView->FlashGeometry(Arr);
		m_pFeatureDetailsPanel->FillPanel(pData->m_pDataset->GetFeature(pData->m_nOID));
    }
}

void wxAxIdentifyView::OnMenu(wxCommandEvent& event)
{
	wxTreeItemId TreeItemId = m_pTreeCtrl->GetSelection();
    wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
    if(pData == nullptr)
		return;

	switch(event.GetId())
	{
	case ID_WGMENU_FLASH:	
	{
        GeometryArray Arr;
		if(pData->m_pGeometry)
			Arr.Add(pData->m_pGeometry);
		else
		{
			wxTreeItemIdValue cookie;
			for ( wxTreeItemId item = m_pTreeCtrl->GetFirstChild(TreeItemId, cookie); item.IsOk(); item = m_pTreeCtrl->GetNextChild(TreeItemId, cookie) )
			{
				pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(item);
				if(pData)
					Arr.Add(pData->m_pGeometry);
			}
		}
		m_pMapView->FlashGeometry(Arr);
	}
	break;
	case ID_WGMENU_PAN:
	{
        GeometryArray Arr;
		if(pData->m_pGeometry)
			Arr.Add(pData->m_pGeometry);
		else
		{
			wxTreeItemIdValue cookie;
			for ( wxTreeItemId item = m_pTreeCtrl->GetFirstChild(TreeItemId, cookie); item.IsOk(); item = m_pTreeCtrl->GetNextChild(TreeItemId, cookie) )
			{
				pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(item);
				if(pData)
					Arr.Add(pData->m_pGeometry);
			}
		}
		OGREnvelope Env;
		for(size_t i = 0; i < Arr.GetCount(); ++i)
		{
			OGREnvelope TempEnv;
			Arr[i]->getEnvelope(&TempEnv);
			Env.Merge(TempEnv);
		}
		OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
		MoveEnvelope(CurrentEnv, Env);
		m_pMapView->Do(CurrentEnv);
		m_pMapView->FlashGeometry(Arr);
	}
	break;
	case ID_WGMENU_ZOOM:
	{
        GeometryArray Arr;
		if(pData->m_pGeometry)
			Arr.Add(pData->m_pGeometry);
		else
		{
			wxTreeItemIdValue cookie;
			for ( wxTreeItemId item = m_pTreeCtrl->GetFirstChild(TreeItemId, cookie); item.IsOk(); item = m_pTreeCtrl->GetNextChild(TreeItemId, cookie) )
			{
				pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(item);
				if(pData)
					Arr.Add(pData->m_pGeometry);
			}
		}
		OGREnvelope Env;
		for(size_t i = 0; i < Arr.GetCount(); ++i)
		{
			OGREnvelope TempEnv;
			Arr[i]->getEnvelope(&TempEnv);
			Env.Merge(TempEnv);
		}
		m_pMapView->Do(Env);
		m_pMapView->FlashGeometry(Arr);
	}
	break;
	default:
	break;
	}
}
