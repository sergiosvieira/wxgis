/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxGxTaskExecDlg class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010 Bishop
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

#include "wxgis/geoprocessingui/gptaskexecdlg.h"
#include "wxgis/framework/progressor.h"

#include "../../art/process_stop.xpm"
#include "../../art/state.xpm"

//////////////////////////////////////////////////////////////////
// wxGxTaskPanel
//////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGxTaskExecDlg, wxDialog)
    EVT_BUTTON(wxID_MORE, wxGxTaskExecDlg::OnExpand)
    EVT_BUTTON(wxID_CANCEL, wxGxTaskExecDlg::OnCancel)
    //EVT_BUTTON(ID_SHOW_BALLOON, wxGxTaskExecDlg::OnShowBallon)
    //EVT_BUTTON(ID_UPDATEMESSAGES, wxGxTaskExecDlg::OnUpdateMessages)
END_EVENT_TABLE()

wxGxTaskExecDlg::wxGxTaskExecDlg(wxGISGPToolManager* pToolManager, IGPCallBack* pCallBack, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style), m_bExpand(false), m_nState(enumGISMessageUnk), m_nTaskID(wxNOT_FOUND)//wxGISGPToolManager* pMngr, IGPTool* pTool, , m_pToolDialogPropNode(NULL), m_nTaskThreadId(-1)
{
    m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(state_xpm));

    m_pToolManager = pToolManager;
    m_pCallBack = pCallBack;

    //m_pTool = pTool;

    m_bMainSizer = new wxBoxSizer( wxVERTICAL );
 	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer( 1, 4, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_pStateBitmap = new wxStaticBitmap( this, wxID_ANY, m_ImageList.GetIcon(4), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_pStateBitmap, 0, wxALL, 5 );

    wxStaticText * soTitle = new wxStaticText(this, wxID_ANY, title);
    wxFont titleFont = this->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    soTitle->SetFont(titleFont);
    soTitle->Wrap( -1 );
	fgSizer1->Add( soTitle, 1, wxALL | wxEXPAND, 5 );


    m_ExpandBitmap = wxBitmap(m_ImageList.GetIcon(4));
    wxImage bwImage = m_ExpandBitmap.ConvertToImage();
    bwImage = bwImage.Rotate90();
    m_ExpandBitmap = wxBitmap(bwImage);
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

    wxBitmap NormalCBitmap = wxBitmap(process_stop_xpm);
    bwImage = NormalCBitmap.ConvertToImage();

    wxBitmap bwCBitmap = bwImage.ConvertToGreyscale();
    m_bpCloseButton = new wxBitmapButton( this, wxID_CANCEL, bwCBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    //m_bpCloseButton->SetBackgroundColour(bgColour);
    m_bpCloseButton->SetBitmapDisabled(bwCBitmap);
    m_bpCloseButton->SetBitmapLabel(bwCBitmap);
    m_bpCloseButton->SetBitmapSelected(NormalCBitmap);
    m_bpCloseButton->SetBitmapHover(NormalCBitmap);
    //wxBitmapButton* bpCloseButton = new wxBitmapButton( this, wxID_CLOSE, wxBitmap(process_stop_xpm), wxDefaultPosition, wxDefaultSize, 0 );
    m_bpCloseButton->SetToolTip(_("Cancel"));
    fgSizer1->Add( m_bpCloseButton, 0, wxALL, 5 );

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
    m_Text = new wxStaticText(this, wxID_ANY, wxEmptyString);//status message very long to fit in control may be and try again status message very long to fit in control may be, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE
    m_Text->Wrap( -1 );
    m_bMainSizer->Add(m_Text, 0, wxEXPAND | wxALL, 5);

    wxGISProgressor* pProgressor = new wxGISProgressor(this);
    pProgressor->SetInitialSize(wxSize(-1, 18));
    pProgressor->SetYield();
    m_bMainSizer->Add(pProgressor, 0, wxALL|wxEXPAND, 5);
    m_pProgressor = static_cast<IProgressor*>(pProgressor);

/*     wxGISProgressor* pProgressor = new wxGISProgressor(this);
    pProgressor->SetInitialSize(wxSize(-1, 18));
    bMainSizer->Add(pProgressor, 0, wxALL|wxEXPAND, 5);
*/
    m_pCheckBox = new wxCheckBox(this, wxID_ANY, _("Close when completed successfully"));
    m_bMainSizer->Add(m_pCheckBox, 0, /*wxEXPAND | */wxALL, 5);

//    wxRichTextCtrl* pRichTextCtrl = new wxRichTextCtrl(this, wxID_ANY);
    m_pHtmlWindow = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_DEFAULT_STYLE | wxBORDER_THEME);
    m_pHtmlWindow->SetInitialSize(wxSize(-1, 255));
    m_bMainSizer->Add(m_pHtmlWindow, 1, wxEXPAND | wxALL, 5);
    m_pHtmlWindow->Show(false);

   	this->SetSizer( m_bMainSizer );
	this->Layout();

    Fit();

 	//this->SetSizeHints( wxDefaultSize, wxDefaultSize );
   //m_bMainSizer->SetSizeHints( this );
}

