/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  FeatureDataset class.
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
#include "wxgis/carto/featuredataset.h"
#include <wx/filename.h>

#include <wx/file.h>

wxGISFeatureDataset::wxGISFeatureDataset(wxString sPath, wxFontEncoding Encoding) : wxGISDataset(sPath), m_poDS(NULL), m_bIsOpened(false), m_psExtent(NULL), m_poLayer(NULL), m_pQuadTree(NULL), m_Encoding(Encoding)
{
}

wxGISFeatureDataset::~wxGISFeatureDataset(void)
{
	wxDELETE(m_psExtent);

	Empty();

	if(m_pQuadTree)
		CPLQuadTreeDestroy(m_pQuadTree);

	if( m_poDS )
		OGRDataSource::DestroyDataSource( m_poDS );
}

OGRLayer* wxGISFeatureDataset::GetLayer(int iLayer)
{
	if(m_bIsOpened)
	{
		m_poLayer->ResetReading();
		return m_poLayer;
	}
	else
	{
		if(Open(iLayer))
			return GetLayer(iLayer);
		else
			return NULL;
	}
	return NULL;	
}

bool wxGISFeatureDataset::Open(int iLayer)
{
	if(m_bIsOpened)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);

	//m_poDS = OGRSFDriverRegistrar::Open( wgWX2MB(m_sPath.c_str()), FALSE );
 //   //bug in FindFileInZip() [gdal-1.6.3\port\cpl_vsil_gzip.cpp]
	//if( m_poDS == NULL )
 //   {
 //       m_sPath.Replace(wxT("\\"), wxT("/"));
 //       m_poDS = OGRSFDriverRegistrar::Open( wgWX2MB(m_sPath.c_str()), FALSE );
 //   }
        
    m_poDS = OGRSFDriverRegistrar::Open( wgWX2MB(m_sPath.c_str()), FALSE );
	if( m_poDS == NULL )
	{
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISFeatureDataset: Open failed! Path '%s'. OGR error: %s"), m_sPath.c_str(), wgMB2WX(err));
		wxLogError(sErr);
		wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
		return false;
	}

	m_poLayer = m_poDS->GetLayer(iLayer);
	if(m_poLayer)
	{
		m_psExtent = new OGREnvelope();
		if(m_poLayer->GetExtent(m_psExtent, true) != OGRERR_NONE)
		{
			wxDELETE(m_psExtent);
			m_psExtent = NULL;
		}

		bool bOLCFastSpatialFilter = m_poLayer->TestCapability(OLCFastSpatialFilter);
		if(!bOLCFastSpatialFilter)
		{
			if(m_psExtent)
			{
				OGREnvelope Env = *m_psExtent;
				CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
				m_pQuadTree = CPLQuadTreeCreate(&Rect, GetFeatureBoundsFunc); 
			}
			//wxFileName FileName(m_sPath);
			//wxString SQLStatement = wxString::Format(wxT("CREATE SPATIAL INDEX ON %s"), FileName.GetName().c_str());
			//m_poDS->ExecuteSQL(wgWX2MB(SQLStatement), NULL, NULL);
		}
	//	bool bOLCRandomRead = pOGRLayer->TestCapability(OLCRandomRead);
	//	bool bOLCSequentialWrite = pOGRLayer->TestCapability(OLCSequentialWrite);
	//	bool bOLCRandomWrite = pOGRLayer->TestCapability(OLCRandomWrite);
	//	bool bOLCFastFeatureCount = pOGRLayer->TestCapability(OLCFastFeatureCount);
	//	bool bOLCFastGetExtent = pOGRLayer->TestCapability(OLCFastGetExtent);
	//	bool bOLCFastSetNextByIndex= pOGRLayer->TestCapability(OLCFastSetNextByIndex);
	//	bool bOLCCreateField = pOGRLayer->TestCapability(OLCCreateField);
	//	bool bOLCDeleteFeature = pOGRLayer->TestCapability(OLCDeleteFeature);
		m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8);
	//	bool bOLCTransactions = pOGRLayer->TestCapability(OLCTransactions);
	//	wxString sFIDColName = wgMB2WX(pOGRLayer->GetFIDColumn());


	//	m_pGISFeatureSet = new IwxGISFeatureSet(m_poLayer);
		
		m_OGRFeatureArray.reserve(m_poLayer->GetFeatureCount(true)); 

		size_t count(0); 
		OGRFeature *poFeature;
		while( (count < CACHE_SIZE) && ((poFeature = m_poLayer->GetNextFeature()) != NULL) )
		{
			AddFeature(poFeature);
			count++;
		}

		//OGRFeature *poFeature;
		//while( (poFeature = m_poLayer->GetNextFeature()) != NULL )
		//	m_pGISFeatureSet->AddFeature(poFeature);
	}

	m_bIsOpened = true;
	return true;
}

OGRSpatialReference* wxGISFeatureDataset::GetSpatialReference(void)
{
	if(!m_bIsOpened)
		if(!Open(0))
			return NULL;
	if(	m_poLayer )
    {
        //OGRSpatialReference* pSparef = m_poLayer->GetSpatialRef();
        //char* pStr(NULL);
        //pSparef->exportToXML(&pStr);
        //wxFile file;
        //file.Create(wxT("d:/test.srml"));
        //wxString data(wgMB2WX(pStr));
        //file.Write(data);
        //CPLFree(pStr);
		return m_poLayer->GetSpatialRef();
    }
	return NULL;
}

OGREnvelope* wxGISFeatureDataset::GetEnvelope(void)
{
	if(m_psExtent)
		return m_psExtent;
	return NULL;
}

