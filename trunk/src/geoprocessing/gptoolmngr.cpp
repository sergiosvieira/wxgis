/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  tools manager.
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

#include "wxgis/geoprocessing/gptoolmngr.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPToolManager
///////////////////////////////////////////////////////////////////////////////

wxGISGPToolManager::wxGISGPToolManager(wxXmlNode* pToolsNode, IGxCatalog* pCatalog) : m_pCatalog(NULL)
{
    m_pToolsNode = pToolsNode;
    m_pCatalog = pCatalog;
    wxXmlNode *pChild = m_pToolsNode->GetChildren();
    while (pChild) 
    {
        wxString sName = pChild->GetPropVal(wxT("object"), NONAME);
        if(sName.IsEmpty() || sName.CmpNoCase(NONAME) == 0)
        {
            pChild = pChild->GetNext();
            continue;
        }
            
		wxObject *pObj = wxCreateDynamicObject(sName);
		IGPTool *pTool = dynamic_cast<IGPTool*>(pObj);
		if(pTool != NULL)
		{
            wxString sInternalName = pTool->GetName();
            m_ToolsMap[sInternalName] = sName;
            wxDELETE(pTool);
        }
        pChild = pChild->GetNext();
    }
}

wxGISGPToolManager::~wxGISGPToolManager(void)
{
    //typedef std::map<wxString, IGPTool*>::iterator IT;
    //for(IT it = m_ToolsMap.begin(); it != m_ToolsMap.end(); ++it)
    //    wxDELETE(it->second);
}

IGPTool* wxGISGPToolManager::GetTool(wxString sToolName)
{
	wxObject *pObj = wxCreateDynamicObject(m_ToolsMap[sToolName]);
    IGPTool* pTool = dynamic_cast<IGPTool*>(pObj);
    if(!pTool)
        return NULL;
    pTool->SetCatalog(m_pCatalog);
	return pTool;
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPParameter
///////////////////////////////////////////////////////////////////////////////
wxGISGPParameter::wxGISGPParameter()
{
    m_bAltered = false;
    m_bHasBeenValidated = false;
    m_bIsValid = false;
    m_pDomain = NULL;
}

wxGISGPParameter::~wxGISGPParameter()
{
    wxDELETE(m_pDomain);
}

bool wxGISGPParameter::GetAltered(void)
{
    return m_bAltered;
}

void wxGISGPParameter::SetAltered(bool bAltered)
{
    m_bAltered = bAltered;
}

bool wxGISGPParameter::GetHasBeenValidated(void)
{
    return m_bHasBeenValidated;
}

void wxGISGPParameter::SetHasBeenValidated(bool bHasBeenValidated)
{
    m_bHasBeenValidated = bHasBeenValidated;
}

bool wxGISGPParameter::GetIsValid(void)
{
    return m_bIsValid;
}

void wxGISGPParameter::SetIsValid(bool bIsValid)
{
    m_bIsValid = bIsValid;
}

wxString wxGISGPParameter::GetName(void)
{
    return m_sName;
}

void wxGISGPParameter::SetName(wxString sName)
{
    m_sName = sName;
}

wxString wxGISGPParameter::GetDisplayName(void)
{
    return m_sDisplayName;
}

void wxGISGPParameter::SetDisplayName(wxString sDisplayName)
{
    m_sDisplayName = sDisplayName;
}

wxGISEnumGPParameterDataType wxGISGPParameter::GetDataType(void)
{
    return m_DataType;
}

void wxGISGPParameter::SetDataType(wxGISEnumGPParameterDataType nType)
{
    m_DataType = nType;
}

wxGISEnumGPParameterDirection wxGISGPParameter::GetDirection(void)
{
    return m_Direction;
}

void wxGISGPParameter::SetDirection(wxGISEnumGPParameterDirection nDirection)
{
    m_Direction = nDirection;
}

wxArrayString* wxGISGPParameter::GetParameterDependencies(void)
{
    return NULL;
}

void wxGISGPParameter::AddParameterDependency(wxString sDependency)
{
}

wxGISEnumGPParameterType wxGISGPParameter::GetParameterType(void)
{
    return m_ParameterType;
}

void wxGISGPParameter::SetParameterType(wxGISEnumGPParameterType nType)
{
    m_ParameterType = nType;
}

wxVariant wxGISGPParameter::GetValue(void)
{
    return m_Value;
}

void wxGISGPParameter::SetValue(wxVariant Val)
{
    m_Value = Val;
}

IGPDomain* wxGISGPParameter::GetDomain(void)
{
    return m_pDomain;
}

void wxGISGPParameter::SetDomain(IGPDomain* pDomain)
{
    m_pDomain = pDomain;
}

