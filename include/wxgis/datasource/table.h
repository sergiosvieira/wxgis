/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Table class.
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
#pragma once

#include "wxgis/datasource/datasource.h"
#define MAXSTRINGSTORE 1000000

class WXDLLIMPEXP_GIS_DS wxGISTable : public wxGISDataset
{
public:
	wxGISTable(OGRLayer* poLayer, wxString sPath, wxGISEnumTableDatasetType nType);
	virtual ~wxGISTable(void);
	//wxGISDataset
    virtual wxString GetName(void){return m_sTableName;};
	virtual void Close(void);
	//wxGISTable
    virtual wxFontEncoding GetEncoding(void){return m_Encoding;};
    virtual void SetEncoding(wxFontEncoding Encoding){m_Encoding = Encoding;};
	virtual wxString GetAsString(long row, int col);
	static wxString GetAsString(OGRFeature* pFeature, int nField, wxFontEncoding Encoding);
	virtual OGRFeature* operator [](long nIndex);
	virtual OGRFeature* GetAt(long nIndex);
	virtual OGRErr CreateFeature(OGRFeature* poFeature);
	virtual OGRErr DeleteFeature(long nFID);
    virtual OGRErr SetFeature (OGRFeature *poFeature);
    virtual void Unload(void);
	virtual void PreLoad(void);
	virtual OGRFeatureDefn* GetDefinition(void);
	virtual void Reset(void);
	virtual OGRFeature* Next(void);
	virtual size_t GetSize(void);
    virtual OGRErr SetFilter(wxGISQueryFilter* pQFilter);
    //virtual OGRErr SetIgnoredFields(wxArrayString &saIgnoredFields);
    virtual wxCriticalSection* GetCriticalSection(void){return &m_CritSect;};
	virtual void SetDataSource(OGRDataSource* pDS){m_pDS = pDS;};
protected:
	OGRDataSource* m_pDS;
	OGRLayer* m_poLayer;
    wxFontEncoding m_Encoding;
	bool m_bOLCStringsAsUTF8, m_bIsDataLoaded;
	bool m_bHasFID;
	wxString m_sTableName;
	int m_nSize;
    std::map<long, OGRFeature*> m_FeaturesMap;
    std::map<long, OGRFeature*>::iterator m_IT;
    wxArrayString m_FeatureStringData;
	short m_FieldCount;
    wxCriticalSection m_CritSect;
};