void wxGISFeatureDataset::SetSpatialFilter(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY)
{
	if(!m_bIsOpened)
		if(!Open(0))
			return;
	if(	m_poLayer )
	{
		m_poLayer->SetSpatialFilterRect(dfMinX, dfMinY, dfMaxX, dfMaxY);
		//m_pGISFeatureSet->Empty();
		//OGRFeature *poFeature;
		//while( (poFeature = m_poLayer->GetNextFeature()) != NULL )
		//	m_pGISFeatureSet->AddFeature(poFeature);
		//m_poLayer->SetSpatialFilter(NULL);
		//if(m_pQuadTree)
		//	CPLQuadTreeSearch(m_pQuadTree, 
	}
}

void wxGISFeatureDataset::Empty(void)
{
	for(size_t i = 0; i < m_OGRFeatureArray.size(); i++)
		OGRFeature::DestroyFeature( m_OGRFeatureArray[i] );
	m_OGRFeatureArray.clear();
}

void wxGISFeatureDataset::AddFeature(OGRFeature* poFeature)
{
	if(m_pQuadTree)
		CPLQuadTreeInsert(m_pQuadTree, poFeature);
	m_OGRFeatureArray.push_back(poFeature);
}

OGRFeature* wxGISFeatureDataset::GetAt(int nIndex) //const    0 based
{
	wxASSERT(nIndex >= 0);
	//wxASSERT(nIndex < m_OGRFeatureArray.size());
	while(nIndex + 1 > m_OGRFeatureArray.size()) //m_pOGRLayer->GetFeatureCount()**
	{
		size_t count(0); 
		OGRFeature *poFeature;
		while( (count < CACHE_SIZE) && (poFeature = m_poLayer->GetNextFeature()) != NULL )
		{
			AddFeature(poFeature);
			count++;
		}
	}
	return m_OGRFeatureArray[nIndex];
}

OGRFeature* wxGISFeatureDataset::operator [](int nIndex) //const    same as GetAt
{
	return GetAt(nIndex);
}

wxString wxGISFeatureDataset::GetAsString(int row, int col)
{
	if(m_poLayer->GetFeatureCount() <= row) 
		return wxString(); 
	else 
	{
		OGRFeature* pFeature = GetAt(row);
		OGRFieldDefn* pDef = pFeature->GetFieldDefnRef(col);
		switch(pDef->GetType())
		{
		case OFTDate:
			{
				int year, mon, day, hour, min, sec, flag;
				pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
				return dt.Format(_("%d-%m-%Y"));//wxString::Format(_("%.2u-%.2u-%.4u"), day, mon, year );
			}
		case OFTTime:
			{
				int year, mon, day, hour, min, sec, flag;
				pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
				return dt.Format(_("%H:%M:%S"));//wxString::Format(_("%.2u:%.2u:%.2u"), hour, min, sec);
			}
		case OFTDateTime:
			{
				int year, mon, day, hour, min, sec, flag;
				pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
				return dt.Format(_("%d-%m-%Y %H:%M:%S"));//wxString::Format(_("%.2u-%.2u-%.4u %.2u:%.2u:%.2u"), day, mon, year, hour, min, sec);
			}
		case OFTReal:				
			return wxString::Format(_("%.6f"), pFeature->GetFieldAsDouble(col));
		default:
            if(m_bOLCStringsAsUTF8 || m_Encoding == wxFONTENCODING_DEFAULT)
                return wgMB2WX(pFeature->GetFieldAsString(col));
            else            
            {                
                wxCSConv conv(m_Encoding);
                return conv.cMB2WX(pFeature->GetFieldAsString(col));
            }
		}
		//return wgMB2WX(GetAt(row)->GetFieldAsString(col));
	}
}

wxGISFeatureSet* wxGISFeatureDataset::GetFeatureSet(IQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
	if(m_OGRFeatureArray.size() < GetSize())
	{
		OGRFeature* poFeature;
		while((poFeature = m_poLayer->GetNextFeature()) != NULL )
		{
			if(pTrackCancel && !pTrackCancel->Continue())
				return NULL;
			AddFeature(poFeature);
		}
	}

	wxGISFeatureSet* pGISFeatureSet = new wxGISFeatureSet(m_OGRFeatureArray.size());
	if(pQFilter)
	{
		wxGISSpatialFilter* pSpaFil = dynamic_cast<wxGISSpatialFilter*>(pQFilter);
		if(pSpaFil && m_pQuadTree)
		{
			int count(0);
			OGREnvelope Env = pSpaFil->GetEnvelope();
			CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
			OGRFeature** pFeatureArr = (OGRFeature**)CPLQuadTreeSearch(m_pQuadTree, &Rect, &count);
			for(int i = 0; i < count; i++)
			{
				if(pTrackCancel && !pTrackCancel->Continue())
					break;
				pGISFeatureSet->AddFeature(pFeatureArr[i]);
			}
			wxDELETEA( pFeatureArr );
		}
	}
	else
	{
		for(size_t i = 0; i < m_OGRFeatureArray.size(); i++)
		{
			if(pTrackCancel && !pTrackCancel->Continue())
				break;
			pGISFeatureSet->AddFeature(m_OGRFeatureArray[i]);
		}
	}
	return pGISFeatureSet;
}

void GetFeatureBoundsFunc(const void* hFeature, CPLRectObj* pBounds)
{
	OGRFeature* pFeature = (OGRFeature*)hFeature;
	if(!pFeature)
		return;
	OGRGeometry* pGeom = pFeature->GetGeometryRef();
	if(!pGeom)
		return;
	OGREnvelope Env;
	pGeom->getEnvelope(&Env);
	pBounds->minx = Env.MinX;
	pBounds->maxx = Env.MaxX;
	pBounds->miny = Env.MinY;
	pBounds->maxy = Env.MaxY;
}
