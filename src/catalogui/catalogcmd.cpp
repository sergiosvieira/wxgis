/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Main Commands class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/catalogui/gxlocationcombobox.h"

#include "wxgis/framework/progressor.h"

#include "wxgis/datasource/datasource.h"
#include "wxgis/datasource/datacontainer.h"
#include "wxgis/datasource/sysop.h"

#include "wxgis/catalogui/gxfolderui.h"

#include "../../art/delete.xpm"
#include "../../art/edit.xpm"

#include "../../art/folder_conn_new.xpm"
#include "../../art/folder_conn_del.xpm"
#include "../../art/folder_new.xpm"
#include "../../art/folder_up.xpm"

#include "../../art/view-refresh.xpm"
#include "../../art/go.xpm"
#include "../../art/properties.xpm"

#include "../../art/folder_16.xpm"
#include "../../art/folder_48.xpm"

#include "../../art/edit_copy.xpm"
#include "../../art/edit_cut.xpm"
#include "../../art/edit_paste.xpm"

#include <wx/dirdlg.h>
#include <wx/file.h>
#include <wx/clipbrd.h>
#include <wx/richmsgdlg.h> 

//	0	Up One Level
//	1	Connect Folder
//	2	Disconnect Folder
//	3	Location
//  4   Delete Item
//  5   Back
//  6   Forward
//  7   Create Folder
//	8	Rename
//	9	Refresh
//  10  Properties
//  11  Copy
//  12  Cut
//  13  Paste
//  14  ?

IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogMainCmd, wxObject)

wxGISCatalogMainCmd::wxGISCatalogMainCmd(void)
{
}

wxGISCatalogMainCmd::~wxGISCatalogMainCmd(void)
{
}

