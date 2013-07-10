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

#include "wxgis/datasource/quadtree.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/vectorop.h"

#define PRELOAD_GEOM_COUNT 2000

void GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds)
{
	wxGISQuadTreeItem* pQuadTreeItem = (wxGISQuadTreeItem*)hFeature;
    if(!pQuadTreeItem)
		return;
	return pQuadTreeItem->FillBounds(pBounds);
}

//-----------------------------------------------------------------------------
// wxGISQuadTreeCursor
//-----------------------------------------------------------------------------

wxGISQuadTreeCursor wxNullQuadTreeCursor;

IMPLEMENT_CLASS(wxGISQuadTreeCursor, wxObject)

wxGISQuadTreeCursor::wxGISQuadTreeCursor() : wxObject()
{
    m_nCurrentItem = 0;
    m_refData = new wxGISQuadTreeCursorRefData();
}

wxGISQuadTreeCursor::wxGISQuadTreeCursor(wxGISQuadTreeItem** ppTreeItems, int nCount)
{
    m_nCurrentItem = 0;
    m_refData = new wxGISQuadTreeCursorRefData(ppTreeItems, nCount);
}


wxGISQuadTreeCursor::~wxGISQuadTreeCursor()
{
}

wxObjectRefData *wxGISQuadTreeCursor::CreateRefData() const
{
    return new wxGISQuadTreeCursorRefData();
}

wxObjectRefData *wxGISQuadTreeCursor::CloneRefData(const wxObjectRefData *data) const
{
    return new wxGISQuadTreeCursorRefData(*(wxGISQuadTreeCursorRefData *)data);
}

bool wxGISQuadTreeCursor::IsOk() const
{ 
    return m_refData != NULL && ((wxGISQuadTreeCursorRefData *)m_refData)->IsValid();//; 
}

bool wxGISQuadTreeCursor::operator == ( const wxGISQuadTreeCursor& obj ) const
{
    if (m_refData == obj.m_refData)
        return true;
    if (!m_refData || !obj.m_refData)
        return false;

    return ( *(wxGISQuadTreeCursorRefData*)m_refData == *(wxGISQuadTreeCursorRefData*)obj.m_refData );
}

int wxGISQuadTreeCursor::GetCount(void) const
{
    return ((wxGISQuadTreeCursorRefData *)m_refData)->GetCount();
}

wxGISQuadTreeItem* wxGISQuadTreeCursor::operator [](size_t nIndex) const
{
    return ((wxGISQuadTreeCursorRefData *)m_refData)->operator [](nIndex);
}

wxGISQuadTreeItem* wxGISQuadTreeCursor::at(size_t nIndex) const
{
	return operator [](nIndex);
}

wxGISQuadTreeItem* const wxGISQuadTreeCursor::Next(void)
{
	if(!((wxGISQuadTreeCursorRefData *)m_refData)->IsValid())
		return NULL;

    if(m_nCurrentItem == GetCount())
		return NULL;
	return ((wxGISQuadTreeCursorRefData *)m_refData)->m_pData[m_nCurrentItem++];
}
	
void wxGISQuadTreeCursor::Reset(void)
{
    m_nCurrentItem = 0;
}

void wxGISQuadTreeCursor::DeleteItem(size_t nIndex)
{
	((wxGISQuadTreeCursorRefData *)m_refData)->DeleteItem(nIndex);
}

//-----------------------------------------------------------------------------
// wxGISQuadTreeFillThread
//-----------------------------------------------------------------------------

wxGISQuadTreeFillThread::wxGISQuadTreeFillThread(wxGISQuadTree* pQuadTree, ITrackCancel * const pTrackCancel) : wxThread(wxTHREAD_DETACHED), m_pTrackCancel(pTrackCancel)
{
    m_pQuadTree = pQuadTree;
}

