/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISApplication class. Base application functionality (commands, menues, etc.)
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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
#include "wxgis/framework/application.h"
#include "wxgis/framework/toolbarmenu.h"

#include "../../art/tool_16.xpm"
#include "../../art/options.xpm"

#include "wx/propdlg.h"
//<wx/generic/propdlg.h>
#include "wx/tokenzr.h"
#include "wx/wxhtml.h"

IMPLEMENT_CLASS(wxGISApplication, wxFrame)

static IApplication* m_pGlobalApp;
extern WXDLLIMPEXP_GIS_FRW IApplication* GetApplication()
{
    return m_pGlobalApp;
}


BEGIN_EVENT_TABLE(wxGISApplication, wxFrame)
    EVT_ERASE_BACKGROUND(wxGISApplication::OnEraseBackground)
    EVT_SIZE(wxGISApplication::OnSize)
	EVT_RIGHT_DOWN(wxGISApplication::OnRightDown)
    EVT_AUITOOLBAR_RIGHT_CLICK(wxID_ANY, wxGISApplication::OnAuiRightDown)
	EVT_MENU_RANGE(ID_PLUGINCMD, ID_PLUGINCMD + 260, wxGISApplication::OnCommand)
	EVT_MENU_RANGE(ID_MENUCMD, ID_MENUCMD + 128, wxGISApplication::OnDropDownCommand)
	EVT_UPDATE_UI_RANGE(ID_PLUGINCMD, ID_PLUGINCMD + 260, wxGISApplication::OnCommandUI)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(wxID_ANY, wxGISApplication::OnToolDropDown)
    EVT_CLOSE(wxGISApplication::OnClose)
END_EVENT_TABLE()

wxGISApplication::wxGISApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style), m_pGISAcceleratorTable(NULL), m_pMenuBar(NULL), m_CurrentTool(NULL), m_pDropDownCommand(NULL), m_pTrackCancel(NULL), m_pszOldLocale(NULL), m_pLocale(NULL)
{
}

wxGISApplication::~wxGISApplication(void)
{
	UnLoadLibs();

    if(m_pszOldLocale != NULL)
		setlocale(LC_NUMERIC, m_pszOldLocale);
    wxDELETE(m_pLocale);
	wxDELETE(m_pszOldLocale);
}

ICommand* wxGISApplication::GetCommand(long CmdID)
{
	for(size_t i = 0; i < m_CommandArray.size(); i++)
		if(m_CommandArray[i] && m_CommandArray[i]->GetID() == CmdID)
			return m_CommandArray[i];
	return NULL;
}

void wxGISApplication::LoadCommands(wxXmlNode* pRootNode)
{
	wxXmlNode *child = pRootNode->GetChildren();
	unsigned int nCmdCounter(0);
	while(child)
	{
		wxString sName = child->GetPropVal(wxT("name"), wxT(""));
		if(!sName.IsEmpty())
		{
			wxObject *obj = wxCreateDynamicObject(sName);
			ICommand *pCmd = dynamic_cast<ICommand*>(obj);
			if(pCmd)
			{
				for(unsigned char i = 0; i < pCmd->GetCount(); i++)
				{
					wxObject *newobj = wxCreateDynamicObject(sName);
					ICommand *pNewCmd = dynamic_cast<ICommand*>(newobj);
					if(pNewCmd && pNewCmd->OnCreate(static_cast<IApplication*>(this)))
					{
						pNewCmd->SetID(ID_PLUGINCMD + nCmdCounter);
						pNewCmd->SetSubType(i);
						//TODO: check doubles
						m_CommandArray.push_back(pNewCmd);
						nCmdCounter++;
					}
				}
				wxDELETE(pCmd);
			}
		}
		child = child->GetNext();
	}
}

IGISCommandBar* wxGISApplication::GetCommandBar(wxString sName)
{
	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
		if(m_CommandBarArray[i]->GetName() == sName)
			return m_CommandBarArray[i];
	return NULL;
}

IGISConfig* wxGISApplication::GetConfig(void)
{
	return m_pConfig;
}

COMMANDBARARRAY* wxGISApplication::GetCommandBars(void)
{
	return &m_CommandBarArray;
}

COMMANDARRAY* wxGISApplication::GetCommands(void)
{
	return &m_CommandArray;
}

wxStatusBar* wxGISApplication::OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& name)
{
	wxGISStatusBar *sb = new wxGISStatusBar(this, id, style, name);
	return static_cast<wxStatusBar*>(sb);
}

