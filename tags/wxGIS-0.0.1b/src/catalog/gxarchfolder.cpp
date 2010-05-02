/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchive classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
#include "wxgis/catalog/gxarchfolder.h"
#include "wxgis/datasource/sysop.h"

#include "../../art/folder_arch1_16.xpm"
#include "../../art/folder_arch1_48.xpm"

#include "cpl_vsi_virtual.h"

/////////////////////////////////////////////////////////////////////////
// wxGxArchive
/////////////////////////////////////////////////////////////////////////

wxGxArchive::wxGxArchive(wxString Path, wxString Name, bool bShowHidden, wxString sType) : wxGxFolder(Path, Name, bShowHidden)
{
    m_sType = sType;
    m_pMBConv = new wxCSConv(wxT("cp-866"));
}

wxGxArchive::~wxGxArchive(void)
{
    wxDELETE(m_pMBConv);
}

wxIcon wxGxArchive::GetLargeImage(void)
{
	return wxIcon(folder_arch1_48_xpm);
}

wxIcon wxGxArchive::GetSmallImage(void)
{
	return wxIcon(folder_arch1_16_xpm);
}

void wxGxArchive::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxBusyCursor wait;

    wxString sArchPath = m_sType + m_sPath;//wxT("/vsizip/") + wxT("/");
    char **papszFileList = VSIReadDir(wgWX2MB(sArchPath));

    if( CSLCount(papszFileList) == 0 )
    {
        wxLogMessage(wxT( "wxGxArchive: no files or directories" ));
    }
    else
    {
        //wxLogDebug(wxT("Files: %s"), wgMB2WX(papszFileList[0]) );
       	//wxArrayString FileNames;
        for(int i = 0; papszFileList[i] != NULL; i++ )
		{
            wxString sFileName(papszFileList[i], *m_pMBConv);
//			wxString sFileName = wgMB2WX(papszFileList[i]);
            VSIStatBufL BufL;
			wxString sFolderPath = sArchPath + wxT("/") + sFileName;

            //int ret = VSIStatL(wgWX2MB(sFolderPath), &BufL);
            int ret = VSIStatL((const char*) sFolderPath.mb_str(*m_pMBConv), &BufL);
            if(ret == 0)
            {
                //int x = 0;
                if(VSI_ISDIR(BufL.st_mode))
                {
					wxGxArchiveFolder* pFolder = new wxGxArchiveFolder(sFolderPath, sFileName, m_pCatalog->GetShowHidden());
					IGxObject* pGxObj = static_cast<IGxObject*>(pFolder);
					bool ret_code = AddChild(pGxObj);
                }
                else
                {
                    m_FileNames.Add(sFolderPath);
                }
            }
		}
    }
    CSLDestroy( papszFileList );

	//load names
	GxObjectArray Array;	
	if(m_pCatalog->GetChildren(sArchPath, &m_FileNames, &Array))
	{
		for(size_t i = 0; i < Array.size(); i++)
		{
            IGxDataset* pDSet = dynamic_cast<IGxDataset*>(Array[i]);
            if(pDSet)
                pDSet->SetPathEncoding(m_pMBConv);
			bool ret_code = AddChild(Array[i]);
			if(!ret_code)
				wxDELETE(Array[i]);
		}
	}
	m_bIsChildrenLoaded = true;
}

bool wxGxArchive::Delete(void)
{
    if(DeleteFile(m_sPath, wxConvCurrent))
	{
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pParent);
		if(pGxObjectContainer == NULL)
			return false;
		return pGxObjectContainer->DeleteChild(this);		
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
        wxLogError(_("Delete failed! OGR error: %s, file '%s'"), wgMB2WX(err), m_sPath.c_str());
		return false;	
    }
}

bool wxGxArchive::Rename(wxString NewName)
{
	//rename ?
	m_sName = NewName; 
	m_pCatalog->ObjectChanged(this);
	return true;
}

void wxGxArchive::EditProperties(wxWindow *parent)
{
}

/////////////////////////////////////////////////////////////////////////
// wxGxArchiveFolder
/////////////////////////////////////////////////////////////////////////

