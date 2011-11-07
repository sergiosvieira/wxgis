/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PropertyPages of Catalog.
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
#include "wxgis/framework/propertypages.h"

#include "../../art/open.xpm"
#include "../../art/state.xpm"

#include "wx/dir.h"

IMPLEMENT_DYNAMIC_CLASS(wxGISMiscPropertyPage, wxPanel)

BEGIN_EVENT_TABLE(wxGISMiscPropertyPage, wxPanel)
	EVT_BUTTON(ID_OPENLOCPATH, wxGISMiscPropertyPage::OnOpenLocPath)
	EVT_BUTTON(ID_OPENSYSPATH, wxGISMiscPropertyPage::OnOpenSysPath)
	EVT_BUTTON(ID_OPENLOGPATH, wxGISMiscPropertyPage::OnOpenLogPath)
END_EVENT_TABLE()

wxGISMiscPropertyPage::wxGISMiscPropertyPage(void) : m_pApp(NULL)
{
}

wxGISMiscPropertyPage::~wxGISMiscPropertyPage()
{
}

bool wxGISMiscPropertyPage::Create(IFrameApplication* application, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
		return false;

    m_pApp = application;
    if(!m_pApp)
        return false;

	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
        return false;    

    wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Locale files folder path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bMainSizer->Add( m_staticText1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bLocPathSizer;
	bLocPathSizer = new wxBoxSizer( wxHORIZONTAL );
	
    //locale files path
	m_LocalePath = new wxTextCtrl( this, ID_LOCPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_LocalePath->ChangeValue( pConfig->GetLocaleDir() );
	bLocPathSizer->Add( m_LocalePath, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_bpOpenLocPath = new wxBitmapButton( this, ID_OPENLOCPATH, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bLocPathSizer->Add( m_bpOpenLocPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bMainSizer->Add( bLocPathSizer, 0, wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Language"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bMainSizer->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );

    //see current locale dir for locales
	
	//wxString m_LangChoiceChoices[] = { _("en") };
	//int m_LangChoiceNChoices = sizeof( m_LangChoiceChoices ) / sizeof( wxString );
    FillLangArray(pConfig->GetLocaleDir());
	m_LangChoice = new wxChoice( this, ID_LANGCHOICE, wxDefaultPosition, wxDefaultSize, m_aLangs, 0 );
	//m_LangChoice->SetSelection( 0 );
    m_LangChoice->SetStringSelection(pConfig->GetLocale());
	bMainSizer->Add( m_LangChoice, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("wxGIS system files folder path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bMainSizer->Add( m_staticText3, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_SysPath = new wxTextCtrl( this, ID_SYSPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_SysPath->ChangeValue( pConfig->GetSysDir() );
	bSizer21->Add( m_SysPath, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_bpOpenSysPath = new wxBitmapButton( this, ID_OPENSYSPATH, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer21->Add( m_bpOpenSysPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bMainSizer->Add( bSizer21, 0, wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("wxGIS log files folder path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bMainSizer->Add( m_staticText4, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer211;
	bSizer211 = new wxBoxSizer( wxHORIZONTAL );
	
	m_LogPath = new wxTextCtrl( this, ID_LOGPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_LogPath->ChangeValue( pConfig->GetLogDir() );
	bSizer211->Add( m_LogPath, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_bpOpenLogPath = new wxBitmapButton( this, ID_OPENLOGPATH, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer211->Add( m_bpOpenLogPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bMainSizer->Add( bSizer211, 0, wxEXPAND, 5 );

    m_checkDebug = new wxCheckBox( this, wxID_ANY, _("Log debug GDAL messages"), wxDefaultPosition, wxDefaultSize, 0 );

	bMainSizer->Add( m_checkDebug, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizerInfo = new wxBoxSizer( wxHORIZONTAL );

    m_staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bMainSizer->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );

    m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(state_xpm));
	
	m_bitmapwarn = new wxStaticBitmap( this, wxID_ANY, m_ImageList.GetIcon(3), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerInfo->Add( m_bitmapwarn, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticTextWarn = new wxStaticText( this, wxID_ANY, _("To apply some changes on this page the application restart needed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextWarn->Wrap( -1 );

    wxFont WarnFont = this->GetFont();
    WarnFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticTextWarn->SetFont(WarnFont);

	bSizerInfo->Add( m_staticTextWarn, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bMainSizer->Add( bSizerInfo, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();

    return true;
}

void wxGISMiscPropertyPage::Apply(void)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return;
    if(m_LangChoice->GetStringSelection() != pConfig->GetLocale() && m_LangChoice->GetStringSelection().IsEmpty() == false)
    {
        m_pApp->SetupLoc(m_LangChoice->GetStringSelection(), m_LocalePath->GetValue());
		if(pConfig)
		{
			pConfig->SetLocale(m_LangChoice->GetStringSelection());
			pConfig->SetLocaleDir(m_LocalePath->GetValue());
		}
    }
    if(m_LogPath->IsModified())
    {
        m_pApp->SetupLog(m_LogPath->GetValue());
		if(pConfig)
		{
       		pConfig->SetLogDir(m_LogPath->GetValue());
		}
    }
    if(m_SysPath->IsModified())
    {
        m_pApp->SetupSys(m_SysPath->GetValue());
		if(pConfig)
		{
			pConfig->SetSysDir(m_SysPath->GetValue());
		}
    }
    if(m_checkDebug->GetValue() != pConfig->GetDebugMode())
    {
        m_pApp->SetDebugMode(m_checkDebug->GetValue());
		if(pConfig)
		{
			pConfig->SetDebugMode(m_checkDebug->GetValue());
		}
    }
}

void wxGISMiscPropertyPage::FillLangArray(wxString sPath)
{
    m_aLangs.Clear();

    wxDir dir(sPath);
    if ( !dir.IsOpened() )
        return;

    wxString sSubdirName;
    bool bAddEn(true);
    wxString sEn(wxT("en"));
    bool cont = dir.GetFirst(&sSubdirName, wxEmptyString, wxDIR_DIRS);
    while ( cont )
    {
        if(bAddEn)
            if(sEn.CmpNoCase(sSubdirName) == 0)
                bAddEn = false;
        m_aLangs.Add(sSubdirName);
        cont = dir.GetNext(&sSubdirName);
    }
    if(bAddEn)
        m_aLangs.Add(wxT("en"));
}

void wxGISMiscPropertyPage::OnOpenLocPath(wxCommandEvent& event)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return;
	wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a folder with locale files")), pConfig->GetLocaleDir(), wxDD_DEFAULT_STYLE |wxDD_DIR_MUST_EXIST );
	if(dlg.ShowModal() == wxID_OK)
	{
		wxString sPath = dlg.GetPath();
        m_LocalePath->ChangeValue( sPath );
        m_LocalePath->SetModified(true);
        m_LangChoice->Clear();
        FillLangArray(sPath);
        m_LangChoice->Append(m_aLangs);
        m_LangChoice->SetSelection( 0 );
	}
}

void wxGISMiscPropertyPage::OnOpenSysPath(wxCommandEvent& event)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return;
    wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a wxGIS system files folder path")), pConfig->GetSysDir(), wxDD_DEFAULT_STYLE |wxDD_DIR_MUST_EXIST );
	if(dlg.ShowModal() == wxID_OK)
	{
		wxString sPath = dlg.GetPath();
        m_SysPath->ChangeValue( sPath );
        m_SysPath->SetModified(true);
	}
}

void wxGISMiscPropertyPage::OnOpenLogPath(wxCommandEvent& event)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return;
	wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a wxGIS log files folder path")), pConfig->GetLogDir(), wxDD_DEFAULT_STYLE |wxDD_DIR_MUST_EXIST );
	if(dlg.ShowModal() == wxID_OK)
	{
		wxString sPath = dlg.GetPath();
        m_LogPath->ChangeValue( sPath );
        m_LogPath->SetModified(true);
	}
}

//http://trac.osgeo.org/gdal/wiki/ConfigOptions
//
//CPL_DEBUG
//
//This may be set to ON, OFF or specific prefixes. If it is ON, all debug messages are reported to stdout. If it is OFF or unset no debug messages are reported. If it is set to a particular value, then only debug messages with that "type" value will be reported. For instance debug messages from the HFA driver are normally reported with type "HFA" (seen in the message).
//
//At the commandline this can also be set with --debug <value> as well as with --config CPL_DEBUG <value>.
//
//CPL_LOG
//
//CPL_LOG_ERRORS
//
//CPL_TIMESTAMP
//
//CPL_MAX_ERROR_REPORTS
//
//CPL_ACCUM_ERROR_MSG
//
//CPL_TMPDIR
//
//GDAL_DATA
//
//Path to directory containing various GDAL data files (EPSG CSV files, S-57 definition files, DXF header and footer files, ...).
//
//This option is read by the GDAL and OGR driver registration functions. It is used to expand EPSG codes into their description in the OSR model (WKT based).
//
//On some builds (Unix), the value can be hard-coded at compilation time to point to the path after installation (/usr/share/gdal/data for example). On Windows platform, this option must be generally declared.
//
//GDAL_DISABLE_CPLLOCALEC
//
//If set to YES (deafault is NO) this option will disable the normal behavior of the CPLLocaleC class which forces the numeric locale to "C" for selected chunks of code using the setlocale() call. Behavior of setlocale() in multi-threaded applications may be undependable but use of this option may result in problem formatting and interpreting numbers properly.
//
//GDAL_FILENAME_IS_UTF8
//
//This option only has an effect on Windows systems (using cpl_vsil_win32.cpp). If set to "NO" (default is YES) then filenames passed to functions like VSIFOpenL() will be passed on directly to CreateFile?() instead of being converted from UTF-8 to wchar_t and passed to CreateFileW(). This effectively restores the pre-GDAL1.8 behavior for handling filenames on Windows and might be appropriate for applications that treat filenames as being in the local encoding.
//
//GEOTIFF_CSV
//
//GDAL Options
//
//GDAL_DISABLE_READDIR_ON_OPEN
//
//Defaults to FALSE.
//
//By default (GDAL_DISABLE_READDIR_ON_OPEN=FALSE), GDAL establishes a list of all the files in the directory of the file passed to GDALOpen(). This can result in speed-ups in some use cases, but also to major slow downs when the directory contains thousands of other files. When set to TRUE, GDAL will not try to establish the list of files.
//
//GDAL_CACHEMAX
//
//This option controls the default GDAL raster block cache size. If its value is small (less than 10000 in 1.7 and earlier, 100000 in 1.8 and later), it is assumed to be measured in megabytes, otherwise in bytes. Note that this value is only consulted the first time the cache size is requested overriding the initial default (40MB in GDAL 1.7, but often changing for each release). To change this value programmatically during operation of the program it is better to use GDALSetCacheMax(int nNewSize) (always in bytes). Before GDAL 1.8.0, the maximum cache size that can be specified is 2 GB, so when specifying in megabytes, don't try setting values bigger than 2047. Since GDAL 1.8.0, it is possible to specify a 64bit value through GDAL_CACHEMAX or with GDALSetCacheMax64(GIntBig nNewSize) (but the maximum practical value on 32 bit OS is between 2 and 4 GB. It is the responsibility of the user to set a consistant value)
//
//GDAL_DRIVER_PATH
//
//Used by GDALDriverManager::AutoLoadDrivers?()
//
//This function will automatically load drivers from shared libraries. It searches the "driver path" for .so (or .dll) files that start with the prefix "gdal_X.so". It then tries to load them and then tries to call a function within them called GDALRegister_X() where the 'X' is the same as the remainder of the shared library basename ('X' is case sensitive), or failing that to call GDALRegisterMe().
//
//There are a few rules for the driver path. If the GDAL_DRIVER_PATH environment variable it set, it is taken to be a list of directories to search separated by colons on UNIX, or semi-colons on Windows. Otherwise the /usr/local/lib/gdalplugins directory, and (if known) the lib/gdalplugins subdirectory of the gdal home directory are searched on UNIX and $(BINDIR)\gdalplugins on Windows.
//
//This option must be set before calling GDALAllRegister(), or an explicit call to GDALDriverManager::AutoLoadDrivers?() will be required.
//
//GDAL_FORCE_CACHING
//
//Defaults to NO.
//
//When set to YES, the GDALDataset::RasterIO() and GDALRasterBand::RasterIO() will use cached IO (access block by block through GDALRasterBand::IReadBlock() API) instead of a potential driver specific implementation of IRasterIO(). This will only have an effect on drivers that specialize IRasterIO() at the dataset or raster band level, for example JP2KAK, NITF, HFA, WCS, ECW, MrSID, JPEG, ...
//
//GDAL_VALIDATE_CREATION_OPTIONS
//
//Defaults to YES.
//
//When using GDALDriver::Create() or GDALDriver::CreateCopy?() (for example when using the -co option of gdal_translate or gdalwarp utilies) , GDAL checks by default that the passed creation options match the syntax of the allowed options declared by the driver. If they don't match the syntax, a warning will be emitted. There should be hardly any reason to set this option to FALSE, as it's either a sign of a user typo, or an error in the declared options of the driver. The latter should be worth a bug report.
//
//GDAL_IGNORE_AXIS_ORIENTATION
//
//GMLJP2OVERRIDE
//
//GDAL_PAM_MODE
//
//GDAL_PAM_PROXY_DIR
//
//GDAL_MAX_DATASET_POOL_SIZE
//
//Defaults to 100. Used by gcore/gdalproxypool.cpp
//
//Number of datasets that can be opened simultaneously by the GDALProxyPool mechanism (used by VRT for example). Can be increased to get better random I/O performance with VRT mosaics made of numerous underlying raster files. Be careful : on Linux systems, the number of file handles that can be opened by a process is generally limited to 1024.
//
//GDAL_SWATH_SIZE
//
//Defaults to 10000000 (10 MB). Used by gcore/rasterio.cpp
//
//Size of the swath when copying raster data from one dataset to another one (in bytes). Should not be smaller than GDAL_CACHEMAX
//
//USE_RRD
//
//Defaults to NO. Used by gcore/gdaldefaultoverviews.cpp
//
//Can be set to YES to use Erdas Imagine format (.aux) as overview format. See  gdaladdo documentation.
//
//GeoTIFF driver options
//
//All the below options are documented in the  GTiff driver documentation.
//
//GTIFF_IGNORE_READ_ERRORS
//
//ESRI_XML_PAM
//
//JPEG_QUALITY_OVERVIEW
//
//GDAL_TIFF_INTERNAL_MASK
//
//GDAL_TIFF_INTERNAL_MASK_TO_8BIT
//
//TIFF_USE_OVR
//
//GTIFF_POINT_GEO_IGNORE
//
//GTIFF_REPORT_COMPD_CS
//
//GDAL_ENABLE_TIFF_SPLIT
//
//GDAL_TIFF_OVR_BLOCKSIZE
//
//OGR Options
//
//CENTER_LONG
//
//CHECK_WITH_INVERT_PROJ
//
//Used by ogr/ogrct.cpp and apps/gdalwarp.cpp. Added in GDAL/OGR 1.7.0
//
//This option can be used to control the behaviour of gdalwarp when warping global datasets or when transforming from/to polar projections, which causes coordinates discontinuities. See http://trac.osgeo.org/gdal/ticket/2305.
//
//The background is that proj.4 does not guarantee that converting from src_srs to dst_srs and then from dst_srs to src_srs will yield to the initial coordinates. This can cause to errors in the computation of the target bounding box of gdalwarp, or to visual artifacts.
//
//If CHECK_WITH_INVERT_PROJ option is not set, gdalwarp will check that the the computed coordinates of the edges of the target image are in the validity area of the target projection. If they are not, it will retry computing them by setting CHECK_WITH_INVERT_PROJ=TRUE that forces ogrct.cpp to check the consistency of each requested projection result with the invert projection.
//
//If set to NO, gdalwarp will behave like GDAL/OGR < 1.7.0
//
//THRESHOLD
//
//Used by ogr/ogrct.cpp. Added in GDAL/OGR 1.7.0
//
//Used in combination with CHECK_WITH_INVERT_PROJ=TRUE. Define the acceptable threshold used to check if the roundtrip from src_srs to dst_srs and from dst_srs to srs_srs yield to the initial coordinates. The value must be expressed in the units of the source SRS (typically degrees for a geographic SRS, meters for a projected SRS)
//
//OGR_ARC_STEPSIZE
//
//Used by OGR_G_CreateFromGML() (for gml:Arc and gml:Circle) and OGRGeometryFactory::approximateArcAngles() to stroke arc to linestrings. Defaults to 4 (degrees). Added in GDAL/OGR 1.8.0
//
//The largest step in degrees along the arc.
//
//OGR_ENABLE_PARTIAL_REPROJECTION
//
//Used by OGRLineString::transform(). Defaults to NO. Added in GDAL/OGR 1.8.0
//
//Can be set to YES to remove points that can't be reprojected. See #3758 for the purpose of this option.
//
//OGR_FORCE_ASCII
//
//Used by OGRGetXML_UTF8_EscapedString() function and by GPX, KML, GeoRSS and GML drivers. Defaults to YES
//
//Those XML based drivers should write UTF8 content. If they are provided with non UTF8 content, they will replace each non-ASCII character by '?' when OGR_FORCE_ASCII=YES.
//
//Set to NO to preserve the content, but beware that the resulting XML file will not be valid and will require manual edition of the encoding in the XML header.
//
//SHAPE_ENCODING
//
//Added in GDAL/OGR 1.9.0.
//
//Shapefile driver specific. This may be set to a OGR Character Encoding name in order to force all DBF files opened with the shapefile driver to be treated as having that encoding instead of trying to interpret the encoding setting of the file itself.
//
//DXF_ENCODING
//
//DXF driver specific. This may be set to a OGR Character Encoding name in order to force all DXF files opened with the DXF driver to be treated as having that encoding instead of trying to interpret the encoding setting of the file itself.
//
//GML_INVERT_AXIS_ORDER_IF_LAT_LONG
//
//GML driver specific. Added in GDAL/OGR 1.8.0. See  GML driver documentation. Also impacts WFS driver behaviour.
//
//GML_CONSIDER_EPSG_AS_URN
//
//GML driver specific. Added in GDAL/OGR 1.8.0. See  GML driver documentation. Also impacts WFS driver behaviour.
//
//GML_SAVE_RESOLVED_TO
//
//GML driver specific. Added in GDAL/OGR 1.8.0. See  GML driver documentation.
//
//GML_SKIP_RESOLVE_ELEMS
//
//GML driver specific. Added in GDAL/OGR 1.8.0. See  GML driver documentation.
//
//GML_FIELDTYPES
//
//GML driver specific. See  GML driver documentation.
//
//OGR_WFS_PAGING_ALLOWED
//
//WFS driver specific. Added in GDAL/OGR 1.8.0. See  WFS driver documentation.
//
//OGR_WFS_PAGE_SIZE
//
//WFS driver specific. Added in GDAL/OGR 1.8.0. See  WFS driver documentation.
//
//OGR_WFS_LOAD_MULTIPLE_LAYER_DEFN
//
//WFS driver specific. Added in GDAL/OGR 1.9.0. Can be set to OFF to prevent emitting DescribeFeatureType? for multiple layers at once (which is the new behaviour of the WFS driver in OGR 1.9.0 for better efficiency)
//
//OGR_EDIGEO_FONT_SIZE_FACTOR
//
//EDIGEO driver specific. Added in GDAL/OGR 1.9.0. See  EDIGEO driver documentation.
//
//OGR_EDIGEO_CREATE_LABEL_LAYERS
//
//EDIGEO driver specific. Added in GDAL/OGR 1.9.0. See  EDIGEO driver documentation.
//
//OGR_EDIGEO_RECODE_TO_UTF8
//
//EDIGEO driver specific. Added in GDAL/OGR 1.9.0. Can be set to OFF to prevent recoding strings in LATIN-1 to UTF-8.
