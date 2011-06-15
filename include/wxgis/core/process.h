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

#include "wxgis/core/process.hpp" 

namespace bp = ::boost::process;

class wxProcessWaitThread;

/** \class wxGISProcess process.h
 *  \brief The process class which stores the application execution data.
 */
class WXDLLIMPEXP_GIS_CORE wxGISProcess : 
	public IProcess
{
public:
    wxGISProcess(wxString sCommand, wxArrayString saParams, IProcessParent* pParent);
    virtual ~wxGISProcess(void);
    // IProcess
    virtual void OnStart(void);
    virtual void OnCancel(void);
    virtual void OnTerminate(int status);
	//IStreamReader
	virtual void ProcessInput(wxString sInputData);
protected:    
    IProcessParent* m_pParent;
    wxProcessWaitThread* m_pProcessWaitThread;
};

///** \class wxStreamReaderThread process.h
// *  \brief The stream reader thread for process callback.
// */
//class wxStreamReaderThread : public wxThread
//{
//public:
//    wxStreamReaderThread(wxGISProcess* pProc, bp::pistream &InputStream);
//	virtual ~wxStreamReaderThread(void);
//    virtual void *Entry();
//    virtual void OnExit();
//protected:
//	wxGISProcess* m_pProc;
//    bp::pistream &m_InputStream;
//	//IStreamReader* m_pReader;
//	//wxInputStream& m_InStream;
//};

/** \class wxProcessWaitThread process.h
 *  \brief The end of the process wait thread for process callback.
 */
class wxProcessWaitThread : public wxThread
{
public:
    wxProcessWaitThread(wxGISProcess* pProc);
	virtual ~wxProcessWaitThread(void);
    virtual void *Entry();
    virtual void OnExit();
    virtual void Terminate(void);
protected:
	wxGISProcess* m_pProc;
    bp::child *m_pChild;
};
