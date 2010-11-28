/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  geoprocessingui header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/geoprocessing/geoprocessing.h"

#define SASHCTRLID	1012
#define TOOLVIEWCTRLID	1015
#define TASKSVIEWCTRLID	1016

/** \class IToolManagerUI
 *  \brief A base class (virtual) for config and run tool from UI.
 */

class IToolManagerUI
{
public:
    virtual ~IToolManagerUI(void){};
    /** \fn bool OnPrepareTool(wxWindow* pParentwnd, wxString sToolName, wxString sInputPath, IGPCallBack* pCallBack, bool bSync)
     *  \brief Show tool config dialog, execute tool and return result via callback.
     *  \param pParentWnd The Parent wxWindow created dialogs
     *  \param sToolName The tool name (internal short name)
     *  \param sInputPath The input path for tool (shows in config dialog)
     *  \param pCallBack The callback executed on tool execution ends
     *  \param bSync If bSync parameter is true, shows Execution dialog, and interface blocked. If bSync parameter is false, created GxTask Object and inserted to wxGxToolExecuteView.
     *  \return false on error.
     */	
    virtual bool OnPrepareTool(wxWindow* pParentWnd, wxString sToolName, wxString sInputPath, IGPCallBack* pCallBack, bool bSync) = 0;
};



