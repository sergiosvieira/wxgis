/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GIS application accelerator table header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
#include <wx/accel.h>
#include "wxgis/core/config.h"

class WXDLLIMPEXP_GIS_FRW wxGISAcceleratorTable
{
public:
	wxGISAcceleratorTable(IFrameApplication* pApp);
	virtual ~wxGISAcceleratorTable(void);
	virtual int Add(wxAcceleratorEntry entry);
	virtual void Remove(wxAcceleratorEntry entry);
	virtual wxAcceleratorTable GetAcceleratorTable(void);
	virtual bool HasChanges(void){return bHasChanges;};
	virtual wxString GetText(int cmd);
	virtual wxAcceleratorEntry GetEntry(int cmd);
	virtual void Store(void);
protected:
	int GetKeyCode(wxString sKeyCode);
	WXDWORD GetFlags(wxString sFlags);
private:
	std::vector<wxAcceleratorEntry> m_AccelEntryArray;
	bool bHasChanges;
	wxAcceleratorTable m_ATab;
	IFrameApplication* m_pApp;
};