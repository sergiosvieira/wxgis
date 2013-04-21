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

#include "wxgis/tskmngr_app/task.h"
#include "wxgis/core/format.h"

//------------------------------------------------------------------------------
// wxGISTask
//------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISTask, wxObject)

wxGISTask::wxGISTask(const wxXmlNode* pIniNode, IGISProcessParent* pProcessParent) : wxGISProcess(pProcessParent)
{
    m_pTaskCategory = dynamic_cast<wxGISTaskCategory*>(pProcessParent);
    if(m_pTaskCategory)
        m_nId = m_pTaskCategory->GetNewId();
    if(pIniNode)
    {
        //load
        m_sName = pIniNode->GetAttribute(wxT("name"), NONAME);
        m_sDescription = pIniNode->GetAttribute(wxT("desc"), NONAME);
        long nVolHi = GetDecimalValue(pIniNode, wxT("vol_hi"), 0);
        long nVolLo = GetDecimalValue(pIniNode, wxT("vol_lo"), 0);
        m_nVolume = wxULongLong(nVolHi, nVolLo);
        m_sExePath = pIniNode->GetAttribute(wxT("exec"), wxEmptyString);
        m_nState = (wxGISEnumTaskStateType)GetDecimalValue(pIniNode, wxT("state"), 0);
        if(m_nState == enumGISTaskWork)
            m_nState = enumGISTaskQuered;
        m_nPriority = GetDecimalValue(pIniNode, wxT("prio"), wxNOT_FOUND);

        wxString sCat = pIniNode->GetAttribute(wxT("cat"), wxT("default"));
        if(m_pTaskCategory)
            m_sExeConfPath = pIniNode->GetAttribute(wxT("execonf"), m_pTaskCategory->GetTaskConfigPath(sCat));
        
        if(pIniNode->GetChildren())
            m_pParams = new wxXmlNode(*pIniNode->GetChildren());
        else
            m_pParams = NULL;

        if(m_nState == enumGISTaskDone)
            m_dfDone = 100;
        else
            m_dfDone = 0;//wxNetMessage::GetFloatValue(pIniNode, wxT("done"), 0.0);
        m_dfPrevDone = m_dfDone;
    }   
}

wxGISTask::~wxGISTask(void)
{
    Stop();
    wxDELETE(m_pParams);
}

int wxGISTask::GetId(void) const
{
    return m_nId;
}

wxXmlNode* wxGISTask::GetAsXml(void)
{
    //save
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("task"));
    pNode->AddAttribute(wxT("name"), m_sName);
    pNode->AddAttribute(wxT("desc"), m_sDescription);
    SetDecimalValue(pNode, wxT("vol_hi"), (long)m_nVolume.GetHi());
    SetDecimalValue(pNode, wxT("vol_lo"), (long)m_nVolume.GetLo());
    pNode->AddAttribute(wxT("exec"), m_sExePath);
    SetDecimalValue(pNode, wxT("state"), m_nState);
    SetDecimalValue(pNode, wxT("prio"), m_nPriority);
    pNode->AddAttribute(wxT("execonf"), m_sExeConfPath);

    pNode->AddChild(new wxXmlNode(*m_pParams));
    //wxNetMessage::SetFloatValue(pNode, wxT("done"), m_dfDone);
 
    return pNode;
}

wxXmlNode* wxGISTask::GetXmlDescription(void)
{
    //save
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("task"));
    SetDecimalValue(pNode,wxT("id"), m_nId);
    pNode->AddAttribute(wxT("name"), m_sName);
    pNode->AddAttribute(wxT("desc"), m_sDescription);
    SetDecimalValue(pNode, wxT("vol_hi"), (long)m_nVolume.GetHi());
    SetDecimalValue(pNode, wxT("vol_lo"), (long)m_nVolume.GetLo());
    SetDecimalValue(pNode, wxT("state"), m_nState);
    SetDecimalValue(pNode, wxT("prio"), m_nPriority);
    SetFloatValue(pNode, wxT("done"), m_dfDone);
    if(m_dtBeg.IsValid())
        SetDateValue(pNode, wxT("beg"), m_dtBeg);
    if(m_dtEstEnd.IsValid())
        SetDateValue(pNode, wxT("end"), m_dtEstEnd);
    pNode->AddChild(new wxXmlNode(*m_pParams));
    /*
    wxXmlNode* pMsgsNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("messages"));
    //send last 50 messages
    int nBegPos = m_staMessages.size() > 50 ? m_staMessages.size() - 50 : 0;
    for(size_t i = nBegPos; i < m_staMessages.size(); ++i)
    {
        wxXmlNode* pMsgNode = new wxXmlNode(pMsgsNode, wxXML_ELEMENT_NODE, wxT("message"));
        SetDecimalValue(pMsgNode, wxT("id"), int(m_nId * 100 + i));
        SetDecimalValue(pMsgNode, wxT("type"), m_staMessages[i].m_nType);
        SetDateValue(pMsgNode, wxT("dt"), m_staMessages[i].m_dt);
        pMsgNode->AddAttribute(wxT("text"), m_staMessages[i].m_sMessage);
    }
    */

    return pNode;
}

