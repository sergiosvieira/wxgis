/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServerUI class.
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

#include "wxgis/remoteserverui/gxremoteserverui.h"
#include "wxgis/catalogui/gxcatalogui.h"

wxGxRemoteServerUI::wxGxRemoteServerUI(INetClientConnection* pNetConn, wxIcon SmallIcon, wxIcon LargeIcon, wxIcon SmallDsblIcon, wxIcon LargeDsblIcon, wxIcon SmallAuthIcon, wxIcon LargeAuthIcon) : wxGxRemoteServer(pNetConn)
{
    m_SmallIcon = SmallIcon;
    m_LargeIcon = LargeIcon;
    m_SmallDsblIcon = SmallDsblIcon;
    m_LargeDsblIcon = LargeDsblIcon;
    m_SmallAuthIcon = SmallAuthIcon;
    m_LargeAuthIcon = LargeAuthIcon;
}

wxGxRemoteServerUI::~wxGxRemoteServerUI(void)
{
}

wxIcon wxGxRemoteServerUI::GetLargeImage(void)
{
    if(m_pNetConn && m_pNetConn->IsConnected())
    {
        if(!m_bAuth)
            return m_LargeAuthIcon;
        else
            return m_LargeIcon;
    }
    else
        return m_LargeDsblIcon;
}

wxIcon wxGxRemoteServerUI::GetSmallImage(void)
{
    if(m_pNetConn && m_pNetConn->IsConnected())
    {
        if(!m_bAuth)
            return m_SmallAuthIcon;
        else
            return m_SmallIcon;
    }
    else
        return m_SmallDsblIcon;
}

bool wxGxRemoteServerUI::Invoke(wxWindow* pParentWnd)
{
	return Connect();
}

bool wxGxRemoteServerUI::Connect(void)
{
	wxBusyCursor wait;
    if(m_pNetConn && !m_pNetConn->IsConnected())
	{
		if(m_pNetConn->Connect())
			return true;
		wxMessageBox(_("Connection error! The server is not available."), _("Error"), wxICON_ERROR | wxOK );
		return false;
	}
	wxMessageBox(_("Connection object is broken!"), _("Error"), wxICON_ERROR | wxOK );
	return false;
}

void wxGxRemoteServerUI::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
        wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pCatalog);
        if(pCatalog)
        {

            IGxSelection* pSel = pCatalog->GetSelection();
            if(pSel)
                pSel->Unselect(m_Children[i], IGxSelection::INIT_ALL);
        }
		m_Children[i]->Detach();
		delete m_Children[i];
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxRemoteServerUI::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
	wxGxRemoteServer::ProcessMessage(msg, pChildNode);
    wxString sName = msg.pMsg->GetDestination();
	if(sName.CmpNoCase(wxT("info")) == 0)
	{
        if(msg.pMsg->GetState() == enumGISMsgStRefuse)
        {
			if(pChildNode)
			{
				wxString sText = pChildNode->GetPropVal(wxT("text"), ERR);
				wxMessageBox(sText, wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK );
			}
		}
		return;
	}
    if(sName.CmpNoCase(wxT("root")) == 0)
    {
		if(pChildNode)
		{
			if(pChildNode->GetName().CmpNoCase(wxT("children")) == 0 && pChildNode->HasProp(wxT("count")))
				m_nChildCount = wxAtoi(pChildNode->GetPropVal(wxT("count"), wxT("0")));
			else if(pChildNode->GetName().CmpNoCase(wxT("child")) == 0)
			{
				//load server root items
				while(pChildNode)
				{
					wxString sClassName = pChildNode->GetPropVal(wxT("class"), ERR);
					if(!sClassName.IsEmpty())
					{
						IGxObject *pObj = dynamic_cast<IGxObject*>(wxCreateDynamicObject(sClassName));
						IRxObjectClient* pRxObjectCli = dynamic_cast<IRxObjectClient*>(pObj);
						if(pRxObjectCli)
							pRxObjectCli->Init(this, pChildNode);
						if(!AddChild(pObj))
							wxDELETE(pObj);
					}
					pChildNode = pChildNode->GetNext();
				}
				m_pCatalog->ObjectChanged(this);
			}
		}
		return;
	}

}

bool wxGxRemoteServerUI::Attach(IGxObject* pParent, IGxCatalog* pCatalog)
{
	if(!wxGxRemoteServer::Attach(pParent, pCatalog))
		return false;
	AddMessageReceiver(wxT("root"), static_cast<INetMessageReceiver*>(this));

	return true;
}