/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDataset classes.
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

#include "wxgis/catalog/gxpostgisdataset.h"

#ifdef wxGIS_USE_POSTGRES
/*
//-----------------------------------------------------------------------------------
// wxGxPostGISTableDataset
//-----------------------------------------------------------------------------------
//wxGxPostGISTableDataset::wxGxPostGISTableDataset(CPLString soPath, wxGISDatasetSPtr pwxGISDataset)
//{
//	m_pwxGISDataset = pwxGISDataset;
//
//	m_sName = pwxGISDataset->GetName();
//}

wxGxPostGISTableDataset::wxGxPostGISTableDataset(CPLString szName, CPLString szSchema, wxGISPostgresDataSourceSPtr pwxGISRemoteConn)
{
    m_pwxGISRemoteConn = pwxGISRemoteConn;
    m_sName = wxString(szName, wxConvUTF8);
    m_szFullyQualifiedName.Printf("%s.%s", szSchema.c_str(), szName.c_str());
}

wxGxPostGISTableDataset::~wxGxPostGISTableDataset(void)
{
}

wxGISDatasetSPtr wxGxPostGISTableDataset::GetDataset(bool bCache, ITrackCancel* pTrackCancel)
{
    if(m_pwxGISDataset)
        return m_pwxGISDataset;
    else
    {
        m_pwxGISDataset = m_pwxGISRemoteConn->GetSubset(m_szFullyQualifiedName);
        return m_pwxGISDataset;
    }
}

CPLString wxGxPostGISTableDataset::GetInternalName(void)
{
    if(m_sPath.empty())
    {
        //m_sPath = soPath + " ";
        m_sPath += GetName().mb_str(wxConvUTF8);
    }
    return m_sPath;
}

//#define PGTEST
//-----------------------------------------------------------------------------------
// wxGxPostGISFeatureDataset
//-----------------------------------------------------------------------------------
//wxGxPostGISFeatureDataset::wxGxPostGISFeatureDataset(CPLString soPath, wxGISDatasetSPtr pwxGISDataset)
//{
//	m_sName = pwxGISDataset->GetName();
//
//	m_pwxGISDataset = pwxGISDataset;
//
////#ifndef PGTEST 
////    m_sPath = soPath + " ";
////    m_sPath += m_sName.mb_str(wxConvUTF8);
////#else
////	//"PG:host='gis-lab.info' dbname='firereporter' port='5432' user='firereporter' password='8QdA4'"
//	m_sPath = "vector:postgres:";
//	m_sPath += m_sName.mb_str(wxConvUTF8);
//	m_sPath += ":";
//	m_sPath += soPath;
//	m_sPath += " sslmode=disable table=";
//	m_sPath += m_sName.mb_str(wxConvUTF8);
////    m_sPath = "vector:postgres:fire:dbname=firereporter host=gis-lab.info port=5432 user=firereporter password=8QdA4 sslmode=disable table=fires";
////#endif
//}

wxGxPostGISFeatureDataset::wxGxPostGISFeatureDataset(CPLString szName, CPLString szSchema, wxGISPostgresDataSourceSPtr pwxGISRemoteConn)
{
    m_pwxGISRemoteConn = pwxGISRemoteConn;
    m_sName = wxString(szName, wxConvUTF8);
    m_szFullyQualifiedName.Printf("%s.%s", szSchema.c_str(), szName.c_str());
}

wxGxPostGISFeatureDataset::~wxGxPostGISFeatureDataset(void)
{
}

wxGISDatasetSPtr wxGxPostGISFeatureDataset::GetDataset(bool bCache, ITrackCancel* pTrackCancel)
{
    if(m_pwxGISDataset)
        return m_pwxGISDataset;
    else
    {
        m_pwxGISDataset = m_pwxGISRemoteConn->GetSubset(m_szFullyQualifiedName);
        return m_pwxGISDataset;
    }
}

CPLString wxGxPostGISFeatureDataset::GetInternalName(void)
{
    if(m_sPath.empty())
    {
        //m_sPath = soPath + " ";
        m_sPath += GetName().mb_str(wxConvUTF8);
    }
    return m_sPath;
}
*/
#endif //wxGIS_USE_POSTGRES
