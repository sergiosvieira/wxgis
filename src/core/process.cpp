/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  stream common classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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

///////////////////////////////////////////////////////////////////////////////
/// Class wxStreamReaderThread
///////////////////////////////////////////////////////////////////////////////

wxStreamReaderThread::wxStreamReaderThread(IStreamReader* pReader, wxInputStream& InStream) : wxThread(), m_InStream(InStream)
{
	m_pReader = pReader;
}

wxStreamReaderThread::~wxStreamReaderThread(void)
{
}

void *wxStreamReaderThread::Entry()
{
  //  if(!m_pInTxtStream)
		//return (ExitCode)-1;
	wxTextInputStream InputStr(m_InStream);
	while(!TestDestroy() && m_InStream.IsOk() && !m_InStream.Eof())
    {
		wxString line = InputStr.ReadLine();
		m_pReader->ProcessInput(line);
		wxThread::Sleep(100);
    }
	return NULL;
}

void wxStreamReaderThread::OnExit()
{
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISProcess
///////////////////////////////////////////////////////////////////////////////

wxGISProcess::wxGISProcess(wxString sCommand, IProcessParent* pParent) : IProcess(sCommand), m_pReadThread(NULL)
{
    m_pParent = pParent;
}

wxGISProcess::~wxGISProcess(void)
{
}

void wxGISProcess::OnStart(long nPID)
{
	m_nState = enumGISTaskWork;
	m_dtBeg = wxDateTime::Now();
	m_pid = nPID;
	//create and start read thread stdin
	if(IsInputOpened())
	{
		if(m_pReadThread)
			m_pReadThread->Delete();
		m_pReadThread = new wxStreamReaderThread(this, *GetInputStream());
		if(!CreateAndRunThread(m_pReadThread, wxT("wxGISProcess"), _("Stream Read")))
			wxLogError(_("The execution communication is unavalible!"));
	}
    //start err thread ?
}

void wxGISProcess::OnTerminate(int pid, int status)
{
	if(m_pReadThread)
		m_pReadThread->Delete();
	m_pReadThread = NULL;
    if(m_pParent && m_nState == enumGISTaskWork)
        m_pParent->OnFinish(this, status != 0);
	m_nState = status == 0 ? enumGISTaskDone : enumGISTaskError;
}

void wxGISProcess::OnCancel(void)
{
	if(m_nState == enumGISTaskWork)
	{
		////send cancel code
		//wxTextOutputStream OutStr(*GetOutputStream());
		//wxString sStopCmd(wxT("STOP"));
		//OutStr.WriteString(sStopCmd);

		wxKillError eErr = Kill(m_pid, wxSIGKILL);// wxSIGINT wxSIGTERM
 
		if(m_pReadThread)
			m_pReadThread->Delete();
 		m_pReadThread = NULL;
	   //and detach
		Detach();
	}
	m_nState = enumGISTaskPaused;
}

void wxGISProcess::ProcessInput(wxString sInputData)
{
}