bool wxGISTask::Delete()
{
    Stop();
    //delete config
    wxRemoveFile(m_sExeConfPath);
    delete this;
    return true;
}

void wxGISTask::ChangeTask(const wxXmlNode* pTaskNode)
{
    Stop();
    //load
    m_sName = pTaskNode->GetAttribute(wxT("name"), m_sName);
    m_sDescription = pTaskNode->GetAttribute(wxT("desc"), m_sDescription);
    m_nState = (wxGISEnumTaskStateType)GetDecimalValue(pTaskNode, wxT("state"), m_nState);
    if(m_nState == enumGISTaskWork)
        m_nState = enumGISTaskQuered;
    m_nPriority = GetDecimalValue(pTaskNode, wxT("prio"), m_nPriority);
    wxRemoveFile(m_sExeConfPath);        
    if(pTaskNode->GetChildren())
        m_pParams = new wxXmlNode(*pTaskNode->GetChildren());
    else
        m_pParams = NULL;
    m_dfDone = 0;
}

long wxGISTask::Execute(void)
{
    //long wxExecute( const wxString &  command,  int  flags = wxEXEC_ASYNC,  wxProcess *  callback = NULL,  const wxExecuteEnv *  env = NULL  );
    wxString sCmd = m_sExePath + wxT(" --execonf \"") + m_sExeConfPath + wxT("\"");
    if(m_pParams)
    {
        wxXmlAttribute *pAttr = m_pParams->GetAttributes();
        while(pAttr)
        {
            sCmd += wxT(" --") + pAttr->GetName();
            if(pAttr->GetValue().Find(wxT(" ")) == wxNOT_FOUND)
                sCmd += wxT(" ") + pAttr->GetValue();
            else
                sCmd += wxT(" \"") + pAttr->GetValue() + wxT("\"");

            pAttr = pAttr->GetNext();
        }
    }
    return wxExecute(sCmd, wxEXEC_ASYNC, this);
}

void wxGISTask::AddInfo(wxGISEnumMessageType nType, const wxString &sInfoData)
{
    wxString sRest;
    if(sInfoData.StartsWith(wxT("VOL: "), &sRest))
    {
        unsigned long long nVol;
        sRest.ToULongLong(&nVol);
        m_nVolume = nVol;
        //m_nVolume = wxAtol(sRest);//update vol then update percents
        //if(m_pTaskCategory)
        //    m_pTaskCategory->ChangeTaskVol(m_nVolume, m_nId);
    }
    else
    {
        if(sInfoData.IsEmpty())
            return;

        MESSAGE msg = {sInfoData, wxDateTime::Now(), nType};
        m_staMessages.push_back(msg);
        if(m_pTaskCategory)
            m_pTaskCategory->ChangeTaskMsg(nType, sInfoData, wxDateTime::Now(), m_staMessages.size() - 1, m_nId);
    }
    //if(m_pTaskCategory)
    //    m_pTaskCategory->ChangeTask(m_nId);
}

void wxGISTask::UpdatePercent(const wxString &sPercentData)
{
    wxGISProcess::UpdatePercent(sPercentData);
    if(m_dfDone - m_dfPrevDone > 0.1)
    {
        m_dfPrevDone = m_dfDone;
        if(m_pTaskCategory)
            m_pTaskCategory->ChangeTask(m_nId);
            //m_pTaskCategory->ChangeTaskPercent(m_dfDone, m_nId);
    }
}

void wxGISTask::SetPriority(long nNewPriority)
{
    m_nPriority = nNewPriority;
}

void wxGISTask::OnTerminate(int pid, int status)
{
    wxGISProcess::OnTerminate(pid, status);
    m_dfPrevDone = 0;

    if(m_pTaskCategory)
        m_pTaskCategory->ChangeTask(m_nId);
}

bool wxGISTask::Start(void)
{
    m_dfPrevDone = 0;
    bool bReturn = wxGISProcess::Start();
    if(bReturn && m_pTaskCategory)
    {
        m_pTaskCategory->ChangeTask(m_nId);
    }
    return bReturn;
}