wxIcon wxGISCatalogMainCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if(!m_IconFolderUp.IsOk())
				m_IconFolderUp = wxIcon(folder_up_xpm);
			return m_IconFolderUp;
		case 1:
			if(!m_IconFolderConn.IsOk())
				m_IconFolderConn = wxIcon(folder_conn_new_xpm);
			return m_IconFolderConn;
		case 2:
			if(!m_IconFolderConnDel.IsOk())
				m_IconFolderConnDel = wxIcon(folder_conn_del_xpm);
			return m_IconFolderConnDel;
		case 4:
			if(!m_IconDel.IsOk())
				m_IconDel = wxIcon(delete_xpm);
			return m_IconDel;
		case 5:
			if(!m_IconGoPrev.IsOk())
			{
				wxBitmap oGoOrigin(go_xpm);
				wxImage oGoPrevious = oGoOrigin.ConvertToImage();
				oGoPrevious = oGoPrevious.Mirror(true);

				m_IconGoPrev.CopyFromBitmap(wxBitmap(oGoPrevious));
			}
			return m_IconGoPrev;
		case 6:
			if(!m_IconGoNext.IsOk())
				m_IconGoNext = wxIcon(go_xpm);
			return m_IconGoNext;
		case 7:
			if(!m_IconFolderNew.IsOk())
				m_IconFolderNew = wxIcon(folder_new_xpm);
			return m_IconFolderNew;
		case 8:	
			if(!m_IconEdit.IsOk())
				m_IconEdit = wxIcon(edit_xpm);
			return m_IconEdit;
		case 9:
			if(!m_IconViewRefresh.IsOk())
				m_IconViewRefresh = wxIcon(view_refresh_xpm);
			return m_IconViewRefresh;
		case 10:
			if(!m_IconProps.IsOk())
				m_IconProps = wxIcon(properties_xpm);
			return m_IconProps;
		case 11:
			if(!m_CopyIcon.IsOk())
				m_CopyIcon = wxIcon(edit_copy_xpm);
			return m_CopyIcon;
		case 12:
			if(!m_CutIcon.IsOk())
				m_CutIcon = wxIcon(edit_cut_xpm);
			return m_CutIcon;
		case 13:
			if(!m_PasteIcon.IsOk())
				m_PasteIcon = wxIcon(edit_paste_xpm);
			return m_PasteIcon;
		case 3:
		default:
			return wxNullIcon;
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
		case 4:	
			return wxString(_("Delete"));
		case 5:	
			return wxString(_("Back"));//
		case 6:	
			return wxString(_("Forward"));//
		case 7:	
			return wxString(_("Create folder"));
		case 8:	
			return wxString(_("Rename"));
		case 9:	
			return wxString(_("Refresh"));
		case 10:	
			return wxString(_("Properties"));
		case 11:	
			return wxString(_("Copy"));
		case 12:	
			return wxString(_("Cut"));
		case 13:	
			return wxString(_("Paste"));
		default:
			return wxEmptyString;
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
		case 5:	
		case 6:	
		case 9:	
			return wxString(_("Catalog"));
		case 7:	
			return wxString(_("New"));
		case 4:	
		case 8:	
		case 11:	
		case 12:	
		case 13:	
			return wxString(_("Edit"));
		case 10:	
			return wxString(_("Miscellaneous"));
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
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				if(!pGxCatalogUI)
					return false;
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
				if(pSel->GetCount() == 0)
					return false;
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
				if(!pGxObject)
					return false;
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
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				if(!pGxCatalogUI)
					return false;
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
				if(pSel->GetCount() == 0)
					return false;

				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
				if(dynamic_cast<wxGxDiscConnection*>(pGxObject.get()))
					return true;
			}
			return false;
		}
		case 3:
			return true;
		case 4:
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				if(!pGxCatalogUI)
					return false;
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
					IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject.get());
					if(pGxObjectEdit && pGxObjectEdit->CanDelete())
						return true;
                }
            }
			return false;
        }
		case 5:
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
                        return pSel->CanUndo();
                }
            }
			return false;
        }
		case 6:
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
                        return pSel->CanRedo();
                }
            }
			return false;
        }
        case 7:
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
                            return pCont->CanCreate(enumGISContainer, enumContUnknown);
                    }
                }
            }
			return false;
        }
		case 8://Rename
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				if(!pGxCatalogUI)
					return false;
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
                size_t nCounter(0);
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject.get());
                    if(pGxObjectEdit && pGxObjectEdit->CanRename())
                        nCounter++;
                }
                return nCounter == 1 ? true : false;
            }
			return false;
        }
		case 9://Refresh
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				if(!pGxCatalogUI)
					return false;
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
				if(pSel->GetCount() > 0)
					return true;
			}
			return false;
		}
		case 10://Properties
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				if(!pGxCatalogUI)
					return false;
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());

                IGxObjectEditUI* pGxObjectEditUI = dynamic_cast<IGxObjectEditUI*>(pGxObject.get());
                if(pGxObjectEditUI)
                    return true;
            }
			return false;
		}
        case 11://Copy
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				if(!pGxCatalogUI)
					return false;
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());
                IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject.get());
                if(pGxObjectEdit)
                    return pGxObjectEdit->CanCopy("");
            }
			return false;
        }
        case 12://Cut
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				if(!pGxCatalogUI)
					return false;
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());
                IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject.get());
                if(pGxObjectEdit)
                    return pGxObjectEdit->CanMove("");
            }
			return false;
        }
        case 13://Paste
        {
            wxClipboardLocker lockClip;
            return wxTheClipboard->IsSupported(wxDF_FILENAME) & wxTheClipboard->IsSupported(wxDF_TEXT);// | wxDF_BITMAP | wxDF_TIFF | wxDF_DIB | wxDF_UNICODETEXT | wxDF_HTML
        }
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
		case 4://delete
			return enumGISCommandNormal;
		case 5://back
		case 6://forward
			return enumGISCommandDropDown;
		case 7://Create folder
		case 8://Rename
		case 9://Refresh
		case 10://Properties
		case 11://copy
		case 12://cut
		case 13://paste
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
		case 4:	
			return wxString(_("Delete item"));
		case 5:	
			return wxString(_("Go to previous location"));
		case 6:	
			return wxString(_("Go to next location"));
		case 7:	
			return wxString(_("Create folder"));
		case 8:	
			return wxString(_("Rename item"));
		case 9:	
			return wxString(_("Refresh item"));
		case 10:	
			return wxString(_("Item properties"));
		case 11:	
			return wxString(_("Copy item(s)"));
		case 12:	
			return wxString(_("Cut item(s)"));
		case 13:	
			return wxString(_("Paste item(s)"));
		default:
			return wxEmptyString;
	}
}