wxGxArchiveFolder::wxGxArchiveFolder(wxString Path, wxString Name, bool bShowHidden) : wxGxFolder(Path, Name, bShowHidden)
{
    m_pMBConv = new wxCSConv(wxT("cp-866"));
}

wxGxArchiveFolder::~wxGxArchiveFolder(void)
{
    wxDELETE(m_pMBConv);
}

wxIcon wxGxArchiveFolder::GetLargeImage(void)
{
	return wxIcon(folder_arch1_48_xpm);
}

wxIcon wxGxArchiveFolder::GetSmallImage(void)
{
	return wxIcon(folder_arch1_16_xpm);
}

void wxGxArchiveFolder::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxBusyCursor wait;
    //VSIFilesystemHandler *poFSHandler = VSIFileManager::GetHandler( wgWX2MB(m_sPath) );
    //char **res = poFSHandler->ReadDir(wgWX2MB(m_sPath));

    wxString sArchPath = m_sPath;
    char **papszFileList = VSIReadDir(wgWX2MB(sArchPath));

    if( CSLCount(papszFileList) == 0 )
    {
        wxLogMessage(wxT( "wxGxArchive: no files or directories" ));
    }
    else
    {
        //wxLogDebug(wxT("Files: %s"), wgMB2WX(papszFileList[0]) );
       	//wxArrayString FileNames;
        for(int i = 0; papszFileList[i] != NULL; i++ )
		{
            wxString sFileName(papszFileList[i], *m_pMBConv);


			//wxString sFileName = wgMB2WX(papszFileList[i]);
            //if(i > 0)
            //    wxLogDebug( wxT("       %s"), sFileName.c_str() );
            VSIStatBufL BufL;
			wxString sFolderPath = sArchPath + wxT("/") + sFileName;
            int ret = VSIStatL((const char*) sFolderPath.mb_str(*m_pMBConv), &BufL);
            //int ret = VSIStatL(wgWX2MB(sFolderPath), &BufL);
            if(ret == 0)
            {
                //int x = 0;
                if(VSI_ISDIR(BufL.st_mode))
                {
					wxGxArchiveFolder* pFolder = new wxGxArchiveFolder(sFolderPath, sFileName, m_pCatalog->GetShowHidden());
					IGxObject* pGxObj = static_cast<IGxObject*>(pFolder);
					bool ret_code = AddChild(pGxObj);
                }
                else
                {
                    m_FileNames.Add(sFolderPath);
                }
            }
		}
    }
    CSLDestroy( papszFileList );

	//load names
	GxObjectArray Array;	
	if(m_pCatalog->GetChildren(sArchPath, &m_FileNames, &Array))
	{
		for(size_t i = 0; i < Array.size(); i++)
		{
            IGxDataset* pDSet = dynamic_cast<IGxDataset*>(Array[i]);
            if(pDSet)
                pDSet->SetPathEncoding(m_pMBConv);
			bool ret_code = AddChild(Array[i]);
			if(!ret_code)
				wxDELETE(Array[i]);
		}
	}
	m_bIsChildrenLoaded = true;
}


    //std::auto_ptr<wxInputStream> in_stream(new wxFFileInputStream(m_sPath));

    //if (in_stream->IsOk())
    //{
    //    wxString sExt = m_sPath;
    //    // look for a filter handler, e.g. for '.gz'
    //    const wxFilterClassFactory *fcf = wxFilterClassFactory::Find(m_sPath, wxSTREAM_FILEEXT);
    //    if (fcf) 
    //    {
    //        in_stream.reset(fcf->NewStream(in_stream.release()));
    //        // pop the extension, so if it was '.tar.gz' it is now just '.tar'
    //        sExt = fcf->PopExtension(m_sPath);
    //    }

    //    // look for a archive handler, e.g. for '.zip' or '.tar'
    //    const wxArchiveClassFactory *acf = wxArchiveClassFactory::Find(sExt, wxSTREAM_FILEEXT);
    //    wxString sProtocol = acf->GetProtocol();
    //    if (acf) 
    //    {
    //        std::auto_ptr<wxArchiveInputStream> arc(acf->NewStream(in_stream.release()));
    //        std::auto_ptr<wxArchiveEntry> entry;

    //        // list the contents of the archive
    //       	wxArrayString FileNames;
    //        wxString sParentPath = wxString::Format(wxT("/vsi%s/"), sProtocol.c_str()) + m_sPath;

    //        while ((entry.reset(arc->GetNextEntry())), entry.get() != NULL)
    //        {
    //            int x;
    //            if(entry->IsDir())
    //                x = 0;
    //            else
    //            {
    //                wxString tmp = entry->GetName();
    //                tmp.Replace(wxT("\\"), wxT("/"));
    //                wxString sFileName = sParentPath + wxFileName::GetPathSeparator() + tmp;//entry->GetName();
    //                sFileName.Replace(wxT("\\"), wxT("/"));
    //                FileNames.Add(sFileName);
    //                //x = 1;
    //            }
    //            //wxLogDebug(wxT("wxGxArchiveFolder:%s"), entry->GetName().c_str());
    //        ////    wxFileName dirname( name, wxEmptyString );
    //        ////    x = 1;
    //            //std::wcout << entry->GetName().c_str() << "\n";
    //        }
	   //     GxObjectArray Array;	
    //         
    //        //wxString name, ext;
    //        //wxFileName::SplitPath(m_sPath, NULL, NULL, &name, &ext);

	   //     if(m_pCatalog->GetChildren(sParentPath, &FileNames, &Array))
	   //     {
		  //      for(size_t i = 0; i < Array.size(); i++)
		  //      {
			 //       bool ret_code = AddChild(Array[i]);
			 //       if(!ret_code)
				//        wxDELETE(Array[i]);
		  //      }
	   //     }
	   //     m_bIsChildrenLoaded = true;
    //    }
    //    else 
    //    {
    //        wxLogError(_("wxGxArchiveFolder: can't handle '%s'"), m_sPath.c_str());
    //    }
    //}

    
    ////auto_ptr<wxArchiveClassFactory> factory(wxArchiveClassFactory::Find(m_sPath, wxSTREAM_FILEEXT));
    ////if (factory.get())
    ////{
    ////    auto_ptr<wxArchiveInputStream> inarc(factory->NewStream(new wxFFileInputStream(m_sPath)));
    ////    std::auto_ptr<wxArchiveEntry> entry;
    ////    while (entry.reset(arch.GetNextEntry()), entry.get() != NULL)
    ////    {
    ////        // access meta-data
    ////        wxString name = entry->GetName();
    ////        // read 'archive' to access the entry's data
    ////        int x;
    ////        if(entry->IsDir())
    ////            x = 0;
    ////        else
    ////            x = 1;
    ////    ////    wxFileName dirname( name, wxEmptyString );
    ////    ////    x = 1;
    ////    }
    ////}