wxGxTaskExecDlg::~wxGxTaskExecDlg(void)
{
}

void wxGxTaskExecDlg::OnExpand(wxCommandEvent & event)
{
    m_bExpand = !m_bExpand;

    if(m_bExpand)
    {
        FillHtmlWindow();
    }

    m_bpExpandButton->SetBitmapDisabled(m_bExpand == false ? m_ExpandBitmapBW : m_ExpandBitmapBWRotated);
    m_bpExpandButton->SetBitmapLabel(m_bExpand == false ? m_ExpandBitmapBW : m_ExpandBitmapBWRotated);
    m_bpExpandButton->SetBitmapSelected(m_bExpand == false ? m_ExpandBitmap : m_ExpandBitmapRotated);
    m_bpExpandButton->SetBitmapHover(m_bExpand == false ? m_ExpandBitmap : m_ExpandBitmapRotated);

    m_pHtmlWindow->Show(m_bExpand);
    wxSize Size = GetSize();
    Fit();
    wxSize NewSize = GetSize();
    SetSize(wxSize(Size.x, NewSize.y));
    //GetParent()->FitInside();
}

//void wxGxTaskExecDlg::SetTaskThreadId(long nId)
//{
//    m_nTaskThreadId = nId;
//}
//
void wxGxTaskExecDlg::FillHtmlWindow()
{
    wxString sText(wxT("<html><body>"));
    for(size_t i = 0 ; i < m_MessageArray.size(); i++)
    {
        switch(m_MessageArray[i].nType)
        {
        case enumGISMessageErr:
            sText += wxString(wxT("<FONT COLOR=red>"));
            sText += m_MessageArray[i].sMessage;
            sText += wxString(wxT("</FONT>"));
            break;
        case enumGISMessageQuestion:
            sText += wxString(wxT("<FONT color=blue>"));
            sText += m_MessageArray[i].sMessage;
            sText += wxString(wxT("</FONT>"));
            break;
        case enumGISMessageInfo:
            sText += wxString(wxT("<b>"));
            sText += m_MessageArray[i].sMessage;
            sText += wxString(wxT("</b>"));
            break;
        case enumGISMessageWarning:
            sText += wxString(wxT("<FONT color=green>"));
            sText += m_MessageArray[i].sMessage;
            sText += wxString(wxT("</FONT>"));
            break;
        case enumGISMessageNorm:
        case enumGISMessageUnk:
        default:
            sText += m_MessageArray[i].sMessage;
        }
        sText += wxString(wxT("<br>"));
    }
    sText += wxString(wxT("</body></html>"));
    /*bool bTest = */m_pHtmlWindow->SetPage(sText);
    m_pHtmlWindow->Scroll(-1, 5000);
}

