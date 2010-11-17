/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  ortho correct geoprocessing tools.
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

/////////////////////////////////////////////////////////////////////////
// wxGISGPOrthoCorrectTool
/////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GP wxGISGPOrthoCorrectTool : 
    public IGPTool,
    public wxObject
{
   DECLARE_DYNAMIC_CLASS(wxGISGPOrthoCorrectTool)

public:
    wxGISGPOrthoCorrectTool(void);
    ~wxGISGPOrthoCorrectTool(void);
    //IGPTool
    virtual wxString GetDisplayName(void);
    virtual wxString GetName(void);
    virtual wxString GetCategory(void);
    virtual bool Execute(ITrackCancel* pTrackCancel);
    virtual bool Validate(void);
    virtual GPParameters* GetParameterInfo(void);
    virtual void SetCatalog(IGxCatalog* pCatalog);
    virtual IGxCatalog* GetCatalog(void);
	virtual wxString GetAsString(void){return wxEmptyString;};
	virtual void SetFromString(wxString sParams){};
protected:
    GPParameters m_pParamArr;
    IGxCatalog* m_pCatalog;
};