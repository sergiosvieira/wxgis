/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSelection class. Selection of IGxObjects in tree or list views
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

#include "wxgis/catalogui/catalogui.h"

// ----------------------------------------------------------------------------
// wxGxSelection
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLU wxGxSelection :
	public IConnectionPointContainer,
	public IGxSelection
{
public:
	wxGxSelection(void);
	virtual ~wxGxSelection(void);
	//IGxSelection
	virtual void Select( IGxObject* pObject,  bool appendToExistingSelection, long nInitiator);
    virtual void Select( IGxObject* pObject);
	virtual void Unselect(IGxObject* pObject, long nInitiator);
	virtual void Clear(long nInitiator);
    virtual size_t GetCount(void);
    virtual size_t GetCount(long nInitiator);
	virtual IGxObject* GetSelectedObjects(size_t nIndex);
	virtual IGxObject* GetSelectedObjects(long nInitiator, size_t nIndex);
    virtual IGxObject* GetLastSelectedObject(void);
	virtual void SetInitiator(long nInitiator);
    virtual void Do(IGxObject* pObject);
    virtual bool CanRedo();
	virtual bool CanUndo();
    virtual wxString Redo(int nPos = -1);
    virtual wxString Undo(int nPos = -1);
	virtual void RemoveDo(wxString sPath);
    virtual void Reset();
    virtual size_t GetDoSize();
    virtual int GetDoPos(void){return m_Pos;};
    virtual wxString GetDoPath(size_t nIndex);
    //virtual wxArrayString* GetDoArray(void){return &m_DoArray;};
    virtual void LockChanges(){m_CritSect.Enter();};
    virtual void UnLockChanges(){m_CritSect.Leave();};
    //IConnectionPointContainer
	virtual long Advise(wxObject* pObject);
protected:
    wxArrayString m_DoArray;
	int m_Pos;
    bool m_bDoOp;
    IGxObject* m_pPrevObject;
    wxCriticalSection m_DoCritSect, m_CritSect;

	std::map<long, GxObjectArray*> m_SelectionMap;
	long m_currentInitiator;
};
