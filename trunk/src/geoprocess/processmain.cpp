/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  Main application class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
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

#include "wxgis/geoprocess/processmain.h"
#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/core/config.h"

#include "wx/defs.h"
#include "wx/cmdline.h"
#include "wx/string.h"
#include "wx/wfstream.h"
#include "wx/txtstrm.h"

int main(int argc, char **argv)
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    wxInitializer initializer;
    if ( !initializer )
    {
        wxFprintf(stderr, _("Failed to initialize the wxWidgets library, aborting."));
        return -1;
    }

	bool success( false );

    // Parse command line arguments
    success = parse_commandline_parameters( argc, argv );

	return success == true ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool parse_commandline_parameters( int argc, char** argv )
{
    // Create the commandline parser
    static const wxCmdLineEntryDesc my_cmdline_desc[] =
    {
        { wxCMD_LINE_SWITCH, wxT( "h" ), wxT( "help" ), _( "Show this help message" ), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, wxT( "v" ), wxT( "version" ), _( "The version of this program" ) },
		{ wxCMD_LINE_OPTION, wxT( "n" ), wxT("name"), _( "The tool name" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, wxT( "p" ), wxT("parameters"), _( "The tool parameters" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_NONE }
    };

    wxCmdLineParser my_parser( my_cmdline_desc, argc, argv );
    my_parser.SetLogo(wxString::Format(_("The wxGISGeoprocess (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010"), APP_VER));

    // Parse the parameters
    int my_parse_success = my_parser.Parse( );
    // Print help if the specify /? or if a syntax error occured.
    if( my_parse_success != 0 )
    {
        return false;
    }

	if( my_parser.Found( wxT( "v" ) ) )
	{
	    wxString out = wxString::Format(_("The wxGISGeoprocess (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010\n"), APP_VER);
	    wxFprintf(stdout, out);
		return true;
	}


    if(my_parser.Found( wxT( "n" ) ) && my_parser.Found( wxT( "p" ) ) )
    {
		//create output stream
		wxFFileOutputStream OutStream(stdout);
		wxTextOutputStream OutTxtStream(OutStream);
		OutTxtStream.WriteString(wxString(wxT("INFO: Run execution\r\n")));
		fflush(stdout);
		for(size_t i = 0; i < 100; i++)
		{
			OutTxtStream.WriteString(wxString::Format(wxT("DONE: %d%%\r"), i + 1));
			fflush(stdout);
			if(i == 50)
			{
				wxString sErrMsg(wxT("Test error"));
				OutTxtStream.WriteString(wxString::Format(wxT("ERR: %s occured\r\n"), sErrMsg.c_str()));
				fflush(stdout);
			}
			wxSleep(1);
		}
		OutTxtStream.WriteString(wxString(wxT("INFO: Finish\r\n")));

//        wxGISAppConfig* pConfig;
//#ifdef WXGISPORTABLE
//        pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, true);
//#else
//	    pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, false);
//#endif
//        wxXmlNode* pToolsChild = pConfig->GetConfigNode(wxT("tools"), false, true);
        //wxGISGPToolManager* pGISGPToolManager(pToolsChild,
        //pGISGPToolManager->GetTool
		return true;
    }

	my_parser.Usage();

    // Either we are using the defaults or the provided parameters were valid.

    //while (getchar() != 'q'); test

    return true;

} 