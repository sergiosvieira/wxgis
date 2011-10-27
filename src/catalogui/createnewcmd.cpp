/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Create New Commands class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/rdb_create.xpm"

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
       //     {
			    //IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			    //if(pGxApp)
			    //{
       //             IGxCatalog* pCatalog = pGxApp->GetCatalog();
       //             if(pCatalog)
       //             {
       //                 wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pCatalog);
       //                 IGxSelection* pSel = pGxCatalogUI->GetSelection();
       //                 if(pSel)
       //                 {
       //                     //create folder
							//IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
       //                     CPLString sFolderPath = CPLFormFilename(pGxObject->GetInternalName(), wxString(_("New folder")).mb_str(wxConvUTF8), NULL);
       //                     //CPLString sFolderPath = pObj->GetInternalName() + wxFileName::GetPathSeparator().mb_str(wxConvUTF8) + wxString(_("New folder")).mb_str(wxConvUTF8);
       //                     if(!CreateDir(sFolderPath))
       //                     {
       //                         wxMessageBox(_("Create folder error!"), _("Error"), wxICON_ERROR | wxOK );
       //                         return;
       //                     }
       //                     //create GxObject
							//if(!m_LargeFolderIcon.IsOk())
							//	m_LargeFolderIcon = wxIcon(folder_48_xpm);
							//if(!m_SmallFolderIcon.IsOk())
							//	m_SmallFolderIcon = wxIcon(folder_16_xpm);

       //                     wxGxFolderUI* pFolder = new wxGxFolderUI(sFolderPath, wxString(_("New folder")), m_LargeFolderIcon, m_SmallFolderIcon);
       //                     IGxObject* pGxFolder = static_cast<IGxObject*>(pFolder);
       //                     IGxObjectContainer* pObjCont = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
       //                     pObjCont->AddChild(pGxFolder);
       //                     pCatalog->ObjectAdded(pGxFolder->GetID());
							////wait while added new GxObject to views
							//wxYield();
       //                     //begin rename GxObject
       //                     wxWindow* pFocusWnd = wxWindow::FindFocus();
       //                     IGxView* pGxView = dynamic_cast<IGxView*>(pFocusWnd);
       //                     if(pGxView)
       //                         pGxView->BeginRename(pGxFolder->GetID());
       //                 }
       //             }
       //         }
       //     }
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
