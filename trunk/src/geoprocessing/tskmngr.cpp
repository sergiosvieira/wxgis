/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISTaskManager - class to communicate with task manager server
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

#include "wxgis/geoprocessing/tskmngr.h"
#include "wxgis/core/config.h"
#include "wxgis/core/format.h"

#define NET_WAIT_TIMEOUT 3

static wxGISTaskManager *g_pTaskManager( NULL );

extern WXDLLIMPEXP_GIS_GP wxGISTaskManager* const GetTaskManager(void)
{
    if(g_pTaskManager == NULL)
        g_pTaskManager = new wxGISTaskManager();
    g_pTaskManager->Reference();
	return g_pTaskManager;
}

//extern WXDLLIMPEXP_GIS_GP void SetTaskManager(wxGISTaskManager* pTM)
//{
//    if(g_pTaskManager != NULL)
//        wxDELETE(g_pTaskManager);
//	g_pTaskManager = pTM;
//}

#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_USER_EXPORTED_OBJARRAY(wxGISTaskMessagesArray);


//------------------------------------------------------------------
// wxGISTaskEvent
//------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISTaskEvent, wxEvent)

wxDEFINE_EVENT( wxGISTASK_ADD, wxGISTaskEvent );
wxDEFINE_EVENT( wxGISTASK_DEL, wxGISTaskEvent );
wxDEFINE_EVENT( wxGISTASK_CHNG, wxGISTaskEvent );
//wxDEFINE_EVENT( wxGISTASK_VOLCHNG, wxGISTaskEvent );
//wxDEFINE_EVENT( wxGISTASK_PERCENTCHNG, wxGISTaskEvent );
wxDEFINE_EVENT( wxGISTASK_MESSAGEADDED, wxGISTaskEvent );

//------------------------------------------------------------------
// wxGISTask
//------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTask, wxObject)

wxGISTask::wxGISTask(const wxXmlNode* pIniNode)
{
    //load
    m_nId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
    m_sName = pIniNode->GetAttribute(wxT("name"), NONAME);
    m_sDescription = pIniNode->GetAttribute(wxT("desc"), NONAME);
    m_sCat = pIniNode->GetAttribute(wxT("cat"), NONAME);
    m_nState = (wxGISEnumTaskStateType)GetDecimalValue(pIniNode, wxT("state"), enumGISTaskUnk);
    m_nPriority = GetDecimalValue(pIniNode, wxT("prio"), wxNOT_FOUND);
    m_dtBeg = GetDateValue(pIniNode, wxT("beg"), wxDateTime::Now());
    m_dtEnd = GetDateValue(pIniNode, wxT("end"), wxDateTime::Now());
    long nVolumeHi = GetDecimalValue(pIniNode, wxT("vol_hi"), 0);
    long nVolumeLo = GetDecimalValue(pIniNode, wxT("vol_lo"), 0);
    m_nVolume = wxULongLong(nVolumeHi, nVolumeLo);
    m_dfDone = GetFloatValue(pIniNode, wxT("done"), 0.0);

    m_pParams = NULL;

    wxXmlNode *Children = pIniNode->GetChildren();
    while(Children)
    {
        if(Children->GetName().IsSameAs(wxT("params"), false))
        {
            if(m_pParams)
                wxDELETE(m_pParams);
            m_pParams = new wxXmlNode(*Children);
        }
        //else if(Children->GetName().IsSameAs(wxT("messages"), false))
        //{
        //    LoadMessages(Children);
        //}
        Children = Children->GetNext();
    }
}

wxGISTask::~wxGISTask(void)
{
    wxDELETE(m_pParams);
}

int wxGISTask::GetId(void) const
{
    return m_nId;
}

wxString wxGISTask::GetName(void) const
{
    return m_sName;
}

wxString wxGISTask::GetDescription(void) const
{
    return m_sDescription;
}

wxString wxGISTask::GetCategory(void) const
{
    return m_sCat;
}