//------------------------------------------------------------------------------
// wxGISTaskCategory
//------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISTaskCategory, wxObject)

wxGISTaskCategory::wxGISTaskCategory(wxString sPath, wxGISTaskManager* pTaskManager, wxString sCatName)
{
    m_pTaskManager = pTaskManager;
    m_sXmlDocPath = sPath;
    m_nMaxTasks = wxThread::GetCPUCount();
    m_nRunningTasks = 0;
    m_sName = sCatName;    
    //try to load xml
    wxXmlDocument doc(m_sXmlDocPath);
    if(doc.IsOk())
    {
        //load doc
        wxXmlNode* pRoot = doc.GetRoot();
        m_sName = pRoot->GetAttribute(wxT("cat"), m_sName);
        m_nMaxTasks = GetDecimalValue(pRoot, wxT("max_exec_task"), m_nMaxTasks);
        wxXmlNode* pTaskNode = pRoot->GetChildren();
        while(pTaskNode)
        {
            wxString sErrMsg;
            if(!AddTask(pTaskNode, wxNOT_FOUND, sErrMsg))
                wxLogError(sErrMsg);
            pTaskNode = pTaskNode->GetNext();
        }
    }
}

wxGISTaskCategory::~wxGISTaskCategory(void)
{
    Save();
    for(wxGISTaskMap::iterator it = m_omTasks.begin(); it != m_omTasks.end(); ++it)
    {
        wxDELETE(it->second);
    }
}

bool wxGISTaskCategory::Save(void)
{
    wxXmlDocument doc;
    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("tasks"));
    pRootNode->AddAttribute(wxT("cat"), m_sName);
    SetDecimalValue(pRootNode, wxT("max_exec_task"), m_nMaxTasks);
    for(wxGISTaskMap::iterator it = m_omTasks.begin(); it != m_omTasks.end(); ++it)
    {
        if(it->second)
        {
            wxXmlNode* pTaskNode = it->second->GetAsXml();
            if(pTaskNode)
            {
                pRootNode->AddChild(pTaskNode);
            }
        }
    }
    doc.SetRoot(pRootNode);
    return doc.Save(m_sXmlDocPath);
}

bool wxGISTaskCategory::AddTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    sErrMsg = wxString(_("Input wxXmlNode pointer is null"));
    wxCHECK_MSG(pTaskNode, false, wxT("Input wxXmlNode pointer is null"));

    wxGISTask *pTask = new wxGISTask(pTaskNode, this);
    m_omTasks[pTask->GetId()] = pTask;

    //store in xml
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStAdd, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Add task succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = pTask->GetXmlDescription();
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        pRootNode->AddChild(pTaskNode);
        
        m_pTaskManager->SendNetMessage(msgout);

        StartNextQueredTask();

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save chnges to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
}

bool wxGISTaskCategory::DelTask(int nTaskId, int nId, wxString &sErrMsg)
{
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }

    if(m_omTasks[nTaskId])
    {
        if(m_omTasks[nTaskId]->GetState() == enumGISTaskWork)
        {
            m_nRunningTasks--;
        }
        if( m_omTasks[nTaskId]->Delete() )
        {
            m_omTasks.erase(nTaskId);
        }
    }
    //store in xml
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStDel, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Delete task succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        SetDecimalValue(pTaskNode, wxT("id"), nTaskId);
        pRootNode->AddChild(pTaskNode);
        
        m_pTaskManager->SendNetMessage(msgout); 

        StartNextQueredTask();

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save chnges to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
}

bool wxGISTaskCategory::StartTask(int nTaskId, int nId, wxString &sErrMsg)
{
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }

    if( m_omTasks[nTaskId] ) 
    {
        m_omTasks[nTaskId]->SetState(enumGISTaskQuered);
    }

    //store in xml
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStStart, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Start task succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        SetDecimalValue(pTaskNode, wxT("id"), nTaskId);
        SetDecimalValue(pTaskNode, wxT("state"), enumGISTaskQuered);
        pRootNode->AddChild(pTaskNode);
        
        m_pTaskManager->SendNetMessage(msgout); 

        StartNextQueredTask();

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save chnges to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
}

