/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  network classes.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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
#include "wxgis/net/network.h"
#include "wxgis/net/netevent.h"

// ----------------------------------------------------------------------------
// wxNetTCPReader
// ----------------------------------------------------------------------------
wxNetReaderThread::wxNetReaderThread(INetConnection* pNetConnection) : wxThread(wxTHREAD_DETACHED)
{
    m_pNetConnection = pNetConnection;
}

void *wxNetReaderThread::Entry()
{
	if(!m_pNetConnection)
		return (ExitCode)-1;

    unsigned char buff[BUFF] = {0};
	while(!TestDestroy())
	{
		//WaitForRead
        m_pNetConnection->ProcessInputNetMessage(buff);
		wxThread::Sleep(SLEEP);
	}
	return (wxThread::ExitCode)0;
}

void wxNetReaderThread::OnExit()
{
}

// ----------------------------------------------------------------------------
// wxNetTCPWriter
// ----------------------------------------------------------------------------
wxNetWriterThread::wxNetWriterThread(INetConnection* pNetConnection) : wxThread(wxTHREAD_DETACHED)
{
    m_pNetConnection = pNetConnection;
}

void *wxNetWriterThread::Entry()
{
	if(!m_pNetConnection)
		return (wxThread::ExitCode)-1;

	while(!TestDestroy())
	{
        //WaitForWrite
        m_pNetConnection->ProcessOutputNetMessage();
        wxThread::Sleep(SLEEP);
	}
	return (wxThread::ExitCode)0;
}

void wxNetWriterThread::OnExit()
{
}

//--------------------------------------------------------------------------
// INetConnection
//--------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(INetConnection, wxEvtHandler)

INetConnection::INetConnection(void) : wxEvtHandler()
{
    m_bIsConnected = false;
    m_bIsConnecting = false;
    m_pInThread = NULL;
    m_pOutThread = NULL;
}

INetConnection::~INetConnection()
{
}

void INetConnection::SendNetMessage(const wxNetMessage & msg)
{
    wxCriticalSectionLocker lock(m_dataCS);
    m_aoMessages.push(msg);
}

bool INetConnection::CreateAndRunThreads(void)
{
	m_pOutThread = new wxNetWriterThread(this);
	if(!CreateAndRunThread(m_pOutThread, wxT("wxNetWriterThread"), wxT("NetWriterThread")))
		return false;	

    wxMilliSleep(SLEEP);

    m_pInThread = new wxNetReaderThread(this);
	if(!CreateAndRunThread(m_pInThread, wxT("wxNetReaderThread"), wxT("NetReaderThread")))
		return false;

    return true;
}

void INetConnection::DestroyThreads(void)
{
    if(m_pSock)
    {
        if( m_pSock->Destroy() )
            m_pSock = NULL;
        else
            wxLogDebug(wxT("Socket not destroyed!!!"));
    }

    if (m_pInThread && m_pInThread->IsRunning())
    {
        while(m_pInThread->Delete() != wxTHREAD_NO_ERROR)
        {
            //wxThread::This()->Sleep(SLEEP);
            wxMilliSleep(SLEEP);
        }
        m_pInThread = NULL;
    }

    if (m_pOutThread && m_pOutThread->IsRunning())
    {
        while(m_pOutThread->Delete() != wxTHREAD_NO_ERROR)
        {
            //wxThread::This()->Sleep(SLEEP);
            wxMilliSleep(SLEEP);
        }
        m_pOutThread = NULL;
    }
       
    //m_pOutThread->Delete();//Wait();

    wxCriticalSectionLocker lock(m_dataCS);
    //cleare quere
    while(m_aoMessages.size() > 0)
        m_aoMessages.pop();
}

