/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Task and TaskCategoryList classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Bishop
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

#include "wxgis/tskmngr_app/tskmngr.h"
#include "wxgis/core/process.h"

#include <wx/thread.h>

/** \class wxGISTask task.h
    \brief The task class.
*/

class wxGISTask : public wxGISProcess
{
    DECLARE_CLASS(wxGISTask)
public:
    wxGISTask(const wxXmlNode* pIniNode, IGISProcessParent* pProcessParent);
    ~wxGISTask(void);
    bool Delete();
    int GetId(void) const;
    //for store in xml
    wxXmlNode* GetAsXml(void);
    //for network send
    wxXmlNode* GetXmlDescription(void);

    void ChangeTask(const wxXmlNode* pTaskNode);

    //
    virtual long Execute(void);
    virtual void AddInfo(wxGISEnumMessageType nType, const wxString &sInfoData);
    virtual void UpdatePercent(const wxString &sPercentData);
    virtual void SetPriority(long nNewPriority);
    virtual long GetPriority(void) const {return m_nPriority;};
    virtual void OnTerminate(int pid, int status);
    virtual bool Start(void);
    virtual size_t GetMessageCount() const {return m_staMessages.size();};

    typedef struct _message
    {
        wxString m_sMessage;
        wxDateTime m_dt;
        wxGISEnumMessageType m_nType;
    }MESSAGE;
    virtual wxGISTask::MESSAGE GetMessage(size_t nIndex) const {return m_staMessages[nIndex];};
protected:
    int m_nId;
    wxGISTaskCategory* m_pTaskCategory;
    wxString m_sName, m_sDescription;    
    wxULongLong m_nVolume;//value in, eg. bytes
    wxString m_sExePath;
    wxString m_sExeConfPath; 
    long m_nPriority;
    wxXmlNode* m_pParams;
    double m_dfPrevDone;
    wxVector<MESSAGE> m_staMessages;
};

WX_DECLARE_HASH_MAP( int, wxGISTask*, wxIntegerHash, wxIntegerEqual, wxGISTaskMap );

/** \class wxGISTaskCategory task.h
    \brief The tasks category class.
*/

class wxGISTaskCategory : 
    public wxObject,
    public IGISProcessParent
{
    DECLARE_CLASS(wxGISTaskCategory)
    friend class wxGISTask;
public:
    wxGISTaskCategory(wxString sPath, wxGISTaskManager* pTaskManager, wxString sCatName = wxEmptyString);
    virtual ~wxGISTaskCategory(void);
    wxString GetName() const{return m_sName;};
    bool Save(void);
    bool AddTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    bool DelTask(int nTaskId, int nId, wxString &sErrMsg);
    bool StartTask(int nTaskId, int nId, wxString &sErrMsg);
    bool StopTask(int nTaskId, int nId, wxString &sErrMsg);
    bool ChangeTaskPriority(int nTaskId, long nPriority, int nId, wxString &sErrMsg);
    bool ChangeTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    bool GetTasks(int nId, wxString &sErrMsg);
    void GetTaskMessages(int nTaskId, int nId);
    int GetNewId();
    wxString GetTaskConfigPath(const wxString& sCatName);
    bool SetMaxExecTasks(int nMaxTaskExec, wxString &sErrMsg);
    short GetExecTaskCount(void) const {return m_nRunningTasks;};
    //IGISProcessParent
    virtual void OnFinish(wxGISProcess* pProcess, bool bHasErrors);
protected:
    void StartNextQueredTask(void);
    int GetQueredTaskId(void);
    void ChangeTask(int nTaskId);
    //void ChangeTaskVol(long nVolume, int nTaskId);
    //void ChangeTaskPercent(double dfDone, int nTaskId);
    void ChangeTaskMsg(wxGISEnumMessageType nType, const wxString &sInfoData, const wxDateTime &dt, int mMsgId, int nTaskId);
protected:
    wxGISTaskManager* m_pTaskManager;
    short m_nMaxTasks, m_nRunningTasks;
    wxString m_sXmlDocPath;
    //wxXmlDocument m_XmlDoc;
    wxGISTaskMap m_omTasks;
    wxString m_sName;
    wxCriticalSection m_CritSect, m_MsgCritSect;
};

/*class wxGPProcess : 
	public wxGISProcess
{
public:
    wxGPProcess(wxString sCommand, wxArrayString saParams, IProcessParent* pParent, ITrackCancel* pTrackCancel);
    virtual ~wxGPProcess(void);
    //virtual void OnTerminate(int status);
	virtual void ProcessInput(wxString sInputData);
protected:
    ITrackCancel* m_pTrackCancel;
	IProgressor* m_pProgressor;
};

typedef struct _wxgisexecddata
{
    IProcess* pProcess;
    IGPToolSPtr pTool;
    ITrackCancel* pTrackCancel;
} WXGISEXECDDATA;

/** \class wxGISGPToolManager gptoolmngr.h
 *  \brief A Geoprocessing tools manager.
 *
 *  Hold the geoprocessing tools list, execute tools, track tool execution statistics
 *//*
class WXDLLIMPEXP_GIS_GP wxGISGPToolManager : 
	public IProcessParent,
	public wxGISConnectionPointContainer
{
public:
    wxGISGPToolManager(void);
    virtual ~wxGISGPToolManager(void);
	virtual bool IsOk(void){return m_bIsOk;};
    virtual IGPToolSPtr GetTool(wxString sToolName, IGxCatalog* pCatalog = NULL);
    virtual int Execute(IGPToolSPtr pTool, ITrackCancel* pTrackCancel = NULL);
    virtual size_t GetToolCount();
    virtual wxString GetPopularTool(size_t nIndex);
    virtual void StartProcess(size_t nIndex);
    virtual void CancelProcess(size_t nIndex);
    //virtual WXGISEXECDDATA GetTask(size_t nIndex);
    virtual wxGISEnumTaskStateType GetProcessState(size_t nIndex);
    virtual wxDateTime GetProcessStart(size_t nIndex);
    virtual wxDateTime GetProcessFinish(size_t nIndex);
    virtual int GetProcessPriority(size_t nIndex);
    virtual void SetProcessPriority(size_t nIndex, int nPriority);
    virtual IGPToolSPtr GetProcessTool(size_t nIndex);
	//IProcessParent
    virtual void OnFinish(IProcess* pProcess, bool bHasErrors);
protected:
	virtual int GetPriorityTaskIndex();
    virtual bool ExecTask(WXGISEXECDDATA &data, size_t nIndex );
    virtual void AddPriority(int nIndex, int nPriority);
	virtual void RunNextTask(void);
public:
    typedef struct _toolinfo
    {
        wxString sClassName;
        int nCount;
        IGPToolSPtr pTool;
    } TOOLINFO;

    typedef struct _taskprioinfo
    {
        int nIndex;
        int nPriority;
    } TASKPRIOINFO;

protected:
    std::multimap<int, wxString> m_ToolsPopularMap;
    std::map<wxString, TOOLINFO> m_ToolsMap;
    std::vector<WXGISEXECDDATA> m_ProcessArray;
    std::vector<TASKPRIOINFO> m_aPriorityArray;
    short m_nMaxTasks, m_nRunningTasks;
    wxString m_sGeoprocessPath;
	bool m_bIsOk;
};
*/