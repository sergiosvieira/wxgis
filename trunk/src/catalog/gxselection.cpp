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

wxGxSelection::wxGxSelection(void)
{
}

wxGxSelection::~wxGxSelection(void)
{
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
	IGxSelection::Select(pObject, appendToExistingSelection, nInitiator);

	//not fire event id NOTFIRESELID
	if(nInitiator == NOTFIRESELID)
		return;

	//fire event
	wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(m_pPointsArray[i]);
		if(pGxSelectionEvents != NULL)
			pGxSelectionEvents->OnSelectionChanged(this, nInitiator);
	}
}

void wxGxSelection::Unselect(IGxObject* pObject, long nInitiator)
{
	IGxSelection::Unselect(pObject, nInitiator);

	//not fire event id NOTFIRESELID
	if(nInitiator == NOTFIRESELID)
		return;

	//fire event
	wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(m_pPointsArray[i]);
		if(pGxSelectionEvents != NULL)
			pGxSelectionEvents->OnSelectionChanged(this, nInitiator);
	}
}

void wxGxSelection::Clear(long nInitiator)
{
	IGxSelection::Clear(nInitiator);
	//fire event
	wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxSelectionEvents* pGxSelectionEvents = dynamic_cast<IGxSelectionEvents*>(m_pPointsArray[i]);
		if(pGxSelectionEvents != NULL)
			pGxSelectionEvents->OnSelectionChanged(this, nInitiator);
	}
}

