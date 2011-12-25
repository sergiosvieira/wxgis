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

#include "wxgis/datasource/cursor.h"

wxFeatureCursor::wxFeatureCursor(void)
{
	Reset();
}

wxFeatureCursor::~wxFeatureCursor()
{
}

void wxFeatureCursor::Reset()
{
	m_Iterator = m_pOIDs.begin();
}

void wxFeatureCursor::Add(OGRFeatureSPtr poFeature)
{
	m_pOIDs.push_back(poFeature);
}

OGRFeatureSPtr wxFeatureCursor::Next()
{
	if(m_pOIDs.empty())
		return OGRFeatureSPtr();
	OGRFeatureSPtr poRet = *m_Iterator;
	++m_Iterator;
	return poRet;
}

void wxFeatureCursor::Clear()
{
	m_pOIDs.clear();
}