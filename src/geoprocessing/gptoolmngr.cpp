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

wxGISGPToolManager::wxGISGPToolManager(wxXmlNode* pToolsNode)
{
    m_pToolsNode = pToolsNode;
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
            if(m_ToolsMap[sInternalName] == NULL)
                m_ToolsMap[sInternalName] = pTool;
        }
        pChild = pChild->GetNext();
    }
}

wxGISGPToolManager::~wxGISGPToolManager(void)
{
    typedef std::map<wxString, IGPTool*>::iterator IT;
    for(IT it = m_ToolsMap.begin(); it != m_ToolsMap.end(); ++it)
        wxDELETE(it->second);
}

IGPTool* wxGISGPToolManager::GetTool(wxString sToolName)
{
    return m_ToolsMap[sToolName];
}