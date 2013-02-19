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
#pragma once

#include "wxgis/net/netfactory.h"
#include "wxgis/net/netevent.h"

#include <wx/dynarray.h>

/** \class wxGISTaskMessage taskmngr.h
    \brief The Task message.
*/
class WXDLLIMPEXP_GIS_GP wxGISTaskMessage
{
public:
    wxGISTaskMessage(int nId, const wxString &sMsg = wxEmptyString, wxGISEnumMessageType nType = enumGISMessageUnk, const wxDateTime &dt = wxDateTime::Now())
    {
        m_nId = nId;
        m_sMessage = sMsg;
        m_dt = dt;
        m_nType = nType;
    }
    wxString m_sMessage;
    wxDateTime m_dt;
    wxGISEnumMessageType m_nType;
    int m_nId;
};

WX_DECLARE_USER_EXPORTED_OBJARRAY(wxGISTaskMessage, wxGISTaskMessagesArray, WXDLLIMPEXP_GIS_GP);


class WXDLLIMPEXP_GIS_GP wxGISTaskEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_ADD, wxGISTaskEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_DEL, wxGISTaskEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_CHNG, wxGISTaskEvent);
//wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_VOLCHNG, wxGISTaskEvent);
//wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_PERCENTCHNG, wxGISTaskEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_MESSAGEADDED, wxGISTaskEvent);

/** \class wxGISTaskEvent taskmngr.h
    \brief The Task event.
*/
class WXDLLIMPEXP_GIS_GP wxGISTaskEvent : public wxEvent
{
public:
    wxGISTaskEvent(int nTaskId = 0, wxEventType eventType = wxGISTASK_ADD, long nMessageId = wxNOT_FOUND) : wxEvent(nTaskId, eventType)
	{        
        m_nMessageId = nMessageId;
	}
	wxGISTaskEvent(const wxGISTaskEvent& event) : wxEvent(event), m_nMessageId(event.m_nMessageId)
	{        
	}
    virtual wxEvent *Clone() const { return new wxGISTaskEvent(*this); }
    long GetMessageId(void) const {return m_nMessageId;};
protected:
    long m_nMessageId;
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxGISTaskEvent)
};

typedef void (wxEvtHandler::*wxGISTaskEventFunction)(wxGISTaskEvent&);

#define wxGISTaskEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxGISTaskEventFunction, func)

#define EVT_GISTASK_ADD(func)  wx__DECLARE_EVT0(wxGISTASK_ADD, wxGISTaskEventHandler(func))
#define EVT_GISTASK_DEL(func)  wx__DECLARE_EVT0(wxGISTASK_DEL, wxGISTaskEventHandler(func))
#define EVT_GISTASK_CHNG(func)  wx__DECLARE_EVT0(wxGISTASK_CHNG, wxGISTaskEventHandler(func))
//#define EVT_GISTASK_VOLCHNG(func)  wx__DECLARE_EVT0(wxGISTASK_VOLCHNG, wxGISTaskEventHandler(func))
//#define EVT_GISTASK_PERCENTCHNG(func)  wx__DECLARE_EVT0(wxGISTASK_PERCENTCHNG, wxGISTaskEventHandler(func))
#define EVT_GISTASK_MESSAGEADDED(func)  wx__DECLARE_EVT0(wxGISTASK_MESSAGEADDED, wxGISTaskEventHandler(func))

/** \class wxGISTask taskmngr.h
    \brief The task class.
*/

class WXDLLIMPEXP_GIS_GP wxGISTask : public wxObject
{
    DECLARE_CLASS(wxGISTask)
public:
    wxGISTask(const wxXmlNode* pIniNode);
    ~wxGISTask(void);
    int GetId(void) const;
    wxString GetName(void) const;
    wxString GetDescription(void) const;
    wxString GetCategory(void) const;
    wxDateTime GetDateBegin(void) const;
    wxDateTime GetDateEnd(void) const;
    void SetDateEnd(const wxDateTime &dtEnd);
    wxULongLong GetVolume(void) const;
    void SetVolume(wxULongLong nNewVolume);
    long GetPriority(void) const;
    void SetPriority(long nPriority);
    double GetDone(void) const;
    void SetDone(double dfDone);
    wxGISEnumTaskStateType GetState(void);
    void SetState(wxGISEnumTaskStateType eState);
    wxXmlNode* GetParameters(void) const;
    void ChangeTask(const wxXmlNode* pTaskNode);
    wxGISTaskMessagesArray GetMessages(void) const;
    wxGISTaskMessage GetMessage(long nMessageId) const;
    void AddMessage(wxGISTaskMessage* pMessage);
protected:
    int m_nId;
    wxString m_sName, m_sDescription, m_sCat;
    wxDateTime m_dtBeg, m_dtEnd;
    wxULongLong m_nVolume;//value in, eg. bytes
    wxGISEnumTaskStateType m_nState;
    long m_nPriority;
    double m_dfDone;
    wxXmlNode* m_pParams;
    wxGISTaskMessagesArray m_oaMessages;
};

