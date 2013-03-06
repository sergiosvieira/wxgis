/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnectionsUI class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2013 Bishop
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxdbconnections.h"
#include "wxgis/catalogui/gxview.h"

/** \class wxGxDBConnectionsUI gxdbconnectionsui.h
    \brief The database connections root item in user interface.
*/

class WXDLLIMPEXP_GIS_CLU wxGxDBConnectionsUI :
    public wxGxDBConnections,
	public IGxObjectUI
{
    DECLARE_DYNAMIC_CLASS(wxGxDBConnectionsUI)
public:
	wxGxDBConnectionsUI(void);
	virtual ~wxGxDBConnectionsUI(void);
    //wxGxDBConnections
    virtual void AddChild( wxGxObject *child );
    //IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxDBConnections.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxDBConnections.NewMenu"));};
    //wxGxDBConnectionsUI
    virtual void BeginRenameOnAdd(wxGxView* const pGxView, const CPLString &szPath);
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
    wxGxView* m_pGxViewToRename;
    CPLString m_szPathToRename;
};
