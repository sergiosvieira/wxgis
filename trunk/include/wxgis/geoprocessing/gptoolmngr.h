/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  tools manager.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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
#include "wxgis/core/process.h"

class WXDLLIMPEXP_GIS_GP wxGISGPToolManager;

/** \class wxGPProcess gptoolmngr.h
 *  \brief A Geoprocess class.
 */
class wxGPProcess : 
	public wxGISProcess
{
public:
    wxGPProcess(wxString sCommand, IProcessParent* pParent, ITrackCancel* pTrackCancel);
    virtual ~wxGPProcess(void);
    virtual void OnTerminate(int pid, int status);
	//IStreamReader
	virtual void ProcessInput(wxString sInputData);
protected:
    ITrackCancel* m_pTrackCancel;
	IProgressor* m_pProgressor;
	//wxTextOutputStream* m_pOutTxtStream;
};

typedef struct _wxgisexecddata
{
    IProcess* pProcess;
    IGPTool* pTool;
    ITrackCancel* pTrackCancel;
    IGPCallBack* pCallBack;
} WXGISEXECDDATA;

/** \class wxGISGPToolManager gptoolmngr.h
 *  \brief A Geoprocessing tools manager.
 *
 *  Hold the geoprocessing tools list, execute tools, track tool execution statistics
 */
class WXDLLIMPEXP_GIS_GP wxGISGPToolManager : public IProcessParent
{
public:
    wxGISGPToolManager(wxXmlNode* pToolsNode);
    virtual ~wxGISGPToolManager(void);
    virtual IGPTool* GetTool(wxString sToolName, IGxCatalog* pCatalog = NULL);
    virtual int OnExecute(IGPTool* pTool, ITrackCancel* pTrackCancel = NULL, IGPCallBack* pCallBack = NULL);
    virtual size_t GetToolCount();
    virtual wxString GetPopularTool(size_t nIndex);
    virtual wxGISEnumTaskStateType GetProcessState(size_t nIndex);
    virtual void StartProcess(size_t nIndex);
    virtual void CancelProcess(size_t nIndex);
    //virtual void PauseProcess(size_t nIndex);
    //virtual WXGISEXECDDATA GetTask(size_t nIndex);
	//IProcessParent
    virtual void OnFinish(IProcess* pProcess, bool bHasErrors);
    virtual wxDateTime GetProcessStart(size_t nIndex);
    virtual wxDateTime GetProcessFinish(size_t nIndex);
    virtual int GetProcessPriority(size_t nIndex);
    virtual IGPTool* GetProcessTool(size_t nIndex);
    virtual void SetProcessPriority(size_t nIndex, int nPriority);
	virtual int GetPriorityTaskIndex();
protected:
    virtual bool ExecTask(WXGISEXECDDATA &data);
    virtual void AddPriority(int nIndex, int nPriority);
public:
    typedef struct _toolinfo
    {
        wxString sClassName;
        int nCount;
        IGPTool* pTool;
    } TOOLINFO;

    typedef struct _taskprioinfo
    {
        int nIndex;
        int nPriority;
    } TASKPRIOINFO;

protected:
    wxXmlNode* m_pToolsNode;
    std::map<wxString, TOOLINFO> m_ToolsMap;
    std::multimap<int, wxString> m_ToolsPopularMap;
    std::vector<WXGISEXECDDATA> m_ProcessArray;
    std::vector<TASKPRIOINFO> m_aPriorityArray;
    short m_nMaxTasks, m_nRunningTasks;
    wxString m_sGeoprocessPath;
};