void wxGISCatalogMainCmd::OnClick(void)
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
					if(pSel->GetCount() == 0)
						break;
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
                    if(!pGxObject)
                        return;
					IGxObject* pParentGxObject = pGxObject->GetParent();
					if(dynamic_cast<IGxObjectContainer*>(pParentGxObject))
					{
						pSel->Select(pParentGxObject->GetID(), false, IGxSelection::INIT_ALL);
					}
					else
					{
						IGxObject* pGrandParentGxObject = pParentGxObject->GetParent();
						pSel->Select(pGrandParentGxObject->GetID(), false, IGxSelection::INIT_ALL);
					}
				}
			}
			break;
		case 1:	
		{
			wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a folder to connect")));
			if(dlg.ShowModal() == wxID_OK)
			{
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
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
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
				wxGxDiscConnection* pDiscConnection = dynamic_cast<wxGxDiscConnection*>(pGxObject.get());
				if(pDiscConnection)
				{
					pGxObject.reset();
					CPLString sPath = pDiscConnection->GetInternalName();
					pGxApp->GetCatalog()->DisconnectFolder(sPath);
				}
			}
			return;
		}
		case 8:
            {
                wxWindow* pFocusWnd = wxWindow::FindFocus();
                IGxView* pGxView = dynamic_cast<IGxView*>(pFocusWnd);
                			
                IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
                IGxObject* pSelObj = NULL;
                if(pGxApp)
                {
                    wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
                    IGxSelection* pSel = pGxCatalogUI->GetSelection();
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());
                    pSelObj = pGxObject.get();
                }
                if(pGxView)
                    pGxView->BeginRename(pSelObj->GetID());
            }			
			break;
        case 4:
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
            {
                //ask to delete?
                bool bAskToDelete(true);
                wxXmlNode* pNode(NULL);
                
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
                IGxSelection* pSel = pGxCatalogUI->GetSelection();

                IGxCatalog* pCat = pGxApp->GetCatalog();
                if(pCat)
                {
                    IGISConfig*  pConfig = pCat->GetConfig();
                    if(pConfig)
                    {
                        pNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("catalog")));
                        if(pNode)
                            bAskToDelete = wxAtoi(pNode->GetAttribute(wxT("ask_delete"), wxT("1"))) == 1;
                        else
                            pNode = pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog")), true);
                    }
                }
                if(bAskToDelete)
                {
                    //show ask dialog
                    wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
					wxRichMessageDialog dlg(pWnd, wxString::Format(_("Do you really want to delete %d item(s)"), pSel->GetCount()), wxString(_("Delete confirm")), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION | wxSTAY_ON_TOP | wxCENTRE);
					dlg.SetExtendedMessage(wxString(_("The result of operation cannot be undone!\nThe deleted items will remove from disk and will not put to the recycled bin.")));
					dlg.ShowCheckBox("Use my choice and do not show this dialog in future");

					int nRes = dlg.ShowModal();
                    

                    if(dlg.IsCheckBoxChecked())
                    {
                        pNode->DeleteAttribute(wxT("ask_delete"));
                        pNode->AddAttribute(wxT("ask_delete"), wxT("0"));
                    }

                    if(nRes == wxID_NO)
                        return;
                }

                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject.get());
                    if(pGxObjectEdit && pGxObjectEdit->CanDelete())
                    {
						pGxObject.reset();
                        if(!pGxObjectEdit->Delete())
                        {
                            wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
                            int nRes = wxMessageBox(wxString::Format(_("Cannot delete '%s'\nContinue?"),pGxObject->GetName().c_str()), _("Error"), wxYES_NO | wxICON_QUESTION, pWnd);
                            if(nRes == wxNO)
                                return;
                        }
                    }
                }
            }
			return;
        }
        case 5:
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
            wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
			if(pGxCatalogUI && GetEnabled())
                return pGxCatalogUI->Undo();
			return;
        }
        case 6:
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
            wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
			if(pGxCatalogUI && GetEnabled())
                return pGxCatalogUI->Redo();
			return;
        }
        case 9:
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    pGxObject->Refresh();
                }
			}
    		break;
		}
        case 10:
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetLastSelectedObjectID());
                IGxObjectEditUI* pGxObjectEdit = dynamic_cast<IGxObjectEditUI*>(pGxObject.get());
                if(pGxObjectEdit)
                    pGxObjectEdit->EditProperties(dynamic_cast<wxWindow*>(m_pApp));
			}
    		break;
		}
        case 11://copy
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
                if(!pSel)
                    return;
                wxDataObjectComposite* pComp = new wxDataObjectComposite();
                pComp->Add(new wxTextDataObject(wxT("COPY")));
                //! Create simple file data object
                wxFileDataObject* fdo = new wxFileDataObject();
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject.get());
                    if(pGxObjectEdit && pGxObjectEdit->CanCopy(""))
                    {
                        wxString sSystemPath(pGxObject->GetInternalName(), wxConvUTF8);
                        fdo->AddFile(sSystemPath);
                    }
                }

                pComp->Add(fdo);

                //! Lock clipboard
                wxClipboardLocker locker;
                if(!locker)
                    wxMessageBox(_("Can't open clipboard"), _("Error"), wxOK | wxICON_ERROR);
                else
                {
                    //! Put data to clipboard
                    if(!wxTheClipboard->AddData(pComp))
                        wxMessageBox(_("Can't copy file(s) to the clipboard"), _("Error"), wxOK | wxICON_ERROR);
                }
            }
    		break;
		}
        case 12://cut
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
                if(!pSel)
                    return;
                wxDataObjectComposite* pComp = new wxDataObjectComposite();
                pComp->Add(new wxTextDataObject(wxT("MOVE")));
                //! Create simple file data object
                wxFileDataObject* fdo = new wxFileDataObject();
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject.get());
                    if(pGxObjectEdit && pGxObjectEdit->CanMove(""))
                    {
                        wxString sSystemPath(pGxObject->GetInternalName(), wxConvUTF8);
                        fdo->AddFile(sSystemPath);
                    }
                }

                pComp->Add(fdo);

                //! Lock clipboard
                wxClipboardLocker locker;
                if(!locker)
                    wxMessageBox(_("Can't open clipboard"), _("Error"), wxOK | wxICON_ERROR);
                else
                {
                    //! Put data to clipboard
                    if(!wxTheClipboard->AddData(pComp))
                        wxMessageBox(_("Can't move file(s) to the clipboard"), _("Error"), wxOK | wxICON_ERROR);
                }
            }
    		break;
		}
        case 13://paste
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pGxCatalogUI->GetSelection();
                if(!pSel)
                    return;
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
                IGxDropTarget* pTarget = dynamic_cast<IGxDropTarget*>(pGxObject.get());
                if(!pTarget)
                    return;

                wxClipboardLocker locker;
                if(!locker)
                    wxMessageBox(_("Can't open clipboard"), _("Error"), wxOK | wxICON_ERROR);
                else
                {
                    wxTextDataObject data;
                    wxTheClipboard->GetData( data );
                    bool bMove(false);
                    if(data.GetText() == wxString(wxT("MOVE")))
                        bMove = true;
                    else if(data.GetText() == wxString(wxT("COPY")))
                        bMove = false;
                    else
                        return;

                    wxFileDataObject filedata;
                    wxTheClipboard->GetData( filedata );
                    wxArrayString filenames = filedata.GetFilenames();

                    pTarget->Drop(filenames, bMove);
                }
            }
    		break;
		}
        case 7:
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
                            CPLString sFolderPath = CPLFormFilename(pGxObject->GetInternalName(), wxString(_("New folder")).mb_str(wxConvUTF8), NULL);
                            //CPLString sFolderPath = pObj->GetInternalName() + wxFileName::GetPathSeparator().mb_str(wxConvUTF8) + wxString(_("New folder")).mb_str(wxConvUTF8);
                            if(!CreateDir(sFolderPath))
                            {
                                wxMessageBox(_("Create folder error!"), _("Error"), wxICON_ERROR | wxOK );
                                return;
                            }
                            //create GxObject
							if(!m_LargeFolderIcon.IsOk())
								m_LargeFolderIcon = wxIcon(folder_48_xpm);
							if(!m_SmallFolderIcon.IsOk())
								m_SmallFolderIcon = wxIcon(folder_16_xpm);

                            wxGxFolderUI* pFolder = new wxGxFolderUI(sFolderPath, wxString(_("New folder")), m_LargeFolderIcon, m_SmallFolderIcon);
                            IGxObject* pGxFolder = static_cast<IGxObject*>(pFolder);
                            IGxObjectContainer* pObjCont = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
                            pObjCont->AddChild(pGxFolder);
                            pCatalog->ObjectAdded(pGxFolder->GetID());
							//wait while added new GxObject to views
							wxYield();
                            //begin rename GxObject
                            wxWindow* pFocusWnd = wxWindow::FindFocus();
                            IGxView* pGxView = dynamic_cast<IGxView*>(pFocusWnd);
                            if(pGxView)
                                pGxView->BeginRename(pGxFolder->GetID());
                        }
                    }
                }
            }
            break;
        case 3:
		default:
			return;
	}
}