bool wxGISTaskCategory::StopTask(int nTaskId, int nId, wxString &sErrMsg)
{
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }

    if( m_omTasks[nTaskId] ) 
    {
        m_omTasks[nTaskId]->Stop();
        m_nRunningTasks--;        
    }

    //store in xml
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStStop, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Stop task succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        SetDecimalValue(pTaskNode, wxT("id"), nTaskId);
        SetDecimalValue(pTaskNode, wxT("state"), enumGISTaskPaused);
        pRootNode->AddChild(pTaskNode);
        
        m_pTaskManager->SendNetMessage(msgout); 

        StartNextQueredTask();

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save changes to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
}

bool wxGISTaskCategory::ChangeTaskPriority(int nTaskId, long nPriority, int nId, wxString &sErrMsg)
{
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }
    if(nPriority == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task priority %d is undefined"), nPriority);
        return false;
    }

    if( m_omTasks[nTaskId] ) 
    {
        m_omTasks[nTaskId]->SetPriority(nPriority);
    }

    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStPriority, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Set task new priority succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        SetDecimalValue(pTaskNode, wxT("id"), nTaskId);
        SetDecimalValue(pTaskNode, wxT("prio"), nPriority);
        pRootNode->AddChild(pTaskNode);
        
        m_pTaskManager->SendNetMessage(msgout); 

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save changes to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }

}

bool wxGISTaskCategory::ChangeTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg)
{
    sErrMsg = wxString(_("Input pointer wxXmlNode is null"));
    wxCHECK_MSG(pTaskNode, false, wxT("Input pointer wxXmlNode is null"));
    int nTaskId = GetDecimalValue(pTaskNode, wxT("id"), wxNOT_FOUND);
    if(nTaskId == wxNOT_FOUND)
    {
        sErrMsg = wxString::Format(_("The task id %d not found"), nTaskId);
        return false;
    }
    if(m_omTasks[nTaskId])
        m_omTasks[nTaskId]->ChangeTask(pTaskNode);
   //store in xml
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStChng, enumGISPriorityHigh, nId);
        sErrMsg = wxString(_("Change task succeeded"));
        msgout.SetMessage(sErrMsg);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        if(m_omTasks[nTaskId])
        {
            wxXmlNode* pTaskNode = m_omTasks[nTaskId]->GetXmlDescription();
            pTaskNode->AddAttribute(wxT("cat"), m_sName);
            pRootNode->AddChild(pTaskNode);
        }
        
        m_pTaskManager->SendNetMessage(msgout); 

        StartNextQueredTask();

        return true;
    }
    else
    {
        sErrMsg = wxString::Format(_("Save changes to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
}

bool wxGISTaskCategory::GetTasks(int nId, wxString &sErrMsg)
{
    wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdGetChildren, enumGISPriorityHigh, nId);
    sErrMsg = wxString(_("Get tasks succeeded"));
    msgout.SetMessage(sErrMsg);
    wxXmlNode* pRootNode = msgout.GetXMLRoot();
    wxXmlNode* pTasksNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("tasks"));
    for(wxGISTaskMap::iterator it = m_omTasks.begin(); it != m_omTasks.end(); ++it)
    { 
        if(it->second)
        {
            wxXmlNode* pTaskNode = it->second->GetXmlDescription();
            pTaskNode->AddAttribute(wxT("cat"), m_sName);
            pTasksNode->AddChild(pTaskNode); 
        }
    }
    m_pTaskManager->SendNetMessage(msgout);
    return true;
}

int wxGISTaskCategory::GetNewId()
{
    if(m_pTaskManager)
        return m_pTaskManager->GetNewId();
    return wxNOT_FOUND;
}

wxString wxGISTaskCategory::GetTaskConfigPath(const wxString& sCatName)
{
    if(m_pTaskManager)
        return m_pTaskManager->GetTaskConfigPath(sCatName);
    return wxEmptyString;
}

void wxGISTaskCategory::OnFinish(wxGISProcess* pProcess, bool bHasErrors)
{
    //if(bHasErrors)
    //    pProcess->SetState(enumGISTaskQuered);
    m_nRunningTasks--;
    StartNextQueredTask();
}

void wxGISTaskCategory::StartNextQueredTask()
{
    wxCriticalSectionLocker lock(m_CritSect);

    if(m_pTaskManager->GetExecTaskCount() >= m_pTaskManager->GetMaxExecTaskCount())
        return;

    for(short i = 0; i < m_nMaxTasks; ++i)
    {
        if(m_nRunningTasks >= m_nMaxTasks)
            return;

        int nTaskId = GetQueredTaskId();
        if(nTaskId != wxNOT_FOUND)
        {
            if(m_omTasks[nTaskId]->Start())
                m_nRunningTasks++;
        }

        //the max exec task total count in categories may be grater than max exec task in manager
        if(m_pTaskManager->GetExecTaskCount() >= m_pTaskManager->GetMaxExecTaskCount())
            return;
    }
}

