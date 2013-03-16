/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnectionUI class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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

#include "wxgis/catalogui/gxremoteconnui.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/catalogui/remoteconndlg.h"
#include "wxgis/catalogui/gxpostgisdatasetui.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/pg_vec_16.xpm"
#include "../../art/pg_vec_48.xpm"
#include "../../art/table_pg_16.xpm"
#include "../../art/table_pg_48.xpm"
#include "../../art/dbschema_16.xpm"
#include "../../art/dbschema_48.xpm"

//--------------------------------------------------------------
//class wxGxRemoteConnectionUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteConnectionUI, wxGxRemoteConnection)

BEGIN_EVENT_TABLE(wxGxRemoteConnectionUI, wxGxRemoteConnection)
    EVT_THREAD(EXIT_EVENT, wxGxRemoteConnectionUI::OnThreadFinished)
END_EVENT_TABLE()

wxGxRemoteConnectionUI::wxGxRemoteConnectionUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIconConn, const wxIcon &SmallIconConn, const wxIcon &LargeIconDisconn, const wxIcon &SmallIconDisconn) : wxGxRemoteConnection(oParent, soName, soPath)
{
    m_oLargeIconConn = LargeIconConn;
    m_oSmallIconConn = SmallIconConn;
    m_oLargeIconDisconn = LargeIconDisconn;
    m_oSmallIconDisconn = SmallIconDisconn;

    m_PendingId = wxNOT_FOUND;
}

wxGxRemoteConnectionUI::~wxGxRemoteConnectionUI(void)
{
}

wxIcon wxGxRemoteConnectionUI::GetLargeImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
        return m_oLargeIconConn;
    else
        return m_oLargeIconDisconn;
}

wxIcon wxGxRemoteConnectionUI::GetSmallImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
        return m_oSmallIconConn;
    else
        return m_oSmallIconDisconn;
}

void wxGxRemoteConnectionUI::EditProperties(wxWindow *parent)
{
	wxGISRemoteConnDlg dlg(m_sPath, parent);
	if(dlg.ShowModal() == wxID_OK)
	{
        Disconnect();
	}
}

bool wxGxRemoteConnectionUI::Invoke(wxWindow* pParentWnd)
{
    wxBusyCursor wait;
    //connect
	if(!Connect())
	{
		wxMessageBox(_("Connect failed!"), _("Error"), wxICON_ERROR | wxOK);
		return false;
	}

    return true;
}

bool wxGxRemoteConnectionUI::Connect(void)
{
    if(IsConnected())
        return true;
    bool bRes = true;
    //add pending item
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pCat)
    {
        m_PendingId = pCat->AddPending(GetId());
        //pCat->ObjectRefreshed(GetId());
    }
    //start thread to load schemes
    if(!CreateAndRunCheckThread())
        return false;
    return bRes;
}

bool wxGxRemoteConnectionUI::CreateAndRunCheckThread(void)
{
    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not create the load thread!"));
        return false;
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the load thread!"));
        return false;
    }
    return true;
}

//thread to load remote DB tables
//before exit we assume that no tables exist
wxThread::ExitCode wxGxRemoteConnectionUI::Entry()
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if(pDSet)
    {        
        if(!pDSet->Open())
        {
            wxThreadEvent event( wxEVT_THREAD, EXIT_EVENT );
            wxQueueEvent( this, event.Clone() );
            return (wxThread::ExitCode)1;
        }
    }
    wsDELETE(pDSet);

    LoadChildren();

    wxThreadEvent event( wxEVT_THREAD, EXIT_EVENT );
    wxQueueEvent( this, event.Clone() );

    return (wxThread::ExitCode)0;
}

void wxGxRemoteConnectionUI::OnThreadFinished(wxThreadEvent& event)
{
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pCat)
    {
        pCat->RemovePending(m_PendingId);
        m_PendingId = wxNOT_FOUND;
        pCat->ObjectRefreshed(GetId());
        pCat->ObjectChanged(GetId());
    }
}

wxGxRemoteDBSchema* wxGxRemoteConnectionUI::GetNewRemoteDBSchema(const wxString &sName, const wxArrayString &saTables, bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource *pwxGISRemoteConn)
{
    if(!m_oLargeIconFeatureClass.IsOk())
        m_oLargeIconFeatureClass = wxIcon(pg_vec_48_xpm);
    if(!m_oSmallIconFeatureClass.IsOk())
        m_oSmallIconFeatureClass = wxIcon(pg_vec_16_xpm);
    if(!m_oLargeIconTable.IsOk())
        m_oLargeIconTable = wxIcon(table_pg_48_xpm);
    if(!m_oSmallIconTable.IsOk())
        m_oSmallIconTable = wxIcon(table_pg_16_xpm);
    if(!m_oLargeIconSchema.IsOk())
        m_oLargeIconSchema = wxIcon(dbschema_48_xpm);
    if(!m_oSmallIconSchema.IsOk())
        m_oSmallIconSchema = wxIcon(dbschema_16_xpm);
    return wxStaticCast(new wxGxRemoteDBSchemaUI(saTables, bHasGeom, bHasGeog, bHasRaster, pwxGISRemoteConn, this, sName, "", m_oLargeIconSchema, m_oSmallIconSchema, m_oLargeIconFeatureClass, m_oSmallIconFeatureClass, m_oLargeIconTable, m_oSmallIconTable), wxGxRemoteDBSchema);
}

//--------------------------------------------------------------
//class wxGxRemoteDBSchemaUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteDBSchemaUI, wxGxRemoteDBSchema)

