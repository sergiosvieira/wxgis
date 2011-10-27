/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnectionUI class.
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxremoteconn.h"

/** \class wxGxRemoteConnectionUI gxfileui.h
    \brief A Remote Connection GxObjectUI.
*/
class WXDLLIMPEXP_GIS_CLU wxGxRemoteConnectionUI :
    public wxGxRemoteConnection,
	public IGxObjectUI,
    public IGxObjectEditUI,
    public IGxObjectWizard
{
public:
	wxGxRemoteConnectionUI(CPLString Path, wxString Name, wxIcon LargeIconConn = wxNullIcon, wxIcon SmallIconConn = wxNullIcon, wxIcon LargeIconDisconn = wxNullIcon, wxIcon SmallIconDisconn = wxNullIcon);
	virtual ~wxGxRemoteConnectionUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxRemoteConnectionUI.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxRemoteConnectionUI.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
protected:
    wxIcon m_oLargeIconConn;
    wxIcon m_oSmallIconConn;
    wxIcon m_oLargeIconDisconn;
    wxIcon m_oSmallIconDisconn;
};
