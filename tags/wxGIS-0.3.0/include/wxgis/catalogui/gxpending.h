/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxPendingUI class. Show pending item in tree or content view
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

class WXDLLIMPEXP_GIS_CLU wxGxPendingUI;

/** \class wxPendingUpdateThread gxpending.h
    \brief The pending item update thread
*/
class wxPendingUpdateThread : public wxThread
{
public:
	wxPendingUpdateThread(wxGxPendingUI* pGxPendingUI);
    virtual void *Entry();
    virtual void OnExit();
private:
    wxGxPendingUI* m_pGxPendingUI;
};

/** \class wxGxPendingUI gxpending.h
    \brief The pending item in tree or content view
*/

class WXDLLIMPEXP_GIS_CLU wxGxPendingUI : 
    public IGxObject,
    public IGxObjectUI
{
public:
	wxGxPendingUI(void);
	virtual ~wxGxPendingUI(void);
	//IGxObject
	virtual wxString GetName(void){return wxString(_("Waiting..."));};
    virtual wxString GetBaseName(void){return GetName();};
    virtual CPLString GetInternalName(void){return CPLString();};
	virtual wxString GetCategory(void){return wxString(_("Waiting..."));};
	virtual bool Attach(IGxObject* pParent, IGxCatalog* pCatalog);
    virtual void Detach(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxEmptyString;};
	virtual wxString NewMenu(void){return wxEmptyString;};
    //
    virtual void OnUpdate(void);
    virtual void OnStopPending(void);
protected:
    wxImageList *m_pImageListSmall, *m_pImageListLarge;
    short m_nCurrentImage;
    wxPendingUpdateThread *m_pThread;
};
