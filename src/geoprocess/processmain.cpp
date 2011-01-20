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
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/core/config.h"
#include "wxgis/catalog/gxcatalog.h"

#include "wx/defs.h"
#include "wx/cmdline.h"

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
		{ wxCMD_LINE_OPTION, wxT( "n" ), wxT("name"), _( "The tool name" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, wxT( "p" ), wxT("parameters"), _( "The tool parameters" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_NONE }
    };

    wxCmdLineParser my_parser( my_cmdline_desc, argc, wxArgv );
    my_parser.SetLogo(wxString::Format(_("The wxGISGeoprocess (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-2011"), APP_VER));

    // Parse command line arguments
    success = parse_commandline_parameters(my_parser);

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
	    wxString out = wxString::Format(_("The wxGISGeoprocess (%s)\nAuthor: Bishop (aka Barishnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2010-2011\n"), APP_VER);
	    wxFprintf(stdout, out);
		return true;
	}


    wxString sToolName, sToolParameters;
    if(parser.Found( wxT( "n" ), &sToolName ) && parser.Found( wxT( "p" ), &sToolParameters ) )
    {
		//wxString input_str(input_data);
		wxString str(sToolParameters.mb_str(), wxConvUTF8);//wc_str(*wxConvCurrent)

		//sToolParameters = wxString(sToolParameters.ToUTF8());

        wxGPTaskExecutor executor;
        return executor.OnExecute(sToolName, sToolParameters);
    }

	parser.Usage();

    // Either we are using the defaults or the provided parameters were valid.

    return true;

} 

//////////////////////////////////////////////////////////////////////////////
// wxGPTaskExecutor
//////////////////////////////////////////////////////////////////////////////

wxGPTaskExecutor::wxGPTaskExecutor() : ITrackCancel()
{
    //TODO: Config support
	//GDAL
    CPLSetConfigOption( "GDAL_CACHEMAX", "128" );

	OGRRegisterAll();
	GDALAllRegister();
	//END GDAL

    SetProgressor(this);
}

wxGPTaskExecutor::~wxGPTaskExecutor()
{
	//GDAL
	GDALDestroyDriverManager();
	OGRCleanupAll();
	//END GDAL
}

bool wxGPTaskExecutor::OnExecute(wxString sToolName, wxString sToolParameters)
{
	//create output stream
    m_StdOutFile.Attach(stdout);
	wxFFileOutputStream OutStream(m_StdOutFile);
	m_pOutTxtStream = new wxTextOutputStream(OutStream);

    wxGISAppConfig* pConfig;
#ifdef WXGISPORTABLE
    pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, true);
#else
    pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, false);
#endif
    wxXmlNode* pToolsChild = pConfig->GetConfigNode(wxT("tools"), false, true);
    if(!pToolsChild)
    {
        PutMessage(wxString(_("Get wxGISToolbox config failed!")), -1, enumGISMessageErr);
        return false;
    }

    //PutMessage(wxString::Format(_("tool - %s params - %s"), sToolName.c_str(), sToolParameters.c_str()), -1, enumGISMessageInfo);


    wxGxCatalog GxCatalog;
    GxCatalog.Init();

    wxGISGPToolManager GISGPToolManager(pToolsChild);
    IGPTool* pTool = GISGPToolManager.GetTool(sToolName, &GxCatalog);
    if(!pTool)
    {
        PutMessage(wxString::Format(_("Get Geoprocessing tool %s failed!"), sToolName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    if(!pTool->SetFromString(sToolParameters))
    {
        PutMessage(wxString::Format(_("Set Geoprocessing tool %s parameters failed!"), sToolName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    if(!pTool->Validate())
    {
        PutMessage(wxString::Format(_("Geoprocessing tool %s validation failed!"), sToolName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    bool bResult = pTool->Execute(this);

    GxCatalog.Detach();

    wxDELETE(m_pOutTxtStream)
    return bResult;
}

void wxGPTaskExecutor::PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType)
{
    if(m_sPrevMsg == sMessage)
        return;
    wxString sOut;

    switch(nType)
    {
    case enumGISMessageErr:
        sOut.Append(wxT("ERR: "));
        break;
    case enumGISMessageWarning:
        sOut.Append(wxT("WARN: "));
        break;
    case enumGISMessageQuestion:
    case enumGISMessageUnk:
    case enumGISMessageNorm:
    case enumGISMessageInfo:
    case enumGISMessageTitle:
    case enumGISMessageOK:
        sOut.Append(wxT("INFO: "));
        break;
    default:
        break;
    }
    sOut.Append(sMessage);
    sOut.Append(wxT("\r\n"));
    m_pOutTxtStream->WriteString(sOut);
	m_StdOutFile.Flush();
    m_sPrevMsg = sMessage;
}

void wxGPTaskExecutor::SetValue(int value)
{
    if(m_nValue == value)
        return;
    m_nValue = value;
    m_pOutTxtStream->WriteString(wxString::Format(wxT("DONE: %d%%\r"), value));
	m_StdOutFile.Flush();
}