wxDateTime wxGISTask::GetDateBegin(void) const
{
    return m_dtBeg;
}

wxDateTime wxGISTask::GetDateEnd(void) const
{
    return m_dtEnd;
}

void wxGISTask::SetDateEnd(const wxDateTime &dtEnd)
{
    m_dtEnd = dtEnd;
}

wxULongLong  wxGISTask::GetVolume(void) const
{
    return m_nVolume;
}

void wxGISTask::SetVolume(wxULongLong nNewVolume)
{
    m_nVolume = nNewVolume;
}

long wxGISTask::GetPriority(void) const
{
    return m_nPriority;
}

void wxGISTask::SetPriority(long nPriority)
{
    m_nPriority = nPriority;
}

double wxGISTask::GetDone(void) const
{
    return m_dfDone;
}

void wxGISTask::SetDone(double dfDone)
{
    m_dfDone = dfDone;
}

wxGISTaskMessagesArray wxGISTask::GetMessages(void) const
{
    return m_oaMessages;
}

wxGISEnumTaskStateType wxGISTask::GetState(void)
{
    return m_nState;
}

void wxGISTask::SetState(wxGISEnumTaskStateType eState)
{
    m_nState = eState;
}

wxXmlNode* wxGISTask::GetParameters(void) const
{
    if(m_pParams)
        return new wxXmlNode(*m_pParams);
    return NULL;
}

void wxGISTask::ChangeTask(const wxXmlNode* pTaskNode)
{
    //load
    m_sName = pTaskNode->GetAttribute(wxT("name"), m_sName);
    m_sDescription = pTaskNode->GetAttribute(wxT("desc"), m_sDescription);
    m_sCat = pTaskNode->GetAttribute(wxT("cat"), m_sCat);
    m_nState = (wxGISEnumTaskStateType)GetDecimalValue(pTaskNode, wxT("state"), m_nState);
    m_nPriority = GetDecimalValue(pTaskNode, wxT("prio"), m_nPriority);
    m_dtBeg = GetDateValue(pTaskNode, wxT("beg"), m_dtBeg);
    m_dtEnd = GetDateValue(pTaskNode, wxT("end"), m_dtEnd);
    long nVolHi = GetDecimalValue(pTaskNode, wxT("vol_hi"), (long)m_nVolume.GetHi());
    long nVolLo = GetDecimalValue(pTaskNode, wxT("vol_lo"), (long)m_nVolume.GetLo());
    m_nVolume = wxULongLong(nVolHi, nVolLo);
    m_dfDone = GetFloatValue(pTaskNode, wxT("done"), m_dfDone);

    wxXmlNode *Children = pTaskNode->GetChildren();
    while(Children)
    {
        if(Children->GetName().IsSameAs(wxT("params"), false))
        {
            if(m_pParams)
                wxDELETE(m_pParams);
            m_pParams = new wxXmlNode(*Children);
        }
        //else if(Children->GetName().IsSameAs(wxT("messages"), false))
        //{
        //    LoadMessages(Children);
        //}
        Children = Children->GetNext();
    }
}

wxGISTaskMessage wxGISTask::GetMessage(long nMessageId) const
{
    for(size_t i = 0; i < m_oaMessages.size(); ++i)
    {
        if(m_oaMessages[i].m_nId == nMessageId)
        {
            return m_oaMessages[i];
        }
    }
    return wxGISTaskMessage(wxNOT_FOUND);
}

void wxGISTask::AddMessage(wxGISTaskMessage* pMessage)
{
    for(size_t i = 0; i < m_oaMessages.size(); ++i)
    {
        if(m_oaMessages[i].m_nId == pMessage->m_nId)
        {
            wxDELETE(pMessage);
            return;
        }
    }
    m_oaMessages.push_back(pMessage);
}

