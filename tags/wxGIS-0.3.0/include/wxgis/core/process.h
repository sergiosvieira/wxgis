/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  external process common classes.
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
#pragma once

#include "wxgis/core/core.h"

#include "wx/txtstrm.h"
#include "wx/thread.h"



/** \class wxStreamReaderThread process.h
 *  \brief The stream reader thread for process callback.
 */
class WXDLLIMPEXP_GIS_CORE wxStreamReaderThread : public wxThread
{
public:
    wxStreamReaderThread(IStreamReader* pReader, wxInputStream& InStream);
	virtual ~wxStreamReaderThread(void);
    virtual void *Entry();
    virtual void OnExit();
protected:
	IStreamReader* m_pReader;
	wxInputStream& m_InStream;
};

/** \class wxGISProcess process.h
 *  \brief The process class which stores the application execution data.
 */
class WXDLLIMPEXP_GIS_CORE wxGISProcess : 
	public IProcess,
	public IStreamReader
{
public:
    wxGISProcess(wxString sCommand, IProcessParent* pParent);
    virtual ~wxGISProcess(void);
    // IProcess
    virtual void OnTerminate(int pid, int status);
    virtual void OnStart(long nPID);
    virtual void OnCancel(void);
	//IStreamReader
	virtual void ProcessInput(wxString sInputData);
protected:
    IProcessParent* m_pParent;
	wxStreamReaderThread* m_pReadThread;
};