//void wxGxTaskExecDlg::OnUpdateMessages(wxCommandEvent & event)
//{
//    FillHtmlWindow();
//}
//
void wxGxTaskExecDlg::OnFinish(bool bHasErrors, IGPTool* pTool)
{
    m_bpCloseButton->Enable(true);
    if(m_pCallBack)
        m_pCallBack->OnFinish(bHasErrors, pTool);
    if(m_pCheckBox->IsChecked())
    {
        Show(false);
        //destroy
        Destroy();
    }

//    //m_nState = bHasErrors == true ? enumGISMessageErr : enumGISMessageOK;
//
//    //m_sHead = wxString::Format(_("Tool %s"), pTool->GetDisplayName().c_str());
//    //if(m_nState == enumGISMessageErr)
//    //{
//    //    m_Icon = m_ImageList.GetIcon(2);
//    //    m_pStateBitmap->SetIcon(m_Icon);
//    //    m_sNote = wxString(_("Completed with errors!"));
//    //}
//    //else
//    //{
//    //    m_Icon = m_ImageList.GetIcon(1);
//    //    m_pStateBitmap->SetIcon(m_Icon);
//    //    wxDELETE(pTool);
//    //    m_sNote = wxString(_("Completed successfully!"));
//    //}
//
//    //wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, ID_SHOW_BALLOON);
//    //::wxPostEvent(this, event);
//
//    //if(m_nState == enumGISMessageOK && m_pCheckBox->IsChecked())
//    //{
//    //    wxSleep(2);
//    //    //remove from parent
//    //    wxGxTasksView* pView = dynamic_cast<wxGxTasksView*>(GetParent());
//    //    pView->RemovePanel(this);
//    //    Show(false);
//    //    //destroy
//    //    this->Destroy();
//    //}
//    //else //move back
//    //{
//    //    wxGxTasksView* pView = dynamic_cast<wxGxTasksView*>(GetParent());
//    //    pView->RemovePanel(this);
//    //    pView->AddPanel(this);
//    //    //refresh;
//    //    Refresh();
//    //}

}
//
//void wxGxTaskExecDlg::OnShowBallon(wxCommandEvent & event)
//{
//    wxGISBaloonTip* pTip = new wxGISBaloonTip(m_sHead, m_Icon, m_sNote);
//    pTip->ShowBaloon(m_nState == enumGISMessageErr ? 15000 : 7000);
//}
//
void wxGxTaskExecDlg::OnCancel(wxCommandEvent & event)
{
    //if exec
    //cancel m_nTaskID
    if(m_pToolManager)
		m_pToolManager->CancelProcess(m_nTaskID);

    //else close dialog
    Show(false);
    //destroy
    this->Destroy();

    //if(m_nState != enumGISMessageUnk)
    //{
    //    if(m_nState == enumGISMessageErr)
    //    {
    //        //create new dialog
    //        wxGISGPToolDlg* pDlg = new wxGISGPToolDlg(m_pTool, m_pMngr, m_pToolDialogPropNode, m_pToolDialogWindow, wxID_ANY, m_pTool->GetDisplayName());
    //        pDlg->Show(true);
    //    }
    //    //remove from parent
    //    wxGxTasksView* pView = dynamic_cast<wxGxTasksView*>(GetParent());
    //    pView->RemovePanel(this);
    //    Show(false);
    //    //destroy
    //    this->Destroy();
    //}
    //else
    //{
    //    PutMessage(_("Execution canceled by user"), -1, enumGISMessageWarning);
    //    Cancel();
    //    m_bpCloseButton->Enable(false);
    //}
}

//void wxGxTaskExecDlg::SetToolDialog(wxWindow* pWindow, wxXmlNode* pNode)
//{
//    m_pToolDialogWindow = pWindow;
//    m_pToolDialogPropNode = pNode;
//}
//
void wxGxTaskExecDlg::PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType)
{
    //wxCriticalSectionLocker locker(m_CritSec);

//    //if(m_nState == enumGISMessageErr)
//    //{
//    //    m_Icon = m_ImageList.GetIcon(2);
//    //    m_pStateBitmap->SetIcon(m_Icon);
//    //    m_sNote = wxString(_("Completed with errors!"));
//    //}
//    //else
//    //{
//    //    m_Icon = m_ImageList.GetIcon(1);
//    //    m_pStateBitmap->SetIcon(m_Icon);
//    //    wxDELETE(pTool);
//    //    m_sNote = wxString(_("Completed successfully!"));
//    //}
    switch(nType)
    {
    case enumGISMessageTitle:
        m_Text->SetLabel(sMessage);
        return;
    case enumGISMessageErr:
        m_Text->SetLabel(sMessage);
        m_Icon = m_ImageList.GetIcon(2);
        m_pStateBitmap->SetIcon(m_Icon);
        break;
    case enumGISMessageQuestion:
    case enumGISMessageInfo:
    case enumGISMessageWarning:
    case enumGISMessageNorm:
    case enumGISMessageUnk:
    default:
        break;
    }


    MESSAGE msg = {nType, sMessage};
    if(nIndex == -1 || m_MessageArray.size() < nIndex)
        m_MessageArray.push_back(msg);
    else
        m_MessageArray.insert(m_MessageArray.begin() + nIndex, msg);
    if(!m_bExpand)
        return;
    //htmlwin -> SetPage("<html><body>Hello, world!</body></html>");
    FillHtmlWindow();
    //wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, ID_UPDATEMESSAGES);
    //::wxPostEvent(this, event);
}

