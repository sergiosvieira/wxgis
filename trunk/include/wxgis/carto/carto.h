/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCarto main header.
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

#pragma once

#include "wxgis/display/display.h"
#include "wxgis/datasource/datasource.h"

enum wxGISEnumMapToolState
{
	enumGISMapNone = 0x0000, 
	enumGISMapPanning = 0x0001,
	enumGISMapZooming = 0x0002,
	enumGISMapRotating = 0x0004
};

class wxGISLayer 
{
public:
	wxGISLayer(void)
	{
		m_sName = wxString(_("new layer"));
		m_dMaxScale = -1;
		m_dMinScale = -1;
		m_bVisible = true;
		m_bCached = false;
        m_iCacheID = 0;
	}
	virtual~wxGISLayer(void){};
	//pure virtual
	virtual OGRSpatialReference* GetSpatialReference(void) = 0;
	virtual void SetSpatialReference(OGRSpatialReference* pSpatialReference) = 0;
	virtual const OGREnvelope* GetEnvelope(void) = 0;
	virtual bool IsValid(void) = 0;
	virtual void Draw(wxGISEnumDrawPhase DrawPhase, ICachedDisplay* pDisplay, ITrackCancel* pTrackCancel) = 0;
	//
	virtual void SetMaximumScale(double dMaxScale){m_dMaxScale = dMaxScale;};
	virtual double GetMaximumScale(void){return m_dMaxScale;};
	virtual void SetMinimumScale(double dMinScale){m_dMinScale = dMinScale;};
	virtual double GetMinimumScale(void){return m_dMinScale;};
	virtual bool GetVisible(void){return m_bVisible;};
	virtual void SetVisible(bool bVisible){m_bVisible = bVisible;};
	virtual void SetName(wxString sName){m_sName = sName;};
	virtual wxString GetName(wxString sName){return m_sName;};
	virtual size_t GetCacheID(void){return m_iCacheID;};
	virtual void SetCacheID(size_t iCacheID){m_iCacheID = iCacheID;};
	virtual bool GetCached(void){return m_bCached;};
	virtual void SetCached(bool bCached){m_bCached = bCached;};
protected:
	double m_dMaxScale, m_dMinScale;
	wxString m_sName;
	bool m_bVisible, m_bCached;
	size_t m_iCacheID;
};


class IRenderer
{
public:
	virtual ~IRenderer(void){};
	virtual bool CanRender(wxGISDataset* pDataset) = 0;	
};

class IFeatureRenderer : public IRenderer
{
public:
	virtual ~IFeatureRenderer(void){};
	virtual void Draw(wxGISGeometrySet* pSet, wxGISEnumDrawPhase DrawPhase, IDisplay* pDisplay, ITrackCancel* pTrackCancel) = 0;
};

class IRasterRenderer : public IRenderer
{
public:
	virtual ~IRasterRenderer(void){};
	virtual void Draw(wxGISDataset* pRasterDataset, wxGISEnumDrawPhase DrawPhase, IDisplay* pDisplay, ITrackCancel* pTrackCancel) = 0;
};