//void wxGISTask::LoadMessages(wxXmlNode* const pMessages)
//{
//    wxCHECK_RET(pMessages, wxT("Input wxXmlNode pointer  is null"));
//
//    /*if(m_oaMessages.GetCount() > 100)
//        m_oaMessages.RemoveAt(0, 50);
//
//    wxXmlNode* pMsg = pMessages->GetChildren();
//    while(pMsg)
//    {
//        wxGISEnumMessageType nType = (wxGISEnumMessageType)GetDecimalValue(pMsg, wxT("type"), (int)enumGISMessageUnk);
//        wxDateTime dt = GetDateValue(pMsg, wxT("dt"), wxDateTime::Now());
//        wxString sMsg = pMsg->GetAttribute(wxT("text"));
//        int nId = GetDecimalValue(pMsg, wxT("id"), wxNOT_FOUND);
//
//        bool bAdd(true);
//        for(size_t i = 0; i < m_oaMessages.size(); ++i)
//        {
//            if(m_oaMessages[i].m_nId == nId)
//            {
//                bAdd = false;
//                break;
//            }
//        }
//
//        if(bAdd && !sMsg.IsEmpty() && nId != wxNOT_FOUND)
//            m_oaMessages.Add(new wxGISTaskMessage(nId, sMsg, nType, dt));
//
//        pMsg = pMsg->GetNext();
//    }*/
//
//    wxXmlNode* pMsg = pMessages->GetChildren();
//    while(pMsg)
//    {
//        wxGISEnumMessageType nType = (wxGISEnumMessageType)GetDecimalValue(pMsg, wxT("type"), (int)enumGISMessageUnk);
//        wxDateTime dt = GetDateValue(pMsg, wxT("dt"), wxDateTime::Now());
//        wxString sMsg = pMsg->GetAttribute(wxT("text"));
//        int nId = GetDecimalValue(pMsg, wxT("id"), wxNOT_FOUND);
//        m_oaMessages.Add(new wxGISTaskMessage(nId, sMsg, nType, dt));
//
//        pMsg = pMsg->GetNext();
//    }
//}

//------------------------------------------------------------------
// wxClientTCPNetConnection
//------------------------------------------------------------------
#define HOST wxT("127.0.0.1")
#define PORT 9980
#define TIMEOUT 10

IMPLEMENT_CLASS(wxGISLocalClientConnection, INetConnection)

BEGIN_EVENT_TABLE( wxGISLocalClientConnection, INetConnection )
  EVT_SOCKET(SOCKET_ID,  wxGISLocalClientConnection::OnSocketEvent)
END_EVENT_TABLE()

wxGISLocalClientConnection::wxGISLocalClientConnection(void) : INetConnection()
{
    m_pSock = NULL;
}

wxGISLocalClientConnection::~wxGISLocalClientConnection(void)
{
    DestroyThreads();
}

bool wxGISLocalClientConnection::Connect(void)
{
	if(m_bIsConnected)
		return true;

    wxString sHost(HOST);
    unsigned short nPort(PORT);
    unsigned short nTimeOut(TIMEOUT);
    wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
    {
        sHost = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/tasks/host")), sHost);
        nPort = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/tasks/port")), nPort);
        nTimeOut = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/tasks/timeout")), nTimeOut);
    }

	//start conn
	IPaddress addr;
	addr.Hostname(sHost);
	addr.Service(nPort);

    // Create the socket
    wxSocketClient* pSock = new wxSocketClient(wxSOCKET_WAITALL | wxSOCKET_BLOCK | wxSOCKET_REUSEADDR);
	m_pSock = pSock;
    m_pSock->SetEventHandler(*this, SOCKET_ID);
    m_pSock->Notify(true);
    m_pSock->SetNotify(wxSOCKET_CONNECTION_FLAG|wxSOCKET_LOST_FLAG);
	m_pSock->SetTimeout(nTimeOut);
    pSock->Connect(addr, false);

    m_bIsConnecting = true;

    return CreateAndRunThreads();
}

