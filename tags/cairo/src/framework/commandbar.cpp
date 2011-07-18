/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCommandBar class, and diferent implementation - wxGISMneu, wxGISToolBar
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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
#include "wxgis/framework/commandbar.h"
#include "../../art/tool_16.xpm"

//----------------------------------------------------------------------
// wxGISCommandBar
//----------------------------------------------------------------------

wxGISCommandBar::wxGISCommandBar(const wxString& sName, const wxString& sCaption, wxGISEnumCommandBars type)
{
	m_sName = sName;
	m_sCaption = sCaption;
	m_type = type;
}

wxGISCommandBar::~wxGISCommandBar(void)
{
}

void wxGISCommandBar::SetName(const wxString& sName)
{
	m_sName = sName;
}

wxString wxGISCommandBar::GetName(void)
{
	return m_sName;
}

void wxGISCommandBar::SetCaption(const wxString& sCaption)
{
	m_sCaption = sCaption;
}

wxString wxGISCommandBar::GetCaption(void)
{
	return wxGetTranslation(m_sCaption);
}

void wxGISCommandBar::SetType(wxGISEnumCommandBars type)
{
	m_type = type;
}

wxGISEnumCommandBars wxGISCommandBar::GetType(void)
{
	return m_type;
}

void wxGISCommandBar::AddCommand(ICommand* pCmd)
{
	m_CommandArray.push_back(pCmd);
}

void wxGISCommandBar::RemoveCommand(size_t nIndex)
{
	wxASSERT(nIndex >= 0 && nIndex < m_CommandArray.size());
	m_CommandArray.erase(m_CommandArray.begin() + nIndex);
}

void wxGISCommandBar::MoveCommandLeft(size_t nIndex)
{
	wxASSERT(nIndex >= 0 && nIndex < m_CommandArray.size());
	ICommand* pCmd = m_CommandArray[nIndex];
	m_CommandArray[nIndex] = m_CommandArray[nIndex - 1];
	m_CommandArray[nIndex - 1] = pCmd;
}

void wxGISCommandBar::MoveCommandRight(size_t nIndex)
{
	wxASSERT(nIndex >= 0 && nIndex < m_CommandArray.size());
	ICommand* pCmd = m_CommandArray[nIndex];
	m_CommandArray[nIndex] = m_CommandArray[nIndex + 1];
	m_CommandArray[nIndex + 1] = pCmd;
}

size_t wxGISCommandBar::GetCommandCount(void)
{
	return m_CommandArray.size();
}

ICommand* wxGISCommandBar::GetCommand(size_t nIndex)
{
	wxASSERT(nIndex >= 0 && nIndex < m_CommandArray.size());
	return m_CommandArray[nIndex];
}

