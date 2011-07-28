/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSelection class. Selection of IGxObjects in tree or list views
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalogui/gxeventui.h"

wxGxSelection::wxGxSelection(void) : m_nPos(wxNOT_FOUND), m_bDoOp(false), m_currentInitiator(wxNOT_FOUND)
{
}

wxGxSelection::~wxGxSelection(void)
{
}

void wxGxSelection::Select( long nObjectID,  bool appendToExistingSelection, long nInitiator )
{
	wxCriticalSectionLocker locker(m_CritSect);
	//wxMutexLocker locker(m_Mutex);

    //m_CritSect.Enter();

    //check for duplicates
	int nIndex = m_SelectionMap[nInitiator].Index(nObjectID);
	if(nIndex != wxNOT_FOUND)
	{
        //m_CritSect.Leave();
        //not fire event id NOTFIRESELID
        if(nInitiator != NOTFIRESELID)
            goto END;
        return;
	}
    m_currentInitiator = nInitiator;

    //m_CritSect.Leave();

    if(!appendToExistingSelection)
	    Clear(nInitiator);

	//m_CritSect.Enter();
	m_SelectionMap[nInitiator].Add( nObjectID );
    //m_CritSect.Leave();

	//not fire event id NOTFIRESELID
	if(nInitiator == NOTFIRESELID)
		return;

    //
    Do( nObjectID );

	//fire event
END:
	wxGxSelectionEvent event(wxGXSELECTION_CHANGED, this, nInitiator);
	for(size_t i = 0; i < m_pPointsArray.size(); ++i)
	{
		if(m_pPointsArray[i] != NULL)
			m_pPointsArray[i]->AddPendingEvent(event);
	}
}

void wxGxSelection::SetInitiator(long nInitiator)
{
    wxCriticalSectionLocker locker(m_CritSect);
	m_currentInitiator = nInitiator;
}

void wxGxSelection::Select( long nObjectID )
{
    m_CritSect.Enter();
	m_currentInitiator = INIT_ALL;
	Clear(INIT_ALL);

    //check for duplicates
	int nIndex = m_SelectionMap[INIT_ALL].Index(nObjectID);
	if(nIndex != wxNOT_FOUND)
        return;
	m_SelectionMap[INIT_ALL].Add(nObjectID);
    m_CritSect.Leave();

	//fire event
	wxGxSelectionEvent event(wxGXSELECTION_CHANGED, this, INIT_ALL);
	for(size_t i = 0; i < m_pPointsArray.size(); ++i)
	{
		if(m_pPointsArray[i] != NULL)
			m_pPointsArray[i]->AddPendingEvent(event);
	}
}

void wxGxSelection::Unselect( long nObjectID, long nInitiator )
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(nInitiator == INIT_ALL)
	{
		for(std::map<long, wxArrayLong>::iterator CI = m_SelectionMap.begin(); CI != m_SelectionMap.end(); ++CI)
		{
			int nIndex = CI->second.Index(nObjectID);
			if(nIndex != wxNOT_FOUND)
				CI->second.Remove(nIndex);
		}
	}
	else
	{
		int nIndex = m_SelectionMap[nInitiator].Index(nObjectID);
		if(nIndex != wxNOT_FOUND)
			m_SelectionMap[nInitiator].RemoveAt(nIndex);
	}
}

void wxGxSelection::Clear(long nInitiator)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(nInitiator == INIT_ALL)
	{
		for(std::map<long, wxArrayLong>::iterator CI = m_SelectionMap.begin(); CI != m_SelectionMap.end(); ++CI)
			CI->second.Clear();
	}
	else
	{
		m_SelectionMap[nInitiator].Clear();
	}
}

size_t wxGxSelection::GetCount(void)
{
	if(m_currentInitiator == INIT_NONE)
		return 0;
	else
		return GetCount(m_currentInitiator);
}

size_t wxGxSelection::GetCount(long nInitiator)
{
    wxCriticalSectionLocker locker(m_CritSect);
	return m_SelectionMap[nInitiator].GetCount();
}