void INetConnection::ProcessOutputNetMessage(void)
{
    wxCriticalSectionLocker lock(m_dataCS);   

    if(!m_pSock || m_pSock->IsDisconnected())
        return;

    if(m_aoMessages.empty())
        return;

    //while(m_aoMessages.size() > 0 && m_pSock)
    //{
        if(m_pSock->WaitForWrite(WAITFOR))
        {
            //m_pSock->SetFlags(wxSOCKET_WAITALL | wxSOCKET_BLOCK);
            wxNetMessage msgout = m_aoMessages.top();
            size_t nSize = m_pSock->WriteMsg(msgout.GetData(), msgout.GetLength()).LastCount();
            //m_pSock->SetFlags(wxSOCKET_WAITALL);
            if(nSize != msgout.GetLength())
            {
                wxLogError(_("wxGISNetServerConnection: failed to send network message"));
                return;
            }
            wxLogDebug(wxString::Format(wxT("INetConnection: %s"), msgout.GetData()));
            m_aoMessages.pop();
        }
    //}
}

void INetConnection::ProcessInputNetMessage(unsigned char * const buff)
{
    if(!m_pSock || m_pSock->IsDisconnected())
        return;
    if(m_pSock->WaitForRead(WAITFOR))
    {
        size_t nSize = m_pSock->ReadMsg(buff, BUFFSIZE).LastCount();
        if ( !nSize )
        {
            wxLogError(wxString::Format(_("Read error (%d): %s"), m_pSock->LastError(), GetSocketErrorMsg(m_pSock->LastError())));
            return;
        }

 		wxNetMessage msg((const char*)buff, nSize);
        if(!msg.IsOk())
        {
            wxLogError(_("Invalid input message"));
            return;
        }
                
        wxGISNetEvent event(0, wxGISNET_MSG, msg);
        PostEvent(event);

        RtlZeroMemory(buff, BUFFSIZE);
    }
}

//--------------------------------------------------------------------------
// Non class functions
//--------------------------------------------------------------------------
bool SendUDP(wxIPV4address addr, wxNetMessage & msg, bool broadcast)
{
    wxIPV4address addrLocal;
    addrLocal.Hostname();	
	// Set up a temporary UDP socket for sending datagrams
	wxDatagramSocket send_udp(addrLocal, (broadcast ? wxSOCKET_BROADCAST | wxSOCKET_NOBIND : wxSOCKET_NOBIND));// | wxSOCKET_NOWAIT | wxSOCKET_NOWAIT
    if ( !send_udp.IsOk() )
    {
        wxLogError(_("SendUDP: failed to create UDP socket"));
        return false;
    }
    
    wxLogDebug(wxT("SendUDP: Created UDP socket at %s:%u"), addrLocal.IPAddress(), addrLocal.Service());
    wxLogDebug(wxT("SendUDP: Testing UDP with peer at %s:%u"), addr.IPAddress(), addr.Service());

    if(!msg.IsOk())
    {
        wxLogError(_("SendUDP: invalid net message"));
        return false;
    }

    if ( send_udp.SendTo(addr, msg.GetData(), msg.GetLength()).LastCount() != msg.GetLength() )
    {
        wxLogError(_("SendUDP: failed to send data"));
        return false;
    }    

    return true;
}

wxString GetSocketErrorMsg(int pSockError)
{
    switch(pSockError)
    {
        case wxSOCKET_NOERROR:
            return wxString(_("wxSOCKET_NOERROR"));

        case wxSOCKET_INVOP:
            return wxString(_("wxSOCKET_INVOP"));

        case wxSOCKET_IOERR:
            return wxString(_("wxSOCKET_IOERR"));

        case wxSOCKET_INVADDR:
            return wxString(_("wxSOCKET_INVADDR"));

        case wxSOCKET_NOHOST:
            return wxString(_("wxSOCKET_NOHOST"));

        case wxSOCKET_INVPORT:
            return wxString(_("wxSOCKET_INVPORT"));

        case wxSOCKET_WOULDBLOCK:
            return wxString(_("wxSOCKET_WOULDBLOCK"));

        case wxSOCKET_TIMEDOUT:
            return wxString(_("wxSOCKET_TIMEDOUT"));

        case wxSOCKET_MEMERR:
            return wxString(_("wxSOCKET_MEMERR"));

        default:
            return wxString(_("Unknown"));
    }
}
