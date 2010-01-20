/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSpatialReferenceValidator class. Return SpatialReference limits
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

#include "wxgis/carto/spvalidator.h"
#include "wxgis/framework/config.h"

#include <wx/wfstream.h>
#include <wx/xml/xml.h>

wxGISSpatialReferenceValidator::wxGISSpatialReferenceValidator(void) : m_bIsOK(false)
{
    m_sPath = wgMB2WX(CPLGetConfigOption("wxGxSpatialReferencesFolder",""));
    if(!m_sPath.IsEmpty())
    {
        m_bIsOK = LoadData(m_sPath);
        return;
    }
	wxGISConfig m_Conf(wxString(wxT("wxCatalog")), CONFIG_DIR);

	wxXmlNode* pConfXmlNode = m_Conf.GetConfigNode(enumGISHKCU, wxString(wxT("catalog")));
	if(!pConfXmlNode)
		pConfXmlNode = m_Conf.GetConfigNode(enumGISHKLM, wxString(wxT("catalog")));
	if(!pConfXmlNode)
		return;

	//search for path
	wxXmlNode* pRootItemsNode = m_Conf.GetConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems"))); 
	m_sPath = GetPath(pRootItemsNode);
    if(!m_sPath.IsEmpty())
    {
        m_bIsOK = LoadData(m_sPath);
        return;
    }
	pRootItemsNode = m_Conf.GetConfigNode(enumGISHKLM, wxString(wxT("catalog/rootitems"))); 
	m_sPath = GetPath(pRootItemsNode);
    if(!m_sPath.IsEmpty())
    {
        m_bIsOK = LoadData(m_sPath);
        return;
    }
}

wxGISSpatialReferenceValidator::~wxGISSpatialReferenceValidator(void)
{
}

bool wxGISSpatialReferenceValidator::LoadData(wxString sPath)
{
	FILE *fp = VSIFOpenL( wgWX2MB(sPath), "r");
	if( fp == NULL )
        return false;
    wxFFileInputStream wfp(fp);
    wxXmlDocument m_pXmlDoc(wfp);
    if(!m_pXmlDoc.IsOk())
        return false;
    wxXmlNode* pRootNode = m_pXmlDoc.GetRoot();
    if(NULL == pRootNode)
        return false;
    wxXmlNode* pChildNode = pRootNode->GetChildren();
    while(pChildNode != NULL)
    {
        wxString sName = pChildNode->GetPropVal(wxT("name"),wxT(""));
        if(!sName.IsEmpty())
        {
            LIMITS lims;
            lims.minx = wxAtof(pChildNode->GetPropVal(wxT("minx"),wxT("-179.99999999")));
            lims.miny = wxAtof(pChildNode->GetPropVal(wxT("miny"),wxT("-89,99999999")));
            lims.maxx = wxAtof(pChildNode->GetPropVal(wxT("maxx"),wxT("179.99999999")));
            lims.maxy = wxAtof(pChildNode->GetPropVal(wxT("maxy"),wxT("89,99999999")));
            m_DataMap[sName] = lims;
       }
        pChildNode = pChildNode->GetNext();
    }
    return true;
}

wxString wxGISSpatialReferenceValidator::GetPath(wxXmlNode* pNode)
{
	if(!pNode)
		return wxEmptyString;

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		if( pChildren->GetPropVal(wxT("name"), NONAME) == wxString(wxT("wxGxSpatialReferencesFolder")))
            return pChildren->GetPropVal(wxT("path"), wxT(""));
		pChildren = pChildren->GetNext();
	}
	return wxEmptyString;
}

bool wxGISSpatialReferenceValidator::IsLimitsSet(wxString sProj)
{
    if(m_DataMap.find(sProj) != m_DataMap.end())
        return true;
    return false;
}
