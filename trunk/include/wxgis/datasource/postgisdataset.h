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
#pragma once

#include "wxgis/datasource/datasource.h"

/** \class wxGISPostGISDataset postgisdataset.h
    \brief The PostGIS Dataset class.
*/
class WXDLLIMPEXP_GIS_DS wxGISPostGISDataset :
	public wxGISDataset
{
public:
	wxGISPostGISDataset(wxString sPath);
	virtual ~wxGISPostGISDataset(void);
	//wxGISDataset
    virtual size_t GetSubsetsCount(void);
    virtual wxGISDataset* GetSubset(size_t nIndex);
    virtual wxString GetName(void);
	virtual void Close(void);
	//wxGISPostGISDataset
	virtual OGRDataSource* GetDataSource(void);
	virtual bool Open(void);
protected:
	OGRDataSource *m_poDS;
};
