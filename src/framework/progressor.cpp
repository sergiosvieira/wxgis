/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressor class. Progress of some process
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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
#include "wxgis/framework/progressor.h"

//---------------------------------------------------------------------------
// wxGISProgressor
//---------------------------------------------------------------------------

wxGISProgressor::wxGISProgressor(wxWindow * parent, wxWindowID id, int range, const wxPoint & pos, const wxSize & size, long style, const wxString name) : wxGauge(parent, id, range, pos, size, style, wxDefaultValidator, name), m_nValue(0), m_bYield(false)
{
}

wxGISProgressor::~wxGISProgressor()
{
}

bool wxGISProgressor::ShowProgress(bool bShow)
{
	return Show(bShow);
}

void wxGISProgressor::SetRange(int range)
{
	wxGauge::SetRange(range);
}

int wxGISProgressor::GetRange() const
{
	return wxGauge::GetRange();
}

void wxGISProgressor::SetValue(int value)
{
	m_nValue = value;
	wxGauge::SetValue(value);
    if(m_bYield)
        ::wxSafeYield(NULL, true);
}

int wxGISProgressor::GetValue() const
{
	return wxGauge::GetValue();
}

void wxGISProgressor::Play(void)
{
	wxGauge::Pulse();
    if(m_bYield)
        ::wxSafeYield(NULL, true);
}

void wxGISProgressor::Stop(void)
{
	wxGauge::SetValue(m_nValue);
    if(m_bYield)
        ::wxSafeYield(NULL, true);
}

void wxGISProgressor::SetYield(bool bYield)
{
	m_bYield = bYield;
}

