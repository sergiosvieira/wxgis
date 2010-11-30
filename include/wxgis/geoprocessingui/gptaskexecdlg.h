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
#pragma once

#include "wxgis/geoprocessingui/geoprocessingui.h"
#include "wxgis/geoprocessing/gptoolmngr.h"

#include "wx/wxhtml.h"
#include "wx/imaglist.h"

//////////////////////////////////////////////////////////////////
// wxGxTaskExecDlg
//////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GPU wxGxTaskExecDlg :
	public wxDialog,
    public ITrackCancel,
    public IGPCallBack
{
 //   enum
	//{
	//	ID_SHOW_BALLOON = wxID_HIGHEST + 30,
 //       ID_UPDATEMESSAGES
	//};
public:
	wxGxTaskExecDlg(wxGISGPToolManager* pToolManager, IGPCallBack* pCallBack, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);//wxTAB_TRAVERSAL|wxBORDER_RAISED );wxGISGPToolManager* pMngr, IGPTool* pTool, 
    virtual ~wxGxTaskExecDlg(void);
    virtual void SetTaskID(int nTaskID);
    //virtual void SetTaskThreadId(long nId);
    //virtual void SetToolDialog(wxWindow* pWindow, wxXmlNode* pNode);
    virtual void FillHtmlWindow();
    //events
    virtual void OnExpand(wxCommandEvent & event);
    virtual void OnCancel(wxCommandEvent & event);
    //virtual void OnShowBallon(wxCommandEvent & event);
    //virtual void OnUpdateMessages(wxCommandEvent & event);
    //IGPCallBack
    virtual void OnFinish(bool bHasErrors = false, IGPTool* pTool = NULL);
    //ITrackCancel
	virtual void PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType);
    typedef struct _message{
        wxGISEnumMessageType nType;
        wxString sMessage;
    }MESSAGE;
protected:
    wxImageList m_ImageList;
    bool m_bExpand;
    wxGISEnumMessageType m_nState;
    wxHtmlWindow* m_pHtmlWindow;
    wxBoxSizer* m_bMainSizer;
    wxBitmap m_ExpandBitmapBW, m_ExpandBitmap, m_ExpandBitmapBWRotated, m_ExpandBitmapRotated;
    wxBitmapButton* m_bpExpandButton;
    wxStaticText * m_Text;
    wxStaticBitmap* m_pStateBitmap;
    wxCheckBox* m_pCheckBox;
    wxBitmapButton* m_bpCloseButton;
    std::vector<MESSAGE> m_MessageArray;

    //long m_nTaskThreadId;
    //wxCriticalSection m_CritSec;
    //IGPTool* m_pTool;
    //wxXmlNode* m_pToolDialogPropNode;
    //wxWindow* m_pToolDialogWindow;

    wxString m_sHead;
    wxString m_sNote;
    wxIcon m_Icon;

    int m_nTaskID;
    IGPCallBack* m_pCallBack;
    wxGISGPToolManager* m_pToolManager;

    DECLARE_EVENT_TABLE();
};

////////////////////////////////////////////////////////////////////
//// wxGxTasksView
////////////////////////////////////////////////////////////////////
//
//class WXDLLIMPEXP_GIS_GPU wxGxTasksView :
//	public wxScrolledWindow,
//	public wxGxView
//{
//    DECLARE_DYNAMIC_CLASS(wxGxTasksView)
//public:
//    wxGxTasksView(void);
//	wxGxTasksView(wxWindow* parent, wxWindowID id = TASKSVIEWCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL|wxBORDER_SUNKEN|wxVSCROLL );
//	virtual ~wxGxTasksView(void);
//	virtual void AddPanel(wxGxTaskPanel* pGxTaskPanel);
//	virtual void InsertPanel(wxGxTaskPanel* pGxTaskPanel, long nPos = 0);
//    virtual void RemovePanel(wxGxTaskPanel* pGxTaskPanel);
////    virtual void Refresh(void){Refresh();};
////IGxView
//    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("ToolView"));
//	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf);
//	virtual void Deactivate(void);
//protected:
//    wxBoxSizer* m_bMainSizer;
//
//DECLARE_EVENT_TABLE()
//};
