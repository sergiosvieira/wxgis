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
#include "wx/process.h"


class WXDLLIMPEXP_GIS_GP wxGISGPToolManager;

///** \class wxGISGPTaskThread gptoolmngr.h
// *  \brief A Geoprocessing tools execution thread.
// */
//class wxGISGPTaskThread : public wxThread
//{
//public:
//	wxGISGPTaskThread(wxGISGPToolManager* pMngr, IGPTool* pTool, ITrackCancel* pTrackCancel);
//    virtual void *Entry();
//    virtual void OnExit();
//private:
//    ITrackCancel* m_pTrackCancel;
//    wxGISGPToolManager* m_pMngr;
//    IGPTool* m_pTool;
//};

/** \class wxGPProcess gptoolmngr.h
 *  \brief A Geoprocess callback.
 */
class wxGPProcess : public wxProcess
{
public:
    wxGPProcess(wxGISGPToolManager* pToolManager, ITrackCancel* pTrackCancel);
    virtual ~wxGPProcess(void);
    virtual void OnTerminate(int pid, int status);
    virtual void OnStartExecution(void);
    virtual void OnCancelExecution(void);
protected:
    wxGISGPToolManager* m_pToolManager;
    ITrackCancel* m_pTrackCancel;
};

typedef struct _wxgisexecddata
{
    wxGPProcess* pGPProcess;
    IGPTool* pTool;
    wxString sCommand;
    wxGISEnumTaskStateType nState;
    ITrackCancel* pTrackCancel;
    IGPCallBack* pCallBack;
} WXGISEXECDDATA;

/** \class wxGISGPToolManager gptoolmngr.h
 *  \brief A Geoprocessing tools manager.
 *
 *  Hold the geoprocessing tools list, execute tools, track tool execution statistics
 */
class WXDLLIMPEXP_GIS_GP wxGISGPToolManager
{
public:
	friend class wxGPProcess;
public:
    wxGISGPToolManager(wxXmlNode* pToolsNode);
    virtual ~wxGISGPToolManager(void);
    virtual IGPTool* GetTool(wxString sToolName, IGxCatalog* pCatalog = NULL);
    virtual int OnExecute(IGPTool* pTool, ITrackCancel* pTrackCancel = NULL, IGPCallBack* pCallBack = NULL);
    virtual size_t GetToolCount();
    virtual wxString GetPopularTool(size_t nIndex);
    //virtual void StartProcess(size_t nIndex);
    //virtual void CancelProcess(size_t nIndex);
    //virtual void PauseProcess(size_t nIndex);
    //virtual WXGISEXECDDATA GetTask(size_t nIndex);
protected:
    virtual void OnFinish(wxGPProcess* pGPProcess, bool bHasErrors);
    virtual bool ExecTask(WXGISEXECDDATA data);
public:
    typedef struct _toolinfo
    {
        wxString sClassName;
        int nCount;
        IGPTool* pTool;
    } TOOLINFO;

protected:
    wxXmlNode* m_pToolsNode;
    std::map<wxString, TOOLINFO> m_ToolsMap;
    std::multimap<int, wxString> m_ToolsPopularMap;
    std::vector<WXGISEXECDDATA> m_ProcessArray;
    short m_nMaxTasks, m_nRunningTasks;
};

