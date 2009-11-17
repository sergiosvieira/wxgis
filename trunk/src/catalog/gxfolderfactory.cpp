#include "wxgis/catalog/gxfolderfactory.h"
#include "wxgis/catalog/gxfolder.h"
#include <wx/filename.h>
#include <wx/dir.h>


IMPLEMENT_DYNAMIC_CLASS(wxGxFolderFactory, wxObject)

wxGxFolderFactory::wxGxFolderFactory(void)
{
}

wxGxFolderFactory::~wxGxFolderFactory(void)
{
}

bool wxGxFolderFactory::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
{
	for(size_t i = 0; i < pFileNames->GetCount(); i++)
	{
		wxString path = pFileNames->Item(i);
		if(wxFileName::DirExists(path))
		{
			//wxDir dir(path);
			//wxFileName dir = wxFileName::DirName(path);
			wxString name, ext;
			wxFileName::SplitPath(path, NULL, NULL, &name, &ext);
			//wxString name = dir.GetName();
			wxGxFolder* pFolder = new wxGxFolder(path, name + ext, m_pCatalog->GetShowHidden());
			IGxObject* pGxObj = static_cast<IGxObject*>(pFolder);
			pObjArray->push_back(pGxObj);
			pFileNames->RemoveAt(i);
			i--;
		}
	}
	return true;
}