IStatusBar* wxGISApplication::GetStatusBar(void)
{
	return dynamic_cast<IStatusBar*>(wxFrame::GetStatusBar());
}

void wxGISApplication::OnEraseBackground(wxEraseEvent& event)
{
    //event.Skip();
}

void wxGISApplication::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void wxGISApplication::OnCommand(wxCommandEvent& event)
{
	Command(GetCommand(event.GetId()));
}

void wxGISApplication::OnDropDownCommand(wxCommandEvent& event)
{
    if(m_pDropDownCommand)
        m_pDropDownCommand->OnDropDownCommand(event.GetId());
}

void wxGISApplication::Command(ICommand* pCmd)
{
    if(!pCmd)
        return;
	ITool* pTool = dynamic_cast<ITool*>(pCmd);
	if(pTool)
	{
		//uncheck
		if(m_CurrentTool)
			m_CurrentTool->SetChecked(false);
		//check
		pTool->SetChecked(true);
		m_CurrentTool = pTool;
	}
	else
		pCmd->OnClick();
}

void wxGISApplication::OnCommandUI(wxUpdateUIEvent& event)
{
    //event.Skip();

	ICommand* pCmd = GetCommand(event.GetId());
	if(pCmd)
	{
        event.Enable(pCmd->GetEnabled());
		if(pCmd->GetKind() == enumGISCommandCheck)
            //if(event.GetChecked() != pCmd->GetChecked())
                event.Check(pCmd->GetChecked());
        //if(event.GetEnabled() != pCmd->GetEnabled())

        wxString sAcc = m_pGISAcceleratorTable->GetText(event.GetId());
        //if(pCmd->GetKind() == enumGISCommandNormal)
        //{
        //    if(sAcc.IsEmpty())
        //    //    event.SetText(pCmd->GetCaption() + wxT("\t"));//accelerator
        //    //else
        //        event.SetText(pCmd->GetCaption() + wxT("\t") + sAcc);//accelerator
        //        //event.SetText(pCmd->GetCaption() + wxT(" ") + sAcc);//accelerator
        //}
        //if(pCmd->GetKind() != enumGISCommandNormal)
        //return;
		////set bitmap
		for(size_t i = 0; i < m_CommandBarArray.size(); i++)
		{
			switch(m_CommandBarArray[i]->GetType())
			{
			case enumGISCBSubMenu:
			case enumGISCBMenubar:
				{
					wxMenu* pMenu = dynamic_cast<wxMenu*>(m_CommandBarArray[i]);
// dirty hack
                    wxMenuItemList& pLst = pMenu->GetMenuItems();
                    wxMenuItemList::iterator iter;
                    for (iter = pLst.begin(); iter != pLst.end(); ++iter)
                    {
                        wxMenuItem* pItem = *iter;
                        if(pItem->IsSubMenu())
                        {
                            pItem->SetBitmap(wxNullBitmap);
                            wxString sT = pItem->GetText();
                            pItem->SetItemLabel(wxT(" "));// derty hack
                            pItem->SetItemLabel(sT);
                        }
                    }
// dirty hack end
					wxMenuItem *pItem = pMenu->FindItem(event.GetId());
					if(pItem != NULL)
					{
                        if(pItem->IsSubMenu())
                            break;

                        if(pItem->GetKind() != enumGISCommandCheck)
                        {
						wxIcon Bmp = pCmd->GetBitmap();
						//if(Bmp.IsOk())
							pItem->SetBitmap(Bmp);//double text??
                            pItem->SetItemLabel(wxT(" ")); // derty hack
                            pItem->SetItemLabel(pCmd->GetCaption() + wxT("\t") + sAcc);
                        }
					}
				}
				break;
			case enumGISCBContextmenu:
				{
					wxMenu* pMenu = dynamic_cast<wxMenu*>(m_CommandBarArray[i]);
// dirty hack
                    wxMenuItemList& pLst = pMenu->GetMenuItems();
                    wxMenuItemList::iterator iter;
                    for (iter = pLst.begin(); iter != pLst.end(); ++iter)
                    {
                        wxMenuItem* pItem = *iter;
                        if(pItem->IsSubMenu())
                        {
                            pItem->SetBitmap(wxNullBitmap);
                            wxString sT = pItem->GetText();
                            pItem->SetItemLabel(wxT(" "));// derty hack
                            pItem->SetItemLabel(sT);
                        }
                    }
// dirty hack end
                    wxMenuItem *pItem = pMenu->FindItem(event.GetId());
					if(pItem != NULL)
					{
                        if(pItem->IsSubMenu())
                            break;
                        if(pItem->GetKind() == enumGISCommandCheck)
							break;

						wxIcon Bmp = pCmd->GetBitmap();
						//if(Bmp.IsOk())
							pItem->SetBitmap(Bmp);//double text??
                        pItem->SetItemLabel(wxT(" ")); // derty hack
#ifdef __WXGTK__
                        if(sAcc.IsEmpty())
                            pItem->SetItemLabel(pCmd->GetCaption());
                        else
                            pItem->SetItemLabel(pCmd->GetCaption() + wxT("          ") + sAcc);
#else
                        pItem->SetItemLabel(pCmd->GetCaption() + wxT("\t") + sAcc);
#endif
                    }
				}
				break;
            case enumGISCBToolbar:
				{
					wxAuiToolBar* pToolbar = dynamic_cast<wxAuiToolBar*>(m_CommandBarArray[i]);
					wxAuiToolBarItem* pTool =pToolbar->FindTool(event.GetId());
					if(pTool != NULL)
					{
                        if(pTool->GetBitmap().IsOk())
                            break;
						wxIcon Bmp = pCmd->GetBitmap();
						if(Bmp.IsOk())
							pTool->SetBitmap(Bmp);
						else
							pTool->SetBitmap(wxBitmap(tool_16_xpm));
						if(!sAcc.IsEmpty())
							pTool->SetShortHelp(pCmd->GetTooltip() + wxT(" (") + sAcc + wxT(")"));//accelerator
                        else
							pTool->SetShortHelp(pCmd->GetTooltip());
					}
				}
				break;
			case enumGISCBNone:
			default:
				break;
			}
		}

        return;
	}
    //event.Skip();
}

