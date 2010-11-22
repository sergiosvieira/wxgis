/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  TCP network plugin class.
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

#include "wxgis/remoteserver/remoteserver.h"

//////////////////////////////////////////////////////////////////////
//// wxClientUDPNotifier
//////////////////////////////////////////////////////////////////////
//
//wxClientUDPNotifier::wxClientUDPNotifier(INetSearch* pPlugin) : wxThread()
//{
//	m_pPlugin = pPlugin;
//	if(!m_pPlugin)
//		return;
//	wxXmlNode* pProps = m_pPlugin->GetProperties();
//	if(pProps)
//		m_nAdvPort = wxAtoi(pProps->GetPropVal(wxT("adv_port"), wxT("1977"));
//	else
//		m_nAdvPort = 1977;
//
//	wxIPV4address LocalAddress; // For the listening 
//	LocalAddress.Service(m_nAdvPort + 2); // port on which we listen for the answers 
//
//	bool bIsAddrSet = LocalAddress.AnyAddress(); 
//	if(!bIsAddrSet)
//	{
//		wxLogError(_("wxClientUDPNotifier: Invalid address"));
//		return;
//	}
//
//	m_socket = new wxDatagramSocket(LocalAddress, wxSOCKET_REUSEADDR | wxSOCKET_NOWAIT); 
//	const int optval = 1; 
//	m_socket->SetOption(SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)); 
//	//m_socket->SetOption(SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)); 
//	//m_socket->SetOption(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
//}
//
//void *wxClientUDPNotifier::Entry()
//{
//	unsigned char buff[BUFF] = {0};
//
//	//start loop
//	while(!TestDestroy())
//	{
//		wxIPV4address BroadCastAddress; // For broadcast sending 
//		BroadCastAddress.Service(m_nAdvPort + 2);
//		//WaitForRead
//		if(m_socket->WaitForRead(0, 100))
//		{
//			m_socket->RecvFrom(BroadCastAddress, &buff, BUFF); 
//			if(m_socket->Error())
//			{
//				wxYieldIfNeeded();
//				wxThread::Sleep(10);
//				continue;
//			}
//			size_t nSize = m_socket->LastCount();
//			wxNetMessage msg(buff, nSize);
//			if(!msg.IsOk())
//			{
//				wxYieldIfNeeded();
//				wxThread::Sleep(10);
//				continue;
//			}
//			if(msg.GetState() == enumGISMsgStOk)
//			{
//				wxXmlNode* pRootNode = msg.GetRoot();
//				wxXmlNode* pChild = pRootNode->GetChildren();
//				while(pChild)
//				{
//					if(pChild->GetName().CmpNoCase(wxT("srv_info")) == 0)
//					{
//						INetSearchCallback* pCallback = m_pPlugin->GetCallback();
//						if(pCallback)
//						{
//							wxString sPort = pChild->GetPropVal(wxT("port"), wxT("1976"));
//							wxString sName = pChild->GetPropVal(wxT("name"), NONAME);
//							wxString sBanner = pChild->GetPropVal(wxT("banner"), wxEmptyString);
//							wxString sHost = BroadCastAddress.Hostname();
//							wxString sIP = BroadCastAddress.IPAddress();
//							//Add to list
//							pCallback->AddHost(sName, m_pPlugin->GetName(), sHost, sBanner, NULL);
//						}
//					}
//					pChild = pChild->GetNext();
//				}
//			}
//		}
//		wxYieldIfNeeded();
//	}
//	return NULL;
//}
//
//void wxClientUDPNotifier::OnExit()
//{
//	if(m_socket)
//		m_socket->Destroy();
//}
//
//void wxClientUDPNotifier::SendBroadcastMsg(void)
//{
//	wxIPV4address BroadCastAddress; // For broadcast sending 
//	BroadCastAddress.Hostname(_("255.255.255.255")); 
//	BroadCastAddress.Service(m_nAdvPort); // port on which we listen for the answers 
//
//	// Create the socket 
//	wxString sMsg = wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStHello, enumGISPriorityHightest);
//	wxNetMessage msg(sMsg, wxID_ANY);
//    if(msg.IsOk())
//    {
//		m_socket->Discard();
//		if(m_socket->WaitForWrite(0, 100))
//		{
//			m_socket->SendTo(BroadCastAddress, msg.GetData(), msg.GetDataLen() ); 
//		}
//    }
//}

////////////////////////////////////////////////////////////////////
// wxClientTCPNetPlugin
////////////////////////////////////////////////////////////////////

//IMPLEMENT_DYNAMIC_CLASS(wxClientTCPNetPlugin, wxObject)
//
//wxClientTCPNetPlugin::wxClientTCPNetPlugin(void) : m_pCallback(NULL)
//{
//}
//
//wxClientTCPNetPlugin::~wxClientTCPNetPlugin(void)
//{
//}
//
//wxXmlNode* wxClientTCPNetPlugin::GetProperties(void)
//{
//    return m_pProperties;
//}
//
//void wxClientTCPNetPlugin::SetProperties(wxXmlNode* pProp)
//{
//    m_pProperties = pProp;
//}
//
//bool wxClientTCPNetPlugin::StartServerSearch()
//{
//    return false;
//}
//
//void wxClientTCPNetPlugin::SetCallback(INetSearchCallback* pCallback)
//{
//    m_pCallback = pCallback;
//}
//
//INetSearchCallback* wxClientTCPNetPlugin::GetCallback(void)
//{
//    return m_pCallback;
//}
//