bool wxGISLocalClientConnection::Disconnect(void)
{
	if(!m_bIsConnected)
		return true;

	m_bIsConnected = false;
    m_bIsConnecting = false;

    DestroyThreads();
    return true;
}

wxString wxGISLocalClientConnection::GetLastError(void) const
{
    return wxString::Format(_("Error (%d): %s"), m_pSock->LastError(), GetSocketErrorMsg(m_pSock->LastError()));
}

void wxGISLocalClientConnection::OnSocketEvent(wxSocketEvent& event)
{
    event.Skip(false);
    wxLogDebug(wxT("wxClientTCPNetConnection: event"));
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            wxLogDebug(wxT("wxClientTCPNetConnection: INPUT"));
        break;
        case wxSOCKET_OUTPUT:
            wxLogDebug(wxT("wxClientTCPNetConnection: OUTPUT"));
            break;
        case wxSOCKET_CONNECTION:
            wxLogDebug(wxT("wxClientTCPNetConnection: CONNECTION"));
            m_bIsConnected = true;
            m_bIsConnecting = false;
            {
            wxNetMessage msgin(enumGISNetCmdHello, enumGISNetCmdStUnk, enumGISPriorityHighest);
            wxGISNetEvent event(0, wxGISNET_MSG, msgin);
            PostEvent(event);
            }
        break;
        case wxSOCKET_LOST:
            wxLogDebug(wxT("wxClientTCPNetConnection: LOST"));
            {
                wxNetMessage msgin(enumGISNetCmdBye, enumGISNetCmdStUnk, enumGISPriorityHighest);
                if(!m_bIsConnected && m_bIsConnecting)
                {
                    m_bIsConnecting = false;
                }
                else
                {
                    m_bIsConnected = false;
                }
                wxGISNetEvent event(0, wxGISNET_MSG, msgin);
                PostEvent(event);
            }
        break;
        default:
            wxLogDebug(wxT("wxClientTCPNetConnection: default"));
            break;
    }
}


//------------------------------------------------------------------
// wxGISTaskManager
//------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTaskManager, wxEvtHandler)

BEGIN_EVENT_TABLE( wxGISTaskManager, wxEvtHandler )
  EVT_GISNET_MSG(wxGISTaskManager::OnGisNetEvent)
  EVT_TIMER(TIMER_ID, wxGISTaskManager::OnTimer)
END_EVENT_TABLE()

wxGISTaskManager::wxGISTaskManager() : wxGISPointer(), m_timer(this, TIMER_ID)
{
//    m_bTaskLoaded = false;
    //start task manager server
    StartTaskManagerServer();
    //create local connection object
    m_pConn = new wxGISLocalClientConnection();
    m_nConnectionPointConnCookie = m_pConn->Advise(this);
    //start timer to connect task manager server
    m_timer.Start(3000, false); //03 sec. disconnect timer
}

wxGISTaskManager::~wxGISTaskManager()
{
    m_timer.Stop();

    if(m_nConnectionPointConnCookie != wxNOT_FOUND)
        m_pConn->Unadvise(m_nConnectionPointConnCookie);
    wxDELETE(m_pConn);

    DeleteAllTasks();
}

void wxGISTaskManager::StartTaskManagerServer()
{
#ifdef __WXMSW__
    wxString sTaskMngrServerPath(wxT("wxgistaskmanager.exe"));
#else
    wxString sTaskMngrServerPath(wxT("wxgistaskmanager"));
#endif
    wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
        sTaskMngrServerPath = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/tasks/exe_path")), sTaskMngrServerPath);
    if(wxExecute(sTaskMngrServerPath + wxT(" -a"), wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE ) == 0)
    {
        wxLogError(_("Task Manager Server start failed. Path '%s'"), sTaskMngrServerPath.c_str());
    }
}