bool wxGISCatalogMainCmd::OnCreate(IFrameApplication* pApp)
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
		case 4:	
			return wxString(_("Delete selected item"));
		case 5:	
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp && GetEnabled())
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
                IGxSelection* pSel = pGxCatalogUI->GetSelection();
                int nPos = pSel->GetDoPos();
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetDoID(nPos - 1));
                if(pGxObject)
                {
                    wxString sPath = pGxObject->GetFullName();
                    if(!sPath.IsEmpty())
                        return sPath;
                }
            }
			return wxString(_("Go to previous location"));
        }
		case 6:	
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp && GetEnabled())
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
                IGxSelection* pSel = pGxCatalogUI->GetSelection();
                int nPos = pSel->GetDoPos();
				IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetDoID(nPos + 1 == pSel->GetDoSize() ? nPos : nPos + 1));
                if(pGxObject)
                {
                    wxString sPath = pGxObject->GetFullName();
                    if(!sPath.IsEmpty())
                        return sPath;
                }
            }
			return wxString(_("Go to next location"));//
        }
		case 7:	
			return wxString(_("Create new folder"));
		case 8:	
			return wxString(_("Rename selected item"));
		case 9:	
			return wxString(_("Refresh selected item"));
		case 10:	
			return wxString(_("Show properties of selected item"));
		case 11:	
			return wxString(_("Copy selected item(s)"));
		case 12:	
			return wxString(_("Cut selected item(s)"));
		case 13:	
			return wxString(_("Paste selected item(s)"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCatalogMainCmd::GetCount(void)
{
	return 14;
}

IToolBarControl* wxGISCatalogMainCmd::GetControl(void)
{
	switch(m_subtype)
	{
		case 3:	
//			if(!m_pGxLocationComboBox)
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
		case 3:	
			return wxString(_("Path:   "));
		default:
			return wxEmptyString;
	}
}

bool wxGISCatalogMainCmd::HasToolLabel(void)
{
	switch(m_subtype)
	{
		case 3:	
			return true;
		default:
			return false;
	}
}

wxMenu* wxGISCatalogMainCmd::GetDropDownMenu(void)
{
	switch(m_subtype)
	{
		case 5:
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
                IGxSelection* pSel = pGxCatalogUI->GetSelection();
                int nPos = pSel->GetDoPos();

                wxMenu* pMenu = new wxMenu();

                for(size_t i = nPos > 7 ? nPos - 7 : 0; i < nPos; ++i)
                {
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetDoID(i));//pSel->GetSelectedObjectID(
                    if(pGxObject)
                    {
                        wxString sPath = pGxObject->GetFullName();
                        if(!sPath.IsEmpty())
                            pMenu->Append(ID_MENUCMD + i, sPath);
                        else
                        {
                            sPath = pGxObject->GetName();
                            if(!sPath.IsEmpty())
                                pMenu->Append(ID_MENUCMD + i, sPath);
                        }
                    }
                }
                return pMenu;                
            }
            return NULL;
        }            
		case 6:	
        {
            IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp /*&& GetEnabled()*/)
			{
                wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
                IGxSelection* pSel = pGxCatalogUI->GetSelection();
                int nPos = pSel->GetDoPos();

                wxMenu* pMenu = new wxMenu();

                for(size_t i = nPos + 1; i < pSel->GetDoSize(); ++i)
                {
                    if(i > nPos + 7)
                        break;
					IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetDoID(i));//pSel->GetSelectedObjectID(
                    if(pGxObject)
                    {
                        wxString sPath = pGxObject->GetFullName();
                        if(!sPath.IsEmpty())
                            pMenu->Append(ID_MENUCMD + i, sPath);
                        else
                        {
                            sPath = pGxObject->GetName();
                            if(!sPath.IsEmpty())
                                pMenu->Append(ID_MENUCMD + i, sPath);
                        }
                    }
                }
                return pMenu;                
            }
            return NULL;
        }            
		default:
			return NULL;
	}
}

