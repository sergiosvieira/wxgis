/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PosGISDataset class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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
#include "wxgis/datasource/postgisdataset.h"
#include "wxgis/core/core.h"
#include "wxgis/datasource/table.h"

wxGISPostgresDataSource::wxGISPostgresDataSource(wxString sName, wxString sCryptPass, wxString sPGPort, wxString sPGAddres, wxString sDBName, wxString sCursor) : wxGISDataset(wxEmptyString), m_poDS(NULL)
{
    m_RefCount = 0;
	m_bIsOpened = false;
	m_nType = enumGISContainer;
	m_nSubType = enumContGDB;
    m_sName = sName;
    m_sCryptPass = sCryptPass;
    m_sPGPort = sPGPort;
    m_sPGAddres = sPGAddres;
    m_sDBName = sDBName;
	m_sCursor = sCursor;
}

wxGISPostgresDataSource::~wxGISPostgresDataSource(void)
{
	Close();
}

void wxGISPostgresDataSource::Close(void)
{
	m_bIsOpened = false;
    if(m_poDS && m_poDS->Dereference() <= 0)
        OGRDataSource::DestroyDataSource( m_poDS );
	m_poDS = NULL;
}

size_t wxGISPostgresDataSource::GetSubsetsCount(void)
{
    if( !m_bIsOpened )
        if( !Open() )
            return 0;

    if( m_poDS )
        return  m_poDS->GetLayerCount();
    return 0;
}

wxGISDataset* wxGISPostgresDataSource::GetSubset(size_t nIndex)
{
    if( !m_bIsOpened )
        if( !Open() )
            return 0;

    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        if(poLayer)
        {
            m_poDS->Reference();
			wxGISDataset* pDataset(NULL);
			//check the layer type
			if(CPLStrnlen(poLayer->GetGeometryColumn(), 100))
			{
				wxGISTable* pTable = new wxGISTable(poLayer, wxEmptyString, enumTablePostgres);//TODO: Think about it
				pTable->SetEncoding(wxFONTENCODING_UTF8);
				pTable->Reference();
				pDataset = static_cast<wxGISDataset*>(pTable);
			}
			else
			{
   //         wxGISFeatureDataset* pDataSet = new wxGISFeatureDataset(m_poDS, poLayer, wxEmptyString, (wxGISEnumVectorDatasetType)m_nSubType);
			}
	        return pDataset;
        }
    }
    return NULL;
}

wxGISDataset* wxGISPostgresDataSource::GetSubset(wxString sTablename)
{
    if( !m_bIsOpened )
        if( !Open() )
            return 0;

    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayerByName(wgWX2MB(sTablename));
        if(poLayer)
        {
			wxGISDataset* pDataset(NULL);
			//check the layer type
			if(CPLStrnlen(poLayer->GetGeometryColumn(), 100))
			{
   //         wxGISFeatureDataset* pDataSet = new wxGISFeatureDataset(m_poDS, poLayer, wxEmptyString, (wxGISEnumVectorDatasetType)m_nSubType);
			}
			else
			{
				wxGISTable* pTable = new wxGISTable(poLayer, wxEmptyString, enumTablePostgres);
				pTable->SetEncoding(wxFONTENCODING_UTF8);
				pTable->Reference();
				pDataset = static_cast<wxGISDataset*>(pTable);
			}
	        return pDataset;
        }
    }
    return NULL;
}

bool wxGISPostgresDataSource::Open()
{
	if(m_bIsOpened)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);
	CPLSetConfigOption("PG_LIST_ALL_TABLES", "YES");
	CPLSetConfigOption("PGCLIENTENCODING", "UTF-8");

    //wxT("PG:host='127.0.0.1' dbname='db' port='5432' user='bishop' password='xxx'")
	m_sPath = wxString::Format(wxT("%s:host='%s' dbname='%s' port='%s' user='%s' password='%s'"), m_sCursor.c_str(), m_sPGAddres.c_str(), m_sDBName.c_str(), m_sPGPort.c_str(), m_sName.c_str(), Decode(m_sCryptPass, CONFIG_DIR));
    m_poDS = OGRSFDriverRegistrar::Open( wgWX2MB(m_sPath), FALSE );
    m_sPath.Clear();
	if( m_poDS == NULL )
	{
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISPostGISDataset: Open failed! Host '%s', Database name '%s', Port='%s'. OGR error: %s"), m_sPGAddres.c_str(), m_sDBName.c_str(), m_sPGPort.c_str(), wgMB2WX(err));
		wxLogError(sErr);
		return false;
	}

	m_bIsOpened = true;
	return true;
}

wxString wxGISPostgresDataSource::GetName(void)
{
    return m_sDBName;
}

OGRDataSource* wxGISPostgresDataSource::GetDataSource(void)
{
    if(m_poDS)
        return m_poDS;
    if( !m_bIsOpened )
        if( !Open() )
            return NULL;
    return m_poDS;
}

wxGISDataset* wxGISPostgresDataSource::ExecuteSQL(wxString sStatement, wxGISSpatialFilter* pSpatialFilter, wxString sDialect)
{
    if( !m_bIsOpened )
        if( !Open() )
            return NULL;
	wxGISDataset* pDataset(NULL);
    if(m_poDS)
	{
		OGRLayer * poLayer = m_poDS->ExecuteSQL(wgWX2MB(sStatement), NULL, wgWX2MB(sDialect));//TODO: implement spatial Filter
		if(	poLayer )
		{
			wxGISTable* pTable = new wxGISTable(poLayer, sStatement, enumTableQueryResult);
			pTable->SetEncoding(wxFONTENCODING_UTF8);
			pTable->Reference();
			pTable->SetDataSource(m_poDS);
			pDataset = static_cast<wxGISDataset*>(pTable);
		}
	}
	return pDataset;
}
