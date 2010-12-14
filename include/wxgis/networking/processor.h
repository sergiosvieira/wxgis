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
#pragma once

#include "wxgis/networking/networking.h"
#include "wxgis/core/core.h"

class WXDLLIMPEXP_GIS_NET wxGISNetMessageProcessor;
/** \class wxMsgInThread processor.h
    \brief The input message thread.
*/
class wxMsgInThread : public wxThread
{
public:
	wxMsgInThread(wxGISNetMessageProcessor* pParent);
    virtual void *Entry();
    virtual void OnExit();
private:
    wxGISNetMessageProcessor* m_pParent;
};

/** \class wxGISNetMessageProcessor processor.h
    \brief The messages processor class.
*/
class WXDLLIMPEXP_GIS_NET wxGISNetMessageProcessor : public INetMessageProcessor
{
public:
	wxGISNetMessageProcessor(void);
	virtual ~wxGISNetMessageProcessor(void);
	//INetMessageProcessor
    virtual WXGISMSG GetInMessage(void);
    virtual void ProcessMessage(WXGISMSG msg);
	virtual void ClearMessageQueue(void);
	virtual void AddMessageReceiver(wxString sName, INetMessageReceiver* pNetMessageReceiver);
	virtual void DelMessageReceiver(wxString sName, INetMessageReceiver* pNetMessageReceiver);
	virtual void ClearMessageReceiver(void);
	//
	virtual bool OnStartMessageThread(void);
	virtual void OnStopMessageThread(void);
protected:
    WXGISMSGQUEUE m_MsgQueue;
    wxCriticalSection m_CriticalSection;
	std::vector<std::pair<wxString, INetMessageReceiver*>> m_MessageReceiverArray;
    wxMsgInThread *m_pMsgInThread;
};