int wxGISTaskCategory::GetQueredTaskId()
{
    long nMinPriority = 10000000000;
    int nOutTaskId = wxNOT_FOUND;
    for(wxGISTaskMap::iterator it = m_omTasks.begin(); it != m_omTasks.end(); ++it)
    { 
        //get next task by priority
        if(it->second && it->second->GetState() == enumGISTaskQuered)
        {
            if(nMinPriority > it->second->GetPriority())
            {
                nMinPriority = it->second->GetPriority();
                nOutTaskId = it->second->GetId();
            }
        }
    }
    return nOutTaskId;
}

bool wxGISTaskCategory::SetMaxExecTasks(int nMaxTaskExec, wxString &sErrMsg)
{
    m_nMaxTasks = nMaxTaskExec;
    if( !Save() )
    {
        sErrMsg = wxString::Format(_("Save chnges to xml file '%s' failed"), m_sXmlDocPath.c_str());
        return false;
    }
    return true;
}


void wxGISTaskCategory::ChangeTask(int nTaskId)
{
    if( Save() )
    {
        wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdStChng, enumGISPriorityHigh);
        wxXmlNode* pRootNode = msgout.GetXMLRoot();
        wxXmlNode* pTaskNode = m_omTasks[nTaskId]->GetXmlDescription();
        pTaskNode->AddAttribute(wxT("cat"), m_sName);
        pRootNode->AddChild(pTaskNode);
        
        m_pTaskManager->SendNetMessage(msgout); 
    }
}

//void wxGISTaskCategory::ChangeTaskVol(long nVolume, int nTaskId)
//{
//    if( Save() )
//    {
//        wxNetMessage msgout(enumGISNetCmdNote, enumGISCmdNoteVol, enumGISPriorityNormal);
//        wxXmlNode* pRootNode = msgout.GetXMLRoot();
//
//        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
//        SetDecimalValue(pTaskNode,wxT("id"), m_omTasks[nTaskId]->GetId());
//        SetDecimalValue(pTaskNode, wxT("vol"), nVolume);
//        //pTaskNode->AddAttribute(wxT("cat"), m_sName);
//        
//        m_pTaskManager->SendNetMessage(msgout); 
//    }
//}

//void wxGISTaskCategory::ChangeTaskPercent(double dfDone, int nTaskId)
//{
//    if( Save() )
//    {
//        wxNetMessage msgout(enumGISNetCmdNote, enumGISCmdNotePercent, enumGISPriorityHigh);
//        wxXmlNode* pRootNode = msgout.GetXMLRoot();
//
//        wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
//        SetDecimalValue(pTaskNode,wxT("id"), m_omTasks[nTaskId]->GetId());
//        SetFloatValue(pTaskNode, wxT("done"), dfDone);
//        if(m_omTasks[nTaskId]->GetFinish().IsValid())
//            SetDateValue(pTaskNode, wxT("end"), m_omTasks[nTaskId]->GetFinish());
//        SetDecimalValue(pTaskNode, wxT("state"), m_omTasks[nTaskId]->GetState());
//        //pTaskNode->AddAttribute(wxT("cat"), m_sName);
//        
//        m_pTaskManager->SendNetMessage(msgout); 
//    }
//}

void wxGISTaskCategory::ChangeTaskMsg(wxGISEnumMessageType nType, const wxString &sInfoData, const wxDateTime &dt, int mMsgId, int nTaskId)
{
    if(sInfoData.IsEmpty())
        return;

    wxCriticalSectionLocker lock(m_MsgCritSect);
    wxNetMessage msgout(enumGISNetCmdNote, enumGISCmdNoteMsg, enumGISPriorityLow);
    wxXmlNode* pRootNode = msgout.GetXMLRoot();

    wxXmlNode* pTaskNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("task"));
    SetDecimalValue(pTaskNode,wxT("id"), m_omTasks[nTaskId]->GetId());
    SetDecimalValue(pTaskNode, wxT("msg_id"), mMsgId);
    SetDecimalValue(pTaskNode, wxT("msg_type"), nType);
    SetDateValue(pTaskNode, wxT("msg_dt"), dt);
    pTaskNode->AddAttribute(wxT("msg"), sInfoData);
    //pTaskNode->AddAttribute(wxT("task_cat"), m_sName);
        
    m_pTaskManager->SendNetMessage(msgout); 
}

