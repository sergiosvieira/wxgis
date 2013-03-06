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
#include "wxgis/catalogui/remoteconndlg.h"
#include "wxgis/catalogui/gxpostgisdatasetui.h"
#include "wxgis/catalog/gxcatalog.h"


#include "../../art/pg_vec_16.xpm"
#include "../../art/pg_vec_48.xpm"
#include "../../art/table_pg_16.xpm"
#include "../../art/table_pg_48.xpm"
#include "../../art/dbschema_16.xpm"
#include "../../art/dbschema_48.xpm"

//--------------------------------------------------------------
//class wxGxRemoteConnectionUI
//--------------------------------------------------------------

wxGxRemoteConnectionUI::wxGxRemoteConnectionUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, wxIcon LargeIconConn, wxIcon SmallIconConn, wxIcon LargeIconDisconn, wxIcon SmallIconDisconn) : wxGxRemoteConnection(oParent, soName, soPath)
{
    m_oLargeIconConn = LargeIconConn;
    m_oSmallIconConn = SmallIconConn;
    m_oLargeIconDisconn = LargeIconDisconn;
    m_oSmallIconDisconn = SmallIconDisconn;
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
        //if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
        //{
        //    EmptyChildren();
        //    m_pwxGISDataset->Close();
        //    m_pwxGISDataset.reset();
        //    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
        //    //GetDataset(false);
        //}
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

/*
wxGxRemoteDBSchema* wxGxRemoteConnectionUI::GetNewRemoteDBSchema(const CPLString &szName, wxGISPostgresDataSourceSPtr pwxGISRemoteCon)
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
    return static_cast<wxGxRemoteDBSchema*>(new wxGxRemoteDBSchemaUI(wxString(szName, wxConvUTF8), pwxGISRemoteCon, m_oLargeIconSchema, m_oSmallIconSchema, m_oLargeIconFeatureClass, m_oSmallIconFeatureClass, m_oLargeIconTable, m_oSmallIconTable));
}

//--------------------------------------------------------------
//class wxGxRemoteDBSchemaUI
//--------------------------------------------------------------
wxGxRemoteDBSchemaUI::wxGxRemoteDBSchemaUI(const wxString &sName, wxGISPostgresDataSourceSPtr pwxGISRemoteConn, wxIcon LargeIcon, wxIcon SmallIcon, wxIcon LargeIconFeatureClass, wxIcon SmallIconFeatureClass, wxIcon LargeIconTable, wxIcon SmallIconTable) : wxGxRemoteDBSchema(sName, pwxGISRemoteConn)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
    m_oLargeIconFeatureClass = LargeIconFeatureClass;
    m_oSmallIconFeatureClass = SmallIconFeatureClass;
    m_oLargeIconTable = LargeIconTable;
    m_oSmallIconTable = SmallIconTable;
}

wxGxRemoteDBSchemaUI::~wxGxRemoteDBSchemaUI(void)
{
}

void wxGxRemoteDBSchemaUI::EditProperties(wxWindow *parent)
{
}

wxIcon wxGxRemoteDBSchemaUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxRemoteDBSchemaUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

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
    }
}
*/