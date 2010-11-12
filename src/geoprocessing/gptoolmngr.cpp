/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  tools manager.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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

#include "wxgis/geoprocessing/gptoolmngr.h"

#include "wx/datetime.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPTaskThread
///////////////////////////////////////////////////////////////////////////////

wxGISGPTaskThread::wxGISGPTaskThread(wxGISGPToolManager* pMngr, IGPTool* pTool, ITrackCancel* pTrackCancel) : wxThread(wxTHREAD_JOINABLE)
{
    m_pTrackCancel = pTrackCancel;
    m_pMngr = pMngr;
    m_pTool = pTool;
}

void *wxGISGPTaskThread::Entry()
{
    //add start msg
    wxDateTime begin = wxDateTime::Now();
    if(m_pTrackCancel)
    {
        m_pTrackCancel->PutMessage(wxString::Format(_("Executing (%s)"), m_pTool->GetName().c_str()), -1, enumGISMessageInfo);
        //add some tool info
        GPParameters* pParams = m_pTool->GetParameterInfo();
        if(pParams)
        {
            m_pTrackCancel->PutMessage(wxString(_("Parameters:")), -1, enumGISMessageInfo);
            for(size_t i = 0; i < pParams->size(); i++)
            {
                IGPParameter* pParam = pParams->operator[](i);
                if(!pParam)
                    continue;
                wxString sParamName = pParam->GetName();
                wxString sParamValue = pParam->GetValue();
                m_pTrackCancel->PutMessage(wxString::Format(wxT("%s = %s"), sParamName.c_str(), sParamValue.c_str()), -1, enumGISMessageNorm);
            }
        }
        m_pTrackCancel->PutMessage(wxString::Format(_("Start Time: %s"), begin.Format().c_str()), -1, enumGISMessageInfo);
    }

    bool bResult = m_pTool->Execute(m_pTrackCancel);

    //add finish message
    wxDateTime end = wxDateTime::Now();
    if(m_pTrackCancel)
    {
        if(bResult)
            m_pTrackCancel->PutMessage(wxString::Format(_("Executed (%s) successfully"), m_pTool->GetName().c_str()), -1, enumGISMessageInfo);
        else
        {
            m_pTrackCancel->PutMessage(wxString::Format(_("An error occured while executing %s. Failed to execute (%s)."), m_pTool->GetName().c_str(), m_pTool->GetName().c_str()), -1, enumGISMessageErr);
        }
        wxTimeSpan span = end - begin;
        m_pTrackCancel->PutMessage(wxString::Format(_("End Time: %s (Elapsed Time: %s)"), end.Format().c_str(), span.Format(_("%H hours %M min. %S sec.")).c_str()), -1, enumGISMessageInfo);

        wxLogMessage(_("wxGISGPToolManager: The tool %s execution took %s (%s)"),m_pTool->GetName().c_str(), span.Format(_("%H hours %M min. %S sec.")).c_str(), bResult == true ? _("success") : _("error"));

        if(bResult)
            m_pTrackCancel->PutMessage(_("Done"), -1, enumGISMessageTitle);
        else
            m_pTrackCancel->PutMessage(_("Error!"), -1, enumGISMessageTitle);
    }
#ifdef __WXGTK__
    wxMutexGuiEnter();
#endif

    m_pMngr->OnFinish(this->GetId(), !bResult, m_pTool);

#ifdef __WXGTK__
    wxMutexGuiLeave();
#endif
    return NULL;
}