void wxGISApplication::SerializeFramePos(bool bSave)
{
	wxXmlNode* pFrameXmlNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame")));

	if(bSave)
	{
		if(!pFrameXmlNode)
			pFrameXmlNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("frame")), true);

		if( IsMaximized() )
		{
			if(pFrameXmlNode->HasProp(wxT("maxi")))
				pFrameXmlNode->DeleteProperty(wxT("maxi"));
			pFrameXmlNode->AddProperty(wxT("maxi"), wxT("1"));
		}
		else
		{
			int x, y, w, h;
			GetClientSize(&w, &h);
			GetPosition(&x, &y);

			wxXmlProperty* prop = pFrameXmlNode->GetProperties();
			while(prop)
			{
				wxXmlProperty* prev_prop = prop;
				prop = prop->GetNext();
				wxDELETE(prev_prop);
			}
			wxXmlProperty* pHProp = new wxXmlProperty(wxT("Height"), wxString::Format(wxT("%u"), h), NULL);
			wxXmlProperty* pWProp = new wxXmlProperty(wxT("Width"), wxString::Format(wxT("%u"), w), pHProp);
			wxXmlProperty* pYProp = new wxXmlProperty(wxT("YPos"), wxString::Format(wxT("%d"), y), pWProp);
			wxXmlProperty* pXProp = new wxXmlProperty(wxT("XPos"), wxString::Format(wxT("%d"), x), pYProp);
			wxXmlProperty* pMaxi = new wxXmlProperty(wxT("maxi"), wxT("0"), pXProp);

			pFrameXmlNode->SetProperties(pMaxi);
		}
		//status bar shown state
		wxXmlNode* pStatusBarNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame/statusbar")));
		if(pStatusBarNode)
		{
			if(pStatusBarNode->HasProp(wxT("shown")))
				pStatusBarNode->DeleteProperty(wxT("shown"));
		}
		else
		{
			pStatusBarNode = new wxXmlNode(pFrameXmlNode, wxXML_ELEMENT_NODE, wxT("statusbar"));
		}
		pStatusBarNode->AddProperty(wxT("shown"), IsStatusBarShown() == true ? wxT("t") : wxT("f"));
	}
	else
	{
		//load
		if(!pFrameXmlNode)
			pFrameXmlNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("frame")));

		if(pFrameXmlNode == NULL)
			return;

		bool bMaxi = wxAtoi(pFrameXmlNode->GetPropVal(wxT("maxi"), wxT("0")));
		if(!bMaxi)
		{
			int x = wxAtoi(pFrameXmlNode->GetPropVal(wxT("XPos"), wxT("50")));
			int y = wxAtoi(pFrameXmlNode->GetPropVal(wxT("YPos"), wxT("50")));
			int w = wxAtoi(pFrameXmlNode->GetPropVal(wxT("Width"), wxT("850")));
			int h = wxAtoi(pFrameXmlNode->GetPropVal(wxT("Height"), wxT("530")));

			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
		//status bar shown state
		wxXmlNode* pStatusBarNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame/statusbar")));
		if(pStatusBarNode)
		{
			bool bStatusBarShow = pStatusBarNode->GetPropVal(wxT("shown"), wxT("t")) == wxString(wxT("t")) ? true : false;
			wxGISApplication::ShowStatusBar(bStatusBarShow);
		}
	}
}