void *wxGISQuadTreeFillThread::Entry()
{
    if(!m_pQuadTree)
        return (wxThread::ExitCode)wxTHREAD_MISC_ERROR;

    m_pQuadTree->CreateQuadTree();
    long nFeaturesCount = m_pQuadTree->GetDataset()->GetFeatureCount(TRUE, m_pTrackCancel);
    if(nFeaturesCount == 0)
    {
        return (wxThread::ExitCode)wxTHREAD_NO_ERROR;     // success
    }

    IProgressor* pProgress(NULL);
	if(m_pTrackCancel)
	{
		m_pTrackCancel->Reset();
		m_pTrackCancel->PutMessage(wxString(_("PreLoad Geometry of ")) + m_pQuadTree->GetDataset()->GetName(), -1, enumGISMessageInfo);
        pProgress = m_pTrackCancel->GetProgressor();
	}  

    size_t nSize = sizeof(wxGISQuadTreeItem*) * m_pQuadTree->GetPreloadItemCount();
    wxGISQuadTreeItem** ppData = (wxGISQuadTreeItem**)CPLMalloc(nSize);
    RtlZeroMemory(ppData, nSize);

    if(TestDestroy())
    {
	    if(m_pTrackCancel)
	    {
		    m_pTrackCancel->PutMessage(_("Cancel"), -1, enumGISMessageInfo);
        }
        return (wxThread::ExitCode)wxTHREAD_NO_ERROR;     // success
    }

    long nCounter(0), nItemCounter(0);

    //get only geometries
    wxArrayString saIgnoredFields = m_pQuadTree->GetDataset()->GetFieldNames();
    saIgnoredFields.Add(wxT("OGR_STYLE"));

    long nReadPos = m_pQuadTree->GetReadPos();
	if(pProgress)
    {
        pProgress->SetRange(nFeaturesCount);
        pProgress->ShowProgress(true);
    } 

    m_pQuadTree->GetDataset()->Reset();
    m_pQuadTree->GetDataset()->SetIgnoredFields(saIgnoredFields);
    wxGISFeature Feature = m_pQuadTree->GetDataset()->GetFeature(nReadPos);
    while(Feature.IsOk())
    {
        //check if Feature will destroy by Ref Count
        wxGISQuadTreeItem* pItem = NULL;
        wxGISGeometry Geom = Feature.GetGeometry();
        if(Geom.IsOk())
        {
            if(Feature.GetRefData()->GetRefCount() > 1)
                pItem = new wxGISQuadTreeItem(Geom, Feature.GetFID());//appologise the feature is buffered in m_pDSet
            else
                pItem = new wxGISQuadTreeItem(Geom.Copy(), Feature.GetFID());
        
            m_pQuadTree->AddItem(pItem);

            ppData[nItemCounter++] = pItem;

            if(nItemCounter == m_pQuadTree->GetPreloadItemCount())
            {
                 m_pQuadTree->QueueEvent(new wxFeatureDSEvent(wxDS_FEATURES_ADDED, wxGISQuadTreeCursor(ppData, nItemCounter)));
                 nItemCounter = 0;
                 ppData = (wxGISQuadTreeItem**)CPLMalloc(nSize);
                 RtlZeroMemory(ppData, nSize);
            }
        }

        nCounter++;
        long nProc = nReadPos + nCounter;
        if(!TestDestroy() && pProgress)
            pProgress->SetValue(nProc);
        m_pQuadTree->SetReadPos(nProc);

        if( TestDestroy() )
        {
            saIgnoredFields.Clear();
            m_pQuadTree->GetDataset()->SetIgnoredFields(saIgnoredFields);

	        if(m_pTrackCancel)
	        {
		        m_pTrackCancel->PutMessage(_("Cancel"), -1, enumGISMessageInfo);
            }  

           	if(pProgress)
            {
                pProgress->ShowProgress(false);
            } 

            return (wxThread::ExitCode)wxTHREAD_NO_ERROR;     // success
        }

        Feature = m_pQuadTree->GetDataset()->Next();
    }

    if(nItemCounter > 0)
    {
        m_pQuadTree->QueueEvent(new wxFeatureDSEvent(wxDS_FEATURES_ADDED, wxGISQuadTreeCursor(ppData, nItemCounter))); 
    }

    if(pProgress)
    {
        pProgress->ShowProgress(false);
    }

	if(m_pTrackCancel)
	{
		m_pTrackCancel->PutMessage(_("Done"), -1, enumGISMessageInfo);
    }

    saIgnoredFields.Clear();
    m_pQuadTree->GetDataset()->SetIgnoredFields(saIgnoredFields);

    m_pQuadTree->GetDataset()->SetCached(nCounter == nFeaturesCount);

	return (wxThread::ExitCode)wxTHREAD_NO_ERROR;     // success
}

void wxGISQuadTreeFillThread::OnExit()
{
    m_pQuadTree->OnThreadExit();
}

