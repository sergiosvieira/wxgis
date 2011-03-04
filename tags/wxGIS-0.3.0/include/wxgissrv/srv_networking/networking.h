/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGIS Server Networking header.
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
#pragma once

#include "wxgis/base.h"
#include "wxgis/networking/networking.h"
#include "wxgissrv/srv_framework/auth.h"

#include "wx/socket.h"

/** \class INetServerConnection networking.h
    \brief The network connection interface class.
*/
class INetServerConnection : public INetConnection
{
public:
	virtual ~INetServerConnection(void){};
	virtual void SetAuth(AUTHRESPOND sUserInfo) = 0;//set user authenticated state true
	virtual AUTHRESPOND GetAuth(void) = 0; // get if user has been authenticated
	virtual void SetAlive(wxDateTime dtm) = 0;
	virtual wxDateTime GetAlive(void) = 0;
};

