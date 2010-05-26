/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  geoprocessing header.
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

#include "wxgis/datasource/datasource.h"
#include "wxgis/catalog/catalog.h"

//GPFunctionTool 1
//GPModelTool 2
//GPScriptTool 3
//GPCustomTool 

enum wxGISEnumGPParameterType
{
    enumGISGPParameterTypeRequired = 1,
    enumGISGPParameterTypeOptional,
    enumGISGPParameterTypeDerived 
};

enum wxGISEnumGPParameterDirection
{
    enumGISGPParameterDirectionInput = 1,
    enumGISGPParameterDirectionOutput 
};

enum wxGISEnumGPParameterDataType
{
    enumGISGPParamDTUnknown = 0,
	enumGISGPParamDTBool,
	enumGISGPParamDTInteger,
	enumGISGPParamDTDouble,
	enumGISGPParamDTString,

	enumGISGPParamDTPath,
	enumGISGPParamDTPathArray
};

class IGPDomain
{
public:
    virtual ~IGPDomain(void){};
};

class IGPParameter
{
public:
    virtual ~IGPParameter(void){};
    virtual bool GetAltered(void) = 0;
    virtual void SetAltered(bool bAltered) = 0;
    virtual bool GetHasBeenValidated(void) = 0;
    virtual void SetHasBeenValidated(bool bHasBeenValidated) = 0;
    virtual bool GetIsValid(void) = 0;
    virtual void SetIsValid(bool bIsValidated) = 0;
    virtual wxString GetName(void) = 0;
    virtual void SetName(wxString sName) = 0;
    virtual wxString GetDisplayName(void) = 0;
    virtual void SetDisplayName(wxString sDisplayName) = 0;
    virtual wxGISEnumGPParameterDataType GetDataType(void) = 0;
    virtual void SetDataType(wxGISEnumGPParameterDataType nType) = 0;
    virtual wxGISEnumGPParameterDirection GetDirection(void) = 0;
    virtual void SetDirection(wxGISEnumGPParameterDirection nDirection) = 0;
    virtual wxArrayString* GetParameterDependencies(void) = 0;
    virtual void AddParameterDependency(wxString sDependency) = 0;
    virtual wxGISEnumGPParameterType GetParameterType(void) = 0;
    virtual void SetParameterType(wxGISEnumGPParameterType nType) = 0;
    virtual wxVariant GetValue(void) = 0;
    virtual void SetValue(wxVariant Val) = 0;
    virtual IGPDomain* GetDomain(void) = 0;
    virtual void SetDomain(IGPDomain* pDomain) = 0;
};

typedef std::vector<IGPParameter*> GPParameters;

class IGPTool
{
public:
    virtual ~IGPTool(void){};
    virtual wxString GetDisplayName(void) = 0;
    virtual wxString GetName(void) = 0;
    virtual wxString GetCategory(void) = 0;
    //virtual Execute(...) = 0;
    virtual GPParameters* GetParameterInfo(void) = 0;
    //virtual GetToolType(void) = 0;
    virtual void SetCatalog(IGxCatalog* pCatalog) = 0;
    virtual IGxCatalog* GetCatalog(void) = 0;
};

