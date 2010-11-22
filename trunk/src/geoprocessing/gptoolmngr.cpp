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

#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/core/config.h"

#include "wx/datetime.h"

/////////////////////////////////////////////////////////////////////////////////
///// Class wxGISGPTaskThread
/////////////////////////////////////////////////////////////////////////////////
//
//wxGISGPTaskThread::wxGISGPTaskThread(wxGISGPToolManager* pMngr, IGPTool* pTool, ITrackCancel* pTrackCancel) : wxThread(wxTHREAD_JOINABLE)
//{
//    m_pTrackCancel = pTrackCancel;
//    m_pMngr = pMngr;
//    m_pTool = pTool;
//}
//
//void *wxGISGPTaskThread::Entry()
//{
//    //add start msg
//    wxDateTime begin = wxDateTime::Now();
//    if(m_pTrackCancel)
//    {
//        m_pTrackCancel->PutMessage(wxString::Format(_("Executing (%s)"), m_pTool->GetName().c_str()), -1, enumGISMessageInfo);
//        //add some tool info
//        GPParameters* pParams = m_pTool->GetParameterInfo();
//        if(pParams)
//        {
//            m_pTrackCancel->PutMessage(wxString(_("Parameters:")), -1, enumGISMessageInfo);
//            for(size_t i = 0; i < pParams->size(); i++)
//            {
//                IGPParameter* pParam = pParams->operator[](i);
//                if(!pParam)
//                    continue;
//                wxString sParamName = pParam->GetName();
//                wxString sParamValue = pParam->GetValue();
//                m_pTrackCancel->PutMessage(wxString::Format(wxT("%s = %s"), sParamName.c_str(), sParamValue.c_str()), -1, enumGISMessageNorm);
//            }
//        }
//        m_pTrackCancel->PutMessage(wxString::Format(_("Start Time: %s"), begin.Format().c_str()), -1, enumGISMessageInfo);
//    }
//
//    bool bResult = m_pTool->Execute(m_pTrackCancel);
//
//    //add finish message
//    wxDateTime end = wxDateTime::Now();
//    if(m_pTrackCancel)
//    {
//        if(bResult)
//            m_pTrackCancel->PutMessage(wxString::Format(_("Executed (%s) successfully"), m_pTool->GetName().c_str()), -1, enumGISMessageInfo);
//        else
//        {
//            m_pTrackCancel->PutMessage(wxString::Format(_("An error occured while executing %s. Failed to execute (%s)."), m_pTool->GetName().c_str(), m_pTool->GetName().c_str()), -1, enumGISMessageErr);
//        }
//        wxTimeSpan span = end - begin;
//        m_pTrackCancel->PutMessage(wxString::Format(_("End Time: %s (Elapsed Time: %s)"), end.Format().c_str(), span.Format(_("%H hours %M min. %S sec.")).c_str()), -1, enumGISMessageInfo);
//
//        wxLogMessage(_("wxGISGPToolManager: The tool %s execution took %s (%s)"),m_pTool->GetName().c_str(), span.Format(_("%H hours %M min. %S sec.")).c_str(), bResult == true ? _("success") : _("error"));
//
//        if(bResult)
//            m_pTrackCancel->PutMessage(_("Done"), -1, enumGISMessageTitle);
//        else
//            m_pTrackCancel->PutMessage(_("Error!"), -1, enumGISMessageTitle);
//    }
//#ifdef __WXGTK__
//    wxMutexGuiEnter();
//#endif
//
//    m_pMngr->OnFinish(this->GetId(), !bResult, m_pTool);
//
//#ifdef __WXGTK__
//    wxMutexGuiLeave();
//#endif
//    return NULL;
//}
//
//void wxGISGPTaskThread::OnExit()
//{
//}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGPProcess
///////////////////////////////////////////////////////////////////////////////

wxGPProcess::wxGPProcess(wxGISGPToolManager* pToolManager, ITrackCancel* pTrackCancel) : wxProcess(wxPROCESS_REDIRECT)
{
    m_pToolManager = pToolManager;
    m_pTrackCancel = pTrackCancel;
}

wxGPProcess::~wxGPProcess(void)
{
}

void wxGPProcess::OnStartExecution(void)
{
    //start read and write threads
}

void wxGPProcess::OnTerminate(int pid, int status)
{
    if(m_pToolManager)
        m_pToolManager->OnFinish(this, status != 0);
}

void wxGPProcess::OnCancelExecution(void)
{
    //send cancel code
    //and detach
    Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPToolManager
///////////////////////////////////////////////////////////////////////////////

wxGISGPToolManager::wxGISGPToolManager(wxXmlNode* pToolsNode) : m_nMaxTasks(50), m_nRunningTasks(0)
{
    m_pToolsNode = pToolsNode;
    m_nMaxTasks = wxAtoi(m_pToolsNode->GetPropVal(wxT("max_tasks"), wxString::Format(wxT("%d"), wxThread::GetCPUCount())));
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
        TOOLINFO info = {sName, nCount, NULL};
        m_ToolsMap[sCmdName] = info;
        m_ToolsPopularMap.insert(std::make_pair(nCount, sCmdName));

        pChild = pChild->GetNext();
    }
}

