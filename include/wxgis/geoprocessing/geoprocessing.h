/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  geoprocessing header.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/catalog/catalog.h"

#define CTSTR _("Conversion Tools")
#define VECSTR _("Vector")
#define DMTSTR _("Data Management Tools")
#define RASSTR _("Raster")
#define STATSTR _("Statistics Tools")

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
    enumGISGPParameterTypeOptional
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
	enumGISGPParamDTText,

	enumGISGPParamDTStringChoice,
	enumGISGPParamDTIntegerChoice,
	enumGISGPParamDTDoubleChoice,

    enumGISGPParamDTFieldAnyChoice,
    enumGISGPParamDTFieldStringChoice,
    enumGISGPParamDTFieldIntegerChoice,
    enumGISGPParamDTFieldRealChoice,
    enumGISGPParamDTFieldDateChoice,
    enumGISGPParamDTFieldTimeChoice,
    enumGISGPParamDTFieldDateTimeChoice,
    enumGISGPParamDTFieldBinaryChoice,
	

    enumGISGPParamDTStringList,
	enumGISGPParamDTIntegerList,
	enumGISGPParamDTDoubleList,
	enumGISGPParamDTStringChoiceEditable,

	enumGISGPParamDTSpatRef,
	enumGISGPParamDTQuery,

	enumGISGPParamDTPath,
	enumGISGPParamDTFolderPath,
	enumGISGPParamDTPathArray,

	enumGISGPParamDTParamArray, /**< The array of IGPParameter's*/

	enumGISGPParamMax
};

class IGxTask
{
public:
    virtual ~IGxTask(void){};
    virtual wxGISEnumTaskStateType GetState(void) = 0;
    virtual long GetPriority(void) const = 0;
    virtual wxDateTime GetStart() const = 0;
    virtual wxDateTime GetFinish() const = 0;
    virtual double GetDonePercent() const = 0;
    //virtual wxString GetLastMessage() = 0;
    virtual bool StartTask() = 0;
    virtual bool StopTask() = 0;
    virtual bool SetPriority(long nNewPriority) = 0;
    //virtual bool PauseTask() = 0;
};

/*
class IGPTool
{
public:
    virtual ~IGPTool(void){};
    virtual const wxString GetDisplayName(void) = 0;
    virtual const wxString GetName(void) = 0;
    virtual const wxString GetCategory(void) = 0;
    virtual bool Execute(ITrackCancel* pTrackCancel) = 0;
    virtual bool Validate(void) = 0;
    virtual GPParameters GetParameterInfo(void) = 0;
    /** \fn wxString GetAsString(void)
     *  \brief Serialize tool parameters to string.
     *  \return The string representation of tool parameters
     */	/*
    virtual const wxString GetAsString(void) = 0;
    /** \fn void SetFromString(wxString sParams)
     *  \brief Serialize tool parameters to string.
     *  \param sParams The string representation of tool parameters
     */	/*
    virtual bool SetFromString(const wxString& sParams) = 0;
    virtual void SetCatalog(IGxCatalog* pCatalog) = 0;
    virtual IGxCatalog* const GetCatalog(void) = 0;
    //virtual GetToolType(void) = 0;
    virtual void Copy(IGPTool* const pTool) = 0;
};
*/