void wxGISTaskManager::OnGisNetEvent(wxGISNetEvent& event)
{
    wxNetMessage msg = event.GetNetMessage();
    wxString sErrMsg;
    switch(msg.GetCommand())
    {
    case enumGISNetCmdBye: //server disconnected
        DeleteAllTasks();
        //start task manager server
        StartTaskManagerServer();
        //start timer to connect task server
        m_timer.Start(5000, false);
        break;
    case enumGISNetCmdHello:
        {
            wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdGetChildren, enumGISPriorityHigh);
            m_pConn->SendNetMessage(msg);
        //QuereTasks(sErrMsg);
        }
        break;
    case enumGISNetCmdNote:
        //if message id != -1 add to notify
        if(msg.GetId() != wxNOT_FOUND)
        {
            TSKMNGR_RESULT Res = {msg.GetId(), msg.GetMessage(), msg.GetState()};
            PushResult(Res);
        }
        else
        {
            switch(msg.GetState())
            {
            case enumGISNetCmdStOk:
                wxLogMessage(msg.GetMessage());
                break;
            case enumGISNetCmdStErr:
                wxLogError(msg.GetMessage());
                break;
            //case enumGISCmdNoteVol:
            //    UpdateVolume(msg.GetXMLRoot()->GetChildren());
            //    break;
            //case enumGISCmdNotePercent:
            //    UpdatePercent(msg.GetXMLRoot()->GetChildren());
            //    break;
            case enumGISCmdNoteMsg:
                AddMessage(msg.GetXMLRoot()->GetChildren());
                break;
            default:
                wxLogVerbose(msg.GetMessage());
                break;
            }
        }
        break;
    case enumGISNetCmdCmd: //do something usefull
        if(msg.GetId() != wxNOT_FOUND)
        {
            TSKMNGR_RESULT Res = {msg.GetId(), msg.GetMessage(), msg.GetState()};
            PushResult(Res);
        }
        if(msg.GetXMLRoot())
        {
            switch(msg.GetState())
            {
            case enumGISCmdStAdd:
                AddTask(msg.GetXMLRoot()->GetChildren());
                break;
            case enumGISCmdStDel:
                DeleteTask(msg.GetXMLRoot()->GetChildren());
                break;
            case enumGISCmdStStart:
            case enumGISCmdStStop:
            case enumGISCmdStChng:
                ChangeTask(msg.GetXMLRoot()->GetChildren());
                break;
            case enumGISCmdGetChildren:
                LoadTasks(msg.GetXMLRoot()->GetChildren());
                break;
            case enumGISCmdStPriority://TODO: change priority for all task simultaniasly
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
}

void wxGISTaskManager::OnTimer( wxTimerEvent & event)
{
    wxCHECK_RET(m_pConn, wxT("Network connection object is null"));
    if(m_pConn->IsConnected())
    {
        m_timer.Stop();
        return;
    }
    m_pConn->Connect();
}

wxGISEnumReturnType wxGISTaskManager::AddTask(const wxString &sCategory, int nMaxExecTaskCount, const wxString &sName, const wxString &sDesc, const wxString &sPath, long nPriority, const wxXmlNode *pParameters, wxString & sMsg)
{
    long nId = wxNewId();

    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStAdd, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pNode->AddAttribute(wxT("cat"), sCategory);
        //for new cat creation
        SetDecimalValue(pNode, wxT("cat_max_task_exec"), nMaxExecTaskCount);
        pNode->AddAttribute(wxT("name"), sName);
        pNode->AddAttribute(wxT("desc"), sDesc);
        pNode->AddAttribute(wxT("exec"), sPath);
        SetDecimalValue(pNode, wxT("prio"), nPriority);
        SetDecimalValue(pNode, wxT("state"), enumGISTaskPaused);//enumGISTaskQuered
        pNode->AddChild(new wxXmlNode(*pParameters));

        m_pConn->SendNetMessage(msg);
    }

    return Wait(nId, sMsg);
}

wxGISEnumReturnType wxGISTaskManager::SetMaxTaskExec(const wxString &sCategory, int nMaxExecTaskCount, wxString & sMsg)
{
    long nId = wxNewId();

    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdSetParam, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("cat"));
        pNode->AddAttribute(wxT("name"), sCategory);
        SetDecimalValue(pNode, wxT("cat_max_task_exec"), nMaxExecTaskCount);
        m_pConn->SendNetMessage(msg);
    }

    return Wait(nId, sMsg);
}

