/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  Catalog Main Commands class.
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
#include "wxgis/remoteserverui/remoteservercmd.h"
#include "wxgis/remoteserverui/serversearchdlg.h"
#include "wxgis/remoteserverui/gxremoteserversui.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/remoteserver_16.xpm"
#include "../../art/remoteservers_16.xpm"


//	0	Search Servers
//	1	Connect Server


IMPLEMENT_DYNAMIC_CLASS(wxGISRemoteCmd, wxObject)

wxGISRemoteCmd::wxGISRemoteCmd(void)
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
			return wxIcon(remoteservers_16_xpm);
		case 1:
			return wxIcon(remoteserver_16_xpm);
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
			return wxString(_("&Connect to server"));
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
	switch(m_subtype)
	{
		case 0://search server
		case 1://connect server
			{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
                wxGxRemoteServersUI* pGxRemoteServersUI = dynamic_cast<wxGxRemoteServersUI*>(pSel->GetLastSelectedObject());
                if(pGxRemoteServersUI)
                    return true;
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
		case 1://connect server
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
			return wxString(_("Connect to remote server"));
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
					wxGxRemoteServersUI* pGxRemoteServersUI = dynamic_cast<wxGxRemoteServersUI*>(pSel->GetLastSelectedObject());
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
					wxGxRemoteServersUI* pGxRemoteServersUI = dynamic_cast<wxGxRemoteServersUI*>(pSel->GetLastSelectedObject());
					if(pGxRemoteServersUI)
					{
						wxWindow* pParentWnd = dynamic_cast<wxWindow*>(pGxApp);
						return pGxRemoteServersUI->CreateConnection(pParentWnd, false);
					}
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
			return wxString(_("Connect to server"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISRemoteCmd::GetCount(void)
{
	return 2;
}

