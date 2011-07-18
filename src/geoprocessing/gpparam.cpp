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

#include <wx/tokenzr.h>

#define MULTI_PARAM_SEPARATOR wxString(wxT("#"))

wxGISGPParameter::wxGISGPParameter()
{
    m_bAltered = false;
    m_bHasBeenValidated = false;
    m_bIsValid = false;
    m_pDomain = NULL;
	m_nSelection = 0;
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

int wxGISGPParameter::GetSelDomainValue(void)
{
	return m_nSelection;
}

void wxGISGPParameter::SetSelDomainValue(int nNewSelection)
{
    m_bHasBeenValidated = false;
	m_nSelection = nNewSelection;
	if(m_pDomain)
	{
		switch(m_DataType)
		{
		case enumGISGPParamDTBool:
		case enumGISGPParamDTInteger:
		case enumGISGPParamDTDouble:
		case enumGISGPParamDTString:
		case enumGISGPParamDTSpatRef:
		case enumGISGPParamDTQuery:
			m_Value = m_pDomain->GetValue(nNewSelection);
			break;
		case enumGISGPParamDTStringList:
		case enumGISGPParamDTPathArray:
		case enumGISGPParamDTPath:
			//TODO: change m_Value ext to filters
			break;        
		case enumGISGPParamDTUnknown:
		default:
			break;        
		}
	}
}

wxString wxGISGPParameter::GetAsString(void)
{
    wxString sStrPar = m_Value.MakeString();
    return sStrPar;
}

bool wxGISGPParameter::SetFromString(wxString sParam)
{
	wxVariant oDomStr;
	if(m_pDomain)
	{
		int nPos = m_pDomain->GetPosByValue(wxVariant(sParam));
		if(nPos != wxNOT_FOUND)
			SetSelDomainValue(nPos);
	}

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
	case enumGISGPParamDTSpatRef:
	case enumGISGPParamDTPath:
	case enumGISGPParamDTPathArray:
    case enumGISGPParamDTStringChoice:
	case enumGISGPParamDTIntegerChoice:
	case enumGISGPParamDTDoubleChoice:
        m_Value = wxVariant(sParam);
        break;        
    case enumGISGPParamDTStringList:
	case enumGISGPParamDTIntegerList:
	case enumGISGPParamDTDoubleList:
		m_Value = wxStringTokenize(sParam, wxT(";"), wxTOKEN_RET_EMPTY);
        break;        
    case enumGISGPParamDTUnknown:
    default:
        m_Value = wxVariant(sParam);
        break;        
    }
    return true;
}

//--------------------------------------------------------------------------------
// wxGISGPMultiParameter
//--------------------------------------------------------------------------------

wxGISGPMultiParameter::wxGISGPMultiParameter() : wxGISGPParameter()
{
}

wxGISGPMultiParameter::~wxGISGPMultiParameter(void)
{
	Clear();
}

void wxGISGPMultiParameter::AddColumn(wxString sName)
{
	size_t nRowCount = GetRowCount();
	m_saColumnNames.Add(sName);
	size_t nColCount = GetColumnCount();
	for(size_t i = 0; i < nRowCount; ++i)
	{
		size_t nPos = i + nColCount;
		m_paParameters.insert(m_paParameters.begin() + nPos, NULL);
	}
}

void wxGISGPMultiParameter::RemoveColumn(size_t nIndex)
{
	size_t nOldColCount = GetColumnCount();
	size_t nRowCount = GetRowCount() - 1;
	for(size_t i = nRowCount; i >= 0; --i)
	{
		size_t nPos = i * nOldColCount + nIndex;
		m_paParameters.erase(m_paParameters.begin() + nPos);
	}
	m_saColumnNames.RemoveAt(nIndex);
}

size_t wxGISGPMultiParameter::GetColumnCount(void)
{
	return m_saColumnNames.GetCount();
}

size_t wxGISGPMultiParameter::GetRowCount(void)
{
	if(GetColumnCount() == 0)
		return 0;
	return m_paParameters.size() / GetColumnCount();
}

void wxGISGPMultiParameter::AddParameter(size_t nColIndex, size_t nRowIndex, IGPParameter* pParam)
{
	wxCHECK_RET(pParam, "the param should be not NULL"); 
	long nPos = nRowIndex * GetColumnCount() + nColIndex;
	if(m_paParameters.size() <= nPos)
	{
		while(m_paParameters.size() <= nPos)
			m_paParameters.push_back(NULL);
	}
	m_paParameters[nPos] = pParam;
}

wxGISEnumGPParameterDataType wxGISGPMultiParameter::GetDataType(void)
{
	return enumGISGPParamDTParamArray;
}

wxString wxGISGPMultiParameter::GetAsString(void)
{
	wxString sOutputStr;
	for(size_t i = 0; i < m_paParameters.size(); ++i)
		sOutputStr += m_paParameters[i]->GetAsString() + MULTI_PARAM_SEPARATOR;
	return sOutputStr;
}

bool wxGISGPMultiParameter::SetFromString(wxString sParam)
{
	wxArrayString saParams = wxStringTokenize(sParam, MULTI_PARAM_SEPARATOR, wxTOKEN_RET_EMPTY);
	for(size_t i = 0 ; i < saParams.GetCount(); ++i)
	{
		wxGISGPParameter* pParam = new wxGISGPParameter();
		if(pParam->SetFromString(saParams[i]))
			m_paParameters.push_back(pParam);
		else
		{
			wxDELETE(pParam);
			return false;
		}
	}
	return true;
}

wxString wxGISGPMultiParameter::GetColumnName(size_t nIndex)
{
	wxASSERT_MSG(nIndex < m_saColumnNames.size(), wxT("Column name index is greate than column count!"));
	return m_saColumnNames[nIndex];
}

bool wxGISGPMultiParameter::GetIsValid(void)
{
	for(size_t i = 0; i < m_paParameters.size(); ++i)
		if(!m_paParameters[i]->GetIsValid())
			return false;
	return true;
}

void wxGISGPMultiParameter::SetIsValid(bool bIsValid)
{
	for(size_t i = 0; i < m_paParameters.size(); ++i)
		m_paParameters[i]->SetIsValid(bIsValid);
}

IGPParameter* wxGISGPMultiParameter::GetParameter(size_t nCol, size_t nRow)
{
	long nPos = nRow * GetColumnCount() + nCol;
	return m_paParameters[nPos];
}

void wxGISGPMultiParameter::Clear()
{
	for(size_t i = 0; i < m_paParameters.size(); ++i)
		wxDELETE(m_paParameters[i]);
	m_paParameters.clear();
	//m_saColumnNames.Clear();
}