void wxGISGPTaskThread::OnExit()
{
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPToolManager
///////////////////////////////////////////////////////////////////////////////

wxGISGPToolManager::wxGISGPToolManager(wxXmlNode* pToolsNode, IGxCatalog* pCatalog) : m_pCatalog(NULL), m_nMaxThreads(50), m_nRunningThreads(0)
{
    m_pToolsNode = pToolsNode;
    m_nMaxThreads = wxAtoi(m_pToolsNode->GetPropVal(wxT("max_tasks"), wxString::Format(wxT("%d"), wxThread::GetCPUCount())));
    m_pCatalog = pCatalog;
    wxXmlNode *pChild = m_pToolsNode->GetChildren();
    while (pChild)
    {
        wxString sName = pChild->GetPropVal(wxT("object"), NONAME);
        if(sName.IsEmpty() || sName.CmpNoCase(NONAME) == 0)
        {
            pChild = pChild->GetNext();
            continue;
        }

        int nCount = wxAtoi(pChild->GetPropVal(wxT("count"), wxT("0")));
        wxString sCmdName = pChild->GetPropVal(wxT("name"), NONAME);
        if(sName.IsEmpty() || sName.CmpNoCase(NONAME) == 0)
        {
		    wxObject *pObj = wxCreateDynamicObject(sName);
		    IGPTool *pTool = dynamic_cast<IGPTool*>(pObj);
		    if(pTool != NULL)
		    {
                sCmdName = pTool->GetName();
                wxDELETE(pTool);
            }
            else 
                continue;
        }
        TOOLINFO info = {sName, nCount};
        m_ToolsMap[sCmdName] = info;

        pChild = pChild->GetNext();
    }
}

wxGISGPToolManager::~wxGISGPToolManager(void)
{
    if(m_pToolsNode->HasProp(wxT("max_tasks")))
        m_pToolsNode->DeleteProperty(wxT("max_tasks"));
    m_pToolsNode->AddProperty(wxT("max_tasks"), wxString::Format(wxT("%d"), m_nMaxThreads));

    //readd tasks
    wxGISConfig::DeleteNodeChildren(m_pToolsNode)
    typedef std::map<wxString, TOOLINFO>::const_iterator IT;
    for(IT pos = m_ToolsMap.begin(); pos != m_ToolsMap.end(); ++pos)
    {
        wxXmlNode* pNewNode = new wxXmlNode(m_pToolsNode, wxXML_ELEMENT_NODE, wxString(wxT("tool")));
		pNewNode->AddProperty(wxT("object"), pos->second.sClassName);
		pNewNode->AddProperty(wxT("name"), pos->first);
		pNewNode->AddProperty(wxT("count"), pos->second.nCount);
    }

    ////kill all active threads
    //typedef std::map<unsigned long, THREADDATA>::const_iterator IT;
    //for(IT pos = m_TasksThreadMap.begin(); pos != m_TasksThreadMap.end(); ++pos)
    //{
    //    //if(pos->second.pThread->IsRunning())
    //        pos->second.pThread->Kill();
    //}

}

IGPTool* wxGISGPToolManager::GetTool(wxString sToolName)
{
	wxObject *pObj = wxCreateDynamicObject(m_ToolsMap[sToolName].ClassName);
    IGPTool* pTool = dynamic_cast<IGPTool*>(pObj);
    if(!pTool)
        return NULL;
    pTool->SetCatalog(m_pCatalog);
	return pTool;
}

long wxGISGPToolManager::OnExecute(IGPTool* pTool, ITrackCancel* pTrackCancel, IGPCallBack* pCallBack)
{
    if(!pTool)
        return -1;
    //get tool name and add it to stat of exec
    wxString sToolName = pTool->GetName();
    m_ToolsMap[sToolName].nCount++;

    //create execute thread
    wxGISGPTaskThread *pThread = new wxGISGPTaskThread(this, pTool, pTrackCancel);
    if(pThread->Create() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("wxGISGPToolManager: Cannot create task (%s) thread"), pTool->GetName().c_str());
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Cannot create task (%s) thread"), pTool->GetDisplayName().c_str()), -1, enumGISMessageErr);
        if(pCallBack)
            pCallBack->OnFinish(true, pTool);
        return -1;
    }
    if(m_nRunningThreads < m_nMaxThreads)
    {
        if(pThread->Run() != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("wxGISGPToolManager: Cannot run task (%s) thread"), pTool->GetName());
            if(pTrackCancel)
                pTrackCancel->PutMessage(wxString::Format(_("Cannot run task (%s) thread"), pTool->GetDisplayName().c_str()), -1, enumGISMessageErr);
            if(pCallBack)
                pCallBack->OnFinish(true, pTool);
            return -1;
        }
        m_nRunningThreads++;
    }
    //thread array for task list
    THREADDATA thdata = {pThread, pCallBack, pTrackCancel};
    m_TasksThreadMap[pThread->GetId()] = thdata;
    return pThread->GetId();
}

