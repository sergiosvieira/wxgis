/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISMap class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#pragma once

#include "wxgis/carto/carto.h"
#include "wxgis/display/displaytransformation.h"

/** \class wxGISMap map.h
    \brief The Map class - array of layers.

    This class stores array of layers.
*/
class WXDLLIMPEXP_GIS_CRT wxGISMap
{
public:
	wxGISMap(void);
	virtual ~wxGISMap(void);
	virtual void SetName(wxString sName){m_sMapName = sName;};
	virtual wxString GetName(void){return m_sMapName;};
	virtual bool AddLayer(wxGISLayerSPtr pLayer);
	virtual void Clear(void);
	virtual size_t GetLayerCount(void){return m_paLayers.size();};
	virtual wxGISLayerSPtr GetLayer(size_t nIndex);
	virtual wxString GetDescription(void){return m_sDescription;};
	virtual void SetDescription(wxString sDescription){m_sDescription = sDescription;};
	virtual OGREnvelope GetFullExtent(void);
	virtual void SetSpatialReference(OGRSpatialReferenceSPtr pSpatialReference);
	virtual OGRSpatialReferenceSPtr GetSpatialReference(void);
protected:
	wxString m_sMapName, m_sDescription;
	std::vector<wxGISLayerSPtr> m_paLayers;
	OGRSpatialReferenceSPtr m_pSpatialReference;
	OGREnvelope m_FullExtent;
	bool m_bFullExtIsInit;
};

/** \class wxGISExtentStack map.h
    \brief The class keep history of map bounds changes.
*/
class WXDLLIMPEXP_GIS_CRT wxGISExtentStack : public wxGISMap
{
public:
	wxGISExtentStack(void);
	virtual ~wxGISExtentStack(void);
	virtual bool CanRedo(void);
	virtual bool CanUndo(void);
	virtual void Redo(void);
	virtual void Undo(void);
	virtual void Clear(void);
	virtual size_t GetSize(void);
	virtual void Do(OGREnvelope &Env);
	virtual OGREnvelope GetCurrentExtent(void);
protected:
	virtual void SetExtent(OGREnvelope &Env);
protected:
	std::vector<OGREnvelope> m_staEnvelope;
	int m_nPos;
	OGREnvelope m_CurrentExtent;
};