wxGISGPToolManager::~wxGISGPToolManager(void)
{
    if(m_pToolsNode->HasProp(wxT("max_tasks")))
        m_pToolsNode->DeleteProperty(wxT("max_tasks"));
    m_pToolsNode->AddProperty(wxT("max_tasks"), wxString::Format(wxT("%d"), m_nMaxTasks));

    //read tasks
    wxGISConfig::DeleteNodeChildren(m_pToolsNode);
    for(std::map<wxString, TOOLINFO>::const_iterator pos = m_ToolsMap.begin(); pos != m_ToolsMap.end(); ++pos)
    {
        wxXmlNode* pNewNode = new wxXmlNode(m_pToolsNode, wxXML_ELEMENT_NODE, wxString(wxT("tool")));
		pNewNode->AddProperty(wxT("object"), pos->second.sClassName);
		pNewNode->AddProperty(wxT("name"), pos->first);
		pNewNode->AddProperty(wxT("count"), wxString::Format(wxT("%d"), pos->second.nCount));
    }

    //kill all processes
    for(size_t i = 0; i < m_ProcessArray.size(); i++)
        m_ProcessArray[i].pGPProcess->OnCancelExecution();

    //delete all existed tools
    for(std::map<wxString, TOOLINFO>::iterator pos = m_ToolsMap.begin(); pos != m_ToolsMap.end(); ++pos)
        wxDELETE(pos->second.pTool);

}

IGPTool* wxGISGPToolManager::GetTool(wxString sToolName, IGxCatalog* pCatalog)
{
    if(m_ToolsMap[sToolName].pTool)
	    return m_ToolsMap[sToolName].pTool;
	wxObject *pObj = wxCreateDynamicObject(m_ToolsMap[sToolName].sClassName);
    IGPTool* pTool = dynamic_cast<IGPTool*>(pObj);
    if(!pTool)
        return NULL;
    pTool->SetCatalog(pCatalog);
    m_ToolsMap[sToolName].pTool = pTool;
	return pTool;
}

bool wxGISGPToolManager::OnExecute(IGPTool* pTool, ITrackCancel* pTrackCancel, IGPCallBack* pCallBack)
{
    if(!pTool)
        return false;
    //get tool name and add it to stat of exec
    wxString sToolName = pTool->GetName();
    m_ToolsMap[sToolName].nCount++;

    wxString sToolParams = pTool->GetAsString();
    wxString sCommand = wxT("wxGISGeoprocess.exe -n ") + sToolName + wxT(" -p ") + sToolParams;

    EXECDDATA data = {new wxGPProcess(this, pTrackCancel), pTool, sCommand, enumGISTaskQuered, pTrackCancel, pCallBack};
    m_ProcessArray.push_back(data);

    if(m_nRunningTasks < m_nMaxTasks)
        return ExecTask(data);

    return true;
}

bool wxGISGPToolManager::ExecTask(EXECDDATA data)
{
    long nRes = wxExecute(data.sCommand, wxEXEC_ASYNC, data.pGPProcess);
    if(nRes != 0)
    {
        data.nState = enumGISTaskError;
        if(data.pTrackCancel)
            data.pTrackCancel->PutMessage(_("Error start task!"), -1, enumGISMessageErr);
        if(data.pCallBack)
            data.pCallBack->OnFinish(true, data.pTool);
        return false;
    }

    data.nState = enumGISTaskWork;
    data.pGPProcess->OnStartExecution();
    m_nRunningTasks++;
    return true;
}

void wxGISGPToolManager::OnFinish(wxGPProcess* pGPProcess, bool bHasErrors)
{
    size_t nIndex;
    for(nIndex = 0; nIndex < m_ProcessArray.size(); nIndex++)
        if(pGPProcess == m_ProcessArray[nIndex].pGPProcess)
            break;
    if(m_ProcessArray[nIndex].pCallBack)
        m_ProcessArray[nIndex].pCallBack->OnFinish(bHasErrors, m_ProcessArray[nIndex].pTool);

    //remove from array
    m_ProcessArray.erase(m_ProcessArray.begin() + nIndex);
    //reduce counter of runnig threads
    m_nRunningTasks--;

    //run not running tasks
    if(m_nRunningTasks < m_nMaxTasks)
    {
        for(nIndex = 0; nIndex < m_ProcessArray.size(); nIndex++)
        {
            if(m_ProcessArray[nIndex].nState == enumGISTaskQuered)
            {
                ExecTask(m_ProcessArray[nIndex]);
                break;
            }
        }
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
    return i->second;
}
