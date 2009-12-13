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

#include "wxgis/framework/progressor.h"
#include "wxgis/datasource.h"

#include <wx/dirdlg.h>
#include <wx/file.h>

//	0	Up One Level
//	1	Connect Folder
//	2	Disconnect Folder
//	3	Location
//	4	Generate SRS
//	5	?

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
		case 4:	
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
		case 4:	
			return wxString(_("Generate SRS"));
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
		case 4:	
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
		case 4://Gen SRS
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
		case 4://Gen SRS
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
			return wxString(_("Gen SRS"));
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
		case 4:
            {
                wxString sProjDir = wxString(wxT("e:\\temp\\srs\\Projected Coordinate Systems"));
                if(!wxDirExists(sProjDir))
		            wxFileName::Mkdir(sProjDir, 0755, wxPATH_MKDIR_FULL);
                wxString sGeogDir = wxString(wxT("e:\\temp\\srs\\Geographic Coordinate Systems"));
                if(!wxDirExists(sGeogDir))
		            wxFileName::Mkdir(sGeogDir, 0755, wxPATH_MKDIR_FULL);
                wxString sLoclDir = wxString(wxT("e:\\temp\\srs\\Vertical Coordinate Systems"));
                if(!wxDirExists(sLoclDir))
		            wxFileName::Mkdir(sLoclDir, 0755, wxPATH_MKDIR_FULL);

                IStatusBar* pStatusBar = m_pApp->GetStatusBar();  
                wxGISProgressor* pProgressor = dynamic_cast<wxGISProgressor*>(pStatusBar->GetProgressor());
                if(pProgressor)
                {
                    pProgressor->Show(true);
                    pProgressor->SetRange(70000);

                    wxString sDirPath;

                    for(size_t i = 0; i < 70000; i++)
                    {
                        OGRSpatialReference SpaRef;
                        OGRErr err = SpaRef.importFromEPSG(i);
                        if(err == OGRERR_NONE)
                        {
                           const char *pszProjection = SpaRef.GetAttrValue("PROJECTION"); 
                           wxString sProjection;
                           if(pszProjection)
                               sProjection = wgMB2WX(pszProjection);
                           if(SpaRef.IsProjected())
                            {
                                const char *pszProjcs = SpaRef.GetAttrValue("PROJCS");
                                wxString sName = wgMB2WX(pszProjcs);
                                if(sName.Find(wxT("depre")) != wxNOT_FOUND)
                                    continue;
                                wxString sFileName;
                                int pos = sName.Find('/');
                                if(pos != wxNOT_FOUND)
                                {
                                    wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
                                    sSubFldr.Trim(true); sSubFldr.Trim(false);
                                    wxString sStorePath = sProjDir + wxFileName::GetPathSeparator() + sSubFldr;
                                    if(!wxDirExists(sStorePath))
		                                wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

                                    sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
                                    sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
                                    sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".srml");
                                }
                                else
                                {
                                    sFileName = sProjDir + wxFileName::GetPathSeparator() + sName + wxT(".srml");
                                }
                                wxFile file;
                                if(file.Create(sFileName))
                                {
                                    char* pData(NULL);
                                    SpaRef.exportToXML(&pData);
                                    wxString Data = wgMB2WX(pData);
                                    file.Write(Data);
                                    CPLFree(pData);
                                }
                            }
                            else if(SpaRef.IsGeographic())
                            {
                                const char *pszProjcs = SpaRef.GetAttrValue("GEOGCS");
                                wxString sName = wgMB2WX(pszProjcs);
                                if(sName.Find(wxT("depre")) != wxNOT_FOUND)
                                    continue;
                                wxString sFileName;
                                int pos = sName.Find('/');
                                if(pos != wxNOT_FOUND)
                                {
                                    wxString sSubFldr = sName.Right(sName.Len() - pos - 1);
                                    sSubFldr.Trim(true); sSubFldr.Trim(false);
                                    wxString sStorePath = sGeogDir + wxFileName::GetPathSeparator() + sSubFldr;
                                    if(!wxDirExists(sStorePath))
		                                wxFileName::Mkdir(sStorePath, 0755, wxPATH_MKDIR_FULL);

                                    sName.Replace(wxString(wxT("/")), wxString(wxT(""))); 
                                    sName.Replace(wxString(wxT("  ")), wxString(wxT(" "))); 
                                    sFileName = sStorePath + wxFileName::GetPathSeparator() + sName + wxT(".srml");
                                }
                                else
                                {
                                    sFileName = sGeogDir + wxFileName::GetPathSeparator() + sName + wxT(".srml");
                                }
                                wxFile file;
                                if(file.Create(sFileName))
                                {
                                    char* pData(NULL);
                                    SpaRef.exportToXML(&pData);
                                    wxString Data = wgMB2WX(pData);
                                    file.Write(Data);
                                    CPLFree(pData);
                                }
                            }
                            else
                            {
                                sDirPath = wxString(wxT("e:\\temp\\srs\\Vertical Coordinate Systems"));
                            //bool bLoc = SpaRef.IsLocal();
                            }
                        }
                        pProgressor->SetValue(i);
                    }
                    pProgressor->Show(false);
                }
                pStatusBar->SetMessage(_("Done"));
            }			
			break;
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
		case 4:	
			return wxString(_("Gen SRS"));
		default:
			return wxString();
	}
}

unsigned char wxGISCatalogMainCmd::GetCount(void)
{
	return 5;
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
		default:
			return false;
	}
}