void wxGISGPToolManager::OnFinish(unsigned long nThreadId, bool bHasErrors, IGPTool* pTool)
{
    IGPCallBack* pCallBack = m_TasksThreadMap[nThreadId].pCallBack;
    if(pCallBack)
        pCallBack->OnFinish(bHasErrors, pTool);
    else
        wxDELETE(pTool);//TODO: think about tool deletion

    //remove from array
    m_TasksThreadMap.erase(nThreadId);
    //reduce counter of runnig threads
    m_nRunningThreads--;

    //run not running tasks
    if(m_nRunningThreads < m_nMaxThreads)
    {
        typedef std::map<unsigned long, THREADDATA>::const_iterator IT;
        for(IT pos = m_TasksThreadMap.begin(); pos != m_TasksThreadMap.end(); ++pos)
        {
            if(pos->second.pThread->IsRunning())
                continue;
            if(pos->second.pThread->Run() != wxTHREAD_NO_ERROR)
            {
                wxLogError(_("wxGISGPToolManager: Cannot run task (%s) thread"), pTool->GetName().c_str());
                if(m_TasksThreadMap[nThreadId].pTrackCancel)
                    m_TasksThreadMap[nThreadId].pTrackCancel->PutMessage(wxString::Format(_("Cannot run task (%s) thread"), pTool->GetDisplayName().c_str()), -1, enumGISMessageErr);
                if(pos->second.pCallBack)
                    pos->second.pCallBack->OnFinish(true, pTool);
                m_TasksThreadMap.erase(pos->first);
                --pos;
            }

            m_nRunningThreads++;
            if(m_nRunningThreads >= m_nMaxThreads)
                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPParameter
///////////////////////////////////////////////////////////////////////////////
wxGISGPParameter::wxGISGPParameter()
{
    m_bAltered = false;
    m_bHasBeenValidated = false;
    m_bIsValid = false;
    m_pDomain = NULL;
    m_nMsgType = wxGISEnumGPMessageUnknown;
    m_sMessage = wxEmptyString;
}

wxGISGPParameter::~wxGISGPParameter()
{
    wxDELETE(m_pDomain);
}

bool wxGISGPParameter::GetAltered(void)
{
    return m_bAltered;
}

void wxGISGPParameter::SetAltered(bool bAltered)
{
    m_bAltered = bAltered;
}

bool wxGISGPParameter::GetHasBeenValidated(void)
{
    return m_bHasBeenValidated;
}

void wxGISGPParameter::SetHasBeenValidated(bool bHasBeenValidated)
{
    m_bHasBeenValidated = bHasBeenValidated;
}

bool wxGISGPParameter::GetIsValid(void)
{
    return m_bIsValid;
}

void wxGISGPParameter::SetIsValid(bool bIsValid)
{
    m_bIsValid = bIsValid;
}

wxString wxGISGPParameter::GetName(void)
{
    return m_sName;
}

void wxGISGPParameter::SetName(wxString sName)
{
    m_sName = sName;
}

wxString wxGISGPParameter::GetDisplayName(void)
{
    return m_sDisplayName;
}

void wxGISGPParameter::SetDisplayName(wxString sDisplayName)
{
    m_sDisplayName = sDisplayName;
}

wxGISEnumGPParameterDataType wxGISGPParameter::GetDataType(void)
{
    return m_DataType;
}

void wxGISGPParameter::SetDataType(wxGISEnumGPParameterDataType nType)
{
    m_DataType = nType;
}

wxGISEnumGPParameterDirection wxGISGPParameter::GetDirection(void)
{
    return m_Direction;
}

void wxGISGPParameter::SetDirection(wxGISEnumGPParameterDirection nDirection)
{
    m_Direction = nDirection;
}

wxArrayString* wxGISGPParameter::GetParameterDependencies(void)
{
    return NULL;
}

void wxGISGPParameter::AddParameterDependency(wxString sDependency)
{
}

wxGISEnumGPParameterType wxGISGPParameter::GetParameterType(void)
{
    return m_ParameterType;
}

void wxGISGPParameter::SetParameterType(wxGISEnumGPParameterType nType)
{
    m_ParameterType = nType;
}

wxVariant wxGISGPParameter::GetValue(void)
{
    return m_Value;
}

void wxGISGPParameter::SetValue(wxVariant Val)
{
    m_bHasBeenValidated = false;
    m_Value = Val;
}

IGPDomain* wxGISGPParameter::GetDomain(void)
{
    return m_pDomain;
}

void wxGISGPParameter::SetDomain(IGPDomain* pDomain)
{
    m_pDomain = pDomain;
}

wxString wxGISGPParameter::GetMessage(void)
{
    return m_sMessage;
}

wxGISEnumGPMessageType wxGISGPParameter::GetMessageType(void)
{
    return m_nMsgType;
}

void wxGISGPParameter::SetMessage(wxGISEnumGPMessageType nType, wxString sMsg)
{
    m_sMessage = sMsg;
    m_nMsgType = nType;
}