wxGISAcceleratorTable* wxGISApplication::GetGISAcceleratorTable(void)
{
	return m_pGISAcceleratorTable;
}

wxGISMenuBar* wxGISApplication::GetMenuBar(void)
{
	return m_pMenuBar;
}

void wxGISApplication::RemoveCommandBar(IGISCommandBar* pBar)
{
	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
	{
		if(m_CommandBarArray[i] == pBar)
		{
			switch(m_CommandBarArray[i]->GetType())
			{
			case enumGISCBMenubar:
				m_pMenuBar->RemoveMenu(pBar);
				break;
			case enumGISCBToolbar:
			case enumGISCBContextmenu:
			case enumGISCBSubMenu:
			case enumGISCBNone:
				break;
			}
			wsDELETE(pBar);
			m_CommandBarArray.erase(m_CommandBarArray.begin() + i);
			break;
		}
	}
}

bool wxGISApplication::AddCommandBar(IGISCommandBar* pBar)
{
	if(!pBar)
		return false;
	pBar->Reference();
	m_CommandBarArray.push_back(pBar);
	switch(pBar->GetType())
	{
	case enumGISCBMenubar:
		m_pMenuBar->AddMenu(pBar);
		break;
	case enumGISCBToolbar:
	case enumGISCBContextmenu:
	case enumGISCBSubMenu:
	case enumGISCBNone:
		break;
	}
	return true;
}

ICommand* wxGISApplication::GetCommand(wxString sCmdName, unsigned char nCmdSubType)
{
	for(size_t i = 0; i < m_CommandArray.size(); i++)
	{
        wxObject* pObj = dynamic_cast<wxObject*>(m_CommandArray[i]);
		wxClassInfo * pInfo = pObj->GetClassInfo();
		wxString sCommandName = pInfo->GetClassName();
		if(sCommandName == sCmdName && m_CommandArray[i]->GetSubType() == nCmdSubType)
			return m_CommandArray[i];
	}
	return NULL;
}

