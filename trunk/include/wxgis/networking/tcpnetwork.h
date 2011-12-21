/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  TCP network server class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#pragma once

#include "wxgis/networking/networking.h"

/** \class wxNetTCPReader tcpnetwork.h
 *  \brief The tcp network connection reader thread.
 */
class WXDLLIMPEXP_GIS_NET wxNetTCPReader : public wxThread
{
public:
    wxNetTCPReader(INetConnection* pNetConnection, wxSocketBase* pSock);
    virtual void *Entry();
    virtual void OnExit();
protected:
	wxSocketBase* m_pSock;
    INetConnection* m_pNetConnection;
};

/** \class wxClientTCPWriter tcpnetwork.h
 *  \brief The tcp network connection writer thread.
 */
class WXDLLIMPEXP_GIS_NET wxNetTCPWriter : public wxThread
{
public:
    wxNetTCPWriter(INetConnection* pNetConnection, wxSocketBase* pSock);
    virtual void *Entry();
    virtual void OnExit();
protected:
	wxSocketBase* m_pSock;
    INetConnection* m_pNetConnection;
};

/** \class wxClientTCPWaitlost tcpnetwork.h
 *  \brief The tcp network connection waitloast thread.
 */
class WXDLLIMPEXP_GIS_NET wxNetTCPWaitlost : public wxThread
{
public:
    wxNetTCPWaitlost(INetConnection* pNetConnection, wxSocketBase* pSock);
    virtual void *Entry();
    virtual void OnExit();
protected:
	wxSocketBase* m_pSock;
    INetConnection* m_pNetConnection;
};