//-----------------------------------------------------------------------------
// wxGISQuadTree
//-----------------------------------------------------------------------------

wxGISQuadTree::wxGISQuadTree(wxGISFeatureDataset* pDSet)
{
    m_pDSet = pDSet;
    m_nPreloadItemCount = PRELOAD_GEOM_COUNT;
    m_pQuadTree = NULL;
    m_pThread = NULL;
    m_nReadPos = 0;
}

wxGISQuadTree::~wxGISQuadTree(void)
{
    DestroyLoadGeometryThread();
    DestroyQuadTree();
}

void wxGISQuadTree::CreateQuadTree(void)
{
    m_Envelope = m_pDSet->GetEnvelope();
    CPLRectObj Rect = {m_Envelope.MinX, m_Envelope.MinY, m_Envelope.MaxX, m_Envelope.MaxY};
    m_pQuadTree = CPLQuadTreeCreate(&Rect, GetGeometryBoundsFunc);
}

void wxGISQuadTree::DestroyQuadTree()
{
	if(m_pQuadTree)
    {
		CPLQuadTreeDestroy(m_pQuadTree);
        m_pQuadTree = NULL;
    }
	for(std::list<wxGISQuadTreeItem*>::iterator it = m_QuadTreeItemList.begin(); it != m_QuadTreeItemList.end(); ++it)
		wxDELETE(*it);
}

void wxGISQuadTree::AddItem(wxGISQuadTreeItem* pItem)
{
	//wxCriticalSectionLocker locker(m_CritSect);
	CPLQuadTreeInsert(m_pQuadTree, (void*)pItem);
	m_QuadTreeItemList.push_back(pItem);
}

wxGISQuadTreeCursor wxGISQuadTree::Search(const CPLRectObj* pAoi)
{
	wxCriticalSectionLocker locker(m_CritSect);

    //TODO: test multithreading
    //if (IsLoading())
    //    return wxGISQuadTreeCursor();

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

	if(bContains && IsLoading())
	{
        int nItemCount(0);
		wxGISQuadTreeItem** ppData = (wxGISQuadTreeItem**)CPLQuadTreeSearch(m_pQuadTree, pAoi, &nItemCount);
		return wxGISQuadTreeCursor(ppData, nItemCount);
	}
	else
	{
        int nItemCount = m_QuadTreeItemList.size();
        int nCurrentItem = 0;
        size_t nSize = sizeof(wxGISQuadTreeItem*) * nItemCount;
		wxGISQuadTreeItem** ppData = (wxGISQuadTreeItem**)CPLMalloc(nSize);
        RtlZeroMemory(ppData, nSize );
		for(std::list<wxGISQuadTreeItem*>::iterator it = m_QuadTreeItemList.begin(); it != m_QuadTreeItemList.end(); ++it)
		{
			ppData[nCurrentItem++] = *it;
		}
		return wxGISQuadTreeCursor(ppData, nItemCount);
	}
}

bool wxGISQuadTree::Load(ITrackCancel* const pTrackCancel)
{   
    wxCriticalSectionLocker locker(m_CritSect);
    m_pTrackCancel = pTrackCancel;
    //create quad tree
    if(m_pDSet != NULL)
    {
        return CreateAndRunLoadGeometryThread();
    }
    else
    {
        m_pQuadTree = NULL;            
    }
    return false;
}

bool wxGISQuadTree::CreateAndRunLoadGeometryThread(void)
{
    if(IsLoading())
        return true;

    m_pThread = new wxGISQuadTreeFillThread(this, m_pTrackCancel);
	return CreateAndRunThread(m_pThread, wxT("wxGISQuadTreeFillThread"), wxT("QuadTreeFillThread"));
}

bool wxGISQuadTree::IsLoading(void) const
{
    if(m_pThread)
    {
        if(m_pThread->IsRunning())
            return true;
    }
    return false;
}

void wxGISQuadTree::DestroyLoadGeometryThread(void)
{
    if (m_pThread)
    {
        if (m_pThread->Delete() != wxTHREAD_NO_ERROR )
        {
            wxLogError("Can't delete the thread!");
        }
    }
}

void wxGISQuadTree::OnThreadExit()
{
    m_pTrackCancel = NULL;
    m_pThread = NULL;
}

void wxGISQuadTree::QueueEvent(wxEvent *event)
{
    m_pDSet->QueueEvent(event);
}
