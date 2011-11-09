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
#pragma once

#include "wxgis/cartoui/cartoui.h"
#include "wxgis/framework/framework.h"
#include "wxgis/cartoui/mapview.h"
#include "wxgis/carto/featurelayer.h"
#include "wxgis/core/format.h"

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/valgen.h>

/** \class wxGISFeatureDetailsPanel identifydlg.h
 *  \brief The wxGISFeatureDetailsPanel class show OGRFeature fields and values.
 */
class wxGISFeatureDetailsPanel : public wxPanel 
{
public:
	wxGISFeatureDetailsPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISFeatureDetailsPanel();
	virtual void FillPanel(const OGREnvelope &Bounds);
protected:
	wxString m_sLocation; 
	wxGISCoordinatesFormat m_CFormat;
	wxStaticText* m_staticText1;
	wxTextCtrl* m_textCtrl;
	wxBitmapButton* m_bpSelStyleButton;
	wxListCtrl* m_listCtrl;
};

/** \class wxGISIdentifyDlg identifydlg.h
 *  \brief The wxGISIdentifyDlg class is dialog/dock window with the results of identify.
 */
class WXDLLIMPEXP_GIS_CTU wxGISIdentifyDlg : public wxPanel 
{
protected:
	enum
	{
		ID_WXGISIDENTIFYDLG = 1000,
		ID_WXGISTREECTRL,
		ID_SWITCHSPLIT
	};		

     DECLARE_DYNAMIC_CLASS(wxGISIdentifyDlg)
public:
	wxGISIdentifyDlg(void);
	wxGISIdentifyDlg( wxWindow* parent, wxWindowID id = ID_WXGISIDENTIFYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISIdentifyDlg();
    virtual bool Create(wxWindow* parent, wxWindowID id = ID_WXGISIDENTIFYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("IdentifyView"));
		
	void SplitterOnIdle( wxIdleEvent& )
	{
		m_splitter->SetSashPosition( 0 );
		m_splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxGISIdentifyDlg::SplitterOnIdle ), NULL, this );
	}	
	//event
	void OnSwitchSplit(wxCommandEvent& event);
protected:
	wxBoxSizer* m_bMainSizer;
	wxFlexGridSizer* m_fgTopSizer;
	wxStaticText* m_staticText1;
	wxChoice* m_LayerChoice;
	wxBitmapButton* m_bpSplitButton;
	wxSplitterWindow* m_splitter;
	wxBitmap m_BmpVert, m_BmpHorz;
	wxTreeCtrl *m_pTreeCtrl;
	wxGISFeatureDetailsPanel* m_pFeatureDetailsPanel;
	wxImageList m_TreeImageList;
	wxXmlNode* m_pConf;

    DECLARE_EVENT_TABLE()
};

/** \class wxAxToolboxView gptoolboxview.h
    \brief The wxAxToolboxView show tool window with tabs(tools tree, tool exec view & etc.).
*/
class WXDLLIMPEXP_GIS_CTU wxAxIdentifyView :
	public wxGISIdentifyDlg,
	public IView
{
protected:
	enum
	{
		ID_WXGISIDENTIFYVIEW = 1000,
	};		

    DECLARE_DYNAMIC_CLASS(wxAxIdentifyView)
public:
    wxAxIdentifyView(void);
	wxAxIdentifyView(wxWindow* parent, wxWindowID id = ID_WXGISIDENTIFYVIEW, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxAxIdentifyView(void);
//IView
    virtual bool Create(wxWindow* parent, wxWindowID id = ID_WXGISIDENTIFYVIEW, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBORDER_NONE | wxTAB_TRAVERSAL, const wxString& name = wxT("IdentifyView"));
	virtual bool Activate(IFrameApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
	virtual void Refresh(void){};
	virtual wxString GetViewName(void){return m_sViewName;};
	virtual wxIcon GetViewIcon(void){return wxNullIcon;};
	virtual void SetViewIcon(wxIcon Icon){};
	//wxGISIdentifyDlg
	virtual void Identify(const OGREnvelope &Bounds);
	virtual void FillTree(wxGISFeatureLayerSPtr pFLayer, wxGISQuadTreeCursorSPtr pCursor);
protected:
	wxString m_sViewName;
    IFrameApplication* m_pApp;
	wxGISMapView* m_pMapView;
    //wxGxToolboxTreeView* m_pGxToolboxView;
    //wxGxToolExecuteView *m_pGxToolExecuteView;
};