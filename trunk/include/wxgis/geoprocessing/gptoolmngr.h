/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
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

#pragma once

#include "wxgis/geoprocessing/geoprocessing.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPToolManager
///////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GP wxGISGPToolManager
{
public:
    wxGISGPToolManager(wxXmlNode* pToolsNode, IGxCatalog* pCatalog = NULL);
    ~wxGISGPToolManager(void);
    IGPTool* GetTool(wxString sToolName);
protected:
    wxXmlNode* m_pToolsNode;
    std::map<wxString, wxString> m_ToolsMap; //internal name, class name
    IGxCatalog* m_pCatalog;
};

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPParameter
///////////////////////////////////////////////////////////////////////////////

class wxGISGPParameter : public IGPParameter
{
public:
    wxGISGPParameter(void);
    virtual ~wxGISGPParameter(void);
    //IGPParameter
    virtual bool GetAltered(void);
    virtual void SetAltered(bool bAltered);
    virtual bool GetHasBeenValidated(void);
    virtual void SetHasBeenValidated(bool bHasBeenValidated);
    virtual bool GetIsValid(void);
    virtual void SetIsValid(bool bIsValid);
    virtual wxString GetName(void);
    virtual void SetName(wxString sName);
    virtual wxString GetDisplayName(void);
    virtual void SetDisplayName(wxString sDisplayName);
    virtual wxGISEnumGPParameterDataType GetDataType(void);
    virtual void SetDataType(wxGISEnumGPParameterDataType nType);
    virtual wxGISEnumGPParameterDirection GetDirection(void);
    virtual void SetDirection(wxGISEnumGPParameterDirection nDirection);
    virtual wxArrayString* GetParameterDependencies(void);
    virtual void AddParameterDependency(wxString sDependency);
    virtual wxGISEnumGPParameterType GetParameterType(void);
    virtual void SetParameterType(wxGISEnumGPParameterType nType);
    virtual wxVariant GetValue(void);
    virtual void SetValue(wxVariant Val);
    virtual IGPDomain* GetDomain(void);
    virtual void SetDomain(IGPDomain* pDomain);
    virtual wxString GetMessage(void);
    virtual wxGISEnumGPMessageType GetÌessageType(void);
    virtual void SetMessage(wxGISEnumGPMessageType nType = wxGISEnumGPMessageUnknown, wxString sMsg = wxEmptyString);
protected:
    bool m_bAltered;
    bool m_bHasBeenValidated;
    bool m_bIsValid;
    wxString m_sName;
    wxString m_sDisplayName;
    wxGISEnumGPParameterDataType m_DataType;
    wxGISEnumGPParameterDirection m_Direction;
    wxGISEnumGPParameterType m_ParameterType;
    wxVariant m_Value;
    IGPDomain* m_pDomain;
    wxString m_sMessage;
    wxGISEnumGPMessageType m_nMsgType;
    //wxArrayString m_ParamDepStr;
};