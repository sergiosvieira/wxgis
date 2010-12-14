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

wxGISPostGISDataset::wxGISPostGISDataset(wxString sPath) : wxGISDataset(sPath), m_poDS(NULL)
{
	m_bIsOpened = false;
	m_nType = enumGISContainer;
	m_nSubType = enumContGDB;
}

wxGISPostGISDataset::~wxGISPostGISDataset(void)
{
	Close();
}

void wxGISPostGISDataset::Close(void)
{
	m_bIsOpened = false;
    if(m_poDS && m_poDS->Dereference() <= 0)
        OGRDataSource::DestroyDataSource( m_poDS );
	m_poDS = NULL;
}

size_t wxGISPostGISDataset::GetSubsetsCount(void)
{
    if( !m_bIsOpened )
        if( !Open() )
            return 0;

    if( m_poDS )
        return  m_poDS->GetLayerCount();
    return 0;
}

wxGISDataset* wxGISPostGISDataset::GetSubset(size_t nIndex)
{
    if( !m_bIsOpened )
        if( !Open() )
            return 0;

    if(m_poDS)
    {
	    //OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
     //   if(poLayer)
     //   {
     //       m_poDS->Reference();
     //       wxGISFeatureDataset* pDataSet = new wxGISFeatureDataset(m_poDS, poLayer, wxEmptyString, (wxGISEnumVectorDatasetType)m_nSubType);
     //       pDataSet->SetEncoding(m_Encoding);
     //       pDataSet->Reference();
     //       return static_cast<wxGISDataset*>(pDataSet);
     //   }
    }
    return NULL;
}

bool wxGISPostGISDataset::Open()
{
	if(m_bIsOpened)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);
	CPLSetConfigOption("PG_LIST_ALL_TABLES", "YES");

    m_poDS = OGRSFDriverRegistrar::Open( wgWX2MB(m_sPath), FALSE );
	if( m_poDS == NULL )
	{
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISPostGISDataset: Open failed! Path '%s'. OGR error: %s"), m_sPath.c_str(), wgMB2WX(err));
		wxLogError(sErr);
		return false;
	}

    int nLayerCount = m_poDS->GetLayerCount();
 ////   if(nLayerCount == 1)
 ////   {
	////    m_poLayer = m_poDS->GetLayer(iLayer);
	////    if(m_poLayer)
	////    {
	////	    m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8);
	////    }
 ////   }
 ////   else
 ////       m_nType = enumGISContainer;
	m_bIsOpened = true;
	return true;
}

wxString wxGISPostGISDataset::GetName(void)
{
    if( !m_bIsOpened )
        if( !Open() )
            return wxEmptyString;
	if(	m_poDS )
    {
		wxString sOut = wgMB2WX(m_poDS->GetName());
		return sOut;
	}
    return wxEmptyString;
}

OGRDataSource* wxGISPostGISDataset::GetDataSource(void)
{
    if(m_poDS)
        return m_poDS;
    if( !m_bIsOpened )
        if( !Open() )
            return NULL;
    return m_poDS;
}

