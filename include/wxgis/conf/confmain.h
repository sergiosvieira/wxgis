/******************************************************************************
 * Project:  wxGIS (GIS common)
 * Purpose:  wxGIS config modification application.
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

#include "wxgis/core/core.h"
#include "wxgis/core/config.h"

DEFINE_SHARED_PTR(wxGISAppConfig);

bool parse_commandline_parameters( wxCmdLineParser& parser );
int main(int argc, char** argv);
wxXmlNode* GetConfigNode(wxArrayString& aTokens, wxGISAppConfigSPtr& pConfig);
void AddProperty(wxXmlNode* pNode, wxString sParamName, wxString sParamValue);
void AddProperty(wxXmlNode* pNode, wxString sParamName, long nParamValue);
void AddProperties(wxXmlNode* pNode, wxString sAttributes);