/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  Main application class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgissrv/srv_app/main.h"
#include "wxgissrv/srv_app/service.h"

//---------------------------------------------
// main
//---------------------------------------------


int main(int argc, char **argv)
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    wxInitializer initializer;
    if ( !initializer )
    {
        wxFprintf(stderr, _("Failed to initialize the wxWidgets library, aborting."));//fprintf(
        return -1;
    }

#ifdef __WXMSW__
	wxLogDebug(wxT("wxSocketBase::Initialize"));
    wxSocketBase::Initialize();
#endif

	bool success( false );

    // Parse command line arguments
    success = parse_commandline_parameters( argc, argv );

#ifdef __WXMSW__
    wxSocketBase::Shutdown();
#endif

	return success == true ? EXIT_SUCCESS : EXIT_FAILURE;
}


bool parse_commandline_parameters( int argc, char** argv )
{
    // Create the commandline parser
    static const wxCmdLineEntryDesc my_cmdline_desc[] =
    {
        { wxCMD_LINE_SWITCH, wxT( "h" ), wxT( "help" ), _( "Show this help message" ),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, wxT( "v" ), wxT( "version" ), _( "The version of this program" ) },
		{ wxCMD_LINE_SWITCH, wxT( "r" ), wxT("run"), _( "Run the monitoring server in standalone mode. Press 'q' to quit." ) },
		{ wxCMD_LINE_SWITCH, wxT( "i" ), wxT("install"), _( "Install monitoring server as service" ) },
		{ wxCMD_LINE_SWITCH, wxT( "u" ), wxT("uninstall"), _( "Uninstall monitoring server service" ) },
		{ wxCMD_LINE_SWITCH, wxT( "s" ), wxT("start"), _( "Start monitoring server service" ) },
		//{ wxCMD_LINE_OPTION, wxT( "i" ), wxT("index"), _( "Index the locations set in DB with max count of nodes" ), wxCMD_LINE_VAL_NUMBER },
		//{ wxCMD_LINE_SWITCH, wxT( "u" ), wxT("update"), _( "Update the locations set in config file (add/remove paths & etc.)" ) },
  //      { wxCMD_LINE_SWITCH, wxT( "l" ), wxT("list"), _( "list path's" ) },
  //      { wxCMD_LINE_SWITCH, wxT( "L" ), wxT("listall"), _( "list all path's" ) },
  //      { wxCMD_LINE_OPTION, wxT( "d" ), wxT("detail"), _("show node details"), wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_NONE }
    };

    wxCmdLineParser my_parser( my_cmdline_desc, argc, argv );
    my_parser.SetLogo(wxString::Format(_("The wxMonitoring server utility (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010"), APP_VER));

	//print params to log
	for(size_t i = 0; i < my_parser.GetParamCount(); i++)
		wxLogDebug(my_parser.GetParam());

    // Parse the parameters
    int my_parse_success = my_parser.Parse( );
    // Print help if the specify /? or if a syntax error occured.
    if( my_parse_success != 0 )
    {
        return false;
    }

	if( my_parser.Found( wxT( "v" ) ) )
	{
	    wxString out = wxString::Format(_("The wxMonitoring server utility (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010\n"), APP_VER);
	    wxFprintf(stdout, out);
		return true;
	}

	if( my_parser.Found( wxT( "r" ) ) )
	{
        wxGISServer Server;
		if(!Server.Start(wxT("wxGISServer"), CONFIG_DIR, my_parser))
		{
            Server.Stop();
			return false;
		}

        wxFprintf(stdout, wxString(_("Press 'q' to quit.\n")));
		while (getchar() != 'q');

		Server.Stop();
		return true;
	}

	if( my_parser.Found( wxT( "i" ) ) )
	{
		wxGISService Service(wxT("wxGISMonitoring"));
		Service.Install();
		return true;
	}

 	if( my_parser.Found( wxT( "u" ) ) )
	{
		wxGISService Service(wxT("wxGISMonitoring"));
		Service.Uninstall();
		return true;
	}
   
 	if( my_parser.Found( wxT( "h" ) ) )
	{
		my_parser.Usage();
		return true;
	}

 	if( my_parser.Found( wxT( "s" ) ) )
	{
		wxFprintf(stdout, wxString(_("Starting service...")));

		wxGISService Service(wxT("wxGISMonitoring"));
		Service.StartService();

		// When we get here, the service has been stopped
		if( Service.GetExitCode() == EXIT_SUCCESS)
			return true;

		return false;
	}

	my_parser.Usage();

    // Either we are using the defaults or the provided parameters were valid.

    return true;
} 
