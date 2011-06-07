/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  stream common classes.
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

#include "wxgis/core/process.h"

#include "cpl_string.h"
#include "wx/filename.h"


/////////////////////////////////////////////////////////////////////////////////
///// Class wxStreamReaderThread
/////////////////////////////////////////////////////////////////////////////////
//
//wxStreamReaderThread::wxStreamReaderThread(wxGISProcess* pProc, bp::pistream &InputStream) : wxThread(), m_InputStream(InputStream)
//{
//	m_pProc = pProc;
//}
//
//wxStreamReaderThread::~wxStreamReaderThread(void)
//{
//}
//
//void *wxStreamReaderThread::Entry()
//{
//    if(!m_pProc)
//        return (ExitCode)-1;
//
//    std::string line; 
//    while(!TestDestroy() && std::getline(m_InputStream, line)) 
//    {
//        wxString sLine;//(line.c_str(), wxConvUTF8(), line.length());
//        m_pProc->ProcessInput(sLine);
//        wxThread::Sleep(85);
//    }
//	return NULL;
//}
//
//void wxStreamReaderThread::OnExit()
//{
//}

///////////////////////////////////////////////////////////////////////////////
/// Class wxStreamReaderThread
///////////////////////////////////////////////////////////////////////////////

wxProcessWaitThread::wxProcessWaitThread(wxGISProcess* pProc) : wxThread(), m_pChild(NULL)
{
	m_pProc = pProc;
}

wxProcessWaitThread::~wxProcessWaitThread(void)
{
}

void *wxProcessWaitThread::Entry()
{
    if(!m_pProc)
        return (ExitCode)-1;

    wxString sCmd = m_pProc->GetCommand();
    wxFileName FName(sCmd);
    std::string exec;
    if(FName.GetPath().IsEmpty())
        exec = bp::find_executable_in_path(std::string(sCmd.mb_str()));
    else
        exec = std::string(sCmd.mb_str());

    std::vector<std::string> args;
    wxArrayString saParams = m_pProc->GetParameters();
    for(size_t i = 0; i < saParams.GetCount(); i++)
        args.push_back(std::string(saParams[i].mb_str())); 

    //"wxGISGeoprocess.exe -n create_ovr -p "My geodata\tsk_exe\08MAY01083427-S2AS_R1C1-005728810020_01_P001.TIF|GAUSS|JPEG|""

//#if defined(BOOST_POSIX_API) 
    bp::context ctx; 
//#elif defined(BOOST_WINDOWS_API) 
//    bp::win32_context ctx; 
//    STARTUPINFOA si; 
//    ::ZeroMemory(&si, sizeof(si)); 
//    si.cb = sizeof(si); 
 //   si.dwFlags |= STARTF_USESHOWWINDOW;
 //   si.wShowWindow = SW_HIDE;//SW_SHOW;//
	//si.dwX = si.dwY = 5000;
//    ctx.startupinfo = &si;
//#else 
//#  error "Unsupported platform." 
//#endif

    ctx.environment = bp::self::get_environment(); 
    ctx.stdout_behavior = bp::capture_stream(); 
    //wxFileName FName(wxString(exec.c_str(), wxConvUTF8));
    //ctx.work_directory = FName.GetPath().mb_str();

    bp::child c = bp::launch(exec, args, ctx);
    m_pChild = &c;

    bp::pistream &is = c.get_stdout(); 

    std::string line;
    while(!TestDestroy() && std::getline(is, line)) 
    {
        wxString sLine(line.c_str(), wxConvUTF8, line.length());
        m_pProc->ProcessInput(sLine);
        wxThread::Sleep(85);
    }

    //wait exit
    bp::status st = c.wait();

    m_pProc->OnTerminate( st.exited() ? st.exit_status() : EXIT_FAILURE );
	return NULL;
}

void wxProcessWaitThread::Terminate(void)
{
    if(m_pChild)
        m_pChild->terminate(true);
}


void wxProcessWaitThread::OnExit()
{
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISProcess
///////////////////////////////////////////////////////////////////////////////

wxGISProcess::wxGISProcess(wxString sCommand, wxArrayString saParams, IProcessParent* pParent) : IProcess(sCommand, saParams), m_pProcessWaitThread(NULL)
{
    m_pParent = pParent;
}

wxGISProcess::~wxGISProcess(void)
{
}

void wxGISProcess::OnStart(void)
{
	m_nState = enumGISTaskWork;
	m_dtBeg = wxDateTime::Now();
    
	//create and start thread
	if(m_pProcessWaitThread)
		m_pProcessWaitThread->Delete();

	m_pProcessWaitThread = new wxProcessWaitThread(this);
	if(!CreateAndRunThread(m_pProcessWaitThread, wxT("wxGISProcess"), _("Stream Wait")))
		wxLogError(_("The execution communication is unavalible!"));
}

void wxGISProcess::OnTerminate(int status)
{
    if(m_pParent && m_nState == enumGISTaskWork)
        m_pParent->OnFinish(this, status != 0);
	m_nState = status == 0 ? enumGISTaskDone : enumGISTaskError;
    m_dtEstEnd = wxDateTime::Now();
}

void wxGISProcess::OnCancel(void)
{
	if(m_nState == enumGISTaskWork)
	{
        if(m_pProcessWaitThread)
            m_pProcessWaitThread->Terminate();
	}
	m_nState = enumGISTaskPaused;
    m_dtEstEnd = wxDateTime::Now();
}

void wxGISProcess::ProcessInput(wxString sInputData)
{
}