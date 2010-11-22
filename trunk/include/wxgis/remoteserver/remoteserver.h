/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  remote server header.
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

#include "wxgis/catalog/catalog.h"

class INetSearchCallback
{
    virtual ~INetSearchCallback(void){};
	//pure virtual
	virtual void AddHost(wxString sName, wxString sPlugin, wxString sHost, wxString sBanner, wxXmlNode* pUserData) = 0;
};

/** \class INetConnFactory remoteserver.h
    \brief The network connection factory interface class.
*/
class INetConnFactory
{
public:
    virtual ~INetConnFactory(void){};
	//pure virtual
	virtual bool StartServerSearch() = 0;
	virtual bool StopServerSearch() = 0;
	virtual bool CanStopServerSearch() = 0;
	virtual void SetCallback(INetSearchCallback* pCallback) = 0;
	virtual INetSearchCallback* GetCallback(void) = 0;
	virtual wxString GetName(void) = 0;
    //INetConnection* GetConnection() = 0;
    /** \fn wxXmlNode* GetProperties(void)
     *  \brief Get Properties of Factory.
     *  \return The properties of the Factory.
	 *
	 *  It should be the new wxXmlNode (not a pointer to other xml data)
     */	 	
	virtual wxXmlNode* GetProperties(void) = 0;
    /** \fn void SetProperties(wxXmlNode* pProp)
     *  \brief Set Properties of plugin.
     *  \param pProp The properties of the plugin
	 *
	 *  Executed while LoadPlugins (after flugin created). 
     */	  
	virtual void SetProperties(const wxXmlNode* pProp) = 0;

};

typedef std::vector<INetConnFactory*> NETCONNFACTORYARRAY;