void wxGISApplication::SerializeCommandBars(bool bSave)
{
	wxXmlNode* pMenuesNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame/menues")));

	wxXmlNode* pToolbarsNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame/toolbars")));

	if(bSave)
	{
		if(!pMenuesNode)
			pMenuesNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("frame/menues")), true);
		else
			wxGISConfig::DeleteNodeChildren(pMenuesNode);

		if(!pToolbarsNode)
			pToolbarsNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("frame/toolbars")), true);
		else
			wxGISConfig::DeleteNodeChildren(pToolbarsNode);


		for(size_t i = m_CommandBarArray.size(); i > 0; i--)
		{
			////skip wxGISToolBarMenu
			//if(m_CommandBarArray[i - 1]->GetName() == TOOLBARMENUNAME)
			//	continue;
			switch(m_CommandBarArray[i - 1]->GetType())
			{
			case enumGISCBMenubar:
			case enumGISCBContextmenu:
				{
					wxXmlNode* pNewNode = new wxXmlNode(pMenuesNode, wxXML_ELEMENT_NODE, wxString(wxT("menu")));
					m_CommandBarArray[i - 1]->Serialize(this, pNewNode, bSave);
				}
				break;
			case enumGISCBToolbar:
				{
					wxXmlNode* pNewNode = new wxXmlNode(pToolbarsNode, wxXML_ELEMENT_NODE, wxString(wxT("toolbar")));
					m_CommandBarArray[i - 1]->Serialize(this, pNewNode, bSave);
				}
				break;
			case enumGISCBSubMenu:
			case enumGISCBNone:
			default:
				break;
			}
		}
	}
	else
	{

		//create wxGISToolBarMenu
		wxGISToolBarMenu* pToolBarMenu = new wxGISToolBarMenu();
		pToolBarMenu->OnCreate(this);
		pToolBarMenu->Reference();
		m_CommandBarArray.push_back(pToolBarMenu);
		pToolBarMenu->Reference();
		m_CommandArray.push_back(pToolBarMenu);

		//load from current user config first & local machine config last
		//load menues
		LoadMenues(pMenuesNode);
		pMenuesNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("frame/menues")));
		LoadMenues(pMenuesNode);

		//load toolbars
		LoadToolbars(pToolbarsNode);
		pToolbarsNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("frame/toolbars")));
		LoadToolbars(pToolbarsNode);

		////////////////
		//wxGISToolBar* pTB = new wxGISToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE, wxT("App.Test"), wxT("Test"), enumGISCBToolbar);
		//pTB->Reference();
		//pTB->SetLeftDockable(true);
		//pTB->SetRightDockable(true);
		//wxAuiToolBar* pATB = static_cast<wxAuiToolBar*>(pTB);
		//pATB->AddLabel(wxID_ANY, wxT("Test:"), 25);
		////wxButton* but = new wxButton( pATB, wxID_ANY, _("Menu"), wxDefaultPosition, wxDefaultSize, 0 );
		////pATB->AddControl(but, wxT("TestCtrl"));
		//wxComboBox* comboBox1 = new wxComboBox( pATB, wxID_ANY, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
		//pATB->AddControl(comboBox1, wxT("TestCtrl"));
		//m_CommandBarArray.push_back(pTB);
		////////////////
		//wxGISToolBarMenu* pToolBarMenu = dynamic_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
		//if(pToolBarMenu)
			pToolBarMenu->Update();
	}
}

void wxGISApplication::ShowStatusBar(bool bShow)
{
	wxFrame::GetStatusBar()->Show(bShow);
}

bool wxGISApplication::IsStatusBarShown(void)
{
	return wxFrame::GetStatusBar()->IsShown();
}

void wxGISApplication::OnRightDown(wxMouseEvent& event)
{
	event.Skip();
	ShowToolBarMenu();
}

void wxGISApplication::OnAuiRightDown(wxAuiToolBarEvent& event)
{
	event.Skip();
	ShowToolBarMenu();
}


void wxGISApplication::ShowToolBarMenu(void)
{
	wxGISToolBarMenu* pToolBarMenu =  dynamic_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
	if(pToolBarMenu)
	{
		PopupMenu(pToolBarMenu);
	}
}

void wxGISApplication::OnMouseDown(wxMouseEvent& event)
{
	if(m_CurrentTool)
		m_CurrentTool->OnMouseDown(event);
}

void wxGISApplication::OnMouseUp(wxMouseEvent& event)
{
	if(m_CurrentTool)
		m_CurrentTool->OnMouseUp(event);
}

void wxGISApplication::OnMouseDoubleClick(wxMouseEvent& event)
{
	if(m_CurrentTool)
		m_CurrentTool->OnMouseDoubleClick(event);
}

void wxGISApplication::OnMouseMove(wxMouseEvent& event)
{
	if(m_CurrentTool)
		m_CurrentTool->OnMouseMove(event);
}

void wxGISApplication::LoadMenues(wxXmlNode* pRootNode)
{
	if(pRootNode)
	{
	//	create all menues and after that serialize each of them?
		wxXmlNode *child = pRootNode->GetChildren();
		while(child)
		{
			bool bAdd = true;
			wxString sName = child->GetPropVal(wxT("name"), wxT(""));
			for(size_t i = 0; i < m_CommandBarArray.size(); i++)
			{
				if(m_CommandBarArray[i]->GetName() == sName)
				{
					bAdd = false;
					break;
				}
			}
			if(bAdd)
			{
				wxString sCaption = child->GetPropVal(wxT("caption"), wxT("No Title"));
				wxGISMenu* pMenu = new wxGISMenu(sName, sCaption, enumGISCBContextmenu);//sCaption for Title enumGISCBMenubar
				pMenu->Serialize(this, child, false);
				pMenu->Reference();
				m_CommandBarArray.push_back(pMenu);
			}
			child = child->GetNext();
		}
	}
}

