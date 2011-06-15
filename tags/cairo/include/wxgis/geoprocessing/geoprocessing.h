/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
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

#define CTSTR _("Conversion Tools")
#define VECSTR _("Vector")
#define DMTSTR _("Data Management Tools")
#define RASSTR _("Raster")

#define TOOLBX_NAME wxT("wxGISToolbox")

#define SEGSTEP 3.0

//GPFunctionTool 1
//GPModelTool 2
//GPScriptTool 3
//GPCustomTool

/** \enum wxGISEnumGPMessageType
    \brief A geoprocessing tool parameter message type.
*/

enum wxGISEnumGPMessageType
{
    wxGISEnumGPMessageUnknown = 0,  /**< The message type is undefined */
	wxGISEnumGPMessageInformation,  /**< The information message*/
	wxGISEnumGPMessageError,        /**< The error message*/
	wxGISEnumGPMessageWarning,      /**< The warning message*/
	wxGISEnumGPMessageRequired,     /**< The required message - show required icon near param edit control*/
	wxGISEnumGPMessageOk,           /**< The ok message - show ok icon near param edit control*/
	wxGISEnumGPMessageNone          /**< The none message - show no icon near param edit control*/
};

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
	enumGISGPParamDTStringChoice,
	enumGISGPParamDTIntegerChoice,
	enumGISGPParamDTDoubleChoice,
    enumGISGPParamDTStringList,
	enumGISGPParamDTIntegerList,
	enumGISGPParamDTDoubleList,

	enumGISGPParamDTSpatRef,
	enumGISGPParamDTQuery,

	enumGISGPParamDTPath,
	enumGISGPParamDTPathArray,

	enumGISGPParamDTParamArray, /**< The array of IGPParameter's*/

	enumGISGPParamMax
};

class IGxTask
{
public:
    virtual ~IGxTask(void){};
    //virtual wxString GetName(void) = 0;
    virtual wxGISEnumTaskStateType GetState(void) = 0;
    virtual int GetPriority(void) = 0;
    virtual void SetPriority(int nNewPriority) = 0;
    virtual wxDateTime GetStart() = 0;
    virtual wxDateTime GetFinish() = 0;
    virtual double GetDonePercent() = 0;
    virtual wxString GetLastMessage() = 0;
    virtual bool StartTask() = 0;
    virtual bool StopTask() = 0;
    virtual bool PauseTask() = 0;
};

class IGPDomain
{
public:
    virtual ~IGPDomain(void){};
    virtual size_t GetCount(void) = 0;
	virtual wxString GetName(size_t nIndex) = 0;
	virtual wxVariant GetValue(size_t nIndex) = 0;
	virtual int GetPosByName(wxString sName) = 0;
	virtual int GetPosByValue(wxVariant oVal) = 0;
    virtual wxVariant GetValueByName(wxString soNameStr) = 0;
protected:
	wxArrayString m_asoNames;
	std::vector<wxVariant> m_asoData;
};

/** \class IGPParameter
 *  \brief A base class (virtual) for all geoprocessing tools parameters.
 */
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
	virtual int GetSelDomainValue(void) = 0;
	virtual void SetSelDomainValue(int nNewSelection) = 0;
    virtual wxString GetMessage(void) = 0;
    virtual wxGISEnumGPMessageType GetMessageType(void) = 0;
    virtual void SetMessage(wxGISEnumGPMessageType nType = wxGISEnumGPMessageUnknown, wxString sMsg = wxEmptyString) = 0;
    /** \fn wxString GetAsString(void)
     *  \brief Serialize parameter to string.
     *  \return The string representation of parameter
     */	
    virtual wxString GetAsString(void) = 0;
    /** \fn void SetFromString(wxString)
     *  \brief Serialize parameter from string.
     *  \param sParam The string representation of parameter
     */	
    virtual bool SetFromString(wxString sParam) = 0;
};

/** \typedef GPParameters
 *  \brief An parameters array.
 */
typedef std::vector<IGPParameter*> GPParameters;

class IGPTool
{
public:
    virtual ~IGPTool(void){};
    virtual wxString GetDisplayName(void) = 0;
    virtual wxString GetName(void) = 0;
    virtual wxString GetCategory(void) = 0;
    virtual bool Execute(ITrackCancel* pTrackCancel) = 0;
    virtual bool Validate(void) = 0;
    virtual GPParameters* GetParameterInfo(void) = 0;
    /** \fn wxString GetAsString(void)
     *  \brief Serialize tool parameters to string.
     *  \return The string representation of tool parameters
     */	
    virtual wxString GetAsString(void) = 0;
    /** \fn void SetFromString(wxString sParams)
     *  \brief Serialize tool parameters to string.
     *  \param sParams The string representation of tool parameters
     */	
    virtual bool SetFromString(wxString sParams) = 0;
    virtual void SetCatalog(IGxCatalog* pCatalog) = 0;
    virtual IGxCatalog* const GetCatalog(void) = 0;
    //virtual GetToolType(void) = 0;
    virtual void Copy(IGPTool* pTool) = 0;
};

DEFINE_SHARED_PTR(IGPTool);

class IGPCallBack
{
public:
    virtual ~IGPCallBack(void){};
    virtual void OnFinish(bool bHasErrors = false, IGPToolSPtr pTool = IGPToolSPtr()) = 0;
};



