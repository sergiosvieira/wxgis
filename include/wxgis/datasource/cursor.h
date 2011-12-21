/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Cursor class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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

/** \class wxFeatureCursor cursor.h
    \brief The class represents an array of OGRFeatures, received by some selection.
*/
class WXDLLIMPEXP_GIS_DS wxFeatureCursor
{
public:
	wxFeatureCursor(void);
	virtual ~wxFeatureCursor(void);
	virtual void Add(OGRFeatureSPtr poFeature);
	virtual void Reset(void);
	virtual void Clear(void);
	virtual OGRFeatureSPtr Next(void);
protected:
	std::list<OGRFeatureSPtr> m_pOIDs;
	std::list<OGRFeatureSPtr>::const_iterator m_Iterator;
};

DEFINE_SHARED_PTR(wxFeatureCursor);