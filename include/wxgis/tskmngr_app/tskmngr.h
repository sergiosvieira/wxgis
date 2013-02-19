/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Task manager class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Bishop
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

#include "wxgis/base.h"
#include "wxgis/core/config.h"
#include "wxgis/core/app.h"
#include "wxgis/tskmngr_app/net.h"

#include <wx/ffile.h>
#include <wx/hashmap.h>

#define TSKDIR wxT("tasks")
#define SUBTSKDIR wxT("subtasks")

class wxGISTaskCategory;
WX_DECLARE_STRING_HASH_MAP(wxGISTaskCategory*, wxGISTaskCategoryMap);

/** \class wxGISTaskManager tskmngr.h
    \brief The main task manager class.
*/

class wxGISTaskManager : 
    public wxObject, 
    public INetEventProcessor
{
    DECLARE_CLASS(wxGISTaskManager)
    friend class wxGISTaskCategory;
public:
	wxGISTaskManager( void );
	~wxGISTaskManager(void);
    void LoadTasks(const wxString & sPath);
    void SetExitState(wxGISNetCommandState nExitState);
    //
    void Exit(void);
    bool Init(void);
    //INetEventProcessor
    virtual void ProcessNetEvent(wxGISNetEvent& event);
    void SendNetMessage(wxNetMessage & msg, int nId = wxNOT_FOUND); 
protected:
    void OnExit(void);
    bool AddTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    bool DelTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    bool ChangeTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    bool StartTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    bool StopTask(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    bool ChangeTaskPriority(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    bool GetTasks(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    void GetTaskDetails(const wxXmlNode* pTaskNode, int nId);
    bool ValidateTask(const wxXmlNode* pTaskNode);
    bool SetMaxExecTasks(const wxXmlNode* pTaskNode, int nId, wxString &sErrMsg);
    int GetNewId(void);
    wxString GetTaskConfigPath(const wxString& sCatName);
    wxString ReplaceForbiddenCharsInFileName(const wxString & name, const wxString ch = wxT("_") );
    int GetExecTaskCount(void);
protected:
    wxFFile m_LogFile;
    wxLocale* m_pLocale; // locale we'll be using
	//char* m_pszOldLocale;
    wxGISLocalNetworkService* m_pNetworkService;
    wxString m_sUserConfigDir;
    wxGISNetCommandState m_nExitState;
    wxCriticalSection m_TaskLock;
    wxCriticalSection m_CounterLock;
    int m_nIdCounter;
    wxGISTaskCategoryMap m_omCategories;
};
