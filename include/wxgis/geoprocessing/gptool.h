/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  base geoprocessing tool.
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

#pragma once

#include "wxgis/geoprocessing/geoprocessing.h"

#define SEPARATOR wxT("|")

/** \class wxGISGPTool gptool.h
    \brief The base class for geoprocessing tools.

    This class implements common functions.
*/

class WXDLLIMPEXP_GIS_GP wxGISGPTool : 
    public IGPTool,
    public wxObject
{
public:
    wxGISGPTool(void);
    ~wxGISGPTool(void);
    //IGPTool
    virtual wxString GetDisplayName(void) = 0;
    virtual wxString GetName(void) = 0;
    virtual wxString GetCategory(void) = 0;
    virtual bool Execute(ITrackCancel* pTrackCancel) = 0;
    virtual bool Validate(void) = 0;
    virtual GPParameters* GetParameterInfo(void) = 0;
    virtual void SetCatalog(IGxCatalog* pCatalog);
    virtual IGxCatalog* GetCatalog(void);
	virtual wxString GetAsString(void);
	virtual void SetFromString(wxString sParams);
    //TODO: export/import tool to XML for server execution
protected:
    GPParameters m_pParamArr;
    IGxCatalog* m_pCatalog;
};