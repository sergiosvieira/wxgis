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

class WXDLLIMPEXP_GIS_CLU wxGxRemoteConnectionUI;
/** \class wxGISRasterRGBARenderer rasterrenderer.h
    \brief The raster layer renderer for RGB data and Alpha channel
*/
class wxChildLoaderThread : public wxThread
{
public:
	wxChildLoaderThread(wxGxRemoteConnectionUI *pGxRemoteConnectionUI, int nBeg, int nEnd, IProgressor* pProgressor = NULL);
    virtual void *Entry();
    virtual void OnExit();
private:
	wxGxRemoteConnectionUI *m_pGxRemoteConnectionUI;
	int m_nBeg;
	int m_nEnd;
    IProgressor* m_pProgressor;
};

#define MAX_LAYERS 100
/** \class wxGxRemoteConnectionUI gxfileui.h
    \brief A Remote Connection GxObjectUI.
*/
class WXDLLIMPEXP_GIS_CLU wxGxRemoteConnectionUI :
    public wxGxRemoteConnection,
	public IGxObjectUI,
    public IGxObjectEditUI,
    public IGxObjectWizard
{
    friend class wxChildLoaderThread;
public:
	wxGxRemoteConnectionUI(CPLString soPath, wxString Name, wxIcon LargeIconConn = wxNullIcon, wxIcon SmallIconConn = wxNullIcon, wxIcon LargeIconDisconn = wxNullIcon, wxIcon SmallIconDisconn = wxNullIcon, wxIcon LargeIconFeatureClass = wxNullIcon, wxIcon SmallIconFeatureClass = wxNullIcon, wxIcon LargeIconTable = wxNullIcon, wxIcon SmallIconTable = wxNullIcon);
	virtual ~wxGxRemoteConnectionUI(void);
    //wxGxRemoteConnection
    virtual void Detach(void);
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
    //wxGxRemoteConnection
	virtual void LoadChildren(void);
    virtual void AddSubDataset(size_t nIndex);
    virtual void OnThreadExit(wxThreadIdType nThreadID);
protected:
    wxIcon m_oLargeIconConn;
    wxIcon m_oSmallIconConn;
    wxIcon m_oLargeIconDisconn;
    wxIcon m_oSmallIconDisconn;
    wxIcon m_oLargeIconFeatureClass, m_oSmallIconFeatureClass;
    wxIcon m_oLargeIconTable, m_oSmallIconTable;
    //
    std::map<wxThreadIdType, wxChildLoaderThread*> m_pmThreads;
    int m_nRunningThreads;
    IProgressor* m_pProgressor;
};

