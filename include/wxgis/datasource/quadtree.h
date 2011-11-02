/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISQuadTree class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#pragma once

#include "wxgis/datasource/datasource.h"

#include <list>

/** \class wxGISQuadTreeItem quadtree.h
    \brief The class to represent geometry and OID in QuadTree.
*/
class wxGISQuadTreeItem
{
public:
	wxGISQuadTreeItem(OGRGeometry* pGeom, long nOID, bool bOwnGeometry = false)
	{
		m_pGeom = pGeom;
		m_nOID = nOID;
		m_bOwnGeometry = bOwnGeometry;
	}
	virtual ~wxGISQuadTreeItem(void)
	{
		if(m_bOwnGeometry) 
			wxDELETE(m_pGeom);
	}
	virtual void FillBounds(CPLRectObj* pBounds)
	{
		if(!m_pGeom)
			return;
		OGREnvelope Env;
		m_pGeom->getEnvelope(&Env);
		if(IsDoubleEquil(Env.MinX, Env.MaxX))
			Env.MaxX += DELTA;
		if(IsDoubleEquil(Env.MinY, Env.MaxY))
			Env.MaxY += DELTA;

		pBounds->minx = Env.MinX;
		pBounds->maxx = Env.MaxX;
		pBounds->miny = Env.MinY;
		pBounds->maxy = Env.MaxY;
	}
	virtual long GetOID(void){ return m_nOID; };
	virtual OGRGeometry* GetGeometry(void){ return m_pGeom; };
protected:
	OGRGeometry* m_pGeom;
	long m_nOID;
	bool m_bOwnGeometry;
};

void GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds);

class wxGISQuadTree;
/** \class wxGISQuadTreeCursor quadtree.h
    \brief The class to store Search results of wxGISQuadTree.
*/
class wxGISQuadTreeCursor
{
	friend class wxGISQuadTree;
public:
	wxGISQuadTreeCursor(void) : m_nItemCount(0), m_pData(NULL){};
	virtual ~wxGISQuadTreeCursor(void){CPLFree(m_pData);};
	virtual int GetCount(void){return m_nItemCount;};
	virtual wxGISQuadTreeItem* const operator [](size_t nIndex)
	{
		if(!m_pData)
			return NULL;
		if(nIndex >= m_nItemCount)
			return NULL;
		return m_pData[nIndex];
	}
	virtual wxGISQuadTreeItem* const Next(void)
	{
		if(!m_pData)
			return NULL;
		if(m_nCurrentItem == m_nItemCount)
			return NULL;
		wxGISQuadTreeItem* pOut = m_pData[m_nCurrentItem];
		m_nCurrentItem++;
		return pOut;
	}
	virtual void Reset(void)
	{
		m_nCurrentItem = 0;
	}
protected:
	wxGISQuadTreeItem** m_pData;
	int m_nItemCount;
	int m_nCurrentItem;
};

DEFINE_SHARED_PTR(wxGISQuadTreeCursor);

/** \class wxGISQuadTree quadtree.h
    \brief The wxGIS QuadTree representation.
*/
class WXDLLIMPEXP_GIS_DS wxGISQuadTree
{
public:
	wxGISQuadTree(const OGREnvelope &Env);
	virtual ~wxGISQuadTree(void);
    virtual void AddItem(OGRGeometry* pGeom, long nOID, bool bOwnGeometry = false);
	virtual wxGISQuadTreeCursorSPtr Search(const CPLRectObj* pAoi = 0);
protected:
    CPLQuadTree* m_pQuadTree;
	std::list<wxGISQuadTreeItem*> m_QuadTreeItemList;
	OGREnvelope m_Envelope;
	wxCriticalSection m_CritSect;
};

DEFINE_SHARED_PTR(wxGISQuadTree);
