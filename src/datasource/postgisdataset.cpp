/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PosGISDataset class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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
#include "wxgis/datasource/featuredataset.h"

wxGISPostgresDataSource::wxGISPostgresDataSource(wxString sName, wxString sPass, wxString sPort, wxString sAddres, wxString sDBName, bool bIsBinaryCursor) : wxGISDataset(""), m_poDS(NULL)
{
	m_bIsOpened = false;
	m_nType = enumGISContainer;
	m_nSubType = enumContRemoteConnection;
    m_sName = sName;
    m_sPass = sPass;
    m_sPort = sPort;
    m_sAddres = sAddres;
    m_sDBName = sDBName;
	m_bIsBinaryCursor = bIsBinaryCursor;
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

wxGISDatasetSPtr wxGISPostgresDataSource::GetSubset(size_t nIndex)
{
    if( !m_bIsOpened )
        if( !Open() )
            return wxGISDatasetSPtr();

    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
		return GetDatasetFromOGRLayer(poLayer);
    }
    return wxGISDatasetSPtr();
}

wxGISDatasetSPtr wxGISPostgresDataSource::GetSubset(wxString sTablename)
{
    if( !m_bIsOpened )
        if( !Open() )
            return wxGISDatasetSPtr();

    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayerByName(sTablename.mb_str(wxConvUTF8));
		return GetDatasetFromOGRLayer(poLayer);
    }
    return wxGISDatasetSPtr();
}

wxGISDatasetSPtr wxGISPostgresDataSource::GetDatasetFromOGRLayer(OGRLayer* poLayer)
{
    wxCriticalSectionLocker locker(m_CritSect);
	wxGISDatasetSPtr pDataset;
	wxCHECK(poLayer, pDataset);
    m_poDS->Reference();
	//check the layer type
	if(!CPLString(poLayer->GetGeometryColumn()).empty())
	{
		m_poDS->Reference();
        wxGISFeatureDatasetSPtr pDataSet = boost::make_shared<wxGISFeatureDataset>(m_sPath, emumVecPostGIS, poLayer, m_poDS);//TODO: perfomance lack!!!
		pDataSet->SetEncoding(wxFONTENCODING_UTF8);
        pDataset = boost::static_pointer_cast<wxGISDataset>(pDataSet);
	}
	else
	{
		m_poDS->Reference();
        wxGISTableSPtr pTable = boost::make_shared<wxGISTable>(m_sPath, enumTablePostgres, poLayer, m_poDS);//TODO: Think about it
		pTable->SetEncoding(wxFONTENCODING_UTF8);
        pDataset = boost::static_pointer_cast<wxGISDataset>(pTable);
	}
	return pDataset;
}

bool wxGISPostgresDataSource::Open()
{
	if(m_bIsOpened)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);
	CPLSetConfigOption("PG_LIST_ALL_TABLES", "YES");
	CPLSetConfigOption("PGCLIENTENCODING", "UTF-8");

    //"PG:host='127.0.0.1' dbname='db' port='5432' user='bishop' password='xxx'"
	wxString sConnStr = wxString::Format(wxT("%s:host='%s' dbname='%s' port='%s' user='%s' password='%s'"), m_bIsBinaryCursor == true ? wxT("PGB") : wxT("PG"), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str(), m_sPass.c_str());
    m_poDS = OGRSFDriverRegistrar::Open( sConnStr.mb_str(wxConvUTF8), FALSE );
	if( m_poDS == NULL )
		return false;
	wxString sPath = wxString::Format(wxT("host='%s' dbname='%s' port='%s' user='%s' password='%s'"), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str(), m_sPass.c_str());
	m_sPath = CPLString(sPath.mb_str(wxConvUTF8));
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

wxGISDatasetSPtr wxGISPostgresDataSource::ExecuteSQL(wxString sStatement, wxGISSpatialFilter* pSpatialFilter, wxString sDialect)
{
    if( !m_bIsOpened )
        if( !Open() )
            return wxGISDatasetSPtr();
	wxGISDatasetSPtr pDataset;
    if(m_poDS)
	{
        CPLString szStatement = sStatement.mb_str();
		OGRLayer * poLayer = m_poDS->ExecuteSQL(szStatement, NULL, sDialect.mb_str());//TODO: implement spatial Filter
		if(	poLayer )
		{
			m_poDS->Reference();
            wxGISTableSPtr pTable = boost::make_shared<wxGISTable>(szStatement, enumTableQueryResult, poLayer, m_poDS);
			pTable->SetEncoding(wxFONTENCODING_UTF8);
            pDataset = boost::static_pointer_cast<wxGISDataset>(pTable);
		}
	}
	return pDataset;
}