void wxGISCommandBar::Serialize(IFrameApplication* pApp, wxXmlNode* pNode, bool bStore)
{
	if(bStore)
	{
		pNode->AddAttribute(wxT("name"), m_sName);
		pNode->AddAttribute(wxT("caption"), m_sCaption);
		for(size_t i = m_CommandArray.size(); i > 0; i--)
		{
			ICommand* pCmd = m_CommandArray[i - 1];
			if(pCmd)
			{
				wxXmlNode* pNewNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxString(wxT("Item")));
				wxGISEnumCommandKind Kind = pCmd->GetKind();
				switch(Kind)
				{
				case enumGISCommandSeparator:
					pNewNode->AddAttribute(wxT("type"), wxT("sep"));
					break;
				case enumGISCommandCheck:
				case enumGISCommandRadio:
				case enumGISCommandNormal:
				case enumGISCommandControl:
				case enumGISCommandDropDown:
				{
					pNewNode->AddAttribute(wxT("type"), wxT("cmd"));
					wxObject* pObj = dynamic_cast<wxObject*>(pCmd);
					if(pObj)
					{
						wxClassInfo* pInfo = pObj->GetClassInfo();
						wxString sClassName = pInfo->GetClassName();
						pNewNode->AddAttribute(wxT("cmd_name"), sClassName);
						pNewNode->AddAttribute(wxT("subtype"), wxString::Format(wxT("%u"), pCmd->GetSubType()));
						pNewNode->AddAttribute(wxT("name"), pCmd->GetCaption());
					}
					break;
				}
				case enumGISCommandMenu:
				{
					pNewNode->AddAttribute(wxT("type"), wxT("menu"));
					IGISCommandBar* pCB = dynamic_cast<IGISCommandBar*>(pCmd);
					if(pCB)
					{
						pNewNode->AddAttribute(wxT("cmd_name"), pCB->GetName());
						pNewNode->AddAttribute(wxT("name"), pCmd->GetCaption());
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
	else
	{
		//m_sName = pNode->GetAttribute(wxT("name"), m_sName);
		//m_sCaption = pNode->GetAttribute(wxT("caption"), m_sCaption);
		wxXmlNode *subchild = pNode->GetChildren();
		while(subchild)
		{
			wxString sType = subchild->GetAttribute(wxT("type"), wxT("sep"));
			if(sType == wxT("cmd"))
			{
				wxString sCmdName = subchild->GetAttribute(wxT("cmd_name"), wxT("None"));
				unsigned char nSubtype = wxAtoi(subchild->GetAttribute(wxT("subtype"), wxT("0")));
				//wxString sName = subchild->GetAttribute(wxT("name"), NONAME);
				ICommand* pSubCmd = pApp->GetCommand(sCmdName, nSubtype);
				if(pSubCmd)
					AddCommand(pSubCmd);
			}
			else if(sType == wxT("menu"))
			{
				//the menu description must be exist in xml before using submenu
				wxString sCmdName = subchild->GetAttribute(wxT("cmd_name"), ERR);
				IGISCommandBar* pGISCommandBar = pApp->GetCommandBar(sCmdName);
				if(pGISCommandBar)
				{
					ICommand* pSubCmd = dynamic_cast<ICommand*>(pGISCommandBar);
					if(pSubCmd)
						AddCommand(pSubCmd);
					else
						AddMenu(dynamic_cast<wxMenu*>(pGISCommandBar), pGISCommandBar->GetCaption());
				}
			}
			else
			{
				ICommand* pSubCmd = pApp->GetCommand(wxT("wxGISCommonCmd"), 3);
				if(pSubCmd)
					AddCommand(pSubCmd);
			}
			subchild = subchild->GetNext();
		}
	}
}

//----------------------------------------------------------------------
// wxGISMenu
//----------------------------------------------------------------------
wxGISMenu::wxGISMenu(const wxString& sName, const wxString& sCaption, wxGISEnumCommandBars type, const wxString& title, long style) : wxMenu(title, style), wxGISCommandBar(sName, sCaption, type)
{
}

wxGISMenu::~wxGISMenu(void)
{
	for(size_t i = 0; i < m_SubmenuArray.size(); ++i)
	{
		Delete(m_SubmenuArray[i].pItem);
		wsDELETE(m_SubmenuArray[i].pBar);
	}
}

void wxGISMenu::AddCommand(ICommand* pCmd)
{
	switch(pCmd->GetKind())
	{
	case enumGISCommandSeparator:
		AppendSeparator();
		break;
	case enumGISCommandMenu:
		{
			IGISCommandBar* pGISCommandBar = dynamic_cast<IGISCommandBar*>(pCmd);
			if(pGISCommandBar)
			{
				pGISCommandBar->Reference();
				SUBMENUDATA data = {AppendSubMenu(dynamic_cast<wxMenu*>(pCmd), pCmd->GetCaption(), pCmd->GetMessage()), pGISCommandBar};
				m_SubmenuArray.push_back(data);
			}
		}
		break;
	case enumGISCommandCheck:
	case enumGISCommandRadio:
    case enumGISCommandNormal:
		{
			wxMenuItem *item = new wxMenuItem(this, pCmd->GetID(), pCmd->GetCaption(), pCmd->GetMessage(), (wxItemKind)pCmd->GetKind());
			wxBitmap Bmp = pCmd->GetBitmap();
			if(Bmp.IsOk())
            {
#ifdef __WIN32__
                wxImage Img = Bmp.ConvertToImage();
                //Img.RotateHue(-0.1);
				item->SetBitmaps(Bmp, Img.ConvertToGreyscale());//SetBitmap//
#else
                item->SetBitmap(Bmp);
#endif
            }
			Append(item);
		}
		break;
	case enumGISCommandDropDown:
		{
			wxMenuItem *item = new wxMenuItem(this, pCmd->GetID(), pCmd->GetCaption(), pCmd->GetMessage(), (wxItemKind)enumGISCommandNormal);
			wxBitmap Bmp = pCmd->GetBitmap();
			if(Bmp.IsOk())
				item->SetBitmap(Bmp);
			Append(item);
		}
		break;
	case enumGISCommandControl:
		return;
	}
	wxGISCommandBar::AddCommand(pCmd);
}

void wxGISMenu::RemoveCommand(size_t nIndex)
{
	if(m_CommandArray[nIndex]->GetKind() == enumGISCommandSeparator)
		Delete(FindItemByPosition(nIndex));
	else
		Destroy(m_CommandArray[nIndex]->GetID());
	wxGISCommandBar::RemoveCommand(nIndex);
}

void wxGISMenu::MoveCommandLeft(size_t nIndex)
{
	wxMenuItem *pMenuItem = Remove(FindItemByPosition(nIndex));
	Insert(nIndex - 1, pMenuItem);
	wxGISCommandBar::MoveCommandLeft(nIndex);
}

void wxGISMenu::MoveCommandRight(size_t nIndex)
{
	wxMenuItem *pMenuItem = Remove(FindItemByPosition(nIndex));
	Insert(nIndex + 1, pMenuItem);
	wxGISCommandBar::MoveCommandRight(nIndex);
}

void wxGISMenu::AddMenu(wxMenu* pMenu, wxString sName)
{
	IGISCommandBar* pGISCommandBar = dynamic_cast<IGISCommandBar*>(pMenu);
	if(pGISCommandBar)
		pGISCommandBar->Reference();
	SUBMENUDATA data = {AppendSubMenu(pMenu, sName), pGISCommandBar};
	m_SubmenuArray.push_back(data);
}


//----------------------------------------------------------------------
// wxGISToolbar
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISToolBar, wxAuiToolBar)
    EVT_MOTION(wxGISToolBar::OnMotion)
END_EVENT_TABLE()

wxGISToolBar::wxGISToolBar(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style, const wxString& sName, const wxString& sCaption, wxGISEnumCommandBars type ) : wxAuiToolBar(parent, id, position, size, style), wxGISCommandBar(sName, sCaption, type), m_pStatusBar(NULL)
{
	IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(parent);
	if(pApp)
	{
		m_pStatusBar = pApp->GetStatusBar();
	}
}

wxGISToolBar::~wxGISToolBar(void)
{
}

void wxGISToolBar::OnMotion(wxMouseEvent& evt)
{
	if(!m_pStatusBar)
		return;
    // figure out messages
    wxAuiToolBarItem* packing_hit_item = FindToolByPositionWithPacking(evt.GetX(), evt.GetY());
    if (packing_hit_item)
    {
		wxString Help = packing_hit_item->GetLongHelp();
		if ( !Help.IsEmpty() )
			m_pStatusBar->SetMessage(Help);
		else
			m_pStatusBar->SetMessage(wxT(""));
    }
    else
    {
		m_pStatusBar->SetMessage(wxT(""));
    }
	evt.Skip();
}

void wxGISToolBar::SetLeftDockable(bool bLDock)
{
	m_bLDock = bLDock;
}

void wxGISToolBar::SetRightDockable(bool bRDock)
{
	m_bRDock = bRDock;
}

bool wxGISToolBar::GetLeftDockable(void)
{
	return m_bLDock;
}

bool wxGISToolBar::GetRightDockable(void)
{
	return m_bRDock;
}

void wxGISToolBar::AddCommand(ICommand* pCmd)
{
	switch(pCmd->GetKind())
	{
	case enumGISCommandMenu:
		return;
	case enumGISCommandSeparator:
	case enumGISCommandCheck:
	case enumGISCommandRadio:
	case enumGISCommandNormal:
		{
		wxBitmap Bitmap = pCmd->GetBitmap();
		if(!Bitmap.IsOk())
			Bitmap = wxBitmap(tool_16_xpm);

		AddTool(pCmd->GetID(), wxStripMenuCodes(pCmd->GetCaption()), Bitmap, wxBitmap(), (wxItemKind)pCmd->GetKind(), pCmd->GetTooltip(), pCmd->GetMessage(), NULL);
		}
		break;
	case enumGISCommandDropDown:
		{
		wxBitmap Bitmap = pCmd->GetBitmap();
		if(!Bitmap.IsOk())
			Bitmap = wxBitmap(tool_16_xpm);

		AddTool(pCmd->GetID(), wxStripMenuCodes(pCmd->GetCaption()), Bitmap, wxBitmap(), (wxItemKind)enumGISCommandNormal, pCmd->GetTooltip(), pCmd->GetMessage(), NULL);
        SetToolDropDown(pCmd->GetID(), true);
		}
		break;
	case enumGISCommandControl:
		{
			IToolControl* pToolCtrl = dynamic_cast<IToolControl*>(pCmd);
			if(pToolCtrl)
			{
				IToolBarControl* pToolBarControl = pToolCtrl->GetControl();
				wxControl* pControl = dynamic_cast<wxControl*>(pToolBarControl);
				if(pControl)
				{
					if(pToolCtrl->HasToolLabel())
					{
						wxString sToolLabel = pToolCtrl->GetToolLabel();
						AddLabel(wxID_ANY, sToolLabel, sToolLabel.Len() * 5);
					}
					pControl->Reparent(this);
					AddControl(pControl);
					//add ctrl to remove map
					m_RemControlMap[m_CommandArray.size()] = pToolBarControl;
				}
				else return;
			}
			else return;
		}
		break;
	default: return;
	}
	wxGISCommandBar::AddCommand(pCmd);
	Realize();
}

void wxGISToolBar::ReAddCommand(ICommand* pCmd)
{
	switch(pCmd->GetKind())
	{
	case enumGISCommandMenu:
		return;
	case enumGISCommandSeparator:
	case enumGISCommandCheck:
	case enumGISCommandRadio:
	case enumGISCommandNormal:
		{
		wxBitmap Bitmap = pCmd->GetBitmap();
		if(!Bitmap.IsOk())
			Bitmap = wxBitmap(tool_16_xpm);

		AddTool(pCmd->GetID(), wxStripMenuCodes(pCmd->GetCaption()), Bitmap, wxBitmap(), (wxItemKind)pCmd->GetKind(), pCmd->GetTooltip(), pCmd->GetMessage(), NULL);
		}
		break;
	case enumGISCommandDropDown:
		{
		wxBitmap Bitmap = pCmd->GetBitmap();
		if(!Bitmap.IsOk())
			Bitmap = wxBitmap(tool_16_xpm);

		AddTool(pCmd->GetID(), wxStripMenuCodes(pCmd->GetCaption()), Bitmap, wxBitmap(), (wxItemKind)enumGISCommandNormal, pCmd->GetTooltip(), pCmd->GetMessage(), NULL);
        SetToolDropDown(pCmd->GetID(), true);
		}
		break;
    case enumGISCommandControl:
			return;
	}
}

void wxGISToolBar::SetName(const wxString& sName)
{
	wxGISCommandBar::SetName(sName);
}

wxString wxGISToolBar::GetName(void)
{
	return wxGISCommandBar::GetName();
}

void wxGISToolBar::SetCaption(const wxString& sCaption)
{
	wxGISCommandBar::SetCaption(sCaption);
}

wxString wxGISToolBar::GetCaption(void)
{
	return wxGISCommandBar::GetCaption();
}

void wxGISToolBar::RemoveCommand(size_t nIndex)
{
	size_t nRealIndex = nIndex;
	//count beforehead controls and check if labels exists
	for(size_t i = 0; i < nIndex; ++i)
	{
		if(m_CommandArray[nIndex]->GetKind() == enumGISCommandControl)
		{
			IToolControl* pToolControl = dynamic_cast<IToolControl*>(m_CommandArray[nIndex]);
			if(pToolControl->HasToolLabel())
				nRealIndex++;
		}
	}
	//check if it's control
	if(m_CommandArray[nIndex]->GetKind() == enumGISCommandControl)
	{
		IToolControl* pToolControl = dynamic_cast<IToolControl*>(m_CommandArray[nIndex]);
		if(pToolControl->HasToolLabel())
		{
			DeleteByIndex(nRealIndex);
			DeleteByIndex(nRealIndex);
		}
		else
			DeleteByIndex(nRealIndex);
		wxDELETE(m_RemControlMap[nIndex]);
	}
	else
		DeleteByIndex(nRealIndex);
	wxGISCommandBar::RemoveCommand(nIndex);
	Realize();
}

void wxGISToolBar::MoveCommandLeft(size_t nIndex)
{
	wxGISCommandBar::MoveCommandLeft(nIndex);
	wxAuiToolBar::Clear();
	for(size_t i = 0; i < m_CommandArray.size(); ++i)
		ReAddCommand(m_CommandArray[i]);
	wxAuiToolBar::Realize();
}

void wxGISToolBar::MoveCommandRight(size_t nIndex)
{
	wxGISCommandBar::MoveCommandRight(nIndex);
	wxAuiToolBar::Clear();
	for(size_t i = 0; i < m_CommandArray.size(); ++i)
		ReAddCommand(m_CommandArray[i]);
	wxAuiToolBar::Realize();
}

void wxGISToolBar::Serialize(IFrameApplication* pApp, wxXmlNode* pNode, bool bStore)
{
	if(bStore)
	{
		pNode->AddAttribute(wxT("size"), wxString::Format(wxT("%u"), GetToolBitmapSize().GetWidth()));
		pNode->AddAttribute(wxT("LeftDockable"), m_bLDock == true ? wxT("t") : wxT("f"));
		pNode->AddAttribute(wxT("RightDockable"), m_bRDock == true ? wxT("t") : wxT("f"));
		wxGISCommandBar::Serialize(pApp, pNode, bStore);
	}
	else
	{
		m_bLDock = pNode->GetAttribute(wxT("LeftDockable"), wxT("f")) == wxT("f") ? false : true;
		m_bRDock = pNode->GetAttribute(wxT("RightDockable"), wxT("f")) == wxT("f") ? false : true;
		short iSize = wxAtoi(pNode->GetAttribute(wxT("size"), wxT("16")));
		SetToolBitmapSize(wxSize(iSize,iSize));

		wxAuiToolBarItemArray prepend_items;
		wxAuiToolBarItemArray append_items;
		ICommand* pCmd = pApp->GetCommand(wxT("wxGISCommonCmd"), 2);
		if(pCmd)
		{
			wxAuiToolBarItem item;
			item.SetKind(wxITEM_SEPARATOR);
			append_items.Add(item);
			item.SetKind(pCmd->GetKind());
			item.SetId(pCmd->GetID());
			item.SetLabel(pCmd->GetCaption());
			append_items.Add(item);
		}
		SetCustomOverflowItems(prepend_items, append_items);
		wxGISCommandBar::Serialize(pApp, pNode, bStore);
		Realize();
	}
}

void wxGISToolBar::AddMenu(wxMenu* pMenu, wxString sName)
{
	//m_SubmenuArray.push_back(AppendSubMenu(pMenu, sName);
}

void wxGISToolBar::Activate(IFrameApplication* pApp)
{
	for(std::map<size_t, IToolBarControl*>::const_iterator IT = m_RemControlMap.begin(); IT != m_RemControlMap.end(); ++IT)
	{
		IT->second->Activate(pApp);
	}
}

void wxGISToolBar::Deactivate(void)
{
	for(std::map<size_t, IToolBarControl*>::const_iterator IT = m_RemControlMap.begin(); IT != m_RemControlMap.end(); ++IT)
	{
		IT->second->Deactivate();
	}
}

void wxGISToolBar::UpdateControls(void)
{
	for(size_t i = 0; i < m_CommandArray.size(); ++i)
	{
		if(m_CommandArray[i]->GetKind() == enumGISCommandControl)
		{
			wxWindow* pWnd = dynamic_cast<wxWindow*>(m_RemControlMap[i]);
			if(pWnd)
			{
				bool bEnable = m_CommandArray[i]->GetEnabled();
				pWnd->Enable(bEnable);
			}
		}
	}
}