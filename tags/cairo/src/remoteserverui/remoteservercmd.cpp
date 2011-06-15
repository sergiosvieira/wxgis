/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  Catalog Main Commands class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2011 Bishop
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
#include "wxgis/remoteserverui/remoteservercmd.h"
#include "wxgis/remoteserverui/serversearchdlg.h"
#include "wxgis/remoteserverui/gxremoteserversui.h"
#include "wxgis/remoteserverui/gxremoteserverui.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/remoteserver_16.xpm"
#include "../../art/remoteservers_16.xpm"
#include "../../art/remoteserver_discon.xpm"


//	0	Search Servers
//	1	Server connection
//	2	Connect
//	3	Disconnect


IMPLEMENT_DYNAMIC_CLASS(wxGISRemoteCmd, wxObject)

wxGISRemoteCmd::wxGISRemoteCmd(void) : m_IconRemServs(remoteservers_16_xpm), m_IconRemServ(remoteserver_16_xpm), m_IconRemServDiscon(remoteserver_discon_xpm)
{
}

wxGISRemoteCmd::~wxGISRemoteCmd(void)
{
}

wxIcon wxGISRemoteCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			return m_IconRemServs;
		case 1:
			return m_IconRemServ;
		case 2:
			return m_IconRemServ;
		case 3:
			return m_IconRemServDiscon;
		default:
			return wxNullIcon;
	}
}

wxString wxGISRemoteCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("&Search servers"));
		case 1:	
			return wxString(_("Server c&onnection"));
		case 2:	
			return wxString(_("&Connect"));
		case 3:	
			return wxString(_("&Disconnect"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISRemoteCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:	
		case 1:	
		case 2:	
		case 3:	
			return wxString(_("Remote Server"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISRemoteCmd::GetChecked(void)
{
	return false;
}

bool wxGISRemoteCmd::GetEnabled(void)
{
	bool bConn(false);
	switch(m_subtype)
	{
		case 0://search server
		case 1://server connection
			{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
                IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());
                wxGxRemoteServersUI* pGxRemoteServersUI = dynamic_cast<wxGxRemoteServersUI*>(pGxObject.get());
                if(pGxRemoteServersUI)
                    return true;
			}		
			return false;
			}
		case 2://connect
		case 3://disconnect
			{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
                IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());
                wxGxRemoteServerUI* pGxRemoteServerUI = dynamic_cast<wxGxRemoteServerUI*>(pGxObject.get());
                if(pGxRemoteServerUI)
					if(m_subtype == 2)
						return  !pGxRemoteServerUI->IsConnected();
					else
						return  pGxRemoteServerUI->IsConnected();
			}		
			return false;
			}
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISRemoteCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://search server
		case 1://server connection
		case 2://connect
		case 3://disconnect
			return enumGISCommandNormal;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISRemoteCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Search for remote server"));
		case 1:	
			return wxString(_("Create connection to remote server"));
		case 2:	
			return wxString(_("Connect to remote server"));
		case 3:	
			return wxString(_("Disconnect from remote server"));
		default:
			return wxEmptyString;
	}
}

void wxGISRemoteCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:	
			{
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
					wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pGxCatalogUI->GetSelection();
                    IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());
					wxGxRemoteServersUI* pGxRemoteServersUI = dynamic_cast<wxGxRemoteServersUI*>(pGxObject.get());
					if(pGxRemoteServersUI)
					{
						wxWindow* pParentWnd = dynamic_cast<wxWindow*>(pGxApp);
						return pGxRemoteServersUI->CreateConnection(pParentWnd, true);
					}
				}
			}
			return;
		case 1:	
			{
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
					wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pGxCatalogUI->GetSelection();
                    IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());
					wxGxRemoteServersUI* pGxRemoteServersUI = dynamic_cast<wxGxRemoteServersUI*>(pGxObject.get());
					if(pGxRemoteServersUI)
					{
						wxWindow* pParentWnd = dynamic_cast<wxWindow*>(pGxApp);
						return pGxRemoteServersUI->CreateConnection(pParentWnd, false);
					}
				}
			}
			return;
		case 2:	//connect
		case 3:	//disconnect
			{
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
					wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pGxCatalogUI->GetSelection();
                    IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());
					wxGxRemoteServerUI* pGxRemoteServerUI = dynamic_cast<wxGxRemoteServerUI*>(pGxObject.get());
					if(pGxRemoteServerUI)
						if(m_subtype == 2)
							pGxRemoteServerUI->Connect();
						else
							pGxRemoteServerUI->Disconnect();
				}
			}
			return;
		default:
			return;
	}
}

bool wxGISRemoteCmd::OnCreate(IApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISRemoteCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Search servers"));
		case 1:	
			return wxString(_("Create connection to server"));
		case 2:	
			return wxString(_("Connect to server"));
		case 3:	
			return wxString(_("Disconnect from server"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISRemoteCmd::GetCount(void)
{
	return 4;
}

