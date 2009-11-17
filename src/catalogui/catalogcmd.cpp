/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Main Commands class.
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
#include "wxgis/catalogui/catalogcmd.h"
#include "../../art/folder_conn_16.xpm"
#include "../../art/location16.xpm"
#include "wxgis/catalog/catalog.h"
#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/catalogui/gxlocationcombobox.h"

#include <wx/dirdlg.h>

//	0	Up One Level
//	1	Connect Folder
//	2	Disconnect Folder
//	3	Location
//	4	?

IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogMainCmd, wxObject)


wxGISCatalogMainCmd::wxGISCatalogMainCmd(void)
{
	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(folder_conn_16_xpm));	//4
	m_ImageList.Add(wxBitmap(location16_xpm));		//7
}

wxGISCatalogMainCmd::~wxGISCatalogMainCmd(void)
{
}

wxIcon wxGISCatalogMainCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			return m_ImageList.GetIcon(10);
		case 1:
			return m_ImageList.GetIcon(2);
		case 2:
			return m_ImageList.GetIcon(3);
		case 3:
			return m_ImageList.GetIcon(5);
		default:
			return wxIcon();
	}
}

wxString wxGISCatalogMainCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("&Up One Level"));
		case 1:	
			return wxString(_("&Connect folder"));
		case 2:	
			return wxString(_("&Disconnect folder"));
		case 3:	
			return wxString(_("Location"));
		default:
			return wxString();
	}
}

wxString wxGISCatalogMainCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:	
		case 1:	
		case 2:	
		case 3:	
			return wxString(_("Catalog"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCatalogMainCmd::GetChecked(void)
{
	return false;
}

bool wxGISCatalogMainCmd::GetEnabled(void)
{
	switch(m_subtype)
	{
		case 0://Up One Level
		{
			//check if not root
			wxGxApplication* pGxApp = dynamic_cast<wxGxApplication*>(m_pApp);
			if(pGxApp)
			{
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
				if(pSel->GetCount() == 0)
					return false;
				GxObjectArray* pArr = pSel->GetSelectedObjects();
				IGxObject* pGxObject = pArr->at(0);
				IGxObject* pParentGxObject = pGxObject->GetParent();
				if(!pParentGxObject)
					return false;
			}
			return true;
		}
		case 1:
			return true;
		case 2:
		{
			//check if dynamic_cast<wxGxDiscConnection*>
			wxGxApplication* pGxApp = dynamic_cast<wxGxApplication*>(m_pApp);
			if(pGxApp)
			{
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
				if(pSel->GetCount() == 0)
					return false;
				GxObjectArray* pArr = pSel->GetSelectedObjects();
				IGxObject* pGxObject = pArr->at(0);
				if(dynamic_cast<wxGxDiscConnection*>(pGxObject))
					return true;
			}
			return false;
		}
		case 3:
			return true;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCatalogMainCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Up One Level
		case 1://Connect folder
		case 2://Disconnect folder
			return enumGISCommandNormal;
		case 3://location
			return enumGISCommandControl;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCatalogMainCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Select parent element"));
		case 1:	
			return wxString(_("Connect folder"));
		case 2:	
			return wxString(_("Disconnect folder"));
		case 3:	
			return wxString(_("Set or get location"));
		default:
			return wxString();
	}
}

void wxGISCatalogMainCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:	
			{
				wxGxApplication* pGxApp = dynamic_cast<wxGxApplication*>(m_pApp);
				if(pGxApp)
				{
					IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
					if(pSel->GetCount() == 0)
						break;
					GxObjectArray* pArr = pSel->GetSelectedObjects();
					IGxObject* pGxObject = pArr->at(0);
					IGxObject* pParentGxObject = pGxObject->GetParent();
					if(dynamic_cast<IGxObjectContainer*>(pParentGxObject))
					{
						pSel->Select(pParentGxObject, false, IGxSelection::INIT_ALL);
					}
					else
					{
						IGxObject* pGrandParentGxObject = pParentGxObject->GetParent();
						pSel->Select(pGrandParentGxObject, false, IGxSelection::INIT_ALL);
					}
				}
			}
			break;
		case 1:	
		{
			wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a folder to connect")));
			if(dlg.ShowModal() == wxID_OK)
			{
				wxGxApplication* pGxApp = dynamic_cast<wxGxApplication*>(m_pApp);
				if(pGxApp)
				{
					wxString sPath = dlg.GetPath();
					pGxApp->GetCatalog()->ConnectFolder(sPath);					
				}
			}
			return;
		}
		case 2:	
		{
			wxGxApplication* pGxApp = dynamic_cast<wxGxApplication*>(m_pApp);
			if(pGxApp)
			{
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
				GxObjectArray* pArr = pSel->GetSelectedObjects();
				IGxObject* pGxObject = pArr->at(0);
				wxGxDiscConnection* pDiscConnection = dynamic_cast<wxGxDiscConnection*>(pGxObject);
				if(pDiscConnection)
				{
					wxString sPath = pDiscConnection->GetPath();
					pGxApp->GetCatalog()->DisconnectFolder(sPath);
				}
			}
			return;
		}
		case 3:
		default:
			return;
	}
}

bool wxGISCatalogMainCmd::OnCreate(IApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCatalogMainCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Up One Level"));
		case 1:	
			return wxString(_("Connect folder"));
		case 2:	
			return wxString(_("Disconnect folder"));
		case 3:	
			return wxString(_("Set or get location"));
		default:
			return wxString();
	}
}

unsigned char wxGISCatalogMainCmd::GetCount(void)
{
	return 4;
}

IToolBarControl* wxGISCatalogMainCmd::GetControl(void)
{
	switch(m_subtype)
	{
		case 0:	
		case 1:	
		case 2:	
			return NULL;
		case 3:	
			//if(!m_pGxLocationComboBox)
			{
				wxArrayString PathArray;
				wxGxLocationComboBox* pGxLocationComboBox = new wxGxLocationComboBox(dynamic_cast<wxWindow*>(m_pApp), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 400, 22 ), PathArray);
				return static_cast<IToolBarControl*>(pGxLocationComboBox);
			}
		default:
			return NULL;
	}
}

wxString wxGISCatalogMainCmd::GetToolLabel(void)
{
	switch(m_subtype)
	{
		case 0:	
		case 1:	
		case 2:	
			return wxEmptyString;
		case 3:	
			return wxString(_("Path: "));
		default:
			return wxEmptyString;
	}
}

bool wxGISCatalogMainCmd::HasToolLabel(void)
{
	switch(m_subtype)
	{
		case 0:	
		case 1:	
		case 2:	
			return false;
		case 3:	
			return true;
		default:
			return false;
	}
}