WX_DECLARE_HASH_MAP( int, wxGISTask*, wxIntegerHash, wxIntegerEqual, wxGISTaskMap );

/** \class wxGISLocalClientConnection tskmngr.h
    \brief The connection to communicate with local Task Manager Server.
*/

class wxGISLocalClientConnection : 
	public INetConnection
{
    DECLARE_CLASS(wxGISLocalClientConnection)
    enum{
        // id for sockets
        SOCKET_ID = 111
    };
public:
	wxGISLocalClientConnection(void);
	virtual ~wxGISLocalClientConnection(void);
	//INetConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
    virtual wxString GetLastError(void) const;
protected:
    //events
    virtual void OnSocketEvent(wxSocketEvent& event);
private:
    DECLARE_EVENT_TABLE()
};

/** \class wxGISTaskManager tskmngr.h
    \brief Task Manager Server communicate class.
*/
class WXDLLIMPEXP_GIS_GP wxGISTaskManager : 
    public wxEvtHandler,
	public wxGISConnectionPointContainer,
    public wxGISPointer
{
    DECLARE_CLASS(wxGISTaskManager)
    enum{
        TIMER_ID = 1015
    };
public:
    wxGISTaskManager(void);
    ~wxGISTaskManager(void);
    void StartTaskManagerServer(void);
    wxGISEnumReturnType AddTask(const wxString &sCategory, int nMaxExecTaskCount, const wxString &sName, const wxString &sDesc, const wxString &sPath, long nPriority, const wxXmlNode *pParameters, wxString & sMsg);
    wxGISTaskMap GetTasks(void);
    wxGISTask* const GetTask(int nTaskId);
    wxGISEnumReturnType DeleteTask(int nTaskId, wxString & sMsg);
    wxGISEnumReturnType StartTask(int nTaskId, wxString & sMsg);
    wxGISEnumReturnType StopTask(int nTaskId, wxString & sMsg);
    wxGISEnumReturnType ChangeTaskPriority(int nTaskId, long nPriority, wxString & sMsg);
    
    int GetRunTaskCount(const wxString& sCat);
    wxGISEnumReturnType SetMaxTaskExec(const wxString &sCategory, int nMaxExecTaskCount, wxString & sMsg);
    //typedefs
    typedef struct _tskmng_result
    {
        int nId;
        wxString sMsg;
        wxGISNetCommandState nSate;
    } TSKMNGR_RESULT;
protected:
    void AddTask(const wxXmlNode* pIniNode);
    void DeleteTask(const wxXmlNode* pIniNode);
    void DeleteAllTasks(void);
    void ChangeTask(const wxXmlNode* pIniNode);
    //void UpdateVolume(const wxXmlNode* pIniNode);
    //void UpdatePercent(const wxXmlNode* pIniNode);
    void AddMessage(const wxXmlNode* pIniNode);
    void LoadTasks(const wxXmlNode* pTasksNode);
    wxGISEnumReturnType QuereTasks(wxString & sMsg);
    
    void PushResult(TSKMNGR_RESULT Res);
    TSKMNGR_RESULT PopResult(int nId);
    wxGISEnumReturnType Wait(int nId, wxString & sMsg);    
    //events
    void OnGisNetEvent(wxGISNetEvent& event);
    void OnTimer( wxTimerEvent & event);
protected:
    wxGISLocalClientConnection* m_pConn;
    wxTimer m_timer;
    wxVector<TSKMNGR_RESULT> m_saTskMngrResults; 
    wxCriticalSection m_ResCritSect;
    wxGISTaskMap m_moTasks;
    long m_nConnectionPointConnCookie;
    //bool m_bTaskLoaded;
private:
    DECLARE_EVENT_TABLE()
};

/** \fn wxGISTaskManager* const GetTaskManager(void)
 *  \brief Global wxGISTaskManager getter.
 */	

WXDLLIMPEXP_GIS_GP wxGISTaskManager* const GetTaskManager(void);

/** \fn void SetTaskManager(wxGISTaskManager* pTM)
    \brief Global wxGISTaskManager setter.
	\param pTM The wxGISTaskManager pointer.
 */	

//WXDLLIMPEXP_GIS_GP void SetTaskManager(wxGISTaskManager* pTM);