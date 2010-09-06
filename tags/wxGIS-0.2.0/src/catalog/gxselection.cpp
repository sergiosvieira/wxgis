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
#include "wxgis/catalog/gxselection.h"

wxGxSelection::wxGxSelection(void) : m_Pos(-1), m_bDoOp(false), m_currentInitiator(-1)
{
}

wxGxSelection::~wxGxSelection(void)
{
	for(std::map<long, GxObjectArray*>::iterator CI = m_SelectionMap.begin(); CI != m_SelectionMap.end(); ++CI)
		wxDELETE(CI->second);
}

long wxGxSelection::Advise(wxObject* pObject)
{
	IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(pObject);
	if(pGxSelectionEvents == NULL)
		return -1;
	return IConnectionPointContainer::Advise(pObject);
}

void wxGxSelection::Select( IGxObject* pObject,  bool appendToExistingSelection, long nInitiator)
{
    //test if already selected
    m_CritSect.Enter();
    if(m_SelectionMap[nInitiator] == NULL)
        m_SelectionMap[nInitiator] = new GxObjectArray;

    if(m_SelectionMap[nInitiator]->size() > 0 && m_SelectionMap[nInitiator]->at(m_SelectionMap[nInitiator]->size() - 1) == pObject)
    {
        m_CritSect.Leave();
	    //not fire event id NOTFIRESELID
	    if(nInitiator != NOTFIRESELID)
            goto END;
        return;
    }
    m_currentInitiator = nInitiator;

    m_CritSect.Leave();

    if(!appendToExistingSelection)
	    Clear(nInitiator);

	m_CritSect.Enter();
    if(m_SelectionMap[nInitiator] == NULL)
	    m_SelectionMap[nInitiator] = new GxObjectArray;
    m_SelectionMap[nInitiator]->push_back(pObject);
    m_CritSect.Leave();

	//not fire event id NOTFIRESELID
	if(nInitiator == NOTFIRESELID)
		return;

    //
    Do(pObject);

	//fire event
	//wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
END:
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(m_pPointsArray[i]);
		if(pGxSelectionEvents != NULL)
			pGxSelectionEvents->OnSelectionChanged(this, nInitiator);
	}
}

void wxGxSelection::SetInitiator(long nInitiator)
{
    wxCriticalSectionLocker locker(m_CritSect);
	m_currentInitiator = nInitiator;
}

void wxGxSelection::Select( IGxObject* pObject)
{
    m_CritSect.Enter();
	m_currentInitiator = INIT_ALL;
	Clear(INIT_ALL);

	if(m_SelectionMap[INIT_ALL] == NULL)
		m_SelectionMap[INIT_ALL] = new GxObjectArray;
	m_SelectionMap[INIT_ALL]->push_back(pObject);
    m_CritSect.Leave();

	//fire event
	//wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(m_pPointsArray[i]);
		if(pGxSelectionEvents != NULL)
			pGxSelectionEvents->OnSelectionChanged(this, INIT_ALL);
	}
}

void wxGxSelection::Unselect(IGxObject* pObject, long nInitiator)
{
	if(pObject == NULL)
		return;

    GxObjectArray::iterator pos;

    m_CritSect.Enter();
	if(nInitiator == INIT_ALL)
	{
		for(std::map<long, GxObjectArray*>::const_iterator CI = m_SelectionMap.begin(); CI != m_SelectionMap.end(); ++CI)
		{
			GxObjectArray::iterator pos = std::find(CI->second->begin(), CI->second->end(), pObject);
			if(pos != CI->second->end())
				CI->second->erase(pos);
		}
	}

    if(m_SelectionMap[nInitiator] == NULL)
    {
        m_CritSect.Leave();
		return;
    }

	pos = std::find(m_SelectionMap[nInitiator]->begin(), m_SelectionMap[nInitiator]->end(), pObject);
	if(pos != m_SelectionMap[nInitiator]->end())
		m_SelectionMap[nInitiator]->erase(pos);
    m_CritSect.Leave();


    wxCriticalSectionLocker locker(m_DoCritSect);
    for(size_t i = 0; i < m_DoArray.GetCount(); i++)
    {
        if(m_DoArray[i] == pObject->GetFullName())
        {
            m_DoArray.RemoveAt(i);
            i--;
        }
    }
    		//while((pos = std::find(m_DoArray.begin(), m_DoArray.end(), pObject)) != m_DoArray.end())
  //      {
  //          if(m_Pos > m_DoArray.begin() - pos)
  //              m_Pos--;
		//	m_DoArray.erase(pos);
  //      }


	////not fire event id NOTFIRESELID
	//if(nInitiator == NOTFIRESELID)
	//	return;

	////fire event
	//wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	//for(size_t i = 0; i < m_pPointsArray.size(); i++)
	//{
	//	IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(m_pPointsArray[i]);
	//	if(pGxSelectionEvents != NULL)
	//		pGxSelectionEvents->OnSelectionChanged(this, nInitiator);
	//}
}

