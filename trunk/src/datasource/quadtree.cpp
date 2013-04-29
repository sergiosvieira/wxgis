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
// wxGISQuadTree
//-----------------------------------------------------------------------------

wxGISQuadTree::wxGISQuadTree(wxGISFeatureDataset* pDSet) : wxThreadHelper()
{
    m_pDSet = pDSet;
    m_nPreloadItemCount = PRELOAD_GEOM_COUNT;
    //if(m_pDSet)
        //m_szPath = (char*)CPLResetExtension(m_pDSet->GetPath(), "sif");
}

wxGISQuadTree::~wxGISQuadTree(void)
{
    DestroyLoadGeometryThread();

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

    if (GetThread() && GetThread()->IsRunning())
        return wxGISQuadTreeCursor();

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

	if(bContains)
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
    m_pTrackCancel = pTrackCancel;
    if(m_pTrackCancel)
	{
        m_pProgress = m_pTrackCancel->GetProgressor();
    }
    else
    {
        m_pProgress = NULL;
    }
    //create quad tree
    if(m_pDSet != NULL)
    {
        return CreateAndRunLoadGeometryThread();

/*            m_Envelope = m_pDSet->GetEnvelope();
        IProgressor* pProgress(NULL);
	    if(pTrackCancel)
	    {
		    pTrackCancel->Reset();
		    pTrackCancel->PutMessage(wxString(_("PreLoad Geometry of ")) + m_pDSet->GetName(), -1, enumGISMessageInfo);
		    pProgress = pTrackCancel->GetProgressor();
		    if(pProgress)
                pProgress->ShowProgress(true);
	    }            
            
        CPLRectObj Rect = {m_Envelope.MinX, m_Envelope.MinY, m_Envelope.MaxX, m_Envelope.MaxY};
        m_pQuadTree = CPLQuadTreeCreate(&Rect, GetGeometryBoundsFunc);

        if(pProgress)
            pProgress->SetRange(m_pDSet->GetFeatureCount(TRUE, pTrackCancel));

        int nCounter(0);
        m_pDSet->Reset();
        wxGISFeature Feature;
        while((Feature = m_pDSet->Next()).IsOk())
        {
            if(pProgress)
                pProgress->SetValue(nCounter++);
            //check if Feature will destroy by Ref Count
            wxGISQuadTreeItem* pItem = NULL;
            if(Feature.GetRefData()->GetRefCount() > 1)
                pItem = new wxGISQuadTreeItem(Feature.GetGeometry(), Feature.GetFID());//appologise the feature is buffered in m_pDSet
            else
                pItem = new wxGISQuadTreeItem(Feature.GetGeometry().Copy(), Feature.GetFID());
            AddItem(pItem);

            //TODO: wxFeatureDSEvent event(wxFEATURES_ADDED);
            //TODO: m_pDSet->PostEvent(event);
        }

        if(pProgress)
            pProgress->ShowProgress(false);

	    if(pTrackCancel)
	    {
		    pTrackCancel->PutMessage(_("Done"), -1, enumGISMessageInfo);
        }
            
        return true;*/
    }
    else
    {
        m_pQuadTree = NULL;            
    }
    return false;
}

