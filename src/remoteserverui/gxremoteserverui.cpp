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
#include "../../art/remoteserverauth_16.xpm"
#include "../../art/remoteserverauth_48.xpm"
#include "../../art/remoteserver_16.xpm"
#include "../../art/remoteserver_48.xpm"


wxGxRemoteServerUI::wxGxRemoteServerUI(INetClientConnection* pNetConn) : wxGxRemoteServer(pNetConn)
{
    m_SmallIcon = wxIcon(remoteserver_16_xpm);
    m_LargeIcon = wxIcon(remoteserver_48_xpm);
    m_SmallAuthIcon = wxIcon(remoteserverauth_16_xpm);
    m_LargeAuthIcon = wxIcon(remoteserverauth_48_xpm);
    wxBitmap RemServDsbld16 = wxBitmap(remoteserver_16_xpm).ConvertToImage().ConvertToGreyscale();
    wxBitmap RemServDsbld48 = wxBitmap(remoteserver_48_xpm).ConvertToImage().ConvertToGreyscale();
    m_SmallDsblIcon.CopyFromBitmap(RemServDsbld16);
    m_LargeDsblIcon.CopyFromBitmap(RemServDsbld48);
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

#include "wxgis/datasource/postgisdataset.h"

bool wxGxRemoteServerUI::Invoke(wxWindow* pParentWnd)
{
	return Connect();
	//PG:host='localhost' dbname='<raster_database>' user='<user>' password='<password>'
	//PG:"dbname='databasename' host='addr' port='5432' user='x' password='y'"
	//wxGISPostGISDataset PGDSet(wxT("PG:host='127.0.0.1' dbname='monitoring_db' port='5432' user='bishop' password='xxx'"));
	//wxString sName = PGDSet.GetName();
	//size_t i = PGDSet.GetSubsetsCount();
	//return false;
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
	else if(!m_pNetConn)
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
	m_pCatalog->ObjectChanged(this);
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
				m_bIsChildrenLoaded = m_nChildCount == m_Children.size();
				m_pCatalog->ObjectChanged(this);
			}
		}
		return;
	}

}

void wxGxRemoteServerUI::OnConnect(void)
{
	AddMessageReceiver(wxT("root"), static_cast<INetMessageReceiver*>(this));
	wxGxRemoteServer::OnConnect();
}

