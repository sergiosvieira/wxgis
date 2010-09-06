/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFileFactory class.
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
#include "wxgis/catalog/gxfilefactory.h"
#include "wxgis/catalog/gxfile.h"
#include <wx/tokenzr.h>

#include "../../art/sr_16(2).xpm"
#include "../../art/sr_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxFileFactory, wxObject)

wxGxFileFactory::wxGxFileFactory(void)
{
    m_LargePRJIcon = wxIcon(sr_48_xpm);
    m_SmallPRJIcon = wxIcon(sr_16_2_xpm);
}

wxGxFileFactory::~wxGxFileFactory(void)
{
}

bool wxGxFileFactory::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
{
	for(size_t i = 0; i < pFileNames->GetCount(); i++)
	{
		wxString path = pFileNames->Item(i);
		//test is dir
		if(wxFileName::DirExists(path))
			continue;

		wxString name, ext;
		wxFileName::SplitPath(path, NULL, NULL, &name, &ext);
		ext.MakeLower();

        //name conv cp866 if zip
        if(path.Find(wxT("/vsizip/")) != wxNOT_FOUND)
        {
            wxString str(name.mb_str(*wxConvCurrent), wxCSConv(wxT("cp-866")));
            name = str;
        }

		IGxObject* pGxObj = NULL;
		if(ext == wxString(wxT("spr")))
		{ 
            name += wxT(".") + ext;
			wxGxPrjFile* pFile = new wxGxPrjFile(path, name, enumSPRfile, m_LargePRJIcon, m_SmallPRJIcon);
			pGxObj = dynamic_cast<IGxObject*>(pFile);
			goto REMOVE;
		}
		if(ext == wxString(wxT("prj")))
		{
			name += wxT(".") + ext;
			wxGxPrjFile* pFile = new wxGxPrjFile(path, name, enumESRIPrjFile, m_LargePRJIcon, m_SmallPRJIcon);
			pGxObj = dynamic_cast<IGxObject*>(pFile);
			goto REMOVE;
		}
        //add extensions from config
        for(size_t j = 0; j < m_ExtArray.size(); j++)
        {
            if(m_ExtArray[j] == ext)
            {
			    name += wxT(".") + ext;
			    wxGxTextFile* pFile = new wxGxTextFile(path, name);
			    pGxObj = dynamic_cast<IGxObject*>(pFile);
			    goto REMOVE;
            }
        }
		continue;
REMOVE:
		pFileNames->RemoveAt(i);
		i--;
		if(pGxObj != NULL)
			pObjArray->push_back(pGxObj);
	}

	return true;
}

void wxGxFileFactory::Serialize(wxXmlNode* pConfig, bool bStore)
{
    if(bStore)
    {
        if(pConfig->HasProp(wxT("factory_name")))
            pConfig->DeleteProperty(wxT("factory_name"));
        pConfig->AddProperty(wxT("factory_name"), GetName());    
        if(pConfig->HasProp(wxT("is_enabled")))
            pConfig->DeleteProperty(wxT("is_enabled"));
        pConfig->AddProperty(wxT("is_enabled"), m_bIsEnabled == true ? wxT("1") : wxT("0"));    
        wxString sExt;
        for(size_t j = 0; j < m_ExtArray.size(); j++)
        {
            sExt += m_ExtArray[j];
            sExt += wxT("|");
        }
        if(pConfig->HasProp(wxT("extensions")))
            pConfig->DeleteProperty(wxT("extensions"));
        pConfig->AddProperty(wxT("extensions"), sExt);    
    }
    else
    {
        m_bIsEnabled = wxAtoi(pConfig->GetPropVal(wxT("is_enabled"), wxT("1")));
        wxString sExts = pConfig->GetPropVal(wxT("extensions"), wxT("txt|log|nfo|"));
	    wxStringTokenizer tkz(sExts, wxString(wxT("|")), false );
	    while ( tkz.HasMoreTokens() )
	    {
		    wxString token = tkz.GetNextToken();
		    token.Replace(wxT("|"), wxT(""));	
		    token.MakeLower();
            m_ExtArray.Add(token);
	    }
    }
}
