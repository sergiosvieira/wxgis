/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  TCP network server class.
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
#include "wxgis/networking/tcpnetwork.h"
#include "wxgis/networking/message.h"

// ----------------------------------------------------------------------------
// wxNetTCPReader
// ----------------------------------------------------------------------------
wxNetTCPReader::wxNetTCPReader(INetConnection* pNetConnection, wxSocketBase* pSock) : wxThread()
{
    m_pNetConnection = pNetConnection;
    m_pSock = pSock;
}

void *wxNetTCPReader::Entry()
{
	if(!m_pSock || !m_pNetConnection)
		return (ExitCode)-1;

    unsigned char buff[BUFF] = {0};
	while(!TestDestroy())
	{
		//WaitForRead
		if(m_pSock->IsConnected())
		{
			m_pSock->ReadMsg(&buff, BUFF); 
			if(m_pSock->Error())
			{
				wxThread::Sleep(50);
				//continue;
			}
			size_t nSize = m_pSock->LastCount();
			if(!m_pSock->Error() && nSize > 0)
			{
				wxNetMessage* pMsg = new wxNetMessage(buff, nSize);
				if(pMsg->IsOk())
				{
					//add message to queure
					WXGISMSG msg = {pMsg, m_pNetConnection->GetUserID()};
					m_pNetConnection->PutInMessage(msg);
				}
				else
					wxDELETE(pMsg);
			}
		}
	}
	return NULL;
}

void wxNetTCPReader::OnExit()
{
}

// ----------------------------------------------------------------------------
// wxNetTCPWriter
// ----------------------------------------------------------------------------
wxNetTCPWriter::wxNetTCPWriter(INetConnection* pNetConnection, wxSocketBase* pSock) : wxThread()
{
    m_pNetConnection = pNetConnection;
    m_pSock = pSock;
}

void *wxNetTCPWriter::Entry()
{
	if(!m_pSock || !m_pNetConnection)
		return (ExitCode)-1;

	while(!TestDestroy())
	{
        //WaitForWrite
        WXGISMSG msg = m_pNetConnection->GetOutMessage();
        if(msg.pMsg)
        {
			if(m_pSock->IsConnected())
			{
				m_pSock->WriteMsg( msg.pMsg->GetData(), msg.pMsg->GetDataLen() ); 
            
				if( m_pSock->LastCount() != msg.pMsg->GetDataLen() || m_pSock->Error())
				{
					m_pNetConnection->PutOutMessage(msg);
					wxThread::Sleep(50);
				}
				else
				{
					wxDELETE(msg.pMsg);
				}
			}
        }
        else
            wxThread::Sleep(100);
	}
	return NULL;
}

void wxNetTCPWriter::OnExit()
{
}

// ----------------------------------------------------------------------------
// wxNetTCPWaitlost
// ----------------------------------------------------------------------------
wxNetTCPWaitlost::wxNetTCPWaitlost(INetConnection* pNetConnection, wxSocketBase* pSock)
{
    m_pNetConnection = pNetConnection;
    m_pSock = pSock;
}

void *wxNetTCPWaitlost::Entry()
{
	if(m_pSock == NULL)
		return (ExitCode)-1;

    bool bLostConn = false;
	while(!TestDestroy())
	{
		if(m_pSock->IsDisconnected())
		{
            bLostConn = true;
	        break;
		}
  //      Yield();
		wxThread::Sleep(1000);
		//if(!m_pNetConnection->IsConnected())
		//	break;
//		if(!m_pSock->IsData())
//			int x = 0;
//		if(m_pSock->Error())
//		{
//			wxSocketError err = m_pSock->LastError();
//			if(err != wxSOCKET_TIMEDOUT)
//				int x = 0;
////			if(wxSOCKET_WOULDBLOCK == err)
////			{
////	            bLostConn = true;
////		        break;
////			}
////			else
////			{
////				wxThread::Sleep(10);
////				continue;
////			}
//		}

////        wxYieldIfNeeded();
//		if(m_pSock->WaitForLost(1, 250))
//		{
//            bLostConn = true;
//	        break;
//		}
//		if(m_pSock->Error())
//		{
//			wxSocketError err = m_pSock->LastError();
//			if(err != wxSOCKET_TIMEDOUT)
//				int x = 0;
//			if(wxSOCKET_WOULDBLOCK == err)
//			{
//	            bLostConn = true;
//		        break;
//			}
//////			else
//////			{
//////				wxThread::Sleep(10);
//////				continue;
//////			}
//		}
//		Yield();
	}

    if(bLostConn && m_pNetConnection)
        m_pNetConnection->Disconnect();

    return NULL;
}

void wxNetTCPWaitlost::OnExit()
{
}

