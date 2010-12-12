/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServersUI class.
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
#include "wxgis/remoteserverui/gxremoteserversui.h"
#include "wxgis/remoteserverui/gxremoteserverui.h"
#include "wxgis/remoteserverui/createnetworkconndlg.h"
#include "wxgis/catalogui/gxcatalogui.h"


#include "../../art/remoteservers_16.xpm"
#include "../../art/remoteservers_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxRemoteServersUI, wxGxRemoteServers)

wxGxRemoteServersUI::wxGxRemoteServersUI(void) : wxGxRemoteServers(), m_RemServs16(remoteservers_16_xpm), m_RemServs48(remoteservers_48_xpm)
{
}

wxGxRemoteServersUI::~wxGxRemoteServersUI(void)
{
}

wxIcon wxGxRemoteServersUI::GetLargeImage(void)
{
	return m_RemServs48;
}

wxIcon wxGxRemoteServersUI::GetSmallImage(void)
{
	return m_RemServs16;
}

void wxGxRemoteServersUI::EmptyChildren(void)
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
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxRemoteServersUI::LoadChildren(wxXmlNode* pConf)
{
	if(!pConf || m_bIsChildrenLoaded)
		return;	
	wxXmlNode* pChild = pConf->GetChildren();
	while(pChild)
	{
		//create and test conn
		wxString sClassName = pChild->GetPropVal(wxT("class"), NONAME);
		if(!sClassName.IsEmpty())
		{
			INetClientConnection *pConn = dynamic_cast<INetClientConnection*>(wxCreateDynamicObject(sClassName));
			if(pConn && pConn->SetProperties(pChild))
			{
				wxGxRemoteServerUI* pServerConn = new wxGxRemoteServerUI(pConn);
				IGxObject* pGxObj = static_cast<IGxObject*>(pServerConn);
				if(!AddChild(pGxObj))
				{
					wxDELETE(pGxObj);
				}
				else//set callback
				{
					pConn->SetCallback(static_cast<INetCallback*>(pServerConn));
				}
			}
		}
		pChild = pChild->GetNext();
	}
	m_bIsChildrenLoaded = true;
}

void wxGxRemoteServersUI::CreateConnection(wxWindow* pParent, bool bSearch)
{
	INetClientConnection* pConn(NULL);
	if(bSearch)
	{
	}
	else
	{
		wxGISCreateNetworkConnDlg dlg(m_apNetConnFact, pParent);
		if(dlg.ShowModal() == wxID_SAVE)
			pConn = dlg.GetConnection();
	}
	if(pConn)
	{
		wxGxRemoteServerUI* pServerConn = new wxGxRemoteServerUI(pConn);
		IGxObject* pGxObj = static_cast<IGxObject*>(pServerConn);
		if(!AddChild(pGxObj))
		{
			wxDELETE(pGxObj);
		}
		else
		{
			pConn->SetCallback(static_cast<INetCallback*>(pServerConn));
			m_pCatalog->ObjectAdded(pGxObj);
			m_pCatalog->ObjectChanged(this);
		}
	}
}
