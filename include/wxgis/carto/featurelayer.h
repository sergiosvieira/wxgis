/******************************************************************************
 * Project:  wxGIS
 * Purpose:  FeatureLayer header.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Bishop
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

#include "wxgis/carto/layer.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/vectorop.h"
#include "wxgis/carto/featurerenderer.h"

/** \class wxGISFeatureLayer featurelayer.h
    \brief The class represent vector datasource in map.
*/

class WXDLLIMPEXP_GIS_CRT wxGISFeatureLayer :
	public wxGISLayer
{
    DECLARE_CLASS(wxGISFeatureLayer)
public:
	wxGISFeatureLayer(const wxString &sName = _("new layer"), wxGISDataset* pwxGISDataset = NULL);
	virtual ~wxGISFeatureLayer(void);
//wxGISLayer
	virtual bool Draw(wxGISEnumDrawPhase DrawPhase, ITrackCancel* const pTrackCancel = NULL);
	virtual bool IsValid(void) const;
	virtual bool IsCacheNeeded(void) const;
	virtual wxGISEnumDatasetType GetType(void) const {return enumGISFeatureDataset;};
//wxGISFeatureLayer
	virtual wxGISQuadTreeCursor Idetify(const wxGISGeometry &Geom); 
    //events
    void OnDSClosed(wxFeatureDSEvent& event);
    void OnDSFeaturesAdded(wxFeatureDSEvent& event);
protected:
    //virtual void LoadGeometry(void);
	virtual long GetPointsInGeometry(const wxGISGeometry& Geom) const;
protected:
    wxGISFeatureDataset* m_pwxGISFeatureDataset;
    wxGISFeatureRenderer* m_pFeatureRenderer;
    long m_nConnectionPointDSCookie;
//	wxGISQuadTree* m_pQuadTree;
private:
	DECLARE_EVENT_TABLE()
};
