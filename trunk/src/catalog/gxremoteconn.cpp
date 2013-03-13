/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnection class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/catalog/gxremoteconn.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/gxpostgisdataset.h"
#include "wxgis/datasource/postgisdataset.h"
#include "wxgis/catalog/gxcatalog.h"

//--------------------------------------------------------------
//class wxGxRemoteConnection
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxRemoteConnection, wxGxObjectContainer)

wxGxRemoteConnection::wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath)
{
    m_pwxGISDataset = NULL;
}

wxGxRemoteConnection::~wxGxRemoteConnection(void)
{
    wsDELETE(m_pwxGISDataset);
}

bool wxGxRemoteConnection::HasChildren()
{
    if(!Connect())
        return false;
    else
        return wxGxObjectContainer::HasChildren();
}

wxGISDataset* const wxGxRemoteConnection::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        wxGISPostgresDataSource* pDSet = new wxGISPostgresDataSource(m_sPath);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    m_pwxGISDataset->Reference();
    return m_pwxGISDataset;
}

bool wxGxRemoteConnection::Delete(void)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if(!pDSet)
        return false;

    bool bRet = pDSet->Delete();
    wsDELETE(pDSet);

    if( !bRet )
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }
    return true;
}

bool wxGxRemoteConnection::Rename(const wxString &sNewName)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if(!pDSet)
        return false;

	if(pDSet->IsOpened())
		pDSet->Close();

    bool bRet = pDSet->Rename(sNewName);
    wsDELETE(pDSet);

	if( !bRet )
	{
		const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
	}	
    return true;
}


bool wxGxRemoteConnection::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

	wxGISDataset* pDSet = GetDatasetFast();
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Copy(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Copy"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
		return false;	
    }
	
    return true;
}

bool wxGxRemoteConnection::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

	wxGISDataset* pDSet = GetDatasetFast();
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Move(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Move"), GetCategory().c_str(), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
		return false;	
    } 

    return true;
}