void wxGISApplication::LoadToolbars(wxXmlNode* pRootNode)
{
	if(pRootNode)
	{
		wxXmlNode *child = pRootNode->GetChildren();
		while(child)
		{
			bool bAdd = true;
			wxString sName = child->GetPropVal(wxT("name"), wxT(""));
			for(size_t i = 0; i < m_CommandBarArray.size(); i++)
			{
				if(m_CommandBarArray[i]->GetName() == sName)
				{
					bAdd = false;
					break;
				}
			}
			if(bAdd)
			{
				wxString sCaption = child->GetPropVal(wxT("caption"), wxT("No Title"));
				wxGISToolBar* pTB = new wxGISToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW, sName, sCaption, enumGISCBToolbar);
				pTB->Serialize(this, child, false);
				pTB->Reference();
				m_CommandBarArray.push_back(pTB);
			}
			child = child->GetNext();
		}
	}
}

void wxGISApplication::OnToolDropDown(wxAuiToolBarEvent& event)
{
    if(event.IsDropDownClicked())
    {
        ICommand* pCmd = GetCommand(event.GetToolId());
        m_pDropDownCommand = dynamic_cast<IDropDownCommand*>(pCmd);
        if(m_pDropDownCommand)
        {
            wxMenu* pMenu = m_pDropDownCommand->GetDropDownMenu();
            if(pMenu)
            {
#ifdef __WXGTK__
                PushEventHandler(pMenu);
                PopupMenu(pMenu, event.GetItemRect().GetBottomLeft());
                PopEventHandler();
#else
                PopupMenu(pMenu, event.GetItemRect().GetBottomLeft());
#endif
            }
            wxDELETE(pMenu);
        }
    }
}

void wxGISApplication::LoadLibs(wxXmlNode* pRootNode)
{
	wxXmlNode *child = pRootNode->GetChildren();
	while(child)
	{
		wxString sPath = child->GetPropVal(wxT("path"), wxT(""));
		if(sPath.Len() > 0)
		{
			//check for doubles
			if(m_LibMap[sPath] != NULL)
			{
				child = child->GetNext();
				continue;
			}

			wxDynamicLibrary* pLib = new wxDynamicLibrary(sPath);
			if(pLib != NULL)
			{
				wxLogMessage(_("wxGISApplication: Library %s loaded"), sPath.c_str());
				m_LibMap[sPath] = pLib;
			}
			else
				wxLogError(_("wxGISApplication: Error loading library %s"), sPath.c_str());
		}
		child = child->GetNext();
	}
}


void wxGISApplication::UnLoadLibs()
{
	wxXmlNode* pLibsNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("libs")));
	if(pLibsNode)
		wxGISConfig::DeleteNodeChildren(pLibsNode);
	else
		pLibsNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("libs")), true);


    for(LIBMAP::iterator item = m_LibMap.begin(); item != m_LibMap.end(); ++item)
	{
		wxXmlNode* pNewNode = new wxXmlNode(pLibsNode, wxXML_ELEMENT_NODE, wxString(wxT("lib")));
		pNewNode->AddProperty(wxT("path"), item->first);

		wxFileName FName(item->first);
		wxString sName = FName.GetName();

		pNewNode->AddProperty(wxT("name"), sName);

		wxDELETE(item->second);
	}
}

bool wxGISApplication::Create(IGISConfig* pConfig)
{
	m_pConfig = pConfig;

	CreateStatusBar();
	wxFrame::GetStatusBar()->SetStatusText(_("Ready"));

    //load libs
	wxXmlNode* pLibsNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);
	pLibsNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);

	//load commands
	wxXmlNode* pCommandsNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("commands")));
	if(pCommandsNode)
		LoadCommands(pCommandsNode);
	//load commandbars
	SerializeCommandBars();
	//load accelerators
	m_pGISAcceleratorTable = new wxGISAcceleratorTable(this, pConfig);


    // create MenuBar
	wxXmlNode* pMenuBarNode = m_pConfig->GetConfigNode(wxString(wxT("frame/menubar")), true, true);

    m_pMenuBar = new wxGISMenuBar(0, static_cast<IApplication*>(this), pMenuBarNode); //wxMB_DOCKABLE
    SetMenuBar(static_cast<wxMenuBar*>(m_pMenuBar));

	//mark menues from menu bar as enumGISTAMMenubar
	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
		if(m_pMenuBar->IsMenuBarMenu(m_CommandBarArray[i]->GetName()))
			m_CommandBarArray[i]->SetType(enumGISCBMenubar);

    // min size for the frame itself isn't completely done.
    // see the end up wxAuiManager::Update() for the test
    // code. For now, just hard code a frame minimum size
    SetMinSize(wxSize(800,480));

	SerializeFramePos(false);
	SetAcceleratorTable(m_pGISAcceleratorTable->GetAcceleratorTable());
    m_pGlobalApp = this;

    wxHtmlWindow *pHtmlText = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_DEFAULT_STYLE | wxBORDER_THEME);
    pHtmlText->SetPage(wxT("<html><body><h1>Error</h1>Some error occurred :-H)</body></hmtl>"));
    pHtmlText->Show(false);
    RegisterChildWindow(pHtmlText);