void wxGISCatalogMainCmd::OnDropDownCommand(int nID)
{
	IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
	if(pGxApp && GetEnabled())
	{
        wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
        IGxSelection* pSel = pGxCatalogUI->GetSelection();
        int nPos = pSel->GetDoPos();
        int nNewPos = nID - ID_MENUCMD;
        if(nNewPos > nPos)
            pGxCatalogUI->Redo(nNewPos);
        else
            pGxCatalogUI->Undo(nNewPos);
    }
}

                //cast ctrl

    //            wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
    //            wxGxObjectDialog dlg(pWnd, wxID_ANY, _("Select projection")); 
				//dlg.SetAllowMultiSelect(false);
				//dlg.AddFilter(new wxGxPrjFileFilter(), true);
				//dlg.SetButtonCaption(_("Select"));
				//dlg.SetStartingLocation(_("Coordinate Systems"));
    //            if(dlg.ShowModalOpen() == wxID_OK)
    //            {
    //                GxObjectArray* pArr = dlg.GetSelectedObjects();
    //                if(!pArr)
    //                    return;
    //                if(pArr->size() < 0)
    //                     return;
				////	for(size_t i = 0; i < pArr->size(); ++i)
				////	{
				////		wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pArr->at(i));
				////		if(!pGxPrjFile)
				////			return;
				////		OGRSpatialReference* pRef = pGxPrjFile->GetSpatialReference();
				////		if(pRef)
				////		{
				////			wxString sProjDir = wxString(wxT("d:\\temp\\srs\\Projected Coordinate Systems"));
				////			if(!wxDirExists(sProjDir))
				////				wxFileName::Mkdir(sProjDir, 0755, wxPATH_MKDIR_FULL);
				////			wxString sGeogDir = wxString(wxT("d:\\temp\\srs\\Geographic Coordinate Systems"));
				////			if(!wxDirExists(sGeogDir))
				////				wxFileName::Mkdir(sGeogDir, 0755, wxPATH_MKDIR_FULL);
				////			wxString sLoclDir = wxString(wxT("d:\\temp\\srs\\Vertical Coordinate Systems"));
				////			if(!wxDirExists(sLoclDir))
				////				wxFileName::Mkdir(sLoclDir, 0755, wxPATH_MKDIR_FULL);

				////			const char *pszProjection = pRef->GetAttrValue("PROJECTION"); 
				////			wxString sProjection;
				////			if(pszProjection)
				////				sProjection = wgMB2WX(pszProjection);
				////			if(pRef->IsProjected())
				////			{
				////				const char *pszProjcs = pRef->GetAttrValue("PROJCS");
				////				wxString sName = wgMB2WX(pszProjcs);
				////				wxString sFileName;
				////				int pos = sName.Find('/');
				////				if(pos != wxNOT_FOUND)
				////				{
				////					wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
				////					sSubFldr.Trim(true); sSubFldr.Trim(false);
				////					wxString sStorePath = sProjDir + wxFileName::GetPathSeparator() + sSubFldr;
				////					if(!wxDirExists(sStorePath))
				////						wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

				////					sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
				////					sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
				////					sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////				}
				////				else
				////				{
				////					sFileName = sProjDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////				}
				////				FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				////				if( fp != NULL )
				////				{
				////					char* pData(NULL);
				////					pRef->exportToWkt(&pData);
				////					VSIFWriteL( pData, 1, strlen(pData), fp );
				////					CPLFree(pData);
				////					VSIFCloseL(fp);
				////				}
				////			}
				////			else if(pRef->IsGeographic())
				////			{
				////				const char *pszProjcs = pRef->GetAttrValue("GEOGCS");
				////				wxString sName = wgMB2WX(pszProjcs);
				////				if(sName.Find(wxT("depre")) != wxNOT_FOUND)
				////					continue;
				////				wxString sFileName;
				////				int pos = sName.Find('/');
				////				if(pos != wxNOT_FOUND)
				////				{
				////					wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
				////					sSubFldr.Trim(true); sSubFldr.Trim(false);
				////					wxString sStorePath = sGeogDir + wxFileName::GetPathSeparator() + sSubFldr;
				////					if(!wxDirExists(sStorePath))
				////						wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

				////					sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
				////					sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
				////					sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////				}
				////				else
				////				{
				////					sFileName = sGeogDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////				}
				////				FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				////				if( fp != NULL )
				////				{
				////					char* pData(NULL);
				////					pRef->exportToWkt(&pData);
				////					VSIFWriteL( pData, 1, strlen(pData), fp );
				////					CPLFree(pData);
				////					VSIFCloseL(fp);
				////				}
				////			}
				////		}

				////		//const char *pszProjcs = pRef->GetAttrValue("PROJCS");
				////		//wxString sName = wgMB2WX(pszProjcs);
				////		//wxString sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				////		//FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				////		//if( fp != NULL )
				////		//{
				////		//	char* pData(NULL);
				////		//	SpaRef.exportToWkt(&pData);
				////		//	VSIFWriteL( pData, 1, strlen(pData), fp );

				////		//	CPLFree(pData);
				////		//	VSIFCloseL(fp);
				////		//}
				////	}

				//	wxGISMapView* pMapView(NULL);
				//	WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
				//	if(pWinArr)
				//	{
				//		for(size_t i = 0; i < pWinArr->size(); ++i)
				//		{
				//			pMapView = dynamic_cast<wxGISMapView*>(pWinArr->at(i));
				//			if(pMapView)
				//				break;
				//		}
				//	}
				//	if(pMapView)
    //                {
				//		wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pArr->at(0));
				//		if(!pGxPrjFile)
				//			return;
    //                    OGRSpatialReference* pRef = pGxPrjFile->GetSpatialReference();
    //                    if(pRef)
    //                    {
    //                        pMapView->SetSpatialReference(pRef);
    //                        pMapView->SetFullExtent();
    //                    }
    //                }
    //            }


    //    //        wxString sProjDir = wxString(wxT("d:\\temp\\srs\\Projected Coordinate Systems"));
    //    //        if(!wxDirExists(sProjDir))
		  //    //      wxFileName::Mkdir(sProjDir, 0755, wxPATH_MKDIR_FULL);
    //    //        wxString sGeogDir = wxString(wxT("d:\\temp\\srs\\Geographic Coordinate Systems"));
    //    //        if(!wxDirExists(sGeogDir))
		  //    //      wxFileName::Mkdir(sGeogDir, 0755, wxPATH_MKDIR_FULL);
    //    //        wxString sLoclDir = wxString(wxT("d:\\temp\\srs\\Vertical Coordinate Systems"));
    //    //        if(!wxDirExists(sLoclDir))
		  //    //      wxFileName::Mkdir(sLoclDir, 0755, wxPATH_MKDIR_FULL);

    //    //        IStatusBar* pStatusBar = m_pApp->GetStatusBar();  
    //    //        wxGISProgressor* pProgressor = dynamic_cast<wxGISProgressor*>(pStatusBar->GetProgressor());
    //    //        if(pProgressor)
    //    //        {
    //    //            pProgressor->Show(true);
    //    //            pProgressor->SetRange(70000);

    //    //            wxString sDirPath;

    //    //            for(size_t i = 2213; i < 2214; ++i)
    //    //            {
    //    //                OGRSpatialReference SpaRef;
    //    //                OGRErr err = SpaRef.importFromEPSG(i);
    //    //                if(err == OGRERR_NONE)
    //    //                {
    //    //                   const char *pszProjection = SpaRef.GetAttrValue("PROJECTION"); 
    //    //                   wxString sProjection;
    //    //                   if(pszProjection)
    //    //                       sProjection = wgMB2WX(pszProjection);
    //    //                   if(SpaRef.IsProjected())
    //    //                    {
    //    //                        const char *pszProjcs = SpaRef.GetAttrValue("PROJCS");
    //    //                        wxString sName = wgMB2WX(pszProjcs);
    //    //                        if(sName.Find(wxT("depre")) != wxNOT_FOUND)
    //    //                            continue;
    //    //                        wxString sFileName;
    //    //                        int pos = sName.Find('/');
    //    //                        if(pos != wxNOT_FOUND)
    //    //                        {
    //    //                            wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
    //    //                            sSubFldr.Trim(true); sSubFldr.Trim(false);
    //    //                            wxString sStorePath = sProjDir + wxFileName::GetPathSeparator() + sSubFldr;
    //    //                            if(!wxDirExists(sStorePath))
		  //    //                          wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

    //    //                            sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
    //    //                            sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
    //    //                            sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
    //    //                        }
    //    //                        else
    //    //                        {
    //    //                            sFileName = sProjDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
    //    //                        }
				//				//FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				//				//if( fp != NULL )
    //    //                        //wxFile file;
    //    //                        //if(file.Create(sFileName))
    //    //                        {
    //    //                            char* pData(NULL);
    //    //                            SpaRef.exportToWkt(&pData);
    //    //                            //wxString Data = wgMB2WX(pData);
    //    //                            //file.Write(Data);
				//				//    VSIFWriteL( pData, 1, strlen(pData), fp );

    //    //                            CPLFree(pData);
				//				//	VSIFCloseL(fp);
    //    //                        }
    //    //                    }
    //    //                    else if(SpaRef.IsGeographic())
    //    //                    {
    //    //                        const char *pszProjcs = SpaRef.GetAttrValue("GEOGCS");
    //    //                        wxString sName = wgMB2WX(pszProjcs);
    //    //                        if(sName.Find(wxT("depre")) != wxNOT_FOUND)
    //    //                            continue;
    //    //                        wxString sFileName;
    //    //                        int pos = sName.Find('/');
    //    //                        if(pos != wxNOT_FOUND)
    //    //                        {
    //    //                            wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
    //    //                            sSubFldr.Trim(true); sSubFldr.Trim(false);
    //    //                            wxString sStorePath = sGeogDir + wxFileName::GetPathSeparator() + sSubFldr;
    //    //                            if(!wxDirExists(sStorePath))
		  //    //                          wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

    //    //                            sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
    //    //                            sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
    //    //                            sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
    //    //                        }
    //    //                        else
    //    //                        {
    //    //                            sFileName = sGeogDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
    //    //                        }
    //    //                        //wxFile file;
    //    //                        //if(file.Create(sFileName))
				//				//FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				//				//if( fp != NULL )
    //    //                        {
    //    //                            char* pData(NULL);
    //    //                            SpaRef.exportToWkt(&pData);
    //    //                            //wxString Data = wgMB2WX(pData);
    //    //                            //file.Write(Data);
				//				//    VSIFWriteL( pData, 1, strlen(pData), fp );
    //    //                            CPLFree(pData);
				//				//	VSIFCloseL(fp);
    //    //                        }
    //    //                    }
    //    //                    else
    //    //                    {
    //    //                        sDirPath = wxString(wxT("d:\\temp\\srs\\Vertical Coordinate Systems"));
    //    //                    //bool bLoc = SpaRef.IsLocal();
    //    //                    }
    //    //                }
    //    //                pProgressor->SetValue(i);
    //    //            }
    //    //            pProgressor->Show(false);
    //    //        }
    //    //        pStatusBar->SetMessage(_("Done"));


