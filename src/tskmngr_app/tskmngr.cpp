/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Task manager class.
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

#include "wxgis/tskmngr_app/tskmngr.h"
#include "wxgis/tskmngr_app/task.h"
#include "wxgis/version.h"
#include "wxgis/core/format.h"

#include <wx/cmdline.h>
#include <wx/dir.h>

//----------------------------------------------------------------------------
// wxGISServer
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTaskManager, wxObject)

wxGISTaskManager::wxGISTaskManager(void)
{
    m_pNetworkService = NULL;

    m_nExitState = enumGISNetCmdStExit;
    m_nMaxExecTasks = wxThread::GetCPUCount() * 2;
    wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
    {
        m_nExitState = (wxGISNetCommandState)oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISMon/app/exit_state")), enumGISNetCmdStExit);//<wxGISMon><app exit_state="9"/></wxGISMon>
        m_nMaxExecTasks = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISMon/app/max_exec_tasks")), m_nMaxExecTasks);
   }
    m_nIdCounter = 0;
}

wxGISTaskManager::~wxGISTaskManager(void)
{
    if(m_pNetworkService)
        Exit();
}

void wxGISTaskManager::Exit(void)
{
    //delete network service
    if(m_pNetworkService)
    {

        m_pNetworkService->Stop();
        SetNetworkService(NULL);        
        m_pNetworkService = NULL;
    }

    wxGISTaskCategoryMap::iterator it;
    for( it = m_omCategories.begin(); it != m_omCategories.end(); ++it )
    {
        //save and delete
        wxGISTaskCategory* pTaskCategory = it->second;
        wxDELETE(pTaskCategory);
    }    
}

bool wxGISTaskManager::Init(void)
{
    //load tasks and etc.
    
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
		m_sUserConfigDir = oConfig.GetLocalConfigDirNonPortable() + wxFileName::GetPathSeparator() + wxString(TSKDIR);
        wxDir dir(m_sUserConfigDir); 
        if( dir.IsOpened() )
        {
            wxString sFilename;
        
            bool cont = dir.GetFirst(&sFilename, wxT("*.xml"), wxDIR_FILES | wxDIR_HIDDEN);
            while ( cont )
            {
                LoadTasks(m_sUserConfigDir + wxFileName::GetPathSeparator() + sFilename);
                cont = dir.GetNext(&sFilename);
            }
        }
	}

    //create network service
    m_pNetworkService = new wxGISLocalNetworkService(static_cast<INetEventProcessor*>(this));
    SetNetworkService(m_pNetworkService);

    if(!m_pNetworkService->Start())
    {
        wxDELETE(m_pNetworkService);
        return false;
    }
    return true;
}