void wxGISTaskCategory::GetTaskMessages(int nTaskId, int nId)
{    
    wxCriticalSectionLocker lock(m_MsgCritSect);
    for(size_t i = 0; i < m_omTasks[nTaskId]->GetMessageCount(); ++i)
        ChangeTaskMsg(m_omTasks[nTaskId]->GetMessage(i).m_nType, m_omTasks[nTaskId]->GetMessage(i).m_sMessage, m_omTasks[nTaskId]->GetMessage(i).m_dt, i, nTaskId);
}

/*

wxGISGPToolManager::wxGISGPToolManager(void) : m_nRunningTasks(0)
{
	m_bIsOk = false;
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return;

    m_nMaxTasks = pConfig->ReadInt(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/max_tasks")), wxThread::GetCPUCount());
#ifdef __WXMSW__
    m_sGeoprocessPath = pConfig->Read(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/gp_exec")), wxString(wxT("wxGISGeoprocess.exe")));
#else
    m_sGeoprocessPath = pConfig->Read(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/gp_exec")), wxString(wxT("wxGISGeoprocess")));
#endif
	wxXmlNode *pToolsNode = pConfig->GetConfigNode(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools")));
	if(!pToolsNode)
		return;

	wxXmlNode *pChild = pToolsNode->GetChildren();
    while (pChild)
    {
        wxString sName = pChild->GetAttribute(wxT("object"), NONAME);
        if(sName.IsEmpty() || sName.CmpNoCase(NONAME) == 0)
        {
            pChild = pChild->GetNext();
            continue;
        }

        int nCount = wxAtoi(pChild->GetAttribute(wxT("count"), wxT("0")));
        wxString sCmdName = pChild->GetAttribute(wxT("name"), NONAME);

	    wxObject *pObj = wxCreateDynamicObject(sName);
        IGPToolSPtr pRetTool(dynamic_cast<IGPTool*>(pObj));
	    if(pRetTool)
	    {
            sCmdName = pRetTool->GetName();
			TOOLINFO info = {sName, nCount, pRetTool};
			m_ToolsMap[sCmdName] = info;
			m_ToolsPopularMap.insert(std::make_pair(nCount, sCmdName));
        }
        else
            wxLogError(_("Create %s object failed!"), sName.c_str());
        pChild = pChild->GetNext();
    }
	m_bIsOk = true;
}

wxGISGPToolManager::~wxGISGPToolManager(void)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(pConfig)
	{
		pConfig->Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/max_tasks")), m_nMaxTasks);
		pConfig->Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/gp_exec")), m_sGeoprocessPath);
	}

    //read tasks
	wxXmlNode *pToolsNode = pConfig->GetConfigNode(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools")));
	if(pToolsNode)
	{
		pConfig->DeleteNodeChildren(pToolsNode);
		for(std::map<wxString, TOOLINFO>::const_iterator pos = m_ToolsMap.begin(); pos != m_ToolsMap.end(); ++pos)
		{
			if(pos->second.sClassName.IsEmpty())
				continue;
			wxXmlNode* pNewNode = new wxXmlNode(pToolsNode, wxXML_ELEMENT_NODE, wxString(wxT("tool")));
			pNewNode->AddAttribute(wxT("object"), pos->second.sClassName);
			pNewNode->AddAttribute(wxT("name"), pos->first);
			pNewNode->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), pos->second.nCount));
		}
	}

    //kill all processes
    for(size_t i = 0; i < m_ProcessArray.size(); ++i)
    {
       	m_ProcessArray[i].pProcess->Cancel();
        wxDELETE(m_ProcessArray[i].pProcess);
    }
    //delete all existed tools
    //for(std::map<wxString, TOOLINFO>::iterator pos = m_ToolsMap.begin(); pos != m_ToolsMap.end(); ++pos)
    //    wxDELETE(pos->second.pTool);

}

IGPToolSPtr wxGISGPToolManager::GetTool(wxString sToolName, IGxCatalog* pCatalog)
{
    if(sToolName.IsEmpty())
        return IGPToolSPtr();
    std::map<wxString, TOOLINFO>::const_iterator it = m_ToolsMap.find(sToolName);
    if(it == m_ToolsMap.end())
        return IGPToolSPtr();

    wxObject *pObj = wxCreateDynamicObject(it->second.sClassName);
    IGPToolSPtr pRetTool(dynamic_cast<IGPTool*>(pObj));
    if(it->second.pTool)
    {
        if(!it->second.pTool->GetCatalog())
            it->second.pTool->SetCatalog(pCatalog);

        if(pRetTool)
        {
            pRetTool->Copy(it->second.pTool.get());
            m_ToolsMap[sToolName].pTool = pRetTool;
            return pRetTool;
        }
        else
            return IGPToolSPtr();
    }

    if(!pRetTool)
    {
        TOOLINFO inf = {wxEmptyString, 0, IGPToolSPtr()};
        m_ToolsMap[sToolName] = inf;
        return IGPToolSPtr();
    }
    pRetTool->SetCatalog(pCatalog);
    m_ToolsMap[sToolName].pTool = pRetTool;
	return pRetTool;
}

int wxGISGPToolManager::Execute(IGPToolSPtr pTool, ITrackCancel* pTrackCancel)
{
    if(!pTool)
        return false;
    //get tool name and add it to stat of exec
    wxString sToolName = pTool->GetName();
    m_ToolsMap[sToolName].nCount++;

    wxString sToolParams = pTool->GetAsString();
    sToolParams.Replace(wxT("\""), wxT("\\\""));

	//wxString sCommand = wxString::Format(wxT("%s -n "), m_sGeoprocessPath.c_str()) + sToolName + wxT(" -p \"") + sToolParams + wxT("\"");

	wxFileName FName(m_sGeoprocessPath);
    wxArrayString saParams;
    saParams.Add(FName.GetName());
    saParams.Add(wxString(wxT("-n")));
    saParams.Add(sToolName);
    saParams.Add(wxString(wxT("-p")));
    saParams.Add(sToolParams);

    WXGISEXECDDATA data = {new wxGPProcess(m_sGeoprocessPath, saParams, static_cast<IProcessParent*>(this), pTrackCancel), pTool, pTrackCancel};
    m_ProcessArray.push_back(data);
    int nTaskID = m_ProcessArray.size() - 1;
    AddPriority(nTaskID, nTaskID);

	StartProcess(nTaskID);

    return nTaskID;
}

bool wxGISGPToolManager::ExecTask(WXGISEXECDDATA &data, size_t nIndex)
{
    wxDateTime begin = wxDateTime::Now();
    if(data.pTrackCancel)
    {
        data.pTrackCancel->PutMessage(wxString::Format(_("Executing (%s)"), data.pTool->GetName().c_str()), -1, enumGISMessageInfo);
        //add some tool info
        GPParameters Params = data.pTool->GetParameterInfo();
        data.pTrackCancel->PutMessage(wxString(_("Parameters:")), -1, enumGISMessageInfo);
		for(size_t i = 0; i < Params.GetCount(); ++i)
        {
            IGPParameter* pParam = Params[i];
            if(!pParam)
                continue;
            wxString sParamName = pParam->GetName();
            wxString sParamValue = pParam->GetValue();
            data.pTrackCancel->PutMessage(wxString::Format(wxT("%s = %s"), sParamName.c_str(), sParamValue.c_str()), -1, enumGISMessageNorm);
        }
        data.pTrackCancel->PutMessage(wxString::Format(_("Start Time: %s"), begin.Format().c_str()), -1, enumGISMessageInfo);
    }

    data.pProcess->Start();
    m_nRunningTasks++;

	wxGISProcessEvent event(wxPROCESS_START, nIndex);
	PostEvent(event);

    return true;
}

void wxGISGPToolManager::OnFinish(IProcess* pProcess, bool bHasErrors)
{
    size_t nIndex;
    for(nIndex = 0; nIndex < m_ProcessArray.size(); nIndex++)
        if(pProcess == m_ProcessArray[nIndex].pProcess)
            break;

    wxDateTime end = wxDateTime::Now();
    if(m_ProcessArray[nIndex].pTrackCancel)
    {
        if(bHasErrors)
        {
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(wxString::Format(_("An error occured while executing %s. Failed to execute (%s)."), m_ProcessArray[nIndex].pTool->GetName().c_str(), m_ProcessArray[nIndex].pTool->GetName().c_str()), -1, enumGISMessageErr);
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(_("Error!"), -1, enumGISMessageTitle);
        }
        else
        {
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(wxString::Format(_("Executed (%s) successfully"), m_ProcessArray[nIndex].pTool->GetName().c_str()), -1, enumGISMessageInfo);
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(_("Done"), -1, enumGISMessageTitle);
        }

        wxTimeSpan span = end - m_ProcessArray[nIndex].pProcess->GetBeginTime();
        m_ProcessArray[nIndex].pTrackCancel->PutMessage(wxString::Format(_("End Time: %s (Elapsed Time: %s)"), end.Format().c_str(), span.Format(_("%H hours %M min. %S sec.")).c_str()), -1, enumGISMessageInfo);
    }

	wxGISProcessEvent event(wxPROCESS_FINISH, nIndex, bHasErrors);
	PostEvent(event);

	RunNextTask();
}

void wxGISGPToolManager::RunNextTask(void)
{
    //remove from array
    //m_ProcessArray.erase(m_ProcessArray.begin() + nIndex);
    //reduce counter of runnig threads
    m_nRunningTasks--;
	if(m_nRunningTasks < 0)
		m_nRunningTasks = 0;

    //run not running tasks
    if(m_nRunningTasks < m_nMaxTasks)
    {
		size_t nPrioIndex = GetPriorityTaskIndex();
		if(nPrioIndex >= 0 && nPrioIndex < m_ProcessArray.size())
			ExecTask(m_ProcessArray[nPrioIndex], nPrioIndex);
    }
}


size_t wxGISGPToolManager::GetToolCount()
{
    return m_ToolsMap.size();
}

wxString wxGISGPToolManager::GetPopularTool(size_t nIndex)
{
    std::multimap<int, wxString>::iterator i = m_ToolsPopularMap.begin();
    std::advance(i, nIndex);
    if(i != m_ToolsPopularMap.end())
        return i->second;
    return wxEmptyString;
}

//WXGISEXECDDATA wxGISGPToolManager::GetTask(size_t nIndex)
//{
//    return m_ProcessArray[nIndex];
//}

wxGISEnumTaskStateType wxGISGPToolManager::GetProcessState(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pProcess->GetState();
}

void wxGISGPToolManager::StartProcess(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
    if(m_nRunningTasks < m_nMaxTasks)
        ExecTask(m_ProcessArray[nIndex], nIndex);
	else
	{
		m_ProcessArray[nIndex].pProcess->SetState(enumGISTaskQuered);
		wxGISProcessEvent event(wxPROCESS_STATE_CHANGED, nIndex);
		PostEvent(event);
	}
}

void wxGISGPToolManager::CancelProcess(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	m_ProcessArray[nIndex].pProcess->Cancel();

	wxGISProcessEvent event(wxPROCESS_CANCELED, nIndex);
	PostEvent(event);

	RunNextTask();
}

wxDateTime wxGISGPToolManager::GetProcessStart(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pProcess->GetBeginTime();
}

wxDateTime wxGISGPToolManager::GetProcessFinish(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pProcess->GetEndTime();
}

int wxGISGPToolManager::GetProcessPriority(size_t nIndex)
{
    for(size_t i = 0; i < m_aPriorityArray.size(); ++i)
        if(m_aPriorityArray[i].nIndex == nIndex)
            return m_aPriorityArray[i].nPriority;
	return m_ProcessArray.size();
}

void wxGISGPToolManager::SetProcessPriority(size_t nIndex, int nPriority)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());

    for(size_t i = 0; i < m_aPriorityArray.size(); ++i)
    {
        if(m_aPriorityArray[i].nIndex == nIndex)
        {
            m_aPriorityArray.erase(m_aPriorityArray.begin() + i);
            return AddPriority(nIndex, nPriority);
        }
    }
}

int wxGISGPToolManager::GetPriorityTaskIndex()
{
	int nPriorityIndex = m_ProcessArray.size();
    for(size_t nIndex = 0; nIndex < m_aPriorityArray.size(); nIndex++)
    {
        if(m_ProcessArray[m_aPriorityArray[nIndex].nIndex].pProcess && m_ProcessArray[m_aPriorityArray[nIndex].nIndex].pProcess->GetState() == enumGISTaskQuered)
        {
            nPriorityIndex = m_aPriorityArray[nIndex].nIndex;
            break;
        }
    }
	return nPriorityIndex;
}

void wxGISGPToolManager::AddPriority(int nIndex, int nPriority)
{
    TASKPRIOINFO info = {nIndex, nPriority};
    bool bIncrease = false;
	for(size_t j = 0; j < m_aPriorityArray.size(); j++)
	{
		if(!bIncrease && m_aPriorityArray[j].nPriority >= nPriority)
        {
            m_aPriorityArray.insert(m_aPriorityArray.begin() + j, info);
            bIncrease = true;
            continue;
        }
        if(bIncrease)
        {
            if(m_aPriorityArray[j].nPriority - m_aPriorityArray[j - 1].nPriority < 1)
                m_aPriorityArray[j].nPriority++;
            else
                return;;
        }
	}
    if(!bIncrease)
        m_aPriorityArray.push_back(info);
}

IGPToolSPtr wxGISGPToolManager::GetProcessTool(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pTool;
}
*/