wxGISEnumReturnType wxGISTaskManager::Wait(int nId, wxString & sMsg)
{
    //block until timeout, error or succeed. wait for nId message
    wxDateTime dtNow(wxDateTime::Now());
    wxTimeSpan sp = wxDateTime::Now() - dtNow;
    while(sp.GetSeconds() < NET_WAIT_TIMEOUT)
    {
        TSKMNGR_RESULT Res = PopResult(nId);
        if(Res.nId == nId)
        {
            sMsg = Res.sMsg;
            if(Res.nSate == enumGISNetCmdStErr)
            {
                return enumGISReturnFailed;
            }
            else
            {
                return enumGISReturnOk;
            }
        }

        wxTheApp->Yield(true);
        sp = wxDateTime::Now() - dtNow;
    };
    return enumGISReturnTimeout;
}

void wxGISTaskManager::PushResult(TSKMNGR_RESULT Res)
{
    wxCriticalSectionLocker locker(m_ResCritSect);
    m_saTskMngrResults.push_back(Res);
}

wxGISTaskManager::TSKMNGR_RESULT wxGISTaskManager::PopResult(int nId)
{
    wxCriticalSectionLocker locker(m_ResCritSect);
    TSKMNGR_RESULT Res = {wxNOT_FOUND, wxEmptyString, enumGISNetCmdStUnk};
    for(size_t i = 0; i < m_saTskMngrResults.size(); ++i)
    {
        if(m_saTskMngrResults[i].nId == nId)
        {
            Res = m_saTskMngrResults[i];
            m_saTskMngrResults.erase(m_saTskMngrResults.begin() + i);
            return Res;
        }
    }
    return Res;
}

void wxGISTaskManager::AddTask(const wxXmlNode* pIniNode)
{
    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
    long nId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
    if(m_moTasks[nId])
        //the task is already exist
        return;

    wxGISTask* pTask = new wxGISTask(pIniNode);
    m_moTasks[pTask->GetId()] = pTask;
    //notify
    wxGISTaskEvent event(pTask->GetId(), wxGISTASK_ADD);
    PostEvent(event);

    //quere messages
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdGetDetails, enumGISPriorityLow, nId);
    m_pConn->SendNetMessage(msg);
}

void wxGISTaskManager::DeleteTask(const wxXmlNode* pIniNode)
{
    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
    if(nTaskId == wxNOT_FOUND)
        return;
    wxDELETE(m_moTasks[nTaskId]);
    m_moTasks.erase(nTaskId);
    //notify
    wxGISTaskEvent event(nTaskId, wxGISTASK_DEL);
    PostEvent(event);
}

void wxGISTaskManager::DeleteAllTasks(void)
{
    for(wxGISTaskMap::iterator it = m_moTasks.begin(); it != m_moTasks.end(); ++it)
    {
        wxGISTaskEvent event(it->first, wxGISTASK_DEL);
        wxDELETE(it->second);
        PostEvent(event);
    }
    m_moTasks.clear();
}

void wxGISTaskManager::ChangeTask(const wxXmlNode* pIniNode)
{
    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
    if(nTaskId == wxNOT_FOUND)
        return;
    if(m_moTasks[nTaskId] == NULL)
        return;
    m_moTasks[nTaskId]->ChangeTask(pIniNode);
    //notify
    wxGISTaskEvent event(nTaskId, wxGISTASK_CHNG);
    PostEvent(event);
}

