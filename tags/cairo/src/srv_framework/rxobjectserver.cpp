/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxRxObjectContainer class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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

#include "wxgissrv/srv_framework/rxobjectserver.h"
#include "wxgis/networking/message.h"

//------------------------------------------------------------------
// wxRxObjectContainer
//------------------------------------------------------------------

wxRxObjectContainer::wxRxObjectContainer() : m_pApp(NULL), m_bIsChildrenLoaded(false)
{
}

wxRxObjectContainer::~wxRxObjectContainer()
{
}

wxXmlNode* wxRxObjectContainer::GetDescription(void)
{
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("child"));
	pNode->AddProperty(wxT("dst"), GetFullName());
	return pNode;
}

void wxRxObjectContainer::SetApplication(IServerApplication* pApp)
{
	m_pApp = pApp;
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->AddMessageReceiver(GetFullName(), static_cast<INetMessageReceiver*>(this));
}

void wxRxObjectContainer::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
	switch(msg.pMsg->GetState())
	{
		case enumGISMsgStGet:
			//no restrictions yet
			if(pChildNode)
			{
				wxString sName = pChildNode->GetName();
				if(sName.CmpNoCase(wxT("children")) == 0)//return children
				{
					//check children available
					if(HasChildren())
					{
						//sent children count
						wxString sBeginMsg = wxString::Format(wxT("<children count=\"%d\"/>"), m_Children.size()); 
						wxNetMessage* pMsg = new wxNetMessage(wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStRefuse, enumGISPriorityNormal + 10, wxNetMessage::FormatXmlString(GetFullName()), sBeginMsg.c_str()));
						WXGISMSG outmsg = {INetMessageSPtr(static_cast<INetMessage*>(pMsg)), msg.nUserID};
						m_pApp->PutOutMessage(outmsg);
						if(m_Children.size() == 0)
							break;
						pMsg = new wxNetMessage(enumGISMsgStOk, enumGISPriorityNormal, wxNetMessage::FormatXmlString(GetFullName()));
						wxXmlNode* pRoot = pMsg->GetRoot();
						if(!pRoot)
						{
							wxDELETE(pMsg);
							break;
						}
						for(size_t i = 0; i < m_Children.size(); i++)
						{
							IRxObjectServer* pRxObject = dynamic_cast<IRxObjectServer*>(m_Children[i]);
							if(pRxObject)
							{
								wxXmlNode* pDesc = pRxObject->GetDescription();
								pRoot->AddChild(pDesc);
							}
						}
						//send it
						outmsg.pMsg = INetMessageSPtr(static_cast<INetMessage*>(pMsg));
						m_pApp->PutOutMessage(outmsg);
					}
					else
					{
						wxNetMessage* pMsg = new wxNetMessage(wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStRefuse, enumGISPriorityNormal + 10, wxNetMessage::FormatXmlString(GetFullName()), wxString(wxT("<children count=\"0\"/>")).c_str()));
						WXGISMSG outmsg = {INetMessageSPtr(static_cast<INetMessage*>(pMsg)), msg.nUserID};
						m_pApp->PutOutMessage(outmsg);
					}
				}
			}
			break;
		case enumGISMsgStCmd:
			{
				wxNetMessage* pMsg = new wxNetMessage(wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStRefuse, enumGISPriorityNormal + 10, wxNetMessage::FormatXmlString(GetFullName())));
				WXGISMSG outmsg = {INetMessageSPtr(static_cast<INetMessage*>(pMsg)), msg.nUserID};
				m_pApp->PutOutMessage(outmsg);
			}
			break;
		default:
			break;
	}

}

void wxRxObjectContainer::Detach(void)
{
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->DelMessageReceiver(GetFullName(), static_cast<INetMessageReceiver*>(this));
	EmptyChildren();
	IGxObjectContainer::Detach();
}

void wxRxObjectContainer::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

bool wxRxObjectContainer::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
    long nChildID = pChild->GetID();
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
    m_pCatalog->ObjectDeleted(nChildID);
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(GetID());
	return true;
}

//------------------------------------------------------------------
// wxRxObject
//------------------------------------------------------------------

wxRxObject::wxRxObject() : m_pApp(NULL)
{
}

wxRxObject::~wxRxObject()
{
}

wxXmlNode* wxRxObject::GetDescription(void)
{
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("child"));
	pNode->AddProperty(wxT("dst"), wxNetMessage::FormatXmlString(GetFullName()));
	return pNode;
}

void wxRxObject::SetApplication(IServerApplication* pApp)
{
	m_pApp = pApp;
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->AddMessageReceiver(GetFullName(), static_cast<INetMessageReceiver*>(this));
}

void wxRxObject::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
}

void wxRxObject::Detach(void)
{
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->DelMessageReceiver(GetFullName(), static_cast<INetMessageReceiver*>(this));
	IGxObject::Detach();
}