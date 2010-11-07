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

bool wxGISMiscPropertyPage::Create(IApplication* application, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    wxPanel::Create(parent, id, pos, size, style, name);

    m_pApp = application;
    if(!m_pApp)
        return false;

    m_pGISConfig = dynamic_cast<wxGISAppConfig*>(m_pApp->GetConfig());
    if(!m_pGISConfig)
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
    m_LocalePath->ChangeValue( m_pGISConfig->GetLocaleDir() );
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
    FillLangArray(m_pGISConfig->GetLocaleDir());
	m_LangChoice = new wxChoice( this, ID_LANGCHOICE, wxDefaultPosition, wxDefaultSize, m_aLangs, 0 );
	//m_LangChoice->SetSelection( 0 );
    m_LangChoice->SetStringSelection(m_pGISConfig->GetLocale());
	bMainSizer->Add( m_LangChoice, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("wxGIS system files folder path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bMainSizer->Add( m_staticText3, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_SysPath = new wxTextCtrl( this, ID_SYSPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_SysPath->ChangeValue( m_pGISConfig->GetSysDir() );
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
    m_LogPath->ChangeValue( m_pGISConfig->GetLogDir() );
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
	
	m_staticTextWarn = new wxStaticText( this, wxID_ANY, _("To apply some changes on this page needed application restart"), wxDefaultPosition, wxDefaultSize, 0 );
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
    if(m_LangChoice->GetStringSelection() != m_pGISConfig->GetLocale() && m_LangChoice->GetStringSelection().IsEmpty() == false)
    {
        m_pApp->SetupLoc(m_LangChoice->GetStringSelection(), m_LocalePath->GetValue());
	    m_pGISConfig->SetLocale(m_LangChoice->GetStringSelection());
	    m_pGISConfig->SetLocaleDir(m_LocalePath->GetValue());
    }
    if(m_LogPath->IsModified())
    {
        m_pApp->SetupLog(m_LogPath->GetValue());
       	m_pGISConfig->SetLogDir(m_LogPath->GetValue());
    }
    if(m_SysPath->IsModified())
    {
        m_pApp->SetupSys(m_SysPath->GetValue());
	    m_pGISConfig->SetSysDir(m_SysPath->GetValue());
    }
    if(m_checkDebug->GetValue() != m_pGISConfig->GetDebugMode())
    {
        m_pApp->SetDebugMode(m_checkDebug->GetValue());
	    m_pGISConfig->SetDebugMode(m_checkDebug->GetValue());
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
	wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a folder with locale files")), m_pGISConfig->GetLocaleDir(), wxDD_DEFAULT_STYLE |wxDD_DIR_MUST_EXIST );
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
    wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a wxGIS system files folder path")), m_pGISConfig->GetSysDir(), wxDD_DEFAULT_STYLE |wxDD_DIR_MUST_EXIST );
	if(dlg.ShowModal() == wxID_OK)
	{
		wxString sPath = dlg.GetPath();
        m_SysPath->ChangeValue( sPath );
        m_SysPath->SetModified(true);
	}
}

void wxGISMiscPropertyPage::OnOpenLogPath(wxCommandEvent& event)
{
	wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a wxGIS log files folder path")), m_pGISConfig->GetLogDir(), wxDD_DEFAULT_STYLE |wxDD_DIR_MUST_EXIST );
	if(dlg.ShowModal() == wxID_OK)
	{
		wxString sPath = dlg.GetPath();
        m_LogPath->ChangeValue( sPath );
        m_LogPath->SetModified(true);
	}
}