bool wxGxRemoteConnection::Connect(void)
{
    if(IsConnected())
        return true;
    bool bRes = true;
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if(pDSet)
    {
        bRes = pDSet->Open();
        if(!bRes)
            return bRes;

        LoadChildren();

        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    wsDELETE(pDSet);
    return bRes;
}

bool wxGxRemoteConnection::Disconnect(void)
{
    if(!IsConnected())
        return true;
    wxGISDataset* pDSet = GetDatasetFast();
    if(pDSet)
    {
        pDSet->Close();
        DestroyChildren();
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    wsDELETE(pDSet);
    return true;
}

bool wxGxRemoteConnection::IsConnected()
{
    wxGISDataset* pDSet = GetDatasetFast();
    return pDSet && pDSet->IsOpened();
}

void wxGxRemoteConnection::Refresh(void)
{
    DestroyChildren();
    LoadChildren();
    wxGxObject::Refresh();
}


void wxGxRemoteConnection::LoadChildren(void)
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    wxGISTable* pInfoScheme = wxDynamicCast(pDSet->ExecuteSQL(wxT("SELECT table_schema, table_name from information_schema.tables WHERE table_schema NOT LIKE 'pg_%' AND table_schema NOT LIKE 'information_schema'")), wxGISTable);//maybe more columns

    //create arraystring of table names for each scheme
    std::map<wxString, wxArrayString> DBSchema;

    wxFeatureCursor Cursor = pInfoScheme->Search();
    wxGISFeature Feature;
    wxArrayString saGeomCol, saGeogCol; 
    while( (Feature = Cursor.Next()).IsOk() )
    {
        wxString sScheme = Feature.GetFieldAsString(0);
        wxString sTable = Feature.GetFieldAsString(0);
        if(sTable.IsSameAs("geometry_columns", false))
        {
            saGeomCol.Add(sScheme + wxT(".") + sTable);
        }
        else if(sTable.IsSameAs("geography_columns", false))
        {
            saGeogCol.Add(sScheme + wxT(".") + sTable);
        }
        else
        {
            DBSchema[sScheme].Add(sTable);
        }
    }

    wxDELETE(pInfoScheme);



        ////get all schemes
        //SELECT * from information_schema.tables WHERE table_schema NOT LIKE 'pg_%' AND table_schema NOT LIKE 'information_schema';

        ////get geometry tables names and schemes
        //select * from geography_columns;
        //select * from public.geometry_columns;
        ////get inherits
        //
        //wxGISDataset* pGeoMetaData = pDSet->ExecuteSQL(wxT("SELECT f_table_schema,f_table_name from geography_columns"));
 
        //SELECT * FROM pg_class WHERE relname LIKE '%_columns';

/*
    OGRPGDataSource* pDS = dynamic_cast<OGRPGDataSource*>(m_pwxGISRemoteConn->GetDataSource());
    Oid nGeogOID = pDS->GetGeographyOID();
    Oid nGeomOID = pDS->GetGeometryOID();
    wxGISTableSPtr pGeostruct;
    if(nGeogOID != 0)
        pGeostruct = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISRemoteConn->ExecuteSQL(wxString(wxT("SELECT f_table_schema,f_table_name from geography_columns"))));//,f_geography_column,type
    else if(nGeomOID != 0)
        pGeostruct = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISRemoteConn->ExecuteSQL(wxString(wxT("SELECT f_table_schema,f_table_name from geometry_columns"))));//,f_geography_column,type

    std::vector<PGTABLEDATA> aDBStruct;
    if(pGeostruct)
    {
        OGRFeatureSPtr pFeature;
        while((pFeature = pGeostruct->Next()) != NULL)
        {
            PGTABLEDATA data = {true, pFeature->GetFieldAsString(1), pFeature->GetFieldAsString(0)};
            aDBStruct.push_back(data);
        }
    }

    std::vector<PGTABLEDATA> aDBStructOut;
    OGRDataSource* poDS = m_pwxGISRemoteConn->GetDataSource();
    for(size_t nIndex = 0; nIndex < m_pwxGISDataset->GetSubsetsCount(); ++nIndex)
    {
        OGRPGTableLayer* pPGTableLayer = dynamic_cast<OGRPGTableLayer*>(poDS->GetLayer(nIndex));
        CPLString sTableName (pPGTableLayer->GetTableName());
        CPLString sTableSchema (pPGTableLayer->GetSchemaName());
        bool bAdd(false);
        for(size_t j = 0; j < aDBStruct.size(); ++j)
        {
            if(aDBStruct[j].sTableName == sTableName && aDBStruct[j].sTableSchema == sTableSchema)
            {
                aDBStructOut.push_back( aDBStruct[j] );
                bAdd = true;
                break;
            }
        }

        if(!bAdd)
        {
            //check inherits
            CPLString osCommand;
            osCommand.Printf("SELECT pg_class.relname FROM pg_class WHERE oid = "
                                "(SELECT pg_inherits.inhparent FROM pg_inherits WHERE inhrelid = "
                                "(SELECT c.oid FROM pg_class c, pg_namespace n WHERE c.relname = '%s' AND c.relnamespace=n.oid AND n.nspname = '%s'))",
                                sTableName.c_str(), sTableSchema.c_str() );
            wxString sSQLExpression(osCommand, wxConvUTF8);
            wxGISTableSPtr pInherits = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISRemoteConn->ExecuteSQL(sSQLExpression));
            if(pInherits)
            {
                OGRFeatureSPtr pFeature;
                while((pFeature = pInherits->Next()) != NULL)
                {
                    CPLString soParentName = pFeature->GetFieldAsString(0);
                    for(size_t j = 0; j < aDBStruct.size(); ++j)
                    {
                        if(aDBStruct[j].sTableName == soParentName && aDBStruct[j].sTableSchema == sTableSchema)
                        {
                            PGTABLEDATA data = {true, sTableName, sTableSchema};
                            aDBStructOut.push_back(data);
                            bAdd = true;
                            break;
                        }
                    }
                }
            }
        }
        if(!bAdd)
        {
            PGTABLEDATA data = {false, sTableName, sTableSchema};
            aDBStructOut.push_back( data );
        }
    }

    std::map<CPLString, wxGxRemoteDBSchema*> DBSchema;

    for(size_t i = 0; i < aDBStructOut.size(); ++i)
    {
        wxGxRemoteDBSchema* pGxRemoteDBSchema = DBSchema[aDBStructOut[i].sTableSchema];
        if(pGxRemoteDBSchema)
            pGxRemoteDBSchema->AddTable(aDBStructOut[i].sTableName, aDBStructOut[i].sTableSchema, aDBStructOut[i].bHasGeometry);
        else
        {
            pGxRemoteDBSchema = GetNewRemoteDBSchema(aDBStructOut[i].sTableSchema, m_pwxGISRemoteConn);
            bool ret_code = AddChild(static_cast<IGxObject*>(pGxRemoteDBSchema));
		    if(!ret_code)
                wxDELETE(pGxRemoteDBSchema);
            else
            {
                pGxRemoteDBSchema->AddTable(aDBStructOut[i].sTableName, aDBStructOut[i].sTableSchema, aDBStructOut[i].bHasGeometry);
                DBSchema[aDBStructOut[i].sTableSchema] = pGxRemoteDBSchema;
            }
        }
    }

  //  for(auto itr = DBSchema.begin(); itr != DBSchema.end(); ++itr)
  //  {
  //      bool ret_code = AddChild(static_cast<IGxObject*>(itr->second));
		//if(!ret_code)
  //          wxDELETE(itr->second);
  //  }*/
}

/*
wxGxRemoteDBSchema* wxGxRemoteConnection::GetNewRemoteDBSchema(const CPLString &szName, wxGISPostgresDataSourceSPtr pwxGISRemoteCon)
{
    return new wxGxRemoteDBSchema(wxString(szName, wxConvUTF8), pwxGISRemoteCon);
}

*/
//--------------------------------------------------------------
//class wxGxRemoteDBSchema
//--------------------------------------------------------------
/*
wxGxRemoteDBSchema::wxGxRemoteDBSchema(const wxString &sName, wxGISPostgresDataSourceSPtr pwxGISRemoteConn)
{
	m_eType = enumContRemoteConnection;
    m_sName = sName;
    m_pwxGISRemoteConn = pwxGISRemoteConn;
}

wxGxRemoteDBSchema::~wxGxRemoteDBSchema(void)
{
}

void wxGxRemoteDBSchema::AddTable(CPLString &szName, CPLString &szSchema, bool bHasGeometry)
{
    IGxObject* pGxObject(NULL);
    if(bHasGeometry)
    {
        wxGxPostGISFeatureDataset* pGxPostGISFeatureDataset = new wxGxPostGISFeatureDataset(szName, szSchema, m_pwxGISRemoteConn);
        pGxObject = static_cast<IGxObject*>(pGxPostGISFeatureDataset);
    }
    else
    {
        wxGxPostGISTableDataset* pGxPostGISTableDataset = new wxGxPostGISTableDataset(szName, szSchema, m_pwxGISRemoteConn);
        pGxObject = static_cast<IGxObject*>(pGxPostGISTableDataset);
    }
    //    wxGISEnumDatasetType eType = pGISDataset->GetType();
    //    IGxObject* pGxObject(NULL);
    //    switch(eType)
    //    {
    //    case enumGISFeatureDataset:
    //        {
    //            wxGxPostGISFeatureDataset* pGxPostGISFeatureDataset = new wxGxPostGISFeatureDataset(pGISDataset->GetPath(), pGISDataset);
    //            pGxObject = static_cast<IGxObject*>(pGxPostGISFeatureDataset);
    //        }
    //        break;
    //    case enumGISTableDataset:
    //        {
    //            wxGxPostGISTableDataset* pGxPostGISTableDataset = new wxGxPostGISTableDataset(pGISDataset->GetPath(), pGISDataset);
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

bool wxGxRemoteDBSchema::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
    long nChildID = pChild->GetID();
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
    m_pCatalog->ObjectDeleted(nChildID);
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(GetID());
	return true;
}
*/
#endif //wxGIS_USE_POSTGRES