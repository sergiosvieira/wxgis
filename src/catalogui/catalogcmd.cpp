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
#include "../../art/oper_16.xpm"

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalogui/catalogui.h"
//#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/catalogui/gxlocationcombobox.h"

#include "wxgis/framework/progressor.h"
#include "wxgis/datasource/datasource.h"

#include <wx/dirdlg.h>
#include <wx/file.h>

#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/gxfile.h"
#include "wxgis/carto/mapview.h"


//	0	Up One Level
//	1	Connect Folder
//	2	Disconnect Folder
//	3	Location
//  4   Delete Item
//  5   Back
//  6   Forward
//  7   Create Folder
//	8	Test change proj
//	9	Refresh
//  10  ?

IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogMainCmd, wxObject)

wxGISCatalogMainCmd::wxGISCatalogMainCmd(void)
{
	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(folder_conn_16_xpm));	//4
	m_ImageList.Add(wxBitmap(location16_xpm));		//9
	m_ImageList.Add(wxBitmap(oper_16_xpm));		    //3

    m_CreateTypesArray.Add(wxString(_("Folder")));
    m_CreateTypesArray.Add(wxString(_("Folder Connection")));
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
		case 4:
			return m_ImageList.GetIcon(14);
		case 5:
			return m_ImageList.GetIcon(11);
		case 6:
			return m_ImageList.GetIcon(12);
		case 7:
			return m_ImageList.GetIcon(13);
		case 9:
			return m_ImageList.GetIcon(15);
		case 8:	
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
			return wxString(_("Test change proj"));
		case 9:	
			return wxString(_("Refresh"));
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
		case 4:	
		case 5:	
		case 6:	
		case 9:	
			return wxString(_("Catalog"));
		case 7:	
			return wxString(_("New"));
		case 8:	
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
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
				if(pSel->GetCount() == 0)
					return false;
				IGxObject* pGxObject = pSel->GetSelectedObjects(0);
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
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
				if(pSel->GetCount() == 0)
					return false;
				IGxObject* pGxObject = pSel->GetSelectedObjects(0);
				if(dynamic_cast<wxGxDiscConnection*>(pGxObject))
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
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); i++)
                {
                    IGxObject* pGxObject = pSel->GetSelectedObjects(i);
                    IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
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
                    IGxSelection* pSel = pCatalog->GetSelection();
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
                    IGxSelection* pSel = pCatalog->GetSelection();
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
                    IGxSelection* pSel = pCatalog->GetSelection();
                    if(pSel)
                    {
                        IGxObject* pCont = pSel->GetSelectedObjects(0);
                        if(pCont)
                        {
                            for(size_t i = 0; i < m_CreateTypesArray.GetCount(); i++)
                                if(pCont->GetCategory() == m_CreateTypesArray[i])
                                    return true;
                        }
                    }
                }
            }
			return false;
        }
		case 8://Gen SRS
			return true;
		case 9://Refresh
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
				if(pSel->GetCount() > 0)
					return true;
			}
			return false;
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
			return enumGISCommandDropDown;
		case 6://forward
			return enumGISCommandDropDown;
		case 7://create folder
			return enumGISCommandNormal;
		case 8://Gen SRS
			return enumGISCommandNormal;
		case 9://Refresh
			return enumGISCommandNormal;
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
			return wxString(_("Test GxDialog"));
		case 9:	
			return wxString(_("Refresh item"));
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
					IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
					if(pSel->GetCount() == 0)
						break;
					IGxObject* pGxObject = pSel->GetSelectedObjects(0);
                    if(!pGxObject)
                        return;
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
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
				IGxObject* pGxObject = pSel->GetSelectedObjects(0);
				wxGxDiscConnection* pDiscConnection = dynamic_cast<wxGxDiscConnection*>(pGxObject);
				if(pDiscConnection)
				{
					wxString sPath = pDiscConnection->GetPath();
					pGxApp->GetCatalog()->DisconnectFolder(sPath);
				}
			}
			return;
		}
		case 8:
            {
                wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
                wxGxObjectDialog dlg(pWnd, wxID_ANY, _("Select projection")); 
				dlg.SetAllowMultiSelect(false);
				dlg.AddFilter(new wxGxPrjFileFilter(), true);
				dlg.SetButtonCaption(_("Select"));
				dlg.SetStartingLocation(_("Coordinate Systems"));
                if(dlg.ShowModalOpen() == wxID_OK)
                {
                    GxObjectArray* pArr = dlg.GetSelectedObjects();
                    if(!pArr)
                        return;
                    if(pArr->size() < 0)
                         return;
				//	for(size_t i = 0; i < pArr->size(); i++)
				//	{
				//		wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pArr->at(i));
				//		if(!pGxPrjFile)
				//			return;
				//		OGRSpatialReference* pRef = pGxPrjFile->GetSpatialReference();
				//		if(pRef)
				//		{
				//			wxString sProjDir = wxString(wxT("d:\\temp\\srs\\Projected Coordinate Systems"));
				//			if(!wxDirExists(sProjDir))
				//				wxFileName::Mkdir(sProjDir, 0755, wxPATH_MKDIR_FULL);
				//			wxString sGeogDir = wxString(wxT("d:\\temp\\srs\\Geographic Coordinate Systems"));
				//			if(!wxDirExists(sGeogDir))
				//				wxFileName::Mkdir(sGeogDir, 0755, wxPATH_MKDIR_FULL);
				//			wxString sLoclDir = wxString(wxT("d:\\temp\\srs\\Vertical Coordinate Systems"));
				//			if(!wxDirExists(sLoclDir))
				//				wxFileName::Mkdir(sLoclDir, 0755, wxPATH_MKDIR_FULL);

				//			const char *pszProjection = pRef->GetAttrValue("PROJECTION"); 
				//			wxString sProjection;
				//			if(pszProjection)
				//				sProjection = wgMB2WX(pszProjection);
				//			if(pRef->IsProjected())
				//			{
				//				const char *pszProjcs = pRef->GetAttrValue("PROJCS");
				//				wxString sName = wgMB2WX(pszProjcs);
				//				wxString sFileName;
				//				int pos = sName.Find('/');
				//				if(pos != wxNOT_FOUND)
				//				{
				//					wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
				//					sSubFldr.Trim(true); sSubFldr.Trim(false);
				//					wxString sStorePath = sProjDir + wxFileName::GetPathSeparator() + sSubFldr;
				//					if(!wxDirExists(sStorePath))
				//						wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

				//					sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
				//					sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
				//					sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				//				}
				//				else
				//				{
				//					sFileName = sProjDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				//				}
				//				FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				//				if( fp != NULL )
				//				{
				//					char* pData(NULL);
				//					pRef->exportToWkt(&pData);
				//					VSIFWriteL( pData, 1, strlen(pData), fp );
				//					CPLFree(pData);
				//					VSIFCloseL(fp);
				//				}
				//			}
				//			else if(pRef->IsGeographic())
				//			{
				//				const char *pszProjcs = pRef->GetAttrValue("GEOGCS");
				//				wxString sName = wgMB2WX(pszProjcs);
				//				if(sName.Find(wxT("depre")) != wxNOT_FOUND)
				//					continue;
				//				wxString sFileName;
				//				int pos = sName.Find('/');
				//				if(pos != wxNOT_FOUND)
				//				{
				//					wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
				//					sSubFldr.Trim(true); sSubFldr.Trim(false);
				//					wxString sStorePath = sGeogDir + wxFileName::GetPathSeparator() + sSubFldr;
				//					if(!wxDirExists(sStorePath))
				//						wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

				//					sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
				//					sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
				//					sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				//				}
				//				else
				//				{
				//					sFileName = sGeogDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				//				}
				//				FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				//				if( fp != NULL )
				//				{
				//					char* pData(NULL);
				//					pRef->exportToWkt(&pData);
				//					VSIFWriteL( pData, 1, strlen(pData), fp );
				//					CPLFree(pData);
				//					VSIFCloseL(fp);
				//				}
				//			}
				//		}

				//		//const char *pszProjcs = pRef->GetAttrValue("PROJCS");
				//		//wxString sName = wgMB2WX(pszProjcs);
				//		//wxString sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
				//		//FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
				//		//if( fp != NULL )
				//		//{
				//		//	char* pData(NULL);
				//		//	SpaRef.exportToWkt(&pData);
				//		//	VSIFWriteL( pData, 1, strlen(pData), fp );

				//		//	CPLFree(pData);
				//		//	VSIFCloseL(fp);
				//		//}
				//	}

					wxGISMapView* pMapView(NULL);
					WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
					if(pWinArr)
					{
						for(size_t i = 0; i < pWinArr->size(); i++)
						{
							pMapView = dynamic_cast<wxGISMapView*>(pWinArr->at(i));
							if(pMapView)
								break;
						}
					}
					if(pMapView)
                    {
						wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pArr->at(0));
						if(!pGxPrjFile)
							return;
                        OGRSpatialReference* pRef = pGxPrjFile->GetSpatialReference();
                        if(pRef)
                        {
                            pMapView->SetSpatialReference(pRef);
                            pMapView->SetFullExtent();
                        }
                    }
                }


        //        wxString sProjDir = wxString(wxT("d:\\temp\\srs\\Projected Coordinate Systems"));
        //        if(!wxDirExists(sProjDir))
		      //      wxFileName::Mkdir(sProjDir, 0755, wxPATH_MKDIR_FULL);
        //        wxString sGeogDir = wxString(wxT("d:\\temp\\srs\\Geographic Coordinate Systems"));
        //        if(!wxDirExists(sGeogDir))
		      //      wxFileName::Mkdir(sGeogDir, 0755, wxPATH_MKDIR_FULL);
        //        wxString sLoclDir = wxString(wxT("d:\\temp\\srs\\Vertical Coordinate Systems"));
        //        if(!wxDirExists(sLoclDir))
		      //      wxFileName::Mkdir(sLoclDir, 0755, wxPATH_MKDIR_FULL);

        //        IStatusBar* pStatusBar = m_pApp->GetStatusBar();  
        //        wxGISProgressor* pProgressor = dynamic_cast<wxGISProgressor*>(pStatusBar->GetProgressor());
        //        if(pProgressor)
        //        {
        //            pProgressor->Show(true);
        //            pProgressor->SetRange(70000);

        //            wxString sDirPath;

        //            for(size_t i = 2213; i < 2214; i++)
        //            {
        //                OGRSpatialReference SpaRef;
        //                OGRErr err = SpaRef.importFromEPSG(i);
        //                if(err == OGRERR_NONE)
        //                {
        //                   const char *pszProjection = SpaRef.GetAttrValue("PROJECTION"); 
        //                   wxString sProjection;
        //                   if(pszProjection)
        //                       sProjection = wgMB2WX(pszProjection);
        //                   if(SpaRef.IsProjected())
        //                    {
        //                        const char *pszProjcs = SpaRef.GetAttrValue("PROJCS");
        //                        wxString sName = wgMB2WX(pszProjcs);
        //                        if(sName.Find(wxT("depre")) != wxNOT_FOUND)
        //                            continue;
        //                        wxString sFileName;
        //                        int pos = sName.Find('/');
        //                        if(pos != wxNOT_FOUND)
        //                        {
        //                            wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
        //                            sSubFldr.Trim(true); sSubFldr.Trim(false);
        //                            wxString sStorePath = sProjDir + wxFileName::GetPathSeparator() + sSubFldr;
        //                            if(!wxDirExists(sStorePath))
		      //                          wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

        //                            sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
        //                            sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
        //                            sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
        //                        }
        //                        else
        //                        {
        //                            sFileName = sProjDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
        //                        }
								//FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
								//if( fp != NULL )
        //                        //wxFile file;
        //                        //if(file.Create(sFileName))
        //                        {
        //                            char* pData(NULL);
        //                            SpaRef.exportToWkt(&pData);
        //                            //wxString Data = wgMB2WX(pData);
        //                            //file.Write(Data);
								//    VSIFWriteL( pData, 1, strlen(pData), fp );

        //                            CPLFree(pData);
								//	VSIFCloseL(fp);
        //                        }
        //                    }
        //                    else if(SpaRef.IsGeographic())
        //                    {
        //                        const char *pszProjcs = SpaRef.GetAttrValue("GEOGCS");
        //                        wxString sName = wgMB2WX(pszProjcs);
        //                        if(sName.Find(wxT("depre")) != wxNOT_FOUND)
        //                            continue;
        //                        wxString sFileName;
        //                        int pos = sName.Find('/');
        //                        if(pos != wxNOT_FOUND)
        //                        {
        //                            wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
        //                            sSubFldr.Trim(true); sSubFldr.Trim(false);
        //                            wxString sStorePath = sGeogDir + wxFileName::GetPathSeparator() + sSubFldr;
        //                            if(!wxDirExists(sStorePath))
		      //                          wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

        //                            sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
        //                            sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
        //                            sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".spr");
        //                        }
        //                        else
        //                        {
        //                            sFileName = sGeogDir + wxFileName::GetPathSeparator() + sName + wxT(".spr");
        //                        }
        //                        //wxFile file;
        //                        //if(file.Create(sFileName))
								//FILE *fp = VSIFOpenL( wgWX2MB(sFileName), "w");
								//if( fp != NULL )
        //                        {
        //                            char* pData(NULL);
        //                            SpaRef.exportToWkt(&pData);
        //                            //wxString Data = wgMB2WX(pData);
        //                            //file.Write(Data);
								//    VSIFWriteL( pData, 1, strlen(pData), fp );
        //                            CPLFree(pData);
								//	VSIFCloseL(fp);
        //                        }
        //                    }
        //                    else
        //                    {
        //                        sDirPath = wxString(wxT("d:\\temp\\srs\\Vertical Coordinate Systems"));
        //                    //bool bLoc = SpaRef.IsLocal();
        //                    }
        //                }
        //                pProgressor->SetValue(i);
        //            }
        //            pProgressor->Show(false);
        //        }
        //        pStatusBar->SetMessage(_("Done"));
            }			
			break;
        case 3:
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
                //get from config
                int answer = wxMessageBox(wxString::Format(_("Do you really want to delete %d item(s)"), pSel->GetCount()), wxString(_("Confirm")), wxCENTRE|wxYES_NO|wxICON_QUESTION, dynamic_cast<wxWindow*>(m_pApp) ); 
                if (answer == wxNO)
                    return;
                for(size_t i = 0; i < pSel->GetCount(); i++)
                {
                    IGxObject* pGxObject = pSel->GetSelectedObjects(i);
                    IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
                    if(pGxObjectEdit)
                        pGxObjectEdit->Delete();                           
                }
            }
			return;
        }

        case 4:
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp && GetEnabled())
            {
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();

                for(size_t i = 0; i < pSel->GetCount(); i++)
                {
                    IGxObject* pGxObject = pSel->GetSelectedObjects(i);
                    IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
                    if(pGxObjectEdit && pGxObjectEdit->CanDelete())
                    {
                        //pSel->Unselect(TempArr[i], IGxSelection::INIT_ALL);
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
			if(pGxApp && GetEnabled())
                return pGxApp->GetCatalog()->Undo();
			return;
        }
        case 6:
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp && GetEnabled())
                return pGxApp->GetCatalog()->Redo();
			return;
        }
        case 9:
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); i++)
                {
				    IGxObject* pGxObject = pSel->GetSelectedObjects(i);
                    pGxObject->Refresh();
                }
			}
    		break;
		}
        case 7:
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
		case 4:	
			return wxString(_("Delete selected item"));
		case 5:	
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp && GetEnabled())
			{
                IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
                int nPos = pSel->GetDoPos();
                wxString sPath = pSel->GetDoPath(nPos - 1);
                if(!sPath.IsEmpty())
                    return sPath;
            }
			return wxString(_("Go to previous location"));
        }
		case 6:	
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp && GetEnabled())
			{
                IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
                int nPos = pSel->GetDoPos();
                wxString sPath = pSel->GetDoPath(nPos + 1 == pSel->GetDoSize() ? nPos : nPos + 1);
                if(!sPath.IsEmpty())
                    return sPath;
            }
			return wxString(_("Go to next location"));//
        }
		case 7:	
			return wxString(_("Create new folder"));
		case 8:	
			return wxString(_("Test change proj"));
		case 9:	
			return wxString(_("Refresh selected item"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCatalogMainCmd::GetCount(void)
{
	return 10;
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
		case 4:	
		case 5:	
		case 6:	
		case 7:	
		case 8:	
		case 9:	
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
		case 4:	
		case 5:	
		case 6:	
		case 7:	
		case 8:	
		case 9:	
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
		case 4:	
		case 5:	
		case 6:	
		case 7:	
		case 8:	
		case 9:	
		default:
			return false;
	}
}

wxMenu* wxGISCatalogMainCmd::GetDropDownMenu(void)
{
	switch(m_subtype)
	{
		case 0:	
		case 1:	
		case 2:	
		case 3:	
		case 4:	
		case 5:
        {
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp /*&& GetEnabled()*/)
			{
                IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
                int nPos = pSel->GetDoPos();

                wxMenu* pMenu = new wxMenu();

                for(size_t i = nPos > 7 ? nPos - 7 : 0; i < nPos; i++)
                {
                    wxString sPath = pSel->GetDoPath(i);
                    if(!sPath.IsEmpty())
                        pMenu->Append(ID_MENUCMD + i, sPath);
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
                IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
                int nPos = pSel->GetDoPos();

                wxMenu* pMenu = new wxMenu();

                for(size_t i = nPos + 1; i < pSel->GetDoSize(); i++)
                {
                    if(i > nPos + 7)
                        break;
                    wxString sPath = pSel->GetDoPath(i);
                    if(!sPath.IsEmpty())
                        pMenu->Append(ID_MENUCMD + i, sPath);
                }
                return pMenu;                
            }
            return NULL;
        }            
		case 7:	
		case 8:	
		case 9:	
		default:
			return NULL;
	}
}

void wxGISCatalogMainCmd::OnDropDownCommand(int nID)
{
	IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
	if(pGxApp && GetEnabled())
	{
        IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
        int nPos = pSel->GetDoPos();
        int nNewPos = nID - ID_MENUCMD;
        if(nNewPos > nPos)
            pGxApp->GetCatalog()->Redo(nNewPos);
        else
            pGxApp->GetCatalog()->Undo(nNewPos);
    }
}