#ifdef __WXGTK__
    wxGISToolBarMenu* pToolBarMenu =  dynamic_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
	if(pToolBarMenu)
	    PushEventHandler(pToolBarMenu);
#endif

    return true;
}

void wxGISApplication::OnClose(wxCloseEvent& event)
{
    event.Skip();

	if(m_pGISAcceleratorTable)
		m_pGISAcceleratorTable->Store();
	wxDELETE(m_pGISAcceleratorTable);
	if(m_pMenuBar)
	{
		wxXmlNode* pMenuBarNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame/menubar")));
		if(!pMenuBarNode)
			pMenuBarNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("frame/menubar")), true);
		m_pMenuBar->Serialize(pMenuBarNode);
	}
	//wxDELETE(m_pMenuBar); delete by wxApp
	SerializeCommandBars(true);
	//delete oposite direction to prevent delete sub menues // refcount!
	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
	    wsDELETE(m_CommandBarArray[i]);

	for(size_t i = 0; i < m_CommandArray.size(); i++)
		wxDELETE(m_CommandArray[i]);

	SerializeFramePos(true);
}

void wxGISApplication::OnAppOptions(void)
{
    //read the config node for property pages and its names
    wxXmlNode *pPPXmlNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("propertypages")));
    if(!pPPXmlNode)
    {
        wxMessageBox(_("No Property Pages"), _("Error"), wxICON_ERROR | wxOK );
        return;
    }
    //load pages to the dialog and show
    wxPropertySheetDialog PropertySheetDialog;
    //(this, wxID_ANY, _("Options"), wxDefaultPosition, wxSize( 480,600 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    if (!PropertySheetDialog.Create(this, wxID_ANY, _("Options"), wxDefaultPosition, wxSize( 480,600 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    //PropertySheetDialog.SetMinSize( wxSize( 480,600 ));
    //PropertySheetDialog.SetSizeHints( wxSize( 480,600 ), wxDefaultSize );
    //PropertySheetDialog. SetClientSize( wxSize( 480,600 ));

    PropertySheetDialog.SetIcon(options_xpm);

    PropertySheetDialog.CreateButtons(wxOK|wxCANCEL);

    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());
    // Add page
    IApplication* pApp = static_cast<IApplication*>(this);
    wxXmlNode *pPropPageNode = pPPXmlNode->GetChildren();
    while(pPropPageNode)
    {
 		wxString sClass = pPropPageNode->GetPropVal(wxT("class"), ERR);
	    wxObject *obj = wxCreateDynamicObject(sClass);
		IPropertyPage *pPage = dynamic_cast<IPropertyPage*>(obj);
		if(pPage != NULL)
		{
            if(pPage->Create(pApp, PropertySheetDialog.GetBookCtrl(), wxID_ANY))
            {
                wxPanel* panel = static_cast<wxPanel*>(pPage);
                if(panel)
                    PropertySheetDialog.GetBookCtrl()->AddPage(panel, pPage->GetPageName());
            }
        }
        pPropPageNode = pPropPageNode->GetNext();
    }

    PropertySheetDialog.LayoutDialog();

    if(PropertySheetDialog.ShowModal() == wxID_OK)
    {
        //apply changes and exit        
        for(size_t i = 0; i < PropertySheetDialog.GetBookCtrl()->GetPageCount(); i++)
        {
            IPropertyPage *pPage = dynamic_cast<IPropertyPage*>(PropertySheetDialog.GetBookCtrl()->GetPage(i));
            if(pPage)
                pPage->Apply();
        }
    }    
}