long wxGxSelection::GetLastSelectedObjectID(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(m_currentInitiator == INIT_NONE || m_SelectionMap[m_currentInitiator].GetCount() == 0)
		return wxNOT_FOUND;
    return m_SelectionMap[m_currentInitiator].Last();
}

long wxGxSelection::GetSelectedObjectID(size_t nIndex)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(m_currentInitiator == INIT_NONE)
		return wxNOT_FOUND;   

    return GetSelectedObjectID(m_currentInitiator, nIndex);
}

long wxGxSelection::GetSelectedObjectID(long nInitiator, size_t nIndex)
{
	wxCriticalSectionLocker locker(m_CritSect);
	if(m_SelectionMap[m_currentInitiator].GetCount() == 0)
		return wxNOT_FOUND;
	if(m_SelectionMap[nInitiator].GetCount() - 1 >= nIndex)
		return m_SelectionMap[nInitiator].Item(nIndex);
	return wxNOT_FOUND;
}

void wxGxSelection::Do( long nObjectID )
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if( m_pPrevID == nObjectID )
        return;

    m_pPrevID = nObjectID;

    if(m_bDoOp)
    {
        m_bDoOp = false;
        return;
    }

    if( !m_DoArray.IsEmpty() )
        if( m_DoArray.Last() == nObjectID )
            return;
    m_nPos++;
    if(m_nPos == m_DoArray.GetCount())
        m_DoArray.Add( nObjectID );
    else if(m_nPos > m_DoArray.GetCount())
    {
        m_DoArray.Add( nObjectID );
        m_nPos = m_DoArray.GetCount() - 1;
    }
    else
    {
	    m_DoArray[m_nPos] = nObjectID;
        size_t nCount = m_DoArray.GetCount() - (m_nPos + 1);
        if(nCount == 0)
            nCount = 1;
        if(m_nPos + 1 < m_DoArray.GetCount() && nCount > 0)
            m_DoArray.RemoveAt(m_nPos + 1, nCount);
    }
}

bool wxGxSelection::CanRedo()
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(m_DoArray.IsEmpty())
	    return false;
    return m_nPos < m_DoArray.GetCount() - 1;
}

bool wxGxSelection::CanUndo()
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(m_DoArray.IsEmpty())
	    return false;
    return m_nPos > 0;
}

long wxGxSelection::Redo(int nPos)
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(nPos == wxNOT_FOUND)
        m_nPos++;
    else
        m_nPos = nPos;
    if(m_nPos < m_DoArray.GetCount())
    {
        m_bDoOp = true;
        return m_DoArray[m_nPos];
    }
    return wxNOT_FOUND;
}

long wxGxSelection::Undo(int nPos)
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(nPos == wxNOT_FOUND)
        m_nPos--;
    else
        m_nPos = nPos;
    if(m_nPos > wxNOT_FOUND)
    {
        m_bDoOp = true;
        return m_DoArray[m_nPos];
    }
    return wxNOT_FOUND;
}

void wxGxSelection::RemoveDo(long nObjectID)
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    for(size_t i = 0; i < m_DoArray.GetCount(); ++i)
    {
        //clean from doubles
        if(i > 0 && m_DoArray[i - 1] == m_DoArray[i])
        {
            m_DoArray.RemoveAt(i);
            i--;
        }
        if(m_DoArray[i] == nObjectID)
        {
            m_DoArray.RemoveAt(i);
            i--;
        }
    }
}

void wxGxSelection::Reset()
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    m_DoArray.Clear();
    m_bDoOp = false;
    m_nPos = wxNOT_FOUND;
}

size_t wxGxSelection::GetDoSize()
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    return m_DoArray.GetCount();
}

long wxGxSelection::GetDoID(size_t nIndex)
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(nIndex > m_DoArray.GetCount() - 1)
        return wxNOT_FOUND;
    return m_DoArray[nIndex];
}
