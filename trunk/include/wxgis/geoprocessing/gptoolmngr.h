/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  tools manager.
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

#pragma once

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wx/thread.h"

class WXDLLIMPEXP_GIS_GP wxGISGPToolManager;

/** \class wxGISGPTaskThread gptoolmngr.h
 *  \brief A Geoprocessing tools execution thread.
 */
class wxGISGPTaskThread : public wxThread
{
public:
	wxGISGPTaskThread(wxGISGPToolManager* pMngr, IGPTool* pTool, ITrackCancel* pTrackCancel);
    virtual void *Entry();
    virtual void OnExit();
private:
    ITrackCancel* m_pTrackCancel;
    wxGISGPToolManager* m_pMngr;
    IGPTool* m_pTool;
};

/** \class wxGISGPToolManager gptoolmngr.h
 *  \brief A Geoprocessing tools manager.
 *
 *  Hold the geoprocessing tools list, execute tools, track tool execution statistics
 */
class WXDLLIMPEXP_GIS_GP wxGISGPToolManager
{
public:
    wxGISGPToolManager(wxXmlNode* pToolsNode, IGxCatalog* pCatalog = NULL);
    virtual ~wxGISGPToolManager(void);
    virtual IGPTool* GetTool(wxString sToolName);
    //return the thread ID
    virtual long OnExecute(IGPTool* pTool, ITrackCancel* pTrackCancel = NULL, IGPCallBack* pCallBack = NULL);
    virtual void OnFinish(unsigned long nThreadId, bool bHasErrors, IGPTool* pTool);
    typedef struct _toolinfo
    {
        wxString sClassName;
        int nCount;
    } TOOLINFO;
    typedef struct _threaddata
    {
        wxGISGPTaskThread* pThread;
        IGPCallBack* pCallBack;
        ITrackCancel* pTrackCancel;
    } THREADDATA;
protected:
    wxXmlNode* m_pToolsNode;
    std::map<wxString, TOOLINFO> m_ToolsMap; //internal name, class name
    IGxCatalog* m_pCatalog;
    short m_nMaxThreads, m_nRunningThreads;
    std::map<unsigned long, THREADDATA> m_TasksThreadMap; //internal name, class name
};

