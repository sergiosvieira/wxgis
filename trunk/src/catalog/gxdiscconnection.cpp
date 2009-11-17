#include "wxgis/catalog/gxdiscconnection.h"
#include "../../art/folder_conn_16.xpm"
#include "../../art/folder_conn_48.xpm"
#include <wx/filename.h>

wxGxDiscConnection::wxGxDiscConnection(wxString Path, wxString Name, bool bShowHidden) : wxGxFolder(Path, Name, bShowHidden)
{
	m_ImageListSmall.Create(16, 16);
	m_ImageListSmall.Add(wxBitmap(folder_conn_16_xpm));
	m_ImageListLarge.Create(48, 48);
	m_ImageListLarge.Add(wxBitmap(folder_conn_48_xpm));
}

wxGxDiscConnection::~wxGxDiscConnection(void)
{
}

wxIcon wxGxDiscConnection::GetLargeImage(void)
{
	bool bIsOk = wxFileName::IsDirReadable(m_sPath);
	if(bIsOk)
		return wxIcon(m_ImageListSmall.GetIcon(0));
	else
		return wxIcon(m_ImageListSmall.GetIcon(1));
}

wxIcon wxGxDiscConnection::GetSmallImage(void)
{
	bool bIsOk = wxFileName::IsDirReadable(m_sPath);
	if(bIsOk)
		return wxIcon(m_ImageListLarge.GetIcon(0));
	else
		return wxIcon(m_ImageListLarge.GetIcon(1));
}

bool wxGxDiscConnection::Delete(void)
{
	return true;
}

bool wxGxDiscConnection::Rename(wxString NewName)
{
	m_sName = NewName; 
	return true;
}

void wxGxDiscConnection::EditProperties(wxWindow *parent)
{
}
