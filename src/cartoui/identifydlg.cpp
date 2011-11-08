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

#include "../../art/splitter_switch.xpm"
#include "../../art/layers.xpm"
#include "../../art/layer16.xpm"
#include "../../art/id.xpm"
//-------------------------------------------------------------------
// wxGISIdentifyDlg
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISIdentifyDlg, wxPanel)

BEGIN_EVENT_TABLE(wxGISIdentifyDlg, wxPanel)
	EVT_BUTTON(wxGISIdentifyDlg::ID_SWITCHSPLIT, wxGISIdentifyDlg::OnSwitchSplit)
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
	m_pNullPane = new wxPanel(m_splitter);
	m_splitter->SplitVertically(m_pTreeCtrl, m_pNullPane, 100);

	this->SetSizer( m_bMainSizer );
	this->Layout();

    return true;
}

wxGISIdentifyDlg::~wxGISIdentifyDlg()
{
}

void wxGISIdentifyDlg::OnSwitchSplit(wxCommandEvent& event)
{
	wxSplitMode eMode = m_splitter->GetSplitMode();
	int nSplitPos = m_splitter->GetSashPosition();
	m_splitter->Unsplit(m_pTreeCtrl);
	m_splitter->Unsplit(m_pNullPane);
	if(eMode == wxSPLIT_HORIZONTAL)
		eMode = wxSPLIT_VERTICAL;
	else
		eMode = wxSPLIT_HORIZONTAL;

	switch(eMode)
	{
	case wxSPLIT_HORIZONTAL:
		m_bpSplitButton->SetBitmap(m_BmpHorz);
		m_splitter->SplitHorizontally(m_pTreeCtrl, m_pNullPane, nSplitPos);
		break;
	case wxSPLIT_VERTICAL:
		m_bpSplitButton->SetBitmap(m_BmpVert);
		m_splitter->SplitVertically(m_pTreeCtrl, m_pNullPane, nSplitPos);
		break;
	};
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
	m_splitter->Unsplit(m_pNullPane);
	switch(eMode)
	{
	case wxSPLIT_HORIZONTAL:
		m_bpSplitButton->SetBitmap(m_BmpHorz);
		m_splitter->SplitHorizontally(m_pTreeCtrl, m_pNullPane, nSplitPos);
		break;
	case wxSPLIT_VERTICAL:
		m_bpSplitButton->SetBitmap(m_BmpVert);
		m_splitter->SplitVertically(m_pTreeCtrl, m_pNullPane, nSplitPos);
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


void wxAxIdentifyView::Identify(const OGREnvelope &Bounds)
{
	wxBusyCursor wait;
	if(!m_pMapView)
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
			wxGISQuadTreeCursorSPtr pCursor = pFLayer->Idetify(EnvelopeToGeometry(Bounds));
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
	if(pCursor->GetCount() < 1)
		return;

	//add root
	wxTreeItemId nRootId = m_pTreeCtrl->AddRoot(wxT("Layers"), 0);
	m_pTreeCtrl->SetItemBold(nRootId);
	//add layers
	wxTreeItemId nLayerId = m_pTreeCtrl->AppendItem(nRootId, pFLayer->GetName(), 1);
    for(size_t i = 0; i < pCursor->GetCount(); ++i)
    {
        wxGISQuadTreeItem* pItem = pCursor->at(i);
        if(!pItem)
            continue;
		m_pTreeCtrl->AppendItem(nLayerId, wxString::Format(wxT("%d"), pItem->GetOID()), 2);//TODO: store layer pointer & OID in user data
    }
	m_pTreeCtrl->ExpandAllChildren(nLayerId);
}

