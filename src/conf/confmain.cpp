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
#include "wxgis/conf/confmain.h"

#include "wx/defs.h"
#include "wx/cmdline.h"
#include "wx/filename.h"
#include "wx/tokenzr.h"


int main(int argc, char **argv)
{
#if wxUSE_UNICODE
    wxChar **wxArgv = new wxChar *[argc + 1];

    {
        int n;

        for (n = 0; n < argc; n++ )
        {
            wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
            wxArgv[n] = wxStrdup(warg);
        }

        wxArgv[n] = NULL;
    }
#else
    #define wxArgv argv
#endif // wxUSE_UNICODE
	
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    wxInitializer initializer;
    if ( !initializer )
    {
        wxFprintf(stderr, _("Failed to initialize the wxWidgets library, aborting."));
        return -1;
    }

	bool success( false );

    // Create the commandline parser
    static const wxCmdLineEntryDesc my_cmdline_desc[] =
    {
        { wxCMD_LINE_SWITCH, wxT( "h" ), wxT( "help" ), _( "Show this help message" ), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, wxT( "v" ), wxT( "version" ), _( "The version of this program" ) },
		{ wxCMD_LINE_OPTION, wxT( "a" ), wxT( "add" ), _( "Add data to path in XML Config." ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_SWITCH, wxT( "d" ), wxT( "del" ), _( "Delete data from XML Config" ) },
		{ wxCMD_LINE_OPTION, wxT( "n" ), wxT("name"), _( "The XMLNode name attribute" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, wxT( "p" ), wxT("path"), _( "The XMLNode path attribute" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, wxT( "e" ), wxT("is_enabled"), _( "The XMLNode is_enabled attribute. is_enabled choice: 1 for enabled or 0 for disabled" ), wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_OPTION, wxT( "c" ), wxT("class"), _( "The XMLNode class attribute" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, wxT( "i" ), wxT("index"), _( "The XMLNode index attribute. Index choice: 0 the first node, -1 - the last node" ), wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_OPTION, wxT( "t" ), wxT("type"), _( "The XMLNode type attribute. Type choice: sep, cmd, menu" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, wxT( "s" ), wxT("sub_type"), _( "The XMLNode sub_type attribute" ), wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_OPTION, wxT( "o" ), wxT("cmd_name"), _( "The XMLNode cmd_name attribute" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, wxT( "x" ), wxT("attributes"), _( "The XMLNode attributes <attr_name>=<attr_value> separated by comma" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_NONE }
    };

    wxCmdLineParser my_parser( my_cmdline_desc, argc, wxArgv );
    my_parser.SetLogo(wxString::Format(_("The wxGISConf (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2011"), APP_VER));
    // Parse command line arguments
    success = parse_commandline_parameters( my_parser );

	return success == true ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool parse_commandline_parameters( wxCmdLineParser& parser )
{
    // Parse the parameters
    int my_parse_success = parser.Parse( );
    // Print help if the specify /? or if a syntax error occured.
    if( my_parse_success != 0 )
    {
        return false;
    }

	if( parser.Found( wxT( "v" ) ) )
	{
	    wxString out = wxString::Format(_("The wxGISConf (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2011\n"), APP_VER);
	    wxFprintf(stdout, out);
		return true;
	}

	wxString sPathInConfig;
	if( parser.Found( wxT( "a" ), &sPathInConfig) )
	{
		wxString sName, sPath, sClass, sType, sCmdName, sAttributes;
		long nIsEnabled, nIndex(-1), nSubType;

		bool bNameSet = parser.Found( wxT( "n" ), &sName );
		bool bPathSet = parser.Found( wxT( "p" ), &sPath );
		bool bClassSet = parser.Found( wxT( "c" ), &sClass );
		bool bTypeSet = parser.Found( wxT( "t" ), &sType );
		bool bCmdNameSet = parser.Found( wxT( "o" ), &sCmdName );

		bool bIsEnabledSet = parser.Found( wxT( "e" ), &nIsEnabled );
		bool bIndexSet = parser.Found( wxT( "i" ), &nIndex );
		bool bSubtypeSet = parser.Found( wxT( "s" ), &nSubType );
		bool bAttributesSet = parser.Found( wxT( "x" ), &sAttributes );

		wxGISAppConfigSPtr pConfig;
		wxArrayString aTokens = wxStringTokenize(sPathInConfig, wxString(wxT("/")));
		wxXmlNode* pNode = GetConfigNode(aTokens, pConfig);
		if(!pNode)
			return false;

		//check duplicates
		wxXmlNode* pNewNode(NULL);
		if(bNameSet)
		{
			wxXmlNode* pChildNode = pNode->GetChildren();
			while(pChildNode)
			{
				if(pChildNode->GetName().CmpNoCase(aTokens.Last()) == 0 && pChildNode->GetPropVal(wxT("name"), NONAME).CmpNoCase(sName) == 0)
				{
					pNewNode = pChildNode;
					break;
				}
				pChildNode = pChildNode->GetNext();
			}
		}
		else if(bClassSet)
		{
			wxXmlNode* pChildNode = pNode->GetChildren();
			while(pChildNode)
			{
				if(pChildNode->GetName().CmpNoCase(aTokens.Last()) == 0 && pChildNode->GetPropVal(wxT("class"), NONAME).CmpNoCase(sName) == 0)
				{
					pNewNode = pChildNode;
					break;
				}
				pChildNode = pChildNode->GetNext();
			}
		}
		else
		{
			if(nIndex == -1)
				pNewNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, aTokens.Last());
			else
			{
				wxXmlNode* pChildNode = pNode->GetChildren();
				long nCounter(0);
				while(pChildNode)
				{
					if(nCounter == nIndex)
					{
						pNewNode = new wxXmlNode(wxXML_ELEMENT_NODE, aTokens.Last());
						pNode->InsertChild(pNewNode, pChildNode);
						break;
					}
					pChildNode = pChildNode->GetNext();
				}
			}
		}

		if(bNameSet)
			AddProperty(pNewNode, wxT("name"), sName);
		if(bNameSet)
			AddProperty(pNewNode, wxT("name"), sName);
		if(bPathSet)
			AddProperty(pNewNode, wxT("path"), sPath);
		if(bClassSet)
			AddProperty(pNewNode, wxT("class"), sClass);
		if(bTypeSet)
			AddProperty(pNewNode, wxT("type"), sType);
		if(bCmdNameSet)
			AddProperty(pNewNode, wxT("cmd_name"), sCmdName);
		if(bIsEnabledSet)
			AddProperty(pNewNode, wxT("is_enabled"), nIsEnabled);
		if(bSubtypeSet)
			AddProperty(pNewNode, wxT("sub_type"), nSubType);
		if(bAttributesSet)
			AddProperties(pNewNode, sAttributes);

		return true;
	}

	if( parser.Found( wxT( "d" )) )
	{
		return false;
	}

	parser.Usage();

    // Either we are using the defaults or the provided parameters were valid.

    return true;

} 


void AddProperty(wxXmlNode* pNode, wxString sParamName, wxString sParamValue)
{
	if(pNode->HasProp(sParamName))
		pNode->DeleteProperty(sParamName);
	pNode->AddProperty(sParamName, sParamValue);
}

void AddProperty(wxXmlNode* pNode, wxString sParamName, long nParamValue)
{
	if(pNode->HasProp(sParamName))
		pNode->DeleteProperty(sParamName);
	pNode->AddProperty(sParamName, wxString::Format(wxT("%d"), nParamValue));
}

void AddProperties(wxXmlNode* pNode, wxString sAttributes)
{
	wxStringTokenizer tkz(sAttributes, wxString(wxT(",")), wxTOKEN_RET_EMPTY );
	while ( tkz.HasMoreTokens() )
	{
		wxString token = tkz.GetNextToken();
		int nPos = wxNOT_FOUND;
		if((nPos = token.Find(wxT("="))) != wxNOT_FOUND)
		{
			wxString sAttrName = token.Left(nPos);
			wxString sAttrValue = token.Right(token.Len() - nPos);
			if(sAttrName.IsEmpty())
				continue;
			AddProperty(pNode, sAttrName, sAttrValue);
		}
	}
}

wxXmlNode* GetConfigNode(wxArrayString& aTokens, wxGISAppConfigSPtr& pConfig)
{
	if(aTokens.GetCount() < 2)
		return NULL;
	pConfig = boost::make_shared<wxGISAppConfig>(aTokens[0], CONFIG_DIR);
	wxXmlNode* pRootNode = pConfig->GetConfigNode(enumGISHKLM, wxEmptyString);
	if(!pRootNode)
		return NULL;

	wxXmlNode* pChildNode = pRootNode->GetChildren();
	for (size_t i = 1; i < aTokens.GetCount() - 1; i++ )
	{
        wxString token = aTokens[i];
		//search # in name
		int nPos = wxNOT_FOUND;
		wxString sPath, sAttrCmp, sAttrName, sAttrValue;
		if(( nPos = token.Find(wxT("#"))) != wxNOT_FOUND)
		{
			sAttrCmp = token.Right(token.Len() - nPos);
			sPath = token.Left(nPos);
		}

		if(( nPos = sAttrCmp.Find(wxT("="))) != wxNOT_FOUND)
		{
			sAttrValue = sAttrCmp.Right(sAttrCmp.Len() - nPos);
			sAttrName = sAttrCmp.Left(nPos);
		}

        while(pChildNode)
        {
			wxString sChildName = pChildNode->GetName();
            if(token == sChildName.MakeLower())
			{
				if(sAttrName.IsEmpty() && sAttrValue.IsEmpty())
				{
					if(i < aTokens.GetCount() - 1)
						pChildNode = pChildNode->GetChildren();
					break;
				}
				else
				{
					if(pChildNode->GetPropVal(sAttrName, wxT("")).CmpNoCase(sAttrValue) == 0 && i < aTokens.GetCount() - 1)
						pChildNode = pChildNode->GetChildren();
					break;
				}
			}
            pChildNode = pChildNode->GetNext();
        }
    }

	if(pChildNode->GetName().CmpNoCase(aTokens[aTokens.GetCount() - 1]) == 0)
    {
        return pChildNode;
    }
	return NULL;
}
