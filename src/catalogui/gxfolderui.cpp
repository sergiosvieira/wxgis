/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderUI class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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
#include "wxgis/catalogui/gxfolderui.h"

#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/framework/progressdlg.h"

//---------------------------------------------------------------------------
// wxGxFolderUI
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxFolderUI, wxGxFolder)

wxGxFolderUI::wxGxFolderUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon & LargeIcon, const wxIcon & SmallIcon) : wxGxFolder(oParent, soName, soPath), wxGxAutoRenamer()
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;

    m_pGxViewToRename = NULL;
}

wxGxFolderUI::~wxGxFolderUI(void)
{
}

wxIcon wxGxFolderUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxFolderUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

void wxGxFolderUI::EditProperties(wxWindow *parent)
{
    //TODO: Linux folder props
#ifdef __WXMSW__
    SHELLEXECUTEINFO SEInf = {0};
    SEInf.cbSize = sizeof(SEInf);    
    SEInf.hwnd = parent->GetHWND();
    SEInf.lpVerb = wxT("properties");
    SEInf.fMask = SEE_MASK_INVOKEIDLIST;

    wxString myString(m_sPath, wxConvUTF8);
    LPCWSTR pszPathStr = myString.wc_str();

    SEInf.lpFile = pszPathStr;
    SEInf.nShow = SW_SHOW;
    ShellExecuteEx(&SEInf);
#endif
}

wxDragResult wxGxFolderUI::CanDrop(wxDragResult def)
{
    return def;
}

bool wxGxFolderUI::Drop(const wxArrayString& GxObjects, bool bMove)
{
    if(GxObjects.GetCount() == 0)
        return false;
    /*
    GxObjectArray Array;
    bool bValid(false);
    //1. try to get initiated GxObjects
	IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
	if(pGxObjectContainer)
    {
        for(size_t i = 0; i < filenames.GetCount(); ++i)
        {
            IGxObject* pGxObj(NULL);
            //as the names is internal check the /vsi
            wxString sCatalogNameProb;
            if(filenames[i].StartsWith(wxT("/vsi"), &sCatalogNameProb))
            {
                int nSlashPos = sCatalogNameProb.Find(wxT("/"));
                if(nSlashPos != wxNOT_FOUND)
                {
                    sCatalogNameProb = sCatalogNameProb.Right(sCatalogNameProb.Len() - nSlashPos - 1);
                    if(wxFileName::GetPathSeparator() == '/')
                        sCatalogNameProb.Replace(wxT("\\"), wxFileName::GetPathSeparator());
                    else
                        sCatalogNameProb.Replace(wxT("/"), wxFileName::GetPathSeparator());
                    pGxObj = pGxObjectContainer->SearchChild(sCatalogNameProb);
                }
            }
            else
            {
                pGxObj = pGxObjectContainer->SearchChild(filenames[i]);
            }
            if(pGxObj)
            {
                bValid = true;
                Array.push_back(pGxObj);
            }
        }
    }

    if(Array.empty())
    {
        char **papszFileList = NULL;
        CPLString szPath;
        CPLString szPathParent;
        IGxObject* pGxParentObj(NULL);

        for(size_t i = 0; i < filenames.GetCount(); ++i)
        {
            //Change to CPLString
            CPLString szFilePath(filenames[i].mb_str(wxConvUTF8));
            if(i == 0)
            {
                szPath = CPLGetPath(szFilePath);
                //drop /vsi
                if(szPath[0] == '/')
                {
                    for(size_t j = 1; j < szPath.length(); ++j)
                    {
                        if(szPath[j] == '/')
                        {
                            szPathParent = CPLString(&szPath[j + 1]);
                            break;
                        }
                    }
                }
                else
                    szPathParent = szPath;
                //get GxObj
            }
            papszFileList = CSLAddString( papszFileList, szFilePath );
        }

        if(!m_pCatalog->GetChildren(szPath, papszFileList, Array))
        {
            CSLDestroy( papszFileList );
            return false;
        }
        else
        {
            //Attach - cat & parent
            if(!szPathParent.empty())
            {
                pGxParentObj = pGxObjectContainer->SearchChild(wxString(szPathParent, wxConvUTF8));
                if(pGxParentObj)
                {
                    bValid = true;
                    for(size_t i = 0; i < Array.size(); ++i)
                        Array[i]->Attach(pGxParentObj, m_pCatalog);
                }
            }
        }
        CSLDestroy( papszFileList );
    }

    if(!bValid)
        return false;
    //create progress dialog
    wxString sTitle = wxString::Format(_("%s %d objects (files)"), bMove == true ? _("Move") : _("Copy"), filenames.GetCount());
    wxWindow* pParentWnd = dynamic_cast<wxWindow*>(GetApplication());
    wxGISProgressDlg ProgressDlg(sTitle, _("Begin operation..."), 100, pParentWnd, wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);

    for(size_t i = 0; i < Array.size(); ++i)
    {
		wxString sMessage = wxString::Format(_("%s %d object (file) from %d"), bMove == true ? _("Move") : _("Copy"), i + 1, Array.size());
		ProgressDlg.SetTitle(sMessage);
		ProgressDlg.PutMessage(sMessage);
        if(!ProgressDlg.Continue())
            break;

        IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(Array[i]);
        if(pGxObjectEdit)
        {
            //IGxObjectContainer* pGxParentObjectContainer = dynamic_cast<IGxObjectContainer*>(Array[i]->GetParent());
            if(bMove && pGxObjectEdit->CanMove(m_sPath))
            {
                if(!pGxObjectEdit->Move(m_sPath, &ProgressDlg))
                {
                    //pGxParentObjectContainer->DeleteChild(Array[i]);

                    //bool ret_code = AddChild(Array[i]);
                    //if(!ret_code)
                    //{
                    //    wxDELETE(Array[i]);
                    //}
                    //else
                    //{
                    //    m_pCatalog->ObjectAdded(Array[i]->GetID());
                    //    m_pCatalog->ObjectRefreshed(pGxParentObj->GetID());
                    //}
                    return false;
                }
            }
            else if(!bMove && pGxObjectEdit->CanCopy(m_sPath))
            {
                if(!pGxObjectEdit->Copy(m_sPath, &ProgressDlg))
                {
                    //bool ret_code = AddChild(Array[i]);
                    //if(!ret_code)
                    //{
                    //    wxDELETE(Array[i]);
                    //}
                    //else
                    //{
                    //    m_pCatalog->ObjectAdded(Array[i]->GetID());
                    //}
                    return false;
                }
            }
            else
                return false;
        }
    }
    m_bIsChildrenLoaded = false;
    LoadChildren();
    m_pCatalog->ObjectRefreshed(GetId());

	ProgressDlg.SetValue(ProgressDlg.GetValue() + 1);
	ProgressDlg.Destroy();*/
    return true;
}