void wxGxTaskExecDlg::SetTaskID(int nTaskID)
{
    m_nTaskID = nTaskID;
    //if(m_pToolManager)
    //{
    //    WXGISEXECDDATA data = m_pToolManager->GetTask(m_nTaskID);
    //    SetLabel(data.pTool->GetDisplayName());
    //}
}


//void wxGxTaskPanel::OnKeyDown(wxKeyEvent & event)
//{
//    if(event.GetKeyCode() == WXK_SHIFT || event.GetKeyCode() == WXK_ALT || event.GetKeyCode() == WXK_CONTROL)
//        return;
//	switch(event.GetKeyCode())
//	{
//	case WXK_ESCAPE:
//        PutMessage(_("Execution canceled by user"), -1, enumGISMessageWarning);
//        Cancel();
//        m_bpCloseButton->Enable(false);
//		break;
//	default:
//		break;
//	}
//}

////////////////////////////////////////////////////////////////////
//// wxGxTasksView
////////////////////////////////////////////////////////////////////
//IMPLEMENT_DYNAMIC_CLASS(wxGxTasksView, wxScrolledWindow)
//
//BEGIN_EVENT_TABLE(wxGxTasksView, wxScrolledWindow)
//END_EVENT_TABLE()
//
//
//wxGxTasksView::wxGxTasksView(void)
//{
//}
//
//wxGxTasksView::wxGxTasksView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
//{
//    Create(parent, id, pos, size, style);
//}
//
//wxGxTasksView::~wxGxTasksView(void)
//{
//}
//
//bool wxGxTasksView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
//{
//    wxScrolledWindow::Create(parent, id, pos, size, style);
//    m_sViewName = wxString(_("Tasks"));
//    m_Icon = wxIcon(tool_16_xpm);
//
//    m_bMainSizer = new wxBoxSizer( wxVERTICAL );
//	//wxGxTaskPanel* pTaskPanel1 = new wxGxTaskPanel( this, wxID_ANY, wxDefaultPosition, wxSize(size.x, 100));//wxDefaultSize
// //   //pTaskPanel->SetBackgroundColour(*wxWHITE);
// //   bMainSizer->Add( pTaskPanel1, 0, wxEXPAND, 5 );
//	//wxGxTaskPanel* pTaskPanel2 = new wxGxTaskPanel( this, wxID_ANY, wxDefaultPosition, wxSize(size.x, 100));
// //   bMainSizer->Add( pTaskPanel2, 0, wxEXPAND, 5 );
//
//    SetScrollbars(20, 20, 50, 50);
//
//  	this->SetSizer( m_bMainSizer );
//	this->Layout();
//
//    return true;
//}
//
//bool wxGxTasksView::Activate(IGxApplication* application, wxXmlNode* pConf)
//{
//	wxGxView::Activate(application, pConf);
//
//	return true;
//}
//
//void wxGxTasksView::Deactivate(void)
//{
//	wxGxView::Deactivate();
//}
//
//void wxGxTasksView::AddPanel(wxGxTaskPanel* pGxTaskPanel)
//{
//    m_bMainSizer->Add( pGxTaskPanel, 0, wxEXPAND, 5 );
//    Layout();
//    FitInside();
//}
//
//void wxGxTasksView::RemovePanel(wxGxTaskPanel* pGxTaskPanel)
//{
//    m_bMainSizer->Detach( pGxTaskPanel );
//    Layout();
//    FitInside();
//}
//
//void wxGxTasksView::InsertPanel(wxGxTaskPanel* pGxTaskPanel, long nPos)
//{
//    m_bMainSizer->Insert(nPos, pGxTaskPanel, 0, wxEXPAND, 5 );
//    Layout();
//    FitInside();
//}
//

