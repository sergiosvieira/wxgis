#include "wxgis/cat_app/catalogframe.h"

//-----------------------------------------------------------
// wxGISCatalogFrame
//-----------------------------------------------------------


BEGIN_EVENT_TABLE(wxGISCatalogFrame, wxGxApplication)
END_EVENT_TABLE()


wxGISCatalogFrame::wxGISCatalogFrame(IGISConfig* pConfig, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGxApplication(pConfig, parent, id, title, pos, size, style)
{
	//set app main icon
	SetIcon(wxIcon(mainframe_xpm));
}

wxGISCatalogFrame::~wxGISCatalogFrame(void)
{
}

void wxGISCatalogFrame::OnAppAbout(void)
{
	wxMessageBox(wxString::Format(_("wxGIS [%s]\nVersion: %s\n(c) 2009 Dmitry Barishnikov (bishop)"), APP_NAME, APP_VER), _("About"), wxICON_INFORMATION | wxOK);
}

