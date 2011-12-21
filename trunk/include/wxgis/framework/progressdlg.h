/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressDlg class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/framework/framework.h"

#include <wx/progdlg.h>

/** \class wxGISProgressDlg progressdlg.h
    \brief The dialog showing progress and some buttons.
*/
class WXDLLIMPEXP_GIS_FRW wxGISProgressDlg : 
	public wxProgressDialog,
	public ITrackCancel,
	public IProgressor
{
public:
	wxGISProgressDlg( const wxString &title, const wxString &message, int  maximum = 100, wxWindow *  parent = NULL, int style = wxPD_AUTO_HIDE|wxPD_APP_MODAL );
	virtual ~wxGISProgressDlg(void);
	//IProgressor
    virtual bool Show(bool bShow);
    virtual void SetRange(int range);
    virtual int GetRange(void);
    virtual void SetValue(int value);
    virtual int GetValue(void);
    virtual void Play(void);
    virtual void Stop(void);
	virtual void SetYield(bool bYield = false){};
	//ITrackCancel
	virtual void Cancel(void);
	virtual bool Continue(void);
	virtual void Reset(void);
	virtual void PutMessage(wxString sMessage, size_t nIndex = wxNOT_FOUND, wxGISEnumMessageType nType = enumGISMessageUnk);
protected:
	int m_nValue;
	wxString m_sLastMessage;
};