wxThread::ExitCode wxGISQuadTree::Entry()
{
    m_Envelope = m_pDSet->GetEnvelope();

	if(m_pTrackCancel)
	{
		m_pTrackCancel->Reset();
		m_pTrackCancel->PutMessage(wxString(_("PreLoad Geometry of ")) + m_pDSet->GetName(), -1, enumGISMessageInfo);
	}  

    CPLRectObj Rect = {m_Envelope.MinX, m_Envelope.MinY, m_Envelope.MaxX, m_Envelope.MaxY};
    m_pQuadTree = CPLQuadTreeCreate(&Rect, GetGeometryBoundsFunc);

    size_t nSize = sizeof(wxGISQuadTreeItem*) * m_nPreloadItemCount;
    wxGISQuadTreeItem** ppData = (wxGISQuadTreeItem**)CPLMalloc(nSize);
    RtlZeroMemory(ppData, nSize);

    if(GetThread()->TestDestroy())//(m_pTrackCancel && !m_pTrackCancel->Continue()) || 
    {
	    if(m_pTrackCancel)
	    {
		    m_pTrackCancel->PutMessage(_("Cancel"), -1, enumGISMessageInfo);
        }
        m_pTrackCancel = NULL;
        return (wxThread::ExitCode)0;     // success
    }

    int nCounter(0), nItemCounter(0);
    //get only geometries
    wxArrayString saIgnoredFields;
    OGRFeatureDefn * const pDef = m_pDSet->GetDefinition();
    if(pDef)
    {
        for(size_t i = 0; i < pDef->GetFieldCount(); ++i)
        {
            saIgnoredFields.Add(wxString(pDef->GetFieldDefn(i)->GetNameRef(), wxConvUTF8));
        }
    }
    saIgnoredFields.Add(wxT("OGR_STYLE"));

	if(m_pProgress)
    {
        int nRange = m_pDSet->GetFeatureCount(TRUE, m_pTrackCancel);
        m_pProgress->SetRange(nRange);
        m_pProgress->ShowProgress(true);
    } 

    m_pDSet->Reset();
    m_pDSet->SetIgnoredFields(saIgnoredFields);
    wxGISFeature Feature;
    while((Feature = m_pDSet->Next()).IsOk())
    {
        //check if Feature will destroy by Ref Count
        wxGISQuadTreeItem* pItem = NULL;
        if(Feature.GetRefData()->GetRefCount() > 1)
            pItem = new wxGISQuadTreeItem(Feature.GetGeometry(), Feature.GetFID());//appologise the feature is buffered in m_pDSet
        else
            pItem = new wxGISQuadTreeItem(Feature.GetGeometry().Copy(), Feature.GetFID());
        
        CPLQuadTreeInsert(m_pQuadTree, (void*)pItem);
        m_QuadTreeItemList.push_back(pItem);

        ppData[nItemCounter++] = pItem;

        if(nItemCounter == m_nPreloadItemCount)
        {
             m_pDSet->QueueEvent(new wxFeatureDSEvent(wxDS_FEATURES_ADDED, wxGISQuadTreeCursor(ppData, nItemCounter)));
             nItemCounter = 0;
             ppData = (wxGISQuadTreeItem**)CPLMalloc(nSize);
             RtlZeroMemory(ppData, nSize);
        }

        if(m_pProgress)
            m_pProgress->SetValue(nCounter++);

        //bool bCancel = m_pTrackCancel && !m_pTrackCancel->Continue();bCancel || 

        if( GetThread()->TestDestroy())
        {
	        if(m_pTrackCancel)
	        {
		        m_pTrackCancel->PutMessage(_("Cancel"), -1, enumGISMessageInfo);
            }
            m_pTrackCancel = NULL;
            return (wxThread::ExitCode)0;     // success
        }

    }

    if(nItemCounter > 0)
    {
        m_pDSet->QueueEvent(new wxFeatureDSEvent(wxDS_FEATURES_ADDED, wxGISQuadTreeCursor(ppData, nItemCounter))); 
    }

    if(m_pProgress)
        m_pProgress->ShowProgress(false);

	if(m_pTrackCancel)
	{
		m_pTrackCancel->PutMessage(_("Done"), -1, enumGISMessageInfo);
    }

    saIgnoredFields.Clear();
    m_pDSet->SetIgnoredFields(saIgnoredFields);

    m_pTrackCancel = NULL;

	return (wxThread::ExitCode)0;     // success
}

bool wxGISQuadTree::CreateAndRunLoadGeometryThread(void)
{
    if(!GetThread())
    {
        if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("Could not create the thread!"));
            return false;
        }
    }

    if(GetThread()->IsRunning())
        return true;

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }

    return true;
}

void wxGISQuadTree::DestroyLoadGeometryThread(void)
{
    //if(m_pTrackCancel)
 	//{
	//	m_pTrackCancel->Cancel();
	//}

    if(m_pProgress)
        m_pProgress = NULL;

    wxThread* pThread = GetThread();
    if (pThread && pThread->IsRunning())
        pThread->Wait();
}
