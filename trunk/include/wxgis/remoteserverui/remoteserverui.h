/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  remote server UI header.
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

#include "wxgis/remoteserver/remoteserver.h"

/** \class INetConnFactoryUI remoteserverui.h
    \brief The network connection factory UI interface class.
*/
class INetConnFactoryUI
{
public:
    virtual ~INetConnFactoryUI(void){};
	virtual wxWindow* GetPropertyPage(wxWindow* pParent) = 0;
};

/** \class INetConnFactoryUI remoteserverui.h
    \brief The network connection factory UI interface class.
*/
class wxNetPropertyPage :
	public wxPanel
{
public:
	wxNetPropertyPage(INetConnFactoryUI* pFactory, wxWindow* pParent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLIP_CHILDREN | wxNO_BORDER | wxTAB_TRAVERSAL) : wxPanel(pParent, id, pos, size, style)
	{
		m_pFactory = pFactory;
	}
	virtual ~wxNetPropertyPage(void){};
	virtual INetConnection* OnSave(void) = 0;
	virtual wxString GetLastError(void){return m_sErrorMsg;};
protected:
	INetConnFactoryUI* m_pFactory;
	wxString m_sErrorMsg;
};