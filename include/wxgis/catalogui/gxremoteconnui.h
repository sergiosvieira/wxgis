/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnectionUI class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#ifdef wxGIS_USE_POSTGRES

/** \class wxGxRemoteConnectionUI gxfileui.h
    \brief A Remote Connection GxObjectUI.
*/

class WXDLLIMPEXP_GIS_CLU wxGxRemoteConnectionUI :
    public wxGxRemoteConnection,
	public IGxObjectUI,
    public IGxObjectEditUI,
    public IGxObjectWizard,
    public wxThreadHelper
{
public:
	wxGxRemoteConnectionUI(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", wxIcon LargeIconConn = wxNullIcon, wxIcon SmallIconConn = wxNullIcon, wxIcon LargeIconDisconn = wxNullIcon, wxIcon SmallIconDisconn = wxNullIcon);
	virtual ~wxGxRemoteConnectionUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxRemoteConnection.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxRemoteConnection.NewMenu"));};
    //IGxRemoteConnection
	virtual bool Connect(void);
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
protected:
    //wxGxRemoteConnection
//    virtual wxGxRemoteDBSchema* GetNewRemoteDBSchema(const CPLString &szName, wxGISPostgresDataSourceSPtr pwxGISRemoteCon);
    virtual wxThread::ExitCode Entry();
    bool CreateAndRunCheckThread(void);
protected:
    wxIcon m_oLargeIconConn, m_oSmallIconConn;
    wxIcon m_oLargeIconDisconn, m_oSmallIconDisconn;
    wxIcon m_oLargeIconFeatureClass, m_oSmallIconFeatureClass;
    wxIcon m_oLargeIconTable, m_oSmallIconTable;
    wxIcon m_oLargeIconSchema, m_oSmallIconSchema;
    long m_PendingId;
};

#endif // wxGIS_USE_POSTGRES

/** \class wxGxRemoteDBSchemaUI gxfileui.h
    \brief A Remote Database schema GxObjectUI.
*/
/*
class WXDLLIMPEXP_GIS_CLU wxGxRemoteDBSchemaUI :
    public wxGxRemoteDBSchema,
	public IGxObjectUI,
    public IGxObjectEditUI
{
public:
	wxGxRemoteDBSchemaUI(const wxString &sName, wxGISPostgresDataSourceSPtr pwxGISRemoteConn, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon, wxIcon LargeIconFeatureClass = wxNullIcon, wxIcon SmallIconFeatureClass = wxNullIcon, wxIcon LargeIconTable = wxNullIcon, wxIcon SmallIconTable = wxNullIcon);
	virtual ~wxGxRemoteDBSchemaUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxEmptyString;};
	virtual wxString NewMenu(void){return wxEmptyString;};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    //wxGxRemoteDBSchema
    virtual void AddTable(CPLString &szName, CPLString &szSchema, bool bHasGeometry);
protected:
    wxIcon m_oLargeIcon, m_oSmallIcon;
    wxIcon m_oLargeIconFeatureClass, m_oSmallIconFeatureClass;
    wxIcon m_oLargeIconTable, m_oSmallIconTable;
};
*/

