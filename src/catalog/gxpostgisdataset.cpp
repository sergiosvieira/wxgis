/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDataset classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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

//-----------------------------------------------------------------------------------
// wxGxPostGISTableDataset
//-----------------------------------------------------------------------------------
wxGxPostGISTableDataset::wxGxPostGISTableDataset(CPLString soPath, wxGISDatasetSPtr pwxGISDataset)
{
	m_sName = pwxGISDataset->GetName();

	m_pwxGISDataset = pwxGISDataset;

    m_sPath = soPath + " ";
    m_sPath += m_sName.mb_str(wxConvUTF8);
}

wxGxPostGISTableDataset::~wxGxPostGISTableDataset(void)
{
}

wxGISDatasetSPtr wxGxPostGISTableDataset::GetDataset(bool bCache, ITrackCancel* pTrackCancel)
{
    return m_pwxGISDataset;
}

#define PGTEST
//-----------------------------------------------------------------------------------
// wxGxPostGISFeatureDataset
//-----------------------------------------------------------------------------------
wxGxPostGISFeatureDataset::wxGxPostGISFeatureDataset(CPLString soPath, wxGISDatasetSPtr pwxGISDataset)
{
	m_sName = pwxGISDataset->GetName();

	m_pwxGISDataset = pwxGISDataset;

//#ifndef PGTEST 
//    m_sPath = soPath + " ";
//    m_sPath += m_sName.mb_str(wxConvUTF8);
//#else
//	//"PG:host='gis-lab.info' dbname='firereporter' port='5432' user='firereporter' password='8QdA4'"
	m_sPath = "vector:postgres:";
	m_sPath += m_sName.mb_str(wxConvUTF8);
	m_sPath += ":";
	m_sPath += soPath;
	m_sPath += " sslmode=disable table=";
	m_sPath += m_sName.mb_str(wxConvUTF8);
//    m_sPath = "vector:postgres:fire:dbname=firereporter host=gis-lab.info port=5432 user=firereporter password=8QdA4 sslmode=disable table=fires";
//#endif
}

wxGxPostGISFeatureDataset::~wxGxPostGISFeatureDataset(void)
{
}

wxGISDatasetSPtr wxGxPostGISFeatureDataset::GetDataset(bool bCache, ITrackCancel* pTrackCancel)
{
    return m_pwxGISDataset;
}