/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  parameter class.
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

#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/geoprocessing/gpdomain.h"

wxGISGPParameter::wxGISGPParameter()
{
    m_bAltered = false;
    m_bHasBeenValidated = false;
    m_bIsValid = false;
    m_pDomain = NULL;
    m_nMsgType = wxGISEnumGPMessageUnknown;
    m_sMessage = wxEmptyString;
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
    m_bHasBeenValidated = false;
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

wxString wxGISGPParameter::GetMessage(void)
{
    return m_sMessage;
}

wxGISEnumGPMessageType wxGISGPParameter::GetMessageType(void)
{
    return m_nMsgType;
}

void wxGISGPParameter::SetMessage(wxGISEnumGPMessageType nType, wxString sMsg)
{
    m_sMessage = sMsg;
    m_nMsgType = nType;
}

wxString wxGISGPParameter::GetAsString(void)
{
    wxString sStrPar = m_Value.MakeString();
    //if(m_pDomain)
    //    sStrPar += wxString::Format(wxT("~%d"), m_pDomain->GetSel());
    return sStrPar;
}

bool wxGISGPParameter::SetFromString(wxString sParam)
{
    switch(m_DataType)
    {
    case enumGISGPParamDTBool:
        m_Value = wxVariant((bool)wxAtoi(sParam));
        break;        
	case enumGISGPParamDTInteger:
        m_Value = wxVariant(wxAtoi(sParam));
        break;        
	case enumGISGPParamDTDouble:
        m_Value = wxVariant(wxAtof(sParam));
        break;        
	case enumGISGPParamDTString:
        m_Value = wxVariant(sParam);
        break;        
	case enumGISGPParamDTSpatRef:
	case enumGISGPParamDTPath:
        m_Value = wxVariant(sParam);
        if(m_pDomain)
        {
            wxString sVal = wxVariant(sParam);
            if(!sVal.IsEmpty())
            {
                wxGISGPGxObjectDomain* poDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pDomain);
                wxFileName oName(sVal);
                for(size_t i = 0; i < poDomain->GetCount(); i++)
                {
                    IGxObjectFilter* poFilter = poDomain->GetFilter(i);
                    if(poFilter)
                    {
                        if(oName.GetExt().CmpNoCase(poFilter->GetExt()) == 0 || poFilter->GetExt() == wxEmptyString)
                        {
                            poDomain->SetSel(i);
                            break;
                        }
                    }
                }
            }
        }
        break;        
    case enumGISGPParamDTStringList:
        m_Value = wxVariant(sParam);
        if(m_pDomain)
        {
            wxString sVal = wxVariant(sParam);
            wxGISGPStringDomain* poDomain = dynamic_cast<wxGISGPStringDomain*>(m_pDomain);
            for(size_t i = 0; i < poDomain->GetCount(); i++)
            {
                if(poDomain->GetInternalString(i) == sVal)
                {
                    poDomain->SetSel(i);
                    break;
                }
            }
        }
        break;        
	case enumGISGPParamDTPathArray:
        m_Value = wxVariant(sParam);
        if(m_pDomain)
        {
            wxString sVal = wxVariant(sParam);
            if(!sVal.IsEmpty())
            {
                wxGISGPGxObjectDomain* poDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pDomain);
                wxFileName oName(sVal);
                for(size_t i = 0; i < poDomain->GetCount(); i++)
                {
                    IGxObjectFilter* poFilter = poDomain->GetFilter(i);
                    if(poFilter)
                    {
                        if(oName.GetExt().CmpNoCase(poFilter->GetExt()) == 0 || poFilter->GetExt() == wxEmptyString)
                        {
                            poDomain->SetSel(i);
                            break;
                        }
                    }
                }
            }
        }
        break;        
    case enumGISGPParamDTUnknown:
    default:
        m_Value = wxVariant(sParam);
        break;        
    }
    return true;
}

