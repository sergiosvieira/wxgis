/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISQuadTree class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/datasource/gdalinh.h"

#include "wx/thread.h"
#include "cpl_quad_tree.h"
#include <list>

class WXDLLIMPEXP_GIS_DS wxGISFeatureDataset;

/** \class wxGISQuadTreeItem quadtree.h
    \brief The class to represent geometry and OID in QuadTree.
*/

class WXDLLIMPEXP_GIS_DS wxGISQuadTreeItem
{
public:
	wxGISQuadTreeItem(const wxGISGeometry &oGeom, long nOID)
	{
		m_oGeom = oGeom;
		m_nOID = nOID;
	}

	wxGISQuadTreeItem(const OGREnvelope &Env, long nOID)
	{
		m_nOID = nOID;
        m_Env = Env;
	}

	virtual ~wxGISQuadTreeItem(void)
	{
	}

	virtual void FillBounds(CPLRectObj* pBounds)
	{
        if(!m_Env.IsInit())
        {
            if(!m_oGeom.IsOk())
			    return;
		    m_Env = m_oGeom.GetEnvelope();
		    if(IsDoubleEquil(m_Env.MinX, m_Env.MaxX))
			    m_Env.MaxX += DELTA;
		    if(IsDoubleEquil(m_Env.MinY, m_Env.MaxY))
			    m_Env.MaxY += DELTA;
        }

		pBounds->minx = m_Env.MinX;
		pBounds->maxx = m_Env.MaxX;
		pBounds->miny = m_Env.MinY;
		pBounds->maxy = m_Env.MaxY;

	}
	virtual long GetOID(void){ return m_nOID; };
	virtual wxGISGeometry GetGeometry(void) const { return m_oGeom; };
	virtual void SetGeometry(const wxGISGeometry &oGeom) { m_oGeom = oGeom; };
protected:
	wxGISGeometry m_oGeom;
    OGREnvelope m_Env;
	long m_nOID;
};

void GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds);

class wxGISQuadTree;
/** \class wxGISQuadTreeCursor quadtree.h
    \brief The class to store Search results of wxGISQuadTree.
*/

class WXDLLIMPEXP_GIS_DS wxGISQuadTreeCursor : public wxObject
{
    DECLARE_CLASS(wxGISQuadTreeCursor)
	friend class wxGISQuadTree;
public:
	wxGISQuadTreeCursor(void);
	wxGISQuadTreeCursor(wxGISQuadTreeItem** ppTreeItems, int nCount);
	virtual ~wxGISQuadTreeCursor(void);

    bool IsOk() const;

    bool operator == ( const wxGISQuadTreeCursor& obj ) const;
    bool operator != (const wxGISQuadTreeCursor& obj) const { return !(*this == obj); };

	virtual int GetCount(void) const;
	virtual wxGISQuadTreeItem* operator [](size_t nIndex) const;
	virtual wxGISQuadTreeItem* at(size_t nIndex) const;
	virtual wxGISQuadTreeItem* const Next(void);
	virtual void Reset(void);
	virtual void DeleteItem(size_t nIndex);
protected:
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;    
protected:
	int m_nCurrentItem;
};

/** \class wxGISQuadTreeCursorRefData quadtree.h
    \brief The reference data class for wxGISQuadTreeCursor
*/

class wxGISQuadTreeCursorRefData : public wxObjectRefData
{
    friend class wxGISQuadTreeCursor;
public:
    wxGISQuadTreeCursorRefData(wxGISQuadTreeItem** ppTreeItems, int nCount)
    {        
        m_pData = ppTreeItems;
        m_nItemCount = nCount;
    }

    wxGISQuadTreeCursorRefData()
    {        
        m_pData = NULL;
        m_nItemCount = 0;
    }

    virtual ~wxGISQuadTreeCursorRefData(void)
    {
        if(m_pData)
            CPLFree(m_pData);
    }

    virtual int GetCount(void) const
    {
        return m_nItemCount;
    }

	virtual wxGISQuadTreeItem* const operator [](size_t nIndex)
    {
	    if(!IsValid())
		    return NULL;
	    if(nIndex >= GetCount())
		    return NULL;
	    return m_pData[nIndex];
    }

	virtual void DeleteItem(size_t nIndex)
    {
        m_pData[nIndex] = NULL;
    }

    wxGISQuadTreeCursorRefData( const wxGISQuadTreeCursorRefData& data )
        : wxObjectRefData()
    {
        m_pData = data.m_pData;
        m_nItemCount = data.m_nItemCount;
    }

    bool operator == (const wxGISQuadTreeCursorRefData& data) const
    {
        wxCHECK_MSG(m_pData && data.m_pData, false, wxT("m_pData or data.m_pData is null"));
        return m_pData == data.m_pData && m_nItemCount == data.m_nItemCount;
    }

    virtual bool IsValid(void) const {return m_pData != NULL;};

protected:
	wxGISQuadTreeItem** m_pData;
	int m_nItemCount;
}; 

extern WXDLLIMPEXP_DATA_GIS_DS(wxGISQuadTreeCursor) wxNullQuadTreeCursor;

/** \class wxGISQuadTreeFillThread quadtree.h
    \brief The wxGIS QuadTree fill data thread.
*/
class wxGISQuadTreeFillThread : public wxThread
{
public:
	wxGISQuadTreeFillThread(wxGISQuadTree* pQuadTree, ITrackCancel * const pTrackCancel = NULL);
    virtual void *Entry();
    virtual void OnExit();
private:
    ITrackCancel* const m_pTrackCancel;
    wxGISQuadTree* m_pQuadTree;
};


/** \class wxGISQuadTree quadtree.h
    \brief The wxGIS QuadTree representation.
*/

class WXDLLIMPEXP_GIS_DS wxGISQuadTree
{
    friend class wxGISQuadTreeFillThread;
public:
	wxGISQuadTree(wxGISFeatureDataset* pDSet);
	virtual ~wxGISQuadTree(void);
	virtual wxGISQuadTreeCursor Search(const CPLRectObj* pAoi = 0);
    virtual bool Load(ITrackCancel* const pTrackCancel);
    virtual bool IsLoading(void) const;
    void DestroyLoadGeometryThread(void);
protected:    
    virtual void AddItem(wxGISQuadTreeItem* pItem);
    virtual void OnThreadExit();
    bool CreateAndRunLoadGeometryThread(void);
    void DestroyQuadTree();
    wxGISFeatureDataset* GetDataset() const {return m_pDSet;};
    short GetPreloadItemCount(void) const {return m_nPreloadItemCount;};
    void CreateQuadTree(void);
    void QueueEvent(wxEvent *event);
    long GetReadPos(void) const {return m_nReadPos;};
    void SetReadPos(long nReadPos) {m_nReadPos = nReadPos;};
protected:
    CPLQuadTree* m_pQuadTree;
	std::list<wxGISQuadTreeItem*> m_QuadTreeItemList;
	OGREnvelope m_Envelope;
	wxCriticalSection m_CritSect;
    wxGISFeatureDataset* m_pDSet;
    ITrackCancel* m_pTrackCancel;

    short m_nPreloadItemCount;

    wxGISQuadTreeFillThread* m_pThread;

    long m_nReadPos;
};
