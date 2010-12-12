/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxRxObject class.
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

#include "wxgis/remoteserver/rxobjectclient.h"

wxRxObject::wxRxObject() : m_pGxRemoteServer(NULL)
{
}

wxRxObject::~wxRxObject()
{
}

bool wxRxObject::Init(wxGxRemoteServer* pGxRemoteServer, wxXmlNode* pProperties)
{
	if(!pProperties && !pGxRemoteServer)
		return false;
	m_pGxRemoteServer = pGxRemoteServer;
	if(!pProperties->HasProp(wxT("dst"))) //set dst from xml
	{
		m_sDst = pProperties->GetPropVal(wxT("dst"), ERR);
		INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pGxRemoteServer);
		if(pNetMessageProcessor)
			pNetMessageProcessor->AddMessageReceiver(m_sDst, static_cast<INetMessageReceiver*>(this));
	}
	return true;
}

void wxRxObject::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
}
