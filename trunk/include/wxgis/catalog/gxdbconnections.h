/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnections class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/catalog/gxfolder.h"

/** \class wxGxDBConnections gxdbconnections.h
    \brief The database connections root item.

	This root item can held connections (*.xconn) and folders items
*/

class WXDLLIMPEXP_GIS_CLT wxGxDBConnections :
	public wxGxFolder,
    public IGxRootObjectProperties,
    public wxObject
{
   DECLARE_DYNAMIC_CLASS(wxGxDBConnections)
public:
	wxGxDBConnections(void);//wxString Path, wxString Name, bool bShowHidden
	virtual ~wxGxDBConnections(void);
	//IGxObject
	virtual wxString GetName(void){return wxString(_("DataBase connections"));};
    virtual wxString GetBaseName(void){return GetName();};
	virtual CPLString GetInternalName(void){return m_sPath;};
	virtual wxString GetCategory(void){return wxString(_("DataBase connections Folder"));};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* const pConfigNode);
    virtual void Serialize(wxXmlNode* pConfigNode);
	//IGxObjectEdit
	virtual bool Delete(void){return false;};
	virtual bool CanDelete(void){return false;};
	virtual bool Rename(wxString NewName){return false;};
	virtual bool CanRename(void){return false;};
	//IGxObjectContainer
    virtual bool CanCreate(long nDataType, long DataSubtype); 
protected:
	wxString m_sInternalPath;
};

