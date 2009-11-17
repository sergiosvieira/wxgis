/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISApplication class. Base application functionality (commands, menues, etc.)
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
#include "wxgis/framework/application.h"
#include "wxgis/framework/toolbarmenu.h"
#include <wx/tokenzr.h>
#include "../../art/default_16.xpm"


BEGIN_EVENT_TABLE(wxGISApplication, wxFrame)
    EVT_ERASE_BACKGROUND(wxGISApplication::OnEraseBackground)
    EVT_SIZE(wxGISApplication::OnSize)
	EVT_RIGHT_DOWN(wxGISApplication::OnRightDown)
	EVT_MENU_RANGE(wxGISApplication::ID_PLUGINCMD, ID_PLUGINCMD + 512, wxGISApplication::OnCommand)
	EVT_UPDATE_UI_RANGE(wxGISApplication::ID_PLUGINCMD, ID_PLUGINCMD + 512, wxGISApplication::OnCommandUI)
END_EVENT_TABLE()

wxGISApplication::wxGISApplication(IGISConfig* pConfig, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style), m_nCmdCounter(0), m_pGISAcceleratorTable(NULL), m_pMenuBar(NULL), m_CurrentTool(NULL)
{
	m_pConfig = pConfig;

	CreateStatusBar();
	wxFrame::GetStatusBar()->SetStatusText(_("Ready"));

	//load commands
	wxXmlNode* pCommandsNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("commands")));
	if(pCommandsNode)
		LoadCommands(pCommandsNode);
	//load commandbars
	SerializeCommandBars();
	//load accelerators
	m_pGISAcceleratorTable = new wxGISAcceleratorTable(this, pConfig);


    // create MenuBar
	wxXmlNode* pMenuBarNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame/menubar")));
	if(!pMenuBarNode)
	{
		wxXmlNode* pMenuBarNodeLM = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("frame/menubar")));
		pMenuBarNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("frame/menubar")), true);
		pMenuBarNode->operator=(*pMenuBarNodeLM);
	}
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
}

wxGISApplication::~wxGISApplication(void)
{
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
	SerializeFramePos(true);

	//delete oposite direction to prevent delete sub menues // refcount!
	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
	    wsDELETE(m_CommandBarArray[i]);

	for(size_t i = 0; i < m_CommandArray.size(); i++)
		wxDELETE(m_CommandArray[i]);
}

ICommand* wxGISApplication::GetCommand(long CmdID)
{
	for(size_t i = 0; i < m_CommandArray.size(); i++)
		if(m_CommandArray[i]->GetID() == CmdID)
			return m_CommandArray[i];
	return NULL;
}

void wxGISApplication::LoadCommands(wxXmlNode* pRootNode)
{
	wxXmlNode *child = pRootNode->GetChildren();
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
						pNewCmd->SetID(ID_PLUGINCMD + m_nCmdCounter);
						pNewCmd->SetSubType(i);
						m_CommandArray.push_back(pNewCmd);
						m_nCmdCounter++;
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
    event.Skip();
}

void wxGISApplication::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void wxGISApplication::OnCommand(wxCommandEvent& event)
{
	OnCommand(GetCommand(event.GetId()));
}

void wxGISApplication::OnCommand(ICommand* pCmd)
{
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
	ICommand* pCmd = GetCommand(event.GetId());
	if(pCmd)
	{
		if(pCmd->GetKind() == enumGISCommandCheck)
			event.Check(pCmd->GetChecked());
		event.Enable(pCmd->GetEnabled());
		wxString sAcc = m_pGISAcceleratorTable->GetText(event.GetId());
		event.SetText(pCmd->GetCaption() + wxT("\t") + sAcc);//accelerator
		////set bitmap
		for(size_t i = 0; i < m_CommandBarArray.size(); i++)
		{
			switch(m_CommandBarArray[i]->GetType())
			{
			case enumGISCBMenubar:
				{
					wxMenu* pMenu = dynamic_cast<wxMenu*>(m_CommandBarArray[i]);
					wxMenuItem *pItem = pMenu->FindItem(event.GetId());
					if(pItem != NULL)
					{
						wxBitmap Bmp = pCmd->GetBitmap();
						//if(Bmp.IsOk())
							pItem->SetBitmap(Bmp);//double text??
					}
				}
				break;
			case enumGISCBToolbar:
				{
					wxAuiToolBar* pToolbar = dynamic_cast<wxAuiToolBar*>(m_CommandBarArray[i]);
					wxAuiToolBarItem* pTool =pToolbar->FindTool(event.GetId());
					if(pTool != NULL)
					{
						wxBitmap Bmp = pCmd->GetBitmap();
						if(Bmp.IsOk())
							pTool->SetBitmap(Bmp);
						else
							pTool->SetBitmap(wxBitmap(default_16_xpm));
						if(!sAcc.IsEmpty())
							pTool->SetShortHelp(pCmd->GetTooltip() + wxT(" (") + sAcc + wxT(")"));//accelerator
					}
				}
				break;
			case enumGISCBNone:
			case enumGISCBContextmenu:
			case enumGISCBSubMenu:
			default:
				break;
			}
		}
	}
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
			ShowStatusBar(bStatusBarShow);
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
		wxClassInfo * pInfo = m_CommandArray[i]->GetClassInfo();
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
			//skip wxGISToolBarMenu
			if(m_CommandBarArray[i - 1]->GetName() == TOOLBARMENUNAME)
				continue;
			switch(m_CommandBarArray[i - 1]->GetType())
			{
			case enumGISCBMenubar:
			case enumGISCBContextmenu:
			case enumGISCBSubMenu:
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
	ShowToolBarMenu();
	event.Skip();
}

void wxGISApplication::ShowToolBarMenu(void)
{
	wxGISToolBarMenu* pToolBarMenu =  dynamic_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
	if(pToolBarMenu)
		PopupMenu(pToolBarMenu);
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
				wxGISMenu* pMenu = new wxGISMenu(sName, sCaption, enumGISCBContextmenu);//sCaption for Title
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
