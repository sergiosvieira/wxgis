/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolder class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalog/gxspatreffolder.h"
#include "cpl_vsi_virtual.h"
#include <wx/stdpaths.h>

/////////////////////////////////////////////////////////////////////////
// wxGxSpatialReferencesFolder
/////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxGxSpatialReferencesFolder, wxObject)

wxGxSpatialReferencesFolder::wxGxSpatialReferencesFolder(void) : wxGxPrjFolder(CPLString(), GetName())
{
}

wxGxSpatialReferencesFolder::~wxGxSpatialReferencesFolder(void)
{
}

void wxGxSpatialReferencesFolder::Init(wxXmlNode* const pConfigNode)
{
    m_sInternalPath = pConfigNode->GetAttribute(wxT("path"), NON);
    if(m_sInternalPath.IsEmpty() || m_sInternalPath == wxString(NON))
    {
        //example: /vsizip/c:/wxGIS/sys/cs.zip/cs
        wxStandardPaths stp;
        wxString sExeDirPath = wxPathOnly(stp.GetExecutablePath());
        m_sInternalPath = wxT("/vsizip/") + sExeDirPath + wxT("/sys/cs.zip/cs");
    }

    m_sInternalPath.Replace(wxT("\\"), wxT("/"));
    wxLogMessage(_("wxGxSpatialReferencesFolder: The path is set to '%s'"), m_sInternalPath.c_str());
    CPLSetConfigOption("wxGxSpatialReferencesFolder", m_sInternalPath.mb_str(wxConvUTF8));

    m_sPath = CPLString(m_sInternalPath.mb_str(wxConvUTF8));
}

void wxGxSpatialReferencesFolder::Serialize(wxXmlNode* pConfigNode)
{
    pConfigNode->AddAttribute(wxT("path"), m_sInternalPath);
}

/////////////////////////////////////////////////////////////////////////
// wxGxPrjFolder
/////////////////////////////////////////////////////////////////////////

wxGxPrjFolder::wxGxPrjFolder(CPLString Path, wxString Name) : wxGxArchiveFolder(Path, Name)
{
}

wxGxPrjFolder::~wxGxPrjFolder(void)
{
}

IGxObject* wxGxPrjFolder::GetArchiveFolder(CPLString szPath, wxString soName)
{
	wxGxPrjFolder* pFolder = new wxGxPrjFolder(szPath, soName);
	return static_cast<IGxObject*>(pFolder);
}
