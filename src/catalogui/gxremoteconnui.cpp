/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnectionUI class.
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

#include "wxgis/catalogui/gxremoteconnui.h"
#include "wxgis/catalogui/remoteconndlg.h"
#include "wxgis/catalogui/gxpostgisdatasetui.h"
#include "wxgis/core/globalfn.h"


#include "../../art/pg_vec_16.xpm"
#include "../../art/pg_vec_48.xpm"
#include "../../art/table_pg_16.xpm"
#include "../../art/table_pg_48.xpm"
#include "../../art/dbschema_16.xpm"
#include "../../art/dbschema_48.xpm"

////-----------------------------------
//// wxChildLoaderThread
////-----------------------------------
//
//wxChildLoaderThread::wxChildLoaderThread(wxGxRemoteConnectionUI *pGxRemoteConnectionUI, int nBeg, int nEnd, IProgressor* pProgressor) : wxThread(wxTHREAD_DETACHED)
//{
//	m_pGxRemoteConnectionUI = pGxRemoteConnectionUI;
//	m_nBeg = nBeg;
//	m_nEnd = nEnd;
//    m_pProgressor = pProgressor;
//}
//
//void *wxChildLoaderThread::Entry()
//{
//    if(!m_pGxRemoteConnectionUI)
//        return (ExitCode)-1;
// 
//    for(int i = m_nBeg; i < m_nEnd; ++i)
//    {
//		if(TestDestroy())
//			return (ExitCode)-1;
//        m_pGxRemoteConnectionUI->AddSubDataset(i);
//        if(m_pProgressor)
//            m_pProgressor->SetValue(m_pProgressor->GetValue() + 1);
//    }
//	return NULL;
//}
//
//void wxChildLoaderThread::OnExit()
//{
//    if(m_pGxRemoteConnectionUI)
//        m_pGxRemoteConnectionUI->OnThreadExit(GetId());
//}

//--------------------------------------------------------------
//class wxGxRemoteConnectionUI
//--------------------------------------------------------------

wxGxRemoteConnectionUI::wxGxRemoteConnectionUI(CPLString soPath, wxString Name, wxIcon LargeIconConn, wxIcon SmallIconConn, wxIcon LargeIconDisconn, wxIcon SmallIconDisconn) : wxGxRemoteConnection(soPath, Name)
{
    m_oLargeIconConn = LargeIconConn;
    m_oSmallIconConn = SmallIconConn;
    m_oLargeIconDisconn = LargeIconDisconn;
    m_oSmallIconDisconn = SmallIconDisconn;

 //   m_pProgressor = nullptr;
	//m_pGxPendingUI = nullptr;
}

wxGxRemoteConnectionUI::~wxGxRemoteConnectionUI(void)
{
}

//void wxGxRemoteConnectionUI::Detach(void)
//{
//    for(auto itr = m_pmThreads.begin(); itr != m_pmThreads.end(); ++itr)
//    {
//        if(itr->second)
//            wgDELETE(itr->second, Kill());
//    }
//	EmptyChildren();
//    wxGxRemoteConnection::Detach();
//}


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
		m_sPath = dlg.GetPath();
		m_sName = dlg.GetName();
		m_pCatalog->ObjectChanged(GetID());
	}
}

bool wxGxRemoteConnectionUI::Invoke(wxWindow* pParentWnd)
{
    //EditProperties(pParentWnd);
    wxBusyCursor wait;
    //connect
    GetDataset();
	if(m_pwxGISDataset == nullptr)
	{
		wxMessageBox(_("Connect failed!"), _("Error"), wxICON_ERROR | wxOK);
		return false;
	}
    m_pCatalog->ObjectChanged(GetID());

    return true;
}


wxGxRemoteDBSchema* wxGxRemoteConnectionUI::GetNewRemoteDBSchema(CPLString &szName, wxGISPostgresDataSourceSPtr pwxGISRemoteCon)
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

