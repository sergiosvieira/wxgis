/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolder class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Bishop
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
#include "wxgis/catalog/gxobject.h"

#include <wx/dir.h>
#include <wx/event.h>
#include <wx/fswatcher.h>

#if wxVERSION_NUMBER <= 2903// && !defined EVT_FSWATCHER(winid, func)
#define EVT_FSWATCHER(winid, func) \
    wx__DECLARE_EVT1(wxEVT_FSWATCHER, winid, wxFileSystemWatcherEventHandler(func))
#endif

/** \class wxGxFolder gxfolder.h
    \brief The Folder GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxFolder :
	public wxGxObjectContainer,
	public IGxObjectEdit
{
    DECLARE_CLASS(wxGxFolder)
public:
    wxGxFolder(void);
	wxGxFolder(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxFolder(void);
	//wxGxObject
    virtual wxString GetBaseName(void) const;
	virtual wxString GetCategory(void) const {return wxString(_("Folder"));};
	virtual void Refresh(void);
    virtual bool Destroy(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	//wxGxObjectContainer    
	//virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void);
    virtual bool CanCreate(long nDataType, long DataSubtype); 
//events
    virtual void OnFileSystemEvent(wxFileSystemWatcherEvent& event);
protected:
	//wxGxFolder
	virtual void LoadChildren(void);
protected:
	bool m_bIsChildrenLoaded;
    wxFileSystemWatcher *m_pWatcher;
private:
    DECLARE_EVENT_TABLE()
};
