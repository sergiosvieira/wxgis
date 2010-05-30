/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
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
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/catalog/gxfilters.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPExportTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPExportTool, wxObject)

wxGISGPExportTool::wxGISGPExportTool(void) : m_pCatalog(NULL)
{
}

wxGISGPExportTool::~wxGISGPExportTool(void)
{
    for(size_t i = 0; i < m_pParamArr.size(); i++)
        wxDELETE(m_pParamArr[i]);
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
    if(m_pParamArr.empty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source feature class"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxShapeFileFilter());
        pParam1->SetDomain(pDomain1);

        m_pParamArr.push_back(pParam1);

        //SQL statement

        //dst path
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("dst_path"));
        pParam2->SetDisplayName(_("Destination feature class"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTPath);
        pParam2->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
        pDomain2->AddFilter(new wxGxShapeFileFilter());
        pParam2->SetDomain(pDomain2);

        //pParam2->AddParameterDependency(wxT("src_path"));

        m_pParamArr.push_back(pParam2);

    }
    return &m_pParamArr;
}

void wxGISGPExportTool::SetCatalog(IGxCatalog* pCatalog)
{
    m_pCatalog = pCatalog;
}

IGxCatalog* wxGISGPExportTool::GetCatalog(void)
{
    return m_pCatalog;
}

bool wxGISGPExportTool::Validate(void)
{
    if(!m_pParamArr[1]->GetAltered())
    {
        if(m_pParamArr[0]->GetIsValid())
        {
            //generate temp name
            wxString sPath = m_pParamArr[0]->GetValue();
            //wxFileName Name(sPath);
            //Name.
            m_pParamArr[1]->SetValue(wxVariant(sPath, wxT("path")));
            m_pParamArr[1]->SetAltered(true);//??
        }
    }

    //check if input & output types is same!
    if(m_pParamArr[0]->GetIsValid())
    {
        if(!m_pParamArr[1]->GetHasBeenValidated())
        {
            //TODO: Maybe IGxDataset in future?
            //IGxDataset* pDset1 = m_pCatalog->SearchChild()
            wxFileName Name1(m_pParamArr[0]->GetValue());
            wxFileName Name2(m_pParamArr[1]->GetValue());
            if(Name1.GetExt() == Name2.GetExt())
            {
                m_pParamArr[1]->SetIsValid(false);
                m_pParamArr[1]->SetMessage(wxGISEnumGPMessageError, _("Cannot export to the same format"));
                return false;
            }
        }
    }
    return true;
}
