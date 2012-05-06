/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Create New Commands class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/catalogui/createnewcmd.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/remoteconndlg.h"
#include "wxgis/catalogui/gxremoteconnui.h"
#include "wxgis/datasource/sysop.h"

#include "../../art/rdb_create.xpm"
#include "../../art/rdb_conn_16.xpm"
#include "../../art/rdb_conn_48.xpm"
#include "../../art/rdb_disconn_16.xpm"
#include "../../art/rdb_disconn_48.xpm"

//	0	Create new remote connection
//  1   ?

IMPLEMENT_DYNAMIC_CLASS(wxGISCreateNewCmd, wxObject)

wxGISCreateNewCmd::wxGISCreateNewCmd(void)
{
}

wxGISCreateNewCmd::~wxGISCreateNewCmd(void)
{
}

wxIcon wxGISCreateNewCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if(!m_IconCreateRemoteConn.IsOk())
				m_IconCreateRemoteConn = wxIcon(rdb_create_xpm);
			return m_IconCreateRemoteConn;
		default:
			return wxNullIcon;
	}
}

wxString wxGISCreateNewCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("&Remote connection"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCreateNewCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("New"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCreateNewCmd::GetChecked(void)
{
	return false;
}

bool wxGISCreateNewCmd::GetEnabled(void)
{
	switch(m_subtype)
	{
		case 0://Create new remote connection
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                IGxCatalog* pCatalog = pGxApp->GetCatalog();
                if(pCatalog)
                {
                    wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pCatalog);
 					if(!pGxCatalogUI)
						return false;
                   IGxSelection* pSel = pGxCatalogUI->GetSelection();
                    if(pSel)
                    {
						IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(0));
                        IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
                        if(pCont)
                            return pCont->CanCreate(enumGISContainer, enumContRemoteConnection);
                    }
                }
            }
			return false;
        }
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCreateNewCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Create new remote connection
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCreateNewCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Create new remote connection"));
		default:
			return wxEmptyString;
	}
}

void wxGISCreateNewCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
            {
			    IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			    if(pGxApp)
			    {
                    IGxCatalog* pCatalog = pGxApp->GetCatalog();
                    if(pCatalog)
                    {
                        wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pCatalog);
                        IGxSelection* pSel = pGxCatalogUI->GetSelection();
                        if(pSel)
                        {
                            //create folder
							IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
							CPLString pszConnFolder = pGxObject->GetInternalName();
							CPLString pszConnName(CheckUniqName(pszConnFolder, wxString(_("new remote connection")), wxString(wxT("xconn"))).mb_str(wxConvUTF8));

							wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
							wxGISRemoteConnDlg dlg(CPLFormFilename(pszConnFolder, pszConnName, "xconn"), pWnd);
							if(dlg.ShowModal() == wxID_OK)
							{
								//create GxObject
								if(!m_LargeConnIcon.IsOk())
									m_LargeConnIcon = wxIcon(rdb_conn_48_xpm);
								if(!m_SmallConnIcon.IsOk())
									m_SmallConnIcon = wxIcon(rdb_conn_16_xpm);
								if(!m_LargeDisconnIcon.IsOk())
									m_LargeDisconnIcon = wxIcon(rdb_disconn_48_xpm);
								if(!m_SmallDisconnIcon.IsOk())
									m_SmallDisconnIcon = wxIcon(rdb_disconn_16_xpm);

								wxGxRemoteConnectionUI* pConn = new wxGxRemoteConnectionUI(dlg.GetPath(), dlg.GetName(), m_LargeConnIcon, m_SmallConnIcon, m_LargeDisconnIcon, m_SmallDisconnIcon);
								IGxObject* pGxConn = static_cast<IGxObject*>(pConn);
								IGxObjectContainer* pObjCont = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
								pObjCont->AddChild(pGxConn);
								pCatalog->ObjectAdded(pGxConn->GetID());
								//wait while added new GxObject to views
								wxYield();
								//begin rename GxObject
								wxWindow* pFocusWnd = wxWindow::FindFocus();
								IGxView* pGxView = dynamic_cast<IGxView*>(pFocusWnd);
								if(pGxView)
									pGxView->BeginRename(pGxConn->GetID());
							}
                        }
                    }
                }
            }
            break;
		default:
			return;
	}
}

bool wxGISCreateNewCmd::OnCreate(IFrameApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCreateNewCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Create new remote connection"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCreateNewCmd::GetCount(void)
{
	return 1;
}