void wxGxSelection::Clear(long nInitiator)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(nInitiator == INIT_ALL)
	{
		for(std::map<long, GxObjectArray*>::const_iterator CI = m_SelectionMap.begin(); CI != m_SelectionMap.end(); ++CI)
            if(CI->second)
                CI->second->clear();
		return;
	}
	if(m_SelectionMap[nInitiator] != NULL)
		m_SelectionMap[nInitiator]->clear();
	//fire event
	//wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	//for(size_t i = 0; i < m_pPointsArray.size(); i++)
	//{
	//	IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(m_pPointsArray[i]);
	//	if(pGxSelectionEvents != NULL)
	//		pGxSelectionEvents->OnSelectionChanged(this, nInitiator);
	//}
}

size_t wxGxSelection::GetCount(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(m_currentInitiator == INIT_NONE || m_SelectionMap[m_currentInitiator] == NULL)
		return 0;
	else
		return m_SelectionMap[m_currentInitiator]->size();
}

size_t wxGxSelection::GetCount(long nInitiator)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(m_SelectionMap[nInitiator] == NULL)
		return 0;
	else
		return m_SelectionMap[nInitiator]->size();
}


IGxObject* wxGxSelection::GetLastSelectedObject(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(m_currentInitiator == INIT_NONE || m_SelectionMap[m_currentInitiator] == NULL)
		return NULL;
    if(m_SelectionMap[m_currentInitiator]->size() == 0)
		return NULL;

    return m_SelectionMap[m_currentInitiator]->at(m_SelectionMap[m_currentInitiator]->size() - 1);
}

IGxObject* wxGxSelection::GetSelectedObjects(size_t nIndex)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(m_currentInitiator == INIT_NONE || m_SelectionMap[m_currentInitiator] == NULL)
		return NULL;

    if(!m_SelectionMap[m_currentInitiator])
        return NULL;

    if(m_SelectionMap[m_currentInitiator]->size() == 0)
        return NULL;

    if(m_SelectionMap[m_currentInitiator]->size() - 1 >= nIndex)
        return m_SelectionMap[m_currentInitiator]->at(nIndex);

    return NULL;
}

IGxObject* wxGxSelection::GetSelectedObjects(long nInitiator, size_t nIndex)
{
   wxCriticalSectionLocker locker(m_CritSect);
   if(m_SelectionMap[nInitiator])
   {
        if(m_SelectionMap[m_currentInitiator]->size() == 0)
            return NULL;
        if(m_SelectionMap[nInitiator]->size() - 1 >= nIndex)
            return m_SelectionMap[nInitiator]->at(nIndex);
   }
    else
        return NULL;
}

void wxGxSelection::Do(IGxObject* pObject)
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(m_pPrevObject == pObject)
        return;

    m_pPrevObject = pObject;

    if(m_bDoOp || pObject == NULL)
    {
        m_bDoOp = false;
        return;
    }

    if(!m_DoArray.IsEmpty())
        if(m_DoArray.Last() == pObject->GetFullName())
            return;
    m_Pos++;
    if(m_Pos == m_DoArray.GetCount())
        m_DoArray.Add(pObject->GetFullName());
    else if(m_Pos > m_DoArray.GetCount())
    {
        m_DoArray.Add(pObject->GetFullName());
        m_Pos = m_DoArray.GetCount() - 1;
    }
    else
    {
	    m_DoArray[m_Pos] = pObject->GetFullName();
	    m_DoArray.RemoveAt(m_Pos + 1, m_DoArray.GetCount() - (m_Pos + 1));
    }
    //Select(pObject);
}

bool wxGxSelection::CanRedo()
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(m_DoArray.IsEmpty())
	    return false;
    return m_Pos < m_DoArray.GetCount() - 1;
}

bool wxGxSelection::CanUndo()
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(m_DoArray.IsEmpty())
	    return false;
    return m_Pos > 0;
}

wxString wxGxSelection::Redo(int nPos)
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(nPos == -1)
        m_Pos++;
    else
        m_Pos = nPos;
    if(m_Pos < m_DoArray.size())
    {
        m_bDoOp = true;
        return m_DoArray[m_Pos];
	    //IGxObject* pObject = m_DoArray[m_Pos];
	    //Select(pObject);
  //      m_bDoOp = false;
    }
    return wxEmptyString;
}

wxString wxGxSelection::Undo(int nPos)
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(nPos == -1)
        m_Pos--;
    else
        m_Pos = nPos;
    if(m_Pos > -1)
    {
        m_bDoOp = true;
        return m_DoArray[m_Pos];
	    //IGxObject* pObject = m_DoArray[m_Pos];
	    //Select(pObject);
  //      m_bDoOp = false;
    }
    return wxEmptyString;
}

void wxGxSelection::RemoveDo(wxString sPath)
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    for(size_t i = 0; i < m_DoArray.size(); i++)
    {
        //clean from doubles
        if(i > 0 && m_DoArray[i - 1] == m_DoArray[i])
        {
            m_DoArray.RemoveAt(i);
            i--;
        }
        if(m_DoArray[i] == sPath)
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
    m_Pos = -1;
}

size_t wxGxSelection::GetDoSize()
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    return m_DoArray.GetCount();
}

wxString wxGxSelection::GetDoPath(size_t nIndex)
{
    wxCriticalSectionLocker locker(m_DoCritSect);
    if(nIndex > m_DoArray.GetCount() - 1)
        return wxEmptyString;
    return m_DoArray[nIndex];
}
