/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  export geoprocessing tools.
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

#include "wxgis/geoprocessing/gpexporttool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPExportTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPExportTool, wxObject)

wxGISGPExportTool::wxGISGPExportTool(void) : m_pParamArr(NULL)
{
}

wxGISGPExportTool::~wxGISGPExportTool(void)
{
    if(m_pParamArr)
    {
        for(size_t i = 0; i < m_pParamArr->size(); i++)
            wxDELETE(m_pParamArr->operator[](i));
        wxDELETE(m_pParamArr);
    }
}

wxString wxGISGPExportTool::GetDisplayName(void)
{
    return wxString(_("Export vector file (single)"));
}

wxString wxGISGPExportTool::GetName(void)
{
    return wxString(wxT("vexport_single"));
}

wxString wxGISGPExportTool::GetCategory(void)
{
    return wxString(_("Conversion Tools/Vector"));
}

GPParameters* wxGISGPExportTool::GetParameterInfo(void)
{
    if(m_pParamArr)
        return m_pParamArr;
    m_pParamArr = new GPParameters;

    //src path
    wxGISGPParameter* pParam1 = new wxGISGPParameter();
    pParam1->SetName(wxT("src_path"));
    pParam1->SetDisplayName(_("Source feature class"));
    pParam1->SetParameterType(enumGISGPParameterTypeRequired);
    pParam1->SetDataType(enumGISGPParamDTPath);
    pParam1->SetDirection(enumGISGPParameterDirectionInput);
    m_pParamArr->push_back(pParam1);

    //SQL statement

    //dst path

    return m_pParamArr;
}