//void wxGxRemoteConnectionUI::LoadChildren(void)
//{
//	wxBusyCursor wait;
//
//	if(m_bIsChildrenLoaded)
//		return;
//
//	if(m_pwxGISDataset == NULL || m_pwxGISRemoteConn == NULL)
//		return;
//
//    OGRPGDataSource* pDS = dynamic_cast<OGRPGDataSource*>(m_pwxGISRemoteConn->GetDataSource());
//    Oid nGeogOID = pDS->GetGeographyOID();
//    Oid nGeomOID = pDS->GetGeometryOID();
//    wxGISTableSPtr pGeostruct;
//    if(nGeogOID != 0)
//        pGeostruct = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISRemoteConn->ExecuteSQL(wxT("SELECT f_table_schema,f_table_name from geography_columns")));//,f_geography_column,type
//    else if(nGeomOID != 0)
//        pGeostruct = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISRemoteConn->ExecuteSQL(wxT("SELECT f_table_schema,f_table_name from geometry_columns")));//,f_geography_column,type
//
//    typedef struct _pgtabledata{
//        bool bHasGeometry;
//        CPLString sTableName;
//        CPLString sTableSchema;
//    }PGTABLEDATA;
//    
//    std::vector<PGTABLEDATA> aDBStruct;
//    if(pGeostruct)
//    {
//        OGRFeatureSPtr pFeature;
//        while((pFeature = pGeostruct->Next()) != nullptr)
//        {
//            PGTABLEDATA data = {true, pFeature->GetFieldAsString(1), pFeature->GetFieldAsString(0)};
//            aDBStruct.push_back(data);
//        }
//    }
//
//    std::vector<PGTABLEDATA> aDBStructOut;
//    OGRDataSource* poDS = m_pwxGISRemoteConn->GetDataSource();
//    for(size_t nIndex = 0; nIndex < m_pwxGISDataset->GetSubsetsCount(); ++nIndex)
//    {
//        OGRPGTableLayer* pPGTableLayer = dynamic_cast<OGRPGTableLayer*>(poDS->GetLayer(nIndex));
//    //    OGRLayer* poLayer = ;
//
//    //    wxGISTableSPtr pDS = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISDataset->GetSubset(i));
//    //    OGRLayer* pLayer = pDS->GetLayerRef();
//    //    CPLString szName = pLayer->GetName();
//
//    //    //OGRPGTableLayer* pPGTableLayer = dynamic_cast<OGRPGTableLayer*>(pDS->GetLayerRef());
//        CPLString sTableName (pPGTableLayer->GetTableName());
//        CPLString sTableSchema (pPGTableLayer->GetSchemaName());
//        bool bAdd(false);
//        for(size_t j = 0; j < aDBStruct.size(); ++j)
//        {
//            if(aDBStruct[j].sTableName == sTableName && aDBStruct[j].sTableSchema == sTableSchema)
//            {
//                aDBStructOut.push_back( aDBStruct[j] );
//                bAdd = true;
//                break;
//            }
//        }
//        if(!bAdd)
//        {
//            PGTABLEDATA data = {false, sTableName, sTableSchema};
//            aDBStructOut.push_back( data );
//        }
//    }
//
//	size_t nCountMax(m_pwxGISDataset->GetSubsetsCount());
//	if(nCountMax == 0)
//	{
//		m_bIsChildrenLoaded = true;
//		return;
//	}
//
//	//get statusbar progressor
//    if(!m_pProgressor)
//    {
//        IFrameApplication* pFApp = dynamic_cast<IFrameApplication*>(GetApplication());
//        if(pFApp)
//        {
//            IStatusBar* pStatusBar = pFApp->GetStatusBar();
//            if(pStatusBar)
//                m_pProgressor = pStatusBar->GetProgressor();
//        }
//    }
//
//    if(m_pProgressor)
//    {
//        m_pProgressor->SetRange(m_pwxGISDataset->GetSubsetsCount());
//        m_pProgressor->Show(true);
//    }
//
//
//	//add pending gxobject
//    if(!m_pGxPendingUI)
//    {
//        m_pGxPendingUI = new wxGxPendingUI();
//        if(!AddChild(m_pGxPendingUI))
//            wxDELETE(m_pGxPendingUI);
//    }
//
//    m_nRunningThreads = 0;
//	wxChildLoaderThread *thread = new wxChildLoaderThread(this, 0, nCountMax, m_pProgressor);
//	if(CreateAndRunThread(thread, wxT("wxChildLoaderThread"), wxT("ChildLoaderThread"))) 
//    {
//        m_nRunningThreads++;
//        m_pmThreads[thread->GetId()] = thread;//store thread in array
//    }
//
//
// //   size_t nCountMax(m_pwxGISDataset->GetSubsetsCount()), nCount(0);
// //   if(nCountMax > MAX_LAYERS)
// //       nCount = MAX_LAYERS;
// //   else
// //       nCount = nCountMax;
//
// //   for(size_t i = 0; i < nCount; ++i)
// //   {
// //       if(m_pProgressor)
// //           m_pProgressor->SetValue(i);
// //       AddSubDataset(i);
//	//}
//
// //   //start threads to load layers
// //   if(nCountMax > MAX_LAYERS)
// //   {
// //       int nCPUCount = wxThread::GetCPUCount();
// //       size_t nBeg(MAX_LAYERS), nEnd;
// //       size_t nPartSize = (nCountMax - nBeg) / nCPUCount;
// //       m_nRunningThreads = 0;
// //       for(int i = 0; i < nCPUCount; ++i)
// //       {
// //           if(i == nCPUCount - 1)
// //               nEnd = nCountMax;
// //           else
// //               nEnd = nPartSize * (i + 1);
// //           //create thread
//	//	    wxChildLoaderThread *thread = new wxChildLoaderThread(this, nBeg, nEnd, m_pProgressor);
//	//	    if(CreateAndRunThread(thread, wxT("wxChildLoaderThread"), wxT("ChildLoaderThread"))) 
// //           {
// //               m_nRunningThreads++;
// //               m_pmThreads[thread->GetId()] = thread;//store thread in array
// //           }
// //           nBeg = nEnd;
// //       }
// //   }
// //   else
// //   {
// //       if(m_pProgressor)
// //           m_pProgressor->Show(false);
// //   }
//
//	m_bIsChildrenLoaded = true;
//}
//
//void wxGxRemoteConnectionUI::OnThreadExit(wxThreadIdType nThreadID)
//{
//    m_pmThreads[nThreadID] = nullptr;
//    m_nRunningThreads--;
//    if(m_nRunningThreads <= 0)
//    {
//        if(m_pProgressor)
//            m_pProgressor->Show(false);
//        if(m_pGxPendingUI)
//            if(DeleteChild(static_cast<IGxObject*>(m_pGxPendingUI)))
//                m_pGxPendingUI = nullptr;
//    }
//    m_pCatalog->ObjectRefreshed(GetID());
//}
//
//void wxGxRemoteConnectionUI::AddSubDataset(size_t nIndex)
//{
//    wxGISDatasetSPtr pGISDataset = m_pwxGISDataset->GetSubset(nIndex);
//	if(!pGISDataset)
//		return;
//
//    wxGISEnumDatasetType eType = pGISDataset->GetType();
//    IGxObject* pGxObject(NULL);
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
//
//    if(pGxObject)
//    {
//		bool ret_code = AddChild(pGxObject);
//		if(!ret_code)
//			wxDELETE(pGxObject);
//    }
//}

//--------------------------------------------------------------
//class wxGxRemoteDBSchemaUI
//--------------------------------------------------------------
wxGxRemoteDBSchemaUI::wxGxRemoteDBSchemaUI(wxString &sName, wxGISPostgresDataSourceSPtr pwxGISRemoteConn, wxIcon LargeIcon, wxIcon SmallIcon, wxIcon LargeIconFeatureClass, wxIcon SmallIconFeatureClass, wxIcon LargeIconTable, wxIcon SmallIconTable) : wxGxRemoteDBSchema(sName, pwxGISRemoteConn)
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