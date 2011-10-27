/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnection class.
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
#pragma once

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxfile.h"

/** \class wxGxRemoteConnection gxremoteconn.h
    \brief A Remote Connection GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteConnection :
	public wxGxFile/*,
	public IGxDataset*/
{
public:
	wxGxRemoteConnection(CPLString Path, wxString Name);
	virtual ~wxGxRemoteConnection(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Remote Database Connection"));};
	////IGxDataset
	//virtual wxGISDatasetSPtr GetDataset(bool bCache = true, ITrackCancel* pTrackCancel = NULL);
	//virtual wxGISEnumDatasetType GetType(void){return enumContRemoteConnection;};
 //   virtual int GetSubType(void){return m_type;};
//protected:
//	wxString m_sName;
//    CPLString m_sPath;
//	wxGISDatasetSPtr m_pwxGISDataset;
//	wxGISEnumTableDatasetType m_type;
};