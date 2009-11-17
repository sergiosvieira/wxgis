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

