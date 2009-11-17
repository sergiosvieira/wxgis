#include "wxgis/catalog/gxfolder.h"
#include "../../art/folder1_16.xpm"
#include "../../art/folder1_48.xpm"

wxGxFolder::wxGxFolder(wxString Path, wxString Name, bool bShowHidden) : m_bShowHidden(bShowHidden), m_bIsChildrenLoaded(false)
{
	m_sName = Name;
	m_sPath = Path;
}

wxGxFolder::~wxGxFolder(void)
{
}

void wxGxFolder::Detach(void)
{
	EmptyChildren();
}

void wxGxFolder::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
}

void wxGxFolder::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Detach();
		delete m_Children[i];
	}
	m_Children.empty();
	m_bIsChildrenLoaded = false;
}

void wxGxFolder::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;
	wxDir dir(m_sPath);
	if(dir.IsOpened())
	{
		WXDWORD style = wxDIR_FILES | wxDIR_DIRS;
		if(m_bShowHidden)
			style |= wxDIR_HIDDEN;

		dir.Traverse(*this, wxEmptyString, style );
	}

	//load names
	GxObjectArray Array;	
	if(m_pCatalog->GetChildren(m_sPath, &m_FileNames, &Array))
	{
		for(size_t i = 0; i < Array.size(); i++)
		{
			bool ret_code = AddChild(Array[i]);
			if(!ret_code)
				wxDELETE(Array[i]);
		}
	}
	m_bIsChildrenLoaded = true;
}

wxIcon wxGxFolder::GetLargeImage(void)
{
	return wxIcon(folder1_48_xpm);
}

wxIcon wxGxFolder::GetSmallImage(void)
{
	return wxIcon(folder1_16_xpm);
}

bool wxGxFolder::Delete(void)
{
	if(wxFileName::Rmdir(m_sPath))
	{
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pParent);
		if(pGxObjectContainer == NULL)
			return false;
		return pGxObjectContainer->DeleteChild(this);		
	}
	else
		return false;	
}

bool wxGxFolder::Rename(wxString NewName)
{
	//rename ?
	m_sName = NewName; 
	m_pCatalog->ObjectChanged(this);
	return true;
}

void wxGxFolder::EditProperties(wxWindow *parent)
{
}

wxDirTraverseResult wxGxFolder::OnFile(const wxString& filename)
{
	m_FileNames.Add(filename);
	return wxDIR_CONTINUE;
}

wxDirTraverseResult wxGxFolder::OnDir(const wxString& dirname)
{
	m_FileNames.Add(dirname);
	return wxDIR_IGNORE;
}

bool wxGxFolder::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(this);
	return true;		

}