void wxGISTaskManager::LoadTasks(const wxXmlNode* pTasksNode)
{
    wxCHECK_RET(pTasksNode, wxT("Input wxXmlNode pointer is null"));
    //if(m_bTaskLoaded)
    //    return;
    if(!pTasksNode->GetName().IsSameAs(wxT("tasks"), false))
    {
        wxLogError(_("wxGISTaskManager: Input data is not tasks list"));
        return;
    }
    wxXmlNode* pTaskNode = pTasksNode->GetChildren();
    while(pTaskNode)
    {
        AddTask(pTaskNode);
        pTaskNode = pTaskNode->GetNext();
    }
    //m_bTaskLoaded = true;
}

wxGISEnumReturnType wxGISTaskManager::QuereTasks(wxString & sMsg)
{
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdGetChildren, enumGISPriorityHigh, nId);
    m_pConn->SendNetMessage(msg);
    return Wait(nId, sMsg);
}

wxGISTaskMap wxGISTaskManager::GetTasks(void)
{
    return m_moTasks;
}

wxGISTask* const wxGISTaskManager::GetTask(int nTaskId)
{
    return m_moTasks[nTaskId];
}

wxGISEnumReturnType wxGISTaskManager::DeleteTask(int nTaskId, wxString & sMsg)
{
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStDel, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        wxGISTask* pTask = GetTask(nTaskId);
        pNode->AddAttribute(wxT("cat"), pTask->GetCategory());
        SetDecimalValue(pNode, wxT("id"), nTaskId);
        m_pConn->SendNetMessage(msg);
    }
    else
    {
        sMsg = wxString(_("Net message create failed"));
        return enumGISReturnFailed;
    }
    return Wait(nId, sMsg);
}

wxGISEnumReturnType wxGISTaskManager::StartTask(int nTaskId, wxString & sMsg)
{
    //check if task is already started
    if(m_moTasks[nTaskId] && (m_moTasks[nTaskId]->GetState() == enumGISTaskQuered || m_moTasks[nTaskId]->GetState() == enumGISTaskQuered))
    {
        sMsg = wxString::Format(_("The task %d already started"), nTaskId);
        return enumGISReturnFailed;
    }
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStStart, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        wxGISTask* pTask = GetTask(nTaskId);
        pNode->AddAttribute(wxT("cat"), pTask->GetCategory());
        SetDecimalValue(pNode, wxT("id"), nTaskId);
        m_pConn->SendNetMessage(msg);
    }
    else
    {
        sMsg = wxString(_("Net message create failed"));
        return enumGISReturnFailed;
    }
    return Wait(nId, sMsg);
}

wxGISEnumReturnType wxGISTaskManager::StopTask(int nTaskId, wxString & sMsg)
{
    if(m_moTasks[nTaskId] && (m_moTasks[nTaskId]->GetState() == enumGISTaskPaused || m_moTasks[nTaskId]->GetState() == enumGISTaskError || m_moTasks[nTaskId]->GetState() == enumGISTaskDone))
    {
        sMsg = wxString::Format(_("The task %d already stopped"), nTaskId);
        return enumGISReturnFailed;
    }
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStStop, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        wxGISTask* pTask = GetTask(nTaskId);
        pNode->AddAttribute(wxT("cat"), pTask->GetCategory());
        SetDecimalValue(pNode, wxT("id"), nTaskId);
        m_pConn->SendNetMessage(msg);
    }
    else
    {
        sMsg = wxString(_("Net message create failed"));
        return enumGISReturnFailed;
    }
    return Wait(nId, sMsg);
}
/* TODO: Add const array id <-> priority
wxGISEnumReturnType wxGISTaskManager::ChangeTasksPriority(int nTaskId, long nPriority, wxString & sMsg)
{
    long nId = wxNewId();
    wxNetMessage msg(enumGISNetCmdCmd, enumGISCmdStPriority, enumGISPriorityHigh, nId);
    wxXmlNode* pRootNode = msg.GetXMLRoot();
    if(pRootNode)
    {
        wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        wxGISTask* pTask = GetTask(nTaskId);
        pNode->AddAttribute(wxT("cat"), pTask->GetCategory());
        SetDecimalValue(pNode, wxT("id"), nTaskId);
        SetDecimalValue(pNode, wxT("prio"), nPriority);
        m_pConn->SendNetMessage(msg);
    }
    else
    {
        sMsg = wxString(_("Net message create failed"));
        return enumGISReturnFailed;
    }
    return Wait(nId, sMsg);
}
*/
int wxGISTaskManager::GetRunTaskCount(const wxString& sCat)
{
    int nCount(0);
    for(wxGISTaskMap::const_iterator it = m_moTasks.begin(); it != m_moTasks.end(); ++it)
    {
        if(it->second && it->second->GetCategory().StartsWith(sCat))
        {
            if(it->second->GetState() == enumGISTaskWork)
                nCount++;
        }
    }
    return nCount;
}

