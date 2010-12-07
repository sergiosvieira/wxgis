/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  classes to pick and process messages from network
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010  Bishop
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
#include "wxgis/networking/processor.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxGISNetMessageProcessor
/////////////////////////////////////////////////////////////////////////////////////////////////////
wxGISNetMessageProcessor::wxGISNetMessageProcessor()
{
}

wxGISNetMessageProcessor::~wxGISNetMessageProcessor()
{
}

WXGISMSG wxGISNetMessageProcessor::GetInMessage(void)
{
    wxCriticalSectionLocker locker(m_CriticalSection);
	WXGISMSG Msg = {NULL, wxNOT_FOUND};
	if(m_MsgQueue.size() > 0)
	{
		Msg = m_MsgQueue.top();
		m_MsgQueue.pop();  
	}
	return Msg;
}

void wxGISNetMessageProcessor::ProcessMessage(WXGISMSG msg)
{
    const wxXmlNode* pRoot = msg.pMsg->GetRoot();
    if(!pRoot)
        wxDELETE(msg.pMsg);
    wxXmlNode* pChild = pRoot->GetChildren();
    if(!pChild)
    {
        wxString sMessage = pRoot->GetNodeContent();
        //log contents and exit
        wxLogMessage(_("wxGISNetMessageProcessor: %s"), sMessage.c_str());
    }
	else
	{
		while(pChild)
		{
			wxString sReceiverName = pChild->GetName();
			std::map<wxString, INetMessageReceiver*>::const_iterator it = m_MessageReceiverMap.find(sReceiverName);
			if(it != m_MessageReceiverMap.end())
				if(it->second)
					it->second->ProcessMessage(msg, pChild);
			pChild = pChild->GetNext();
		}
	}

    wxDELETE(msg.pMsg);
}

void wxGISNetMessageProcessor::AddMessageReceiver(wxString sName, INetMessageReceiver* pNetMessageReceiver)
{
	if(pNetMessageReceiver)//the receiver can register several names
		m_MessageReceiverMap[sName] = pNetMessageReceiver;
}

void wxGISNetMessageProcessor::ClearMessageReceiver()
{
	m_MessageReceiverMap.clear();
}


void wxGISNetMessageProcessor::ClearMessageQueue(void)
{
    //wxCriticalSectionLocker locker(m_CriticalSection);
    while( m_MsgQueue.size() > 0 )
    {
	    WXGISMSG Msg = m_MsgQueue.top();
	    m_MsgQueue.pop();  
        wxDELETE(Msg.pMsg);
    }
}

bool wxGISNetMessageProcessor::OnStartMessageThread(void)
{
    //1. Start in thread
	m_pMsgInThread = new wxMsgInThread(this);
	if(!CreateAndRunThread(m_pMsgInThread, wxT("wxGISNetMessageProcessor"), wxT("Messages")))
		return false;
	else
		wxLogMessage(wxString::Format(_("wxGISNetMessageProcessor: %s queue thread 0x%lx started"), wxT("Messages"), m_pMsgInThread->GetId()));
	return true;
}

void wxGISNetMessageProcessor::OnStopMessageThread(void)
{
	//clear messages 
	ClearMessageQueue();

    // Stop in thread
    if(m_pMsgInThread)
        m_pMsgInThread->Delete();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxMsgInThread
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxMsgInThread::wxMsgInThread(wxGISNetMessageProcessor* pParent) : wxThread(), m_pParent(NULL)
{
	m_pParent = pParent;
}

void wxMsgInThread::OnExit()
{
	m_pParent = NULL;
}

void *wxMsgInThread::Entry()
{
	if(m_pParent == NULL)
		return (ExitCode)-1;

	while(!TestDestroy())
	{
		WXGISMSG msg = m_pParent->GetInMessage();
		while(msg.pMsg)    
		{      
			m_pParent->ProcessMessage(msg);
			msg = m_pParent->GetInMessage();
		}
        //wxYieldIfNeeded();
        //Yield();
		wxThread::Sleep(150);
	}
    return NULL;
}