void wxGISTaskManager::ProcessNetEvent(wxGISNetEvent& event)
{
    wxNetMessage msg = event.GetNetMessage();
    wxCHECK_RET(msg.GetXMLRoot(), wxT("No xml root in message"));

    wxString sErr;

    switch(msg.GetCommand())
    {
    case enumGISNetCmdCmd:
        switch(msg.GetState())
        {
            case enumGISCmdStAdd: //Add new task to specified category. Return result of operation
                if(!AddTask(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
                {
                    wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
                    msgout.SetMessage(sErr);
                    SendNetMessage(msgout, event.GetId());
                }
                break;
            case enumGISCmdStDel: //Delete task by TaskId. Return result of operation
                if(!DelTask(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
                {
                    wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
                    msgout.SetMessage(sErr);
                    SendNetMessage(msgout, event.GetId());              
                }
                break;       
            case enumGISCmdStChng://Change task by TaskId. Return result of operation
                if(!ChangeTask(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
                {
                    wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
                    msgout.SetMessage(sErr);
                    SendNetMessage(msgout, event.GetId());                
                }
                break; 
            case enumGISCmdStStart:
                if(!StartTask(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
                {
                    wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
                    msgout.SetMessage(sErr);
                    SendNetMessage(msgout, event.GetId());                
                }
                break; 
            case enumGISCmdStStop:
                if(!StopTask(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
                {
                    wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
                    msgout.SetMessage(sErr);
                    SendNetMessage(msgout, event.GetId());                
                }
                break;             
/*            case enumGISCmdStPriority: TODO:
                if(!ChangeTaskPriority(msg.GetXMLRoot()->GetChildren(), msg.GetId(), sErr))
                {
                    wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
                    msgout.SetMessage(sErr);
                    SendNetMessage(msgout, event.GetId());                
                }
                break; */            
            case enumGISCmdGetChildren://Get tasks lits
                if(!GetTasks(msg.GetXMLRoot(), msg.GetId(), sErr))
                {
                    wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
                    msgout.SetMessage(sErr);
                    SendNetMessage(msgout, event.GetId());                
                }
                break; 
            case enumGISCmdGetDetails://Get task details
                GetTaskDetails(msg.GetXMLRoot(), msg.GetId());
                break;             
            case enumGISCmdSetParam://Set max parallel executed tasks
                if(!SetMaxExecTasks(msg.GetXMLRoot(), msg.GetId(), sErr))
                {
                    wxNetMessage msgout(enumGISNetCmdNote, enumGISNetCmdStErr, enumGISPriorityLow, msg.GetId());
                    msgout.SetMessage(sErr);
                    SendNetMessage(msgout, event.GetId());                
                }
                break;             
            default:            
                break;
        }
        break;
    case enumGISNetCmdSetExitSt:
        SetExitState(msg.GetState());
        break;
    case enumGISNetCmdBye://get from client exit state: exit if no connection, stay executing tasks or wait any new connections
        if(m_pNetworkService->GetConnectionCount() <= 0)
            //exit or not
            OnExit();
        break;
    }    
}

void wxGISTaskManager::SetExitState(wxGISNetCommandState nExitState)
{
    if(nExitState == m_nExitState || m_nExitState == enumGISNetCmdStNoExit)//if equil do nothing
        return;

    if(nExitState == enumGISNetCmdStNoExit)//if no exit set no exit
    {
        m_nExitState = enumGISNetCmdStNoExit;
        return;
    }

    if(nExitState == enumGISNetCmdStExit && m_nExitState == enumGISNetCmdStExitAfterExec)
    {
        m_nExitState = enumGISNetCmdStExitAfterExec;
        return;
    }

    m_nExitState = nExitState;
}

void wxGISTaskManager::OnExit(void)
{
    if(m_nExitState == enumGISNetCmdStNoExit)
        return;
    if(m_nExitState == enumGISNetCmdStExitAfterExec && GetExecTaskCount() > 0)
        return;
    wxTheApp->Exit();
}

int wxGISTaskManager::GetExecTaskCount(void)
{
    int nRes(0);
    for(wxGISTaskCategoryMap::const_iterator it = m_omCategories.begin(); it != m_omCategories.end(); ++it)
    {        
        if(it->second)
            nRes += it->second->GetExecTaskCount();
    }
    return nRes;
}

bool wxGISTaskManager::AddTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!ValidateTask(pTaskNode))
    {
        sErrMsg = wxString(_("Add task failed. Ivalid task description"));
        return false;
    }
    wxCriticalSectionLocker locker(m_TaskLock);
    
    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxT("default"));
    wxLogMessage(_("Add task to category '%s' "), sCat);
    if(!m_omCategories[sCat])
    {
        wxString sNewPath = m_sUserConfigDir + wxFileName::GetPathSeparator() + ReplaceForbiddenCharsInFileName(sCat) + wxString(wxT(".xml"));
        m_omCategories[sCat] = new wxGISTaskCategory( sNewPath, this, sCat);
        int nMaxTaskExec = GetDecimalValue(pTaskNode, wxT("cat_max_task_exec"), wxThread::GetCPUCount());
        if(!m_omCategories[sCat]->SetMaxExecTasks(nMaxTaskExec, sErrMsg))
            return false;
    }    
    return m_omCategories[sCat]->AddTask(pTaskNode, nId, sErrMsg);
}

wxString wxGISTaskManager::ReplaceForbiddenCharsInFileName(const wxString & name, const wxString ch/*=wxT(“_”)*/)
{
    wxString sOutput = name;
    wxString forbidden = wxFileName::GetForbiddenChars();
    forbidden += wxT(" ()[]{}");
    int size=forbidden.Length();
    for (int i = 0; i < size; ++i)
        sOutput.Replace( wxString(forbidden[i]), ch, true);
    sOutput.Replace( ch + ch, ch, true);
    return sOutput.MakeLower();
}

bool wxGISTaskManager::DelTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;

    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    wxLogMessage(_("Delete task #%d from category '%s' "), nTaskId, sCat);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->DelTask(nTaskId, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}

bool wxGISTaskManager::StartTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;

    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    wxLogMessage(_("StartTask task #%d from category '%s' "), nTaskId, sCat);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->StartTask(nTaskId, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}

bool wxGISTaskManager::StopTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;

    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    wxLogMessage(_("StopTask task #%d from category '%s' "), nTaskId, sCat);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->StopTask(nTaskId, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}

/* TODO:
bool wxGISTaskManager::ChangeTasksPriority(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;

    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    long nTaskPriority = GetDecimalValue(pTaskNode, wxT("prio"), wxNOT_FOUND);
    wxLogMessage(_("Change task #%d from category '%s' priority to %d"), nTaskId, sCat, nTaskPriority);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->ChangeTaskPriority(nTaskId, nTaskPriority, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}
*/
bool wxGISTaskManager::ChangeTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return false;
    wxCriticalSectionLocker locker(m_TaskLock);

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    wxLogMessage(_("Change task #%d from category '%s' "), nTaskId, sCat);
    if(m_omCategories[sCat])
        return m_omCategories[sCat]->ChangeTask(pTaskNode, nId, sErrMsg);
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sCat.c_str());
    return false;
}

bool wxGISTaskManager::GetTasks(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode* is null"));
    wxString sTasksCat = pTaskNode->GetAttribute(wxT("tasks_cat"));
    if(!sTasksCat.IsEmpty() && m_omCategories[sTasksCat])
    {
        return m_omCategories[sTasksCat]->GetTasks(nId, sErrMsg);
    }
    else
    {
        bool bRes = true;
        for(wxGISTaskCategoryMap::iterator it = m_omCategories.begin(); it != m_omCategories.end(); ++it)
        {
            bool bTempRes;
            bTempRes = it->second->GetTasks(nId, sErrMsg);
            if(!bTempRes)
                bRes = false;
        }
        return bRes;
    }
}

bool wxGISTaskManager::ValidateTask(const wxXmlNode* pTaskNode)
{
    return pTaskNode->HasAttribute(wxT("name")) && pTaskNode->HasAttribute(wxT("exec")) && pTaskNode->HasAttribute(wxT("state"));
}

void wxGISTaskManager::LoadTasks(const wxString & sPath)
{
    wxGISTaskCategory* pCat = new wxGISTaskCategory(sPath, this);
    m_omCategories[pCat->GetName()] = pCat;
}

int wxGISTaskManager::GetNewId()
{
    wxCriticalSectionLocker locker(m_CounterLock);

    return m_nIdCounter++;
}

wxString wxGISTaskManager::GetTaskConfigPath( const wxString& sCatName )
{
    wxString sSubTaskConfigDir = m_sUserConfigDir + wxFileName::GetPathSeparator() + SUBTSKDIR;
    if(!wxDirExists(sSubTaskConfigDir))
		wxFileName::Mkdir(sSubTaskConfigDir, 0755, wxPATH_MKDIR_FULL);
    wxDateTime dt(wxDateTime::Now());
    wxString sTaskConfigPath = sSubTaskConfigDir + wxFileName::GetPathSeparator() + wxString::Format(wxT("%s_%s%u.xml"), ReplaceForbiddenCharsInFileName(sCatName).c_str(), dt.GetValue().ToString().c_str(), wxNewId());
    return sTaskConfigPath;
}

void wxGISTaskManager::SendNetMessage(wxNetMessage & msg, int nId)
{
    if(m_pNetworkService)
        m_pNetworkService->SendNetMessage(msg, nId);
}

bool wxGISTaskManager::SetMaxExecTasks(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    sErrMsg = wxString(wxT("Input wxXmlNode* is null"));
    wxCHECK_MSG(pTaskNode, false, sErrMsg);
    wxString sTasksCat = pTaskNode->GetAttribute(wxT("name"));
    if(!sTasksCat.IsEmpty())
    {
        if(!m_omCategories[sTasksCat])
        {
            wxString sNewPath = m_sUserConfigDir + wxFileName::GetPathSeparator() + ReplaceForbiddenCharsInFileName(sTasksCat) + wxString(wxT(".xml"));
            m_omCategories[sTasksCat] = new wxGISTaskCategory( sNewPath, this, sTasksCat);
        }    

        int nMaxTaskExec = GetDecimalValue(pTaskNode, wxT("cat_max_task_exec"), wxThread::GetCPUCount());
        sErrMsg = wxString(_("Maximum parallel executed task set successful"));
        return m_omCategories[sTasksCat]->SetMaxExecTasks(nMaxTaskExec, sErrMsg);
    }
    sErrMsg = wxString::Format(_("The category '%s' is undefined"), sTasksCat.c_str());
    return false;
}

void wxGISTaskManager::GetTaskDetails(const wxXmlNode* pTaskNode, int nId)
{
    wxCHECK_RET(pTaskNode, wxT("Input wxXmlNode* is null"));
    if(!pTaskNode->HasAttribute(wxT("id")))
        return;

    wxString sCat = pTaskNode->GetAttribute(wxT("cat"), wxEmptyString);
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    
    if(m_omCategories[sCat])
    {
        m_omCategories[sCat]->GetTaskMessages(nTaskId, nId);
    }
}

//TODO: timer for task execution (look for categories where no running task and task quered)
//If found - it->second->StartNextQueredTask();