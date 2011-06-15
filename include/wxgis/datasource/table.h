/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Table class.
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
#pragma once

#include "wxgis/datasource/cursor.h"
#include "wxgis/datasource/filter.h"

//#define MAXSTRINGSTORE 1000000


/** \class wxGISTable table.h
    \brief A GIS Table class.

    This class support basic operations on datasource. No spatial daya avaliable, but the OGRFeature is main part of this class, so it's posible to get some spatial information (not recommendet).
*/
class WXDLLIMPEXP_GIS_DS wxGISTable : public wxGISDataset
{
public:
	wxGISTable(CPLString sPath, int nSubType, OGRLayer* poLayer = NULL, OGRDataSource* poDS = NULL);
	virtual ~wxGISTable(void);
	//wxGISDataset
    virtual wxString GetName(void);
	virtual void Close(void);
	//wxGISTable
    virtual wxFontEncoding GetEncoding(void){return m_Encoding;};
    virtual void SetEncoding(wxFontEncoding Encoding){m_Encoding = Encoding;};
	virtual wxString GetAsString(size_t row, int col);
	static wxString GetAsString(OGRFeatureSPtr pFeature, int nField, wxFontEncoding Encoding);
	virtual OGRFeatureSPtr Next(void);
	virtual OGRFeatureSPtr operator [](size_t nIndex);
	virtual OGRFeatureSPtr GetAt(size_t nIndex);
	virtual OGRFeatureSPtr GetFeature(long nFID);
	virtual OGRErr StoreFeature(OGRFeatureSPtr poFeature);
	virtual OGRFeatureSPtr CreateFeature(void);
	virtual bool CanDeleteFeature(void);
	virtual OGRErr DeleteFeature(long nFID);
    virtual OGRErr SetFeature(OGRFeatureSPtr poFeature);
	virtual OGRFeatureDefn* const GetDefinition(void);
	virtual void Reset(void);
    virtual OGRErr SetFilter(wxGISQueryFilter* pQFilter);
	virtual wxFeatureCursorSPtr Search(wxGISQueryFilter* pQFilter, bool bOnlyFirst = false);
    virtual OGRErr SetIgnoredFields(wxArrayString &saIgnoredFields);
	virtual size_t GetFeatureCount(ITrackCancel* pTrackCancel = NULL);
	virtual bool Open(int iLayer = 0, int bUpdate = 0, ITrackCancel* pTrackCancel = NULL);
	virtual bool Delete(int iLayer = 0, ITrackCancel* pTrackCancel = NULL);
	virtual bool Rename(wxString sNewName);
	virtual bool Copy(CPLString szDestPath, ITrackCancel* pTrackCancel = NULL);
	virtual bool Move(CPLString szDestPath, ITrackCancel* pTrackCancel = NULL);
	virtual char **GetFileList();
	virtual OGRDataSource* const GetDataSource(void){return m_poDS;};
protected:
	virtual void LoadFeatures(ITrackCancel* pTrackCancel = NULL);
    virtual void UnloadFeatures(void);
protected:
	OGRDataSource* m_poDS;
	OGRLayer* m_poLayer;
	wxString m_sTableName;

    wxFontEncoding m_Encoding;
	bool m_bOLCStringsAsUTF8, m_bIsDataLoaded;
	bool m_bHasFID;
	long m_nFeatureCount;
	long m_nFIDCounter;
    std::map<long, OGRFeatureSPtr> m_FeaturesMap;
    std::map<long, OGRFeatureSPtr>::iterator m_IT;

	wxString m_sCurrentFilter;
	//short m_FieldCount;
    //wxArrayString m_FeatureStringData;
};

DEFINE_SHARED_PTR(wxGISTable);
