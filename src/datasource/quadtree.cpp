/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISQuadTree class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/datasource/quadtree.h"

/*
void GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds)
{
	wxGISQuadTreeItem* pQuadTreeItem = (wxGISQuadTreeItem*)hFeature;
    if(!pQuadTreeItem)
		return;
	return pQuadTreeItem->FillBounds(pBounds);
}

wxGISQuadTree::wxGISQuadTree(const OGREnvelope &Env)
{
	m_Envelope = Env;
    CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
    m_pQuadTree = CPLQuadTreeCreate(&Rect, GetGeometryBoundsFunc);
}

wxGISQuadTree::~wxGISQuadTree(void)
{
	if(m_pQuadTree)
    {
		CPLQuadTreeDestroy(m_pQuadTree);
        m_pQuadTree = NULL;
    }
	for(std::list<wxGISQuadTreeItem*>::iterator it = m_QuadTreeItemList.begin(); it != m_QuadTreeItemList.end(); ++it)
		wxDELETE(*it);
}

void wxGISQuadTree::AddItem(OGRGeometry* pGeom, long nOID, bool bOwnGeometry)
{
	wxCriticalSectionLocker locker(m_CritSect);
	wxGISQuadTreeItem* pItem = new wxGISQuadTreeItem(pGeom, nOID, bOwnGeometry);
	CPLQuadTreeInsert(m_pQuadTree, (void*)pItem);
	m_QuadTreeItemList.push_back(pItem);
}

wxGISQuadTreeCursorSPtr wxGISQuadTree::Search(const CPLRectObj* pAoi)
{
	wxCriticalSectionLocker locker(m_CritSect);
    bool bContains(false);
	if(pAoi)
	{
		OGREnvelope pInputEnv;
		pInputEnv.MaxX = pAoi->maxx;
		pInputEnv.MaxY = pAoi->maxy;
		pInputEnv.MinX = pAoi->minx;
		pInputEnv.MinY = pAoi->miny;

		if(!IsDoubleEquil(m_Envelope.MaxX, pInputEnv.MaxX) || !IsDoubleEquil(m_Envelope.MaxY, pInputEnv.MaxY) || !IsDoubleEquil(m_Envelope.MinX, pInputEnv.MinX) || !IsDoubleEquil(m_Envelope.MinY, pInputEnv.MinY))
			bContains = m_Envelope.Contains(pInputEnv) != 0;
	}

	wxGISQuadTreeCursorSPtr pResult = boost::make_shared<wxGISQuadTreeCursor>();
	if(bContains)
	{
		pResult->m_pData = (wxGISQuadTreeItem**)CPLQuadTreeSearch(m_pQuadTree, pAoi, &pResult->m_nItemCount);
		pResult->Reset();
		return pResult;
	}
	else
	{
		pResult->m_pData = (wxGISQuadTreeItem**)CPLMalloc(sizeof(wxGISQuadTreeItem*) * m_QuadTreeItemList.size());
		pResult->m_nItemCount = m_QuadTreeItemList.size();
		pResult->m_nCurrentItem = 0;
		for(std::list<wxGISQuadTreeItem*>::iterator it = m_QuadTreeItemList.begin(); it != m_QuadTreeItemList.end(); ++it)
		{
			pResult->m_pData[pResult->m_nCurrentItem] = *it;
			pResult->m_nCurrentItem++;
		}
		pResult->Reset();
		return pResult;
	}
}
*/