//void wxGISTaskManager::UpdateVolume(const wxXmlNode* pIniNode)
//{
//    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
//    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
//    if(nTaskId == wxNOT_FOUND)
//        return;
//    wxGISTask* pTasks = m_moTasks[nTaskId];
//    if(pTasks == NULL)
//        return;
//
//    long nNewVolume = GetDecimalValue(pIniNode, wxT("vol"), pTasks->GetVolume());
//    pTasks->SetVolume(nNewVolume);
//    //notify
//    wxGISTaskEvent event(nTaskId, wxGISTASK_VOLCHNG);
//    PostEvent(event);
//}
//
//void wxGISTaskManager::UpdatePercent(const wxXmlNode* pIniNode)
//{
//    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
//    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
//    if(nTaskId == wxNOT_FOUND)
//        return;
//    wxGISTask* pTasks = m_moTasks[nTaskId];
//    if(pTasks == NULL)
//        return;
//
//    double dfDone = GetFloatValue(pIniNode, wxT("done"), pTasks->GetDone());
//    wxDateTime dtEnd = GetDateValue(pIniNode, wxT("end"), pTasks->GetDateEnd());
//    wxGISEnumTaskStateType eState = (wxGISEnumTaskStateType)GetDecimalValue(pIniNode, wxT("state"), (int)pTasks->GetState());
//    pTasks->SetDone(dfDone);
//    pTasks->SetDateEnd(dtEnd);
//    pTasks->SetState(eState);
//
//    //notify
//    wxGISTaskEvent event(nTaskId, wxGISTASK_PERCENTCHNG);
//    PostEvent(event);
//}

void wxGISTaskManager::AddMessage(const wxXmlNode* pIniNode)
{
    wxCHECK_RET(pIniNode, wxT("Input wxXmlNode pointer is null"));
    int nTaskId = GetDecimalValue(pIniNode, wxT("id"), wxNOT_FOUND);
    if(nTaskId == wxNOT_FOUND)
        return;
    wxGISTask* pTasks = m_moTasks[nTaskId];
    if(pTasks == NULL)
        return;

    long nMsgId = GetDecimalValue(pIniNode, wxT("msg_id"), wxNOT_FOUND);
    if(nMsgId == wxNOT_FOUND)
        return;

    wxGISEnumMessageType eType = (wxGISEnumMessageType)GetDecimalValue(pIniNode, wxT("msg_type"), enumGISMessageUnk);
    wxDateTime dt = GetDateValue(pIniNode, wxT("msg_dt"), wxDateTime::Now());
    wxString sInfoData = pIniNode->GetAttribute(wxT("msg"));

    if(!dt.IsValid() || sInfoData.IsEmpty())
        return;

    pTasks->AddMessage(new wxGISTaskMessage(nMsgId, sInfoData, eType, dt));

    //notify
    wxGISTaskEvent event(nTaskId, wxGISTASK_MESSAGEADDED, nMsgId);
    PostEvent(event);
}