BEGIN_EVENT_TABLE(wxGxRemoteDBSchemaUI, wxGxRemoteDBSchema)
    EVT_THREAD(EXIT_EVENT, wxGxRemoteDBSchemaUI::OnThreadFinished)
END_EVENT_TABLE()

wxGxRemoteDBSchemaUI::wxGxRemoteDBSchemaUI(const wxArrayString &saTables, bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon, const wxIcon &LargeIconFeatureClass, const wxIcon &SmallIconFeatureClass, const wxIcon &LargeIconTable, const wxIcon &SmallIconTable) : wxGxRemoteDBSchema(saTables, bHasGeom, bHasGeog, bHasRaster, pwxGISRemoteConn, oParent, soName, soPath)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
    m_oLargeIconFeatureClass = LargeIconFeatureClass;
    m_oSmallIconFeatureClass = SmallIconFeatureClass;
    m_oLargeIconTable = LargeIconTable;
    m_oSmallIconTable = SmallIconTable;
    
    m_PendingId = wxNOT_FOUND;
}

wxGxRemoteDBSchemaUI::~wxGxRemoteDBSchemaUI(void)
{
}

void wxGxRemoteDBSchemaUI::EditProperties(wxWindow *parent)
{
    //TODO: change permissions and properties
}

wxIcon wxGxRemoteDBSchemaUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxRemoteDBSchemaUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

bool wxGxRemoteDBSchemaUI::HasChildren(void)
{
    if(m_bChildrenLoaded)
        return wxGxObjectContainer::HasChildren(); 
    if(GetThread() && GetThread()->IsRunning())
        return wxGxObjectContainer::HasChildren(); 

    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pCat)
    {
        m_PendingId = pCat->AddPending(GetId());
        //pCat->ObjectRefreshed(GetId());
    }
    //start thread to load schemes
    if(!CreateAndRunLoadChildrenThread())
        return false;

    return wxGxObjectContainer::HasChildren(); 
}

bool wxGxRemoteDBSchemaUI::CreateAndRunLoadChildrenThread(void)
{
    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not create the load thread!"));
        return false;
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the load thread!"));
        return false;
    }
    return true;
}

//thread to load remote DB tables
//before exit we assume that no tables exist
wxThread::ExitCode wxGxRemoteDBSchemaUI::Entry()
{
    LoadChildren();

    wxThreadEvent event( wxEVT_THREAD, EXIT_EVENT );
    wxQueueEvent( this, event.Clone() );

    return (wxThread::ExitCode)0;
}

void wxGxRemoteDBSchemaUI::OnThreadFinished(wxThreadEvent& event)
{
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pCat)
    {
        pCat->RemovePending(m_PendingId);
        m_PendingId = wxNOT_FOUND;
        pCat->ObjectRefreshed(GetId());
        pCat->ObjectChanged(GetId());
    }
}

void wxGxRemoteDBSchemaUI::AddTable(const wxString &sTableName, const wxGISEnumDatasetType eType)
{
    switch(eType)
    {
    case enumGISFeatureDataset:
        m_pwxGISRemoteConn->Reference();
        new wxGxPostGISFeatureDatasetUI(GetName(), m_pwxGISRemoteConn, this, sTableName, "", m_oLargeIconFeatureClass, m_oSmallIconFeatureClass);
        break;
    case enumGISRasterDataset:
        break;
    case enumGISTableDataset:
    default:
        m_pwxGISRemoteConn->Reference();
        new wxGxPostGISTableDatasetUI(GetName(), m_pwxGISRemoteConn, this, sTableName, "", m_oLargeIconTable, m_oSmallIconTable);
        break;
    };
/*
void wxGxRemoteDBSchemaUI::AddTable(CPLString &szName, CPLString &szSchema, bool bHasGeometry)
{
    IGxObject* pGxObject(NULL);
    if(bHasGeometry)
    {
        wxGxPostGISFeatureDatasetUI* pGxPostGISFeatureDataset = new wxGxPostGISFeatureDatasetUI(szName, szSchema, m_pwxGISRemoteConn, m_oLargeIconFeatureClass, m_oSmallIconFeatureClass);
        pGxObject = static_cast<IGxObject*>(pGxPostGISFeatureDataset);
    }
    else
    {
        wxGxPostGISTableDatasetUI* pGxPostGISTableDataset = new wxGxPostGISTableDatasetUI(szName, szSchema, m_pwxGISRemoteConn, m_oLargeIconTable, m_oSmallIconTable);
        pGxObject = static_cast<IGxObject*>(pGxPostGISTableDataset);
    }
//    switch(eType)
//    {
//    case enumGISFeatureDataset:
//        {
//            wxGxPostGISFeatureDatasetUI* pGxPostGISFeatureDataset = new wxGxPostGISFeatureDatasetUI(pGISDataset->GetPath(), pGISDataset, m_oLargeIconFeatureClass, m_oSmallIconFeatureClass);
//            pGxObject = static_cast<IGxObject*>(pGxPostGISFeatureDataset);
//        }
//        break;
//    case enumGISTableDataset:
//        {
//            wxGxPostGISTableDatasetUI* pGxPostGISTableDataset = new wxGxPostGISTableDatasetUI(pGISDataset->GetPath(), pGISDataset, m_oLargeIconTable, m_oSmallIconTable);
//            pGxObject = static_cast<IGxObject*>(pGxPostGISTableDataset);
//        }
//        break;
//    case enumGISRasterDataset:
//        break;
//    default:
//    case enumGISContainer:
//        break;
//    };


    if(pGxObject)
    {
	    bool ret_code = AddChild(pGxObject);
	    if(!ret_code)
		    wxDELETE(pGxObject);
    }*/
}

#endif //wxGIS_USE_POSTGRES