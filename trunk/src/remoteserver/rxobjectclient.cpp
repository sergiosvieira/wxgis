/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxRxObject class.
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

#include "wxgis/remoteserver/rxobjectclient.h"
#include "wxgis/networking/message.h"

//------------------------------------------------------------
// wxRxObject
//------------------------------------------------------------

wxRxObject::wxRxObject() : m_pGxRemoteServer(NULL)
{
}

wxRxObject::~wxRxObject()
{
}

bool wxRxObject::Init(wxGxRemoteServer* pGxRemoteServer, wxXmlNode* pProperties)
{
	if(!pProperties && !pGxRemoteServer)
		return false;
	m_pGxRemoteServer = pGxRemoteServer;
	if(pProperties->HasProp(wxT("dst"))) //set dst from xml
	{
		m_sDst = pProperties->GetPropVal(wxT("dst"), ERR);
		INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pGxRemoteServer);
		if(pNetMessageProcessor)
			pNetMessageProcessor->AddMessageReceiver(m_sDst, static_cast<INetMessageReceiver*>(this));
	}
	return true;
}

void wxRxObject::Detach(void)
{
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pGxRemoteServer);
	if(pNetMessageProcessor)
		pNetMessageProcessor->DelMessageReceiver(m_sDst, static_cast<INetMessageReceiver*>(this));
	IGxObject::Detach();
}

void wxRxObject::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
}


//------------------------------------------------------------
// wxRxObjectContainer
//------------------------------------------------------------

wxRxObjectContainer::wxRxObjectContainer(void) : m_pGxRemoteServer(NULL), m_bIsChildrenLoaded(false), m_nChildCount(0)
{
}

wxRxObjectContainer::~wxRxObjectContainer(void)
{
}

void wxRxObjectContainer::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
	if(pChildNode)
	{
		if(pChildNode->GetName().CmpNoCase(wxT("children")) == 0 && pChildNode->HasProp(wxT("count")))
			m_nChildCount = wxAtoi(pChildNode->GetPropVal(wxT("count"), wxT("0")));
		if(m_nChildCount == 0)
		{
			m_bIsChildrenLoaded = true;
			return;
		}
		if(pChildNode->GetName().CmpNoCase(wxT("child")) == 0)
		{
			//load items
			while(pChildNode)
			{
				wxString sDst = pChildNode->GetPropVal(wxT("dst"), ERR);
				bool bAdd(true);
				for(size_t i = 0; i < m_sDstArray.GetCount(); i++)
				{
					if(sDst == m_sDstArray[i])
					{
						bAdd = false;
						break;
					}
				}

				wxString sClassName = pChildNode->GetPropVal(wxT("class"), ERR);
				if(!sClassName.IsEmpty() && bAdd)
				{
					IGxObject *pObj = dynamic_cast<IGxObject*>(wxCreateDynamicObject(sClassName));
					IRxObjectClient* pRxObjectCli = dynamic_cast<IRxObjectClient*>(pObj);
					if(pRxObjectCli)
						pRxObjectCli->Init(m_pGxRemoteServer, pChildNode);
					if(!AddChild(pObj))
						wxDELETE(pObj);
					m_sDstArray.Add(sDst);
				}
				pChildNode = pChildNode->GetNext();
			}
			m_bIsChildrenLoaded = m_nChildCount == m_Children.size();
			m_pCatalog->ObjectChanged(this);
		}
	}
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

bool wxRxObjectContainer::LoadChildren()
{
	if(m_bIsChildrenLoaded)
		return true;
	//send child request
	wxString sMsg = wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStGet, enumGISPriorityNormal, m_sDst, wxT("<children/>"));
    wxNetMessage* pMsg = new wxNetMessage(sMsg);
    if(pMsg->IsOk())
    {
        WXGISMSG msg = {INetMessageSPtr(static_cast<INetMessage*>(pMsg)), wxNOT_FOUND};
        m_pGxRemoteServer->PutOutMessage(msg);
		//add pending icon or item
		return true;
    }
    else
        wxDELETE(pMsg)
	return false;
}

void wxRxObjectContainer::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
}

void wxRxObjectContainer::Detach(void)
{
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pGxRemoteServer);
	if(pNetMessageProcessor)
		pNetMessageProcessor->DelMessageReceiver(m_sDst, static_cast<INetMessageReceiver*>(this));
	EmptyChildren();
	IGxObject::Detach();
}

bool wxRxObjectContainer::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
    m_pCatalog->ObjectDeleted(pChild);
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(this);
	return true;
}

bool wxRxObjectContainer::Init(wxGxRemoteServer* pGxRemoteServer, wxXmlNode* pProperties)
{
	if(!pProperties && !pGxRemoteServer)
		return false;
	m_pGxRemoteServer = pGxRemoteServer;
	if(pProperties->HasProp(wxT("dst"))) //set dst from xml
	{
		m_sDst = pProperties->GetPropVal(wxT("dst"), ERR);
		INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pGxRemoteServer);
		if(pNetMessageProcessor)
			pNetMessageProcessor->AddMessageReceiver(m_sDst, static_cast<INetMessageReceiver*>(this));
		return true;
	}
	return true;
}
