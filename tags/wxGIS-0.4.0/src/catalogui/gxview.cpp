/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxView class.
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
#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/catalog/gxdiscconnections.h"

//-----------------------------------------------
// wxGxView
//-----------------------------------------------

wxGxView::wxGxView(void)
{
    m_Icon = wxNullIcon;
}

wxGxView::~wxGxView(void)
{
}

bool wxGxView::Activate(IFrameApplication* application, wxXmlNode* pConf)
{ 
	m_pGxApplication = dynamic_cast<IGxApplication*>(application); 
	if(!m_pGxApplication)
		return false;
	//m_pCatalog = m_pApplication->GetCatalog(); 
	m_pXmlConf = pConf;
	return true; 
}

void wxGxView::Deactivate(void)
{
	m_pGxApplication = NULL;
	//m_pCatalog = NULL;
	m_pXmlConf = NULL;
}

bool wxGxView::Applies(IGxSelection* Selection)
{ 
	return NULL == Selection ? false : true; 
};

wxString wxGxView::GetViewName(void)
{
	return m_sViewName;
}

wxIcon wxGxView::GetViewIcon(void)
{
    return m_Icon;
}

void wxGxView::SetViewIcon(wxIcon Icon)
{
    m_Icon = Icon;
}

int GxObjectCompareFunction(IGxObject* pObject1, IGxObject* pObject2, long sortData)
{
	IGxObjectSort* pGxObjectSort1 = dynamic_cast<IGxObjectSort*>(pObject1);
    IGxObjectSort* pGxObjectSort2 = dynamic_cast<IGxObjectSort*>(pObject2);
    if(pGxObjectSort1 && !pGxObjectSort2)
		return sortData == 0 ? 1 : -1;
    if(!pGxObjectSort1 && pGxObjectSort2)
		return sortData == 0 ? -1 : 1;
    if(pGxObjectSort1 && pGxObjectSort2)
    {
        bool bAlwaysTop1 = pGxObjectSort1->IsAlwaysTop();
        bool bAlwaysTop2 = pGxObjectSort2->IsAlwaysTop();
        if(bAlwaysTop1 && !bAlwaysTop2)
		    return sortData == 0 ? 1 : -1;
        if(!bAlwaysTop1 && bAlwaysTop2)
		    return sortData == 0 ? -1 : 1;
        bool bSortEnables1 = pGxObjectSort1->IsSortEnabled();
        bool bSortEnables2 = pGxObjectSort2->IsSortEnabled();
        if(!bSortEnables1 || !bSortEnables1)
            return 0;
    }

	bool bDiscConnection1 = dynamic_cast<wxGxDiscConnection*>(pObject1) != NULL;
    bool bDiscConnection2 = dynamic_cast<wxGxDiscConnection*>(pObject2) != NULL;
    if(bDiscConnection1 && !bDiscConnection2)
		return sortData == 0 ? 1 : -1;
    if(!bDiscConnection1 && bDiscConnection2)
		return sortData == 0 ? -1 : 1;
    if(bDiscConnection1 && bDiscConnection2)
    {
        bool bShare1 = pObject1->GetName().Left(2) == wxT("\\\\");
        bool bShare2 = pObject2->GetName().Left(2) == wxT("\\\\");
        if(bShare1 && !bShare2)
            return 1;
        if(!bShare1 && bShare2)
            return 0;
    }

	bool bContainerDst1 = dynamic_cast<IGxDataset*>(pObject1) != NULL;
    bool bContainerDst2 = dynamic_cast<IGxDataset*>(pObject2) != NULL;
	bool bContainer1 = dynamic_cast<IGxObjectContainer*>(pObject1) != NULL;
    bool bContainer2 = dynamic_cast<IGxObjectContainer*>(pObject2) != NULL;
    if(bContainer1 && !bContainerDst1 && bContainerDst2)
	    return sortData == 0 ? 1 : -1;
    if(bContainer2 && !bContainerDst2 && bContainerDst1)
	    return sortData == 0 ? -1 : 1;
    if(bContainer1 && !bContainer2)
	    return sortData == 0 ? 1 : -1;
    if(!bContainer1 && bContainer2)
	    return sortData == 0 ? -1 : 1;

	return pObject1->GetName().CmpNoCase(pObject2->GetName()) * (sortData == 0 ? -1 : 1);
}