//    wxFileSystem fs;
//    wxString filename;
//    wxFileSystem::AddHandler(new wxArchiveFSHandler);
////    wxFileName dirname( wxT("D:\\work\\Projects\\wxGIS\\build\\debug\\sys\\cs.zip#zip:"), wxEmptyString );
//    //wxString sFileName = wxFileSystem::FileNameToURL(dirname);
//    //wxString wildcard = sFileName;//wxT("");//document.template.dat#zip:*.txt
//    wxString wildcard = wxT("D:\\work\\Projects\\wxGIS\\build\\debug\\sys\\cs.zip#zip:cs/*");//dirname.GetFullName();
//    if(wxFileSystem::HasHandlerForPath(wildcard))//sFileName
//    {
//        for(filename = fs.FindFirst(wildcard, 0); !filename.IsEmpty(); filename = fs.FindNext())
//        {
//            wxFileName sname( filename, wxEmptyString );
//            int x;
//            if(sname.IsDir())
//                x = 0;
//            else
//                x = 1;
//        }
//    }

    //wxFileSystem::AddHandler(new wxArchiveFSHandler);
    //wxFileName dirname( wxT("D:\\work\\Projects\\wxGIS\\build\\debug\\sys\\cs.zip"), wxEmptyString );
    //wxString sFileName = wxFileSystem::FileNameToURL(dirname);
    //if(wxFileSystem::HasHandlerForPath(sFileName))
    //{
    //    wxDir dir(sFileName);
    //    int x;
    //    if ( !dir.IsOpened() )
    //        x = 0;
    //    else
    //        x = 1;
    //}