bool wxGISApplication::SetupLog(wxString sLogPath)
{
	if(sLogPath.IsEmpty())
	{
		wxLogError(_("wxGISApplication: Failed to get log folder"));
        return false;
	}

	if(!wxDirExists(sLogPath))
		wxFileName::Mkdir(sLogPath, 0777, wxPATH_MKDIR_FULL);


	wxDateTime dt(wxDateTime::Now());
	wxString logfilename = sLogPath + wxFileName::GetPathSeparator() + wxString::Format(wxT("log_%.4d%.2d%.2d.log"),dt.GetYear(), dt.GetMonth() + 1, dt.GetDay());

    if(m_LogFile.IsOpened())
        m_LogFile.Close();

	if(!m_LogFile.Open(logfilename.GetData(), wxT("a+")))
		wxLogError(_("wxGISApplication: Failed to open log file %s"), logfilename.c_str());

	wxLog::SetActiveTarget(new wxLogStderr(m_LogFile.fp()));

#ifdef WXGISPORTABLE
	wxLogMessage(wxT("Portable"));
#endif
	wxLogMessage(wxT(" "));
	wxLogMessage(wxT("####################################################################"));
	wxLogMessage(wxT("##                    %s                    ##"),wxNow().c_str());
	wxLogMessage(wxT("####################################################################"));
	long dFreeMem =  (long)(wxGetFreeMemory().ToLong() / 1048576);
	wxLogMessage(_("HOST '%s': OS desc - %s, free memory - %u Mb"), wxGetFullHostName().c_str(), wxGetOsDescription().c_str(), dFreeMem);
	wxLogMessage(_("wxGISApplication: %s %s is initializing..."), GetAppName(), GetAppVersionString());
	wxLogMessage(_("wxGISApplication: Log file: %s"), logfilename.c_str());

    return true;
}

bool wxGISApplication::SetupSys(wxString sSysPath)
{
	//setup sys dir
	if(!wxDirExists(sSysPath))
	{
		wxLogError(wxString::Format(_("wxGISCatalogApp: System folder is absent! Lookup path '%s'"), sSysPath.c_str()));
		return false;
	}
    return true;
}

bool wxGISApplication::SetupLoc(wxString sLoc, wxString sLocPath)
{
    wxLogMessage(_("wxGISApplication: Initialize locale"));

    if(m_pszOldLocale != NULL)
		setlocale(LC_NUMERIC, m_pszOldLocale);
	wxDELETE(m_pszOldLocale);
    wxDELETE(m_pLocale);

	//init locale
    if ( !sLoc.IsEmpty() )
    {
		int iLocale(0);
		const wxLanguageInfo* loc_info = wxLocale::FindLanguageInfo(sLoc);
		if(loc_info != NULL)
		{
			iLocale = loc_info->Language;
			wxLogMessage(_("wxGISApplication: Language is set to %s"), loc_info->Description.c_str());
		}

        // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
        // false just because it failed to load wxstd catalog

        m_pLocale = new wxLocale();
        if ( !m_pLocale->Init(iLocale, wxLOCALE_CONV_ENCODING) )
        {
            wxLogError(wxT("wxGISApplication: This language is not supported by the system."));
            return false;
        }
    }

	//m_locale.Init(wxLANGUAGE_DEFAULT);

    // normally this wouldn't be necessary as the catalog files would be found
    // in the default locations, but when the program is not installed the
    // catalogs are in the build directory where we wouldn't find them by
    // default
	wxString sLocalePath = sLocPath + wxFileName::GetPathSeparator() + sLoc;
	if(wxDirExists(sLocalePath))
	{
		wxLocale::AddCatalogLookupPathPrefix(sLocalePath);

		// Initialize the catalogs we'll be using
		//load multicat from locale
		wxArrayString trans_arr;
		wxDir::GetAllFiles(sLocalePath, &trans_arr, wxT("*_cat.mo"));

		for(size_t i = 0; i < trans_arr.size(); i++)
		{
			wxFileName name(trans_arr[i]);
			m_pLocale->AddCatalog(name.GetName());
		}

		// this catalog is installed in standard location on Linux systems and
		// shows that you may make use of the standard message catalogs as well
		//
		// if it's not installed on your system, it is just silently ignored
	#ifdef __LINUX__
		{
			wxLogNull noLog;
			m_pLocale->AddCatalog(_T("fileutils"));
		}
	#endif
	}

	//support of dot in doubles and floats
	m_pszOldLocale = strdup(setlocale(LC_NUMERIC, NULL));
    if( setlocale(LC_NUMERIC,"C") == NULL )
        m_pszOldLocale = NULL;

    return true;
}



