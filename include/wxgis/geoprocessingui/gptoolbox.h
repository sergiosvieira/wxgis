/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  toolbox classes.
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
#pragma once

#include "wxgis/catalog/catalog.h"
#include "wxgis/geoprocessing/gptoolmngr.h"

/////////////////////////////////////////////////////////////////////////
// wxGxToolbox
/////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GPU wxGxToolbox :
	public IGxObjectUI,
	public IGxObjectContainer
{
public:
	wxGxToolbox(wxGISGPToolManager* pToolMngr = NULL, wxXmlNode* pPropNode = NULL);//wxString Path, wxString Name, bool bShowHidden
	wxGxToolbox(wxXmlNode* pDataNode, wxGISGPToolManager* pToolMngr = NULL, wxXmlNode* pPropNode = NULL);
	virtual ~wxGxToolbox(void);
	//IGxObject
	virtual void Detach(void);
	virtual wxString GetName(void);
	virtual wxString GetCategory(void){return wxString(_("Toolbox"));};
	virtual void Refresh(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxToolBox.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxToolBox.NewMenu"));};
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){LoadChildren(); return m_Children.size() > 0 ? true : false;};
	//wxGxToolbox
	virtual void LoadChildren(void);
	virtual void LoadChildrenFromXml(wxXmlNode* pNode);
	virtual void EmptyChildren(void);
protected:
	//wxString m_sPath;
	wxString m_sName;
	bool m_bIsChildrenLoaded;
    wxXmlNode* m_pDataNode;
    wxGISGPToolManager* m_pToolMngr;
    wxXmlNode* m_pPropNode;
};

/////////////////////////////////////////////////////////////////////////
// wxGxRootToolbox
/////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GPU wxGxRootToolbox :
	public wxGxToolbox,
    public IGxRootObjectProperties,
    public wxObject
{
   DECLARE_DYNAMIC_CLASS(wxGxRootToolbox)
public:
	wxGxRootToolbox(void);//wxString Path, wxString Name, bool bShowHidden
	virtual ~wxGxRootToolbox(void);
	//IGxObject
	virtual void Detach(void);
	virtual wxString GetName(void){return wxString(_("Toolboxes"));};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* pConfigNode);
    virtual wxXmlNode* GetProperties(void);
	//wxGxRootToolbox
	virtual void LoadChildren(void);
protected:
	wxString m_sPath;
	//wxArrayString m_FileNames;
	bool m_bIsChildrenLoaded;
    wxXmlDocument m_XmlDoc;
};

/////////////////////////////////////////////////////////////////////////
// wxGxTool
/////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GPU wxGxTool :
    public IGxObject,
	public IGxObjectUI, 
    public IGxObjectWizard
{
public:
	wxGxTool(wxGISGPToolManager* pToolMngr = NULL, wxXmlNode* pPropNode = NULL);//wxString Path, wxString Name, bool bShowHidden
	wxGxTool(wxXmlNode* pDataNode, wxGISGPToolManager* pToolMngr = NULL, wxXmlNode* pPropNode = NULL);
	virtual ~wxGxTool(void);
	//IGxObject
    virtual wxString GetName(void){return m_sName;};
	virtual wxString GetCategory(void){return wxString(_("Tool"));};
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxTool.ContextMenu"));};
	virtual wxString NewMenu(void){return wxEmptyString;};
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
	//wxGxTool
protected:
	//wxString m_sPath;
	wxString m_sName;
    IGPTool* m_pTool;
    wxXmlNode* m_pDataNode;
    wxGISGPToolManager* m_pToolMngr;
    wxXmlNode* m_pPropNode;

    std::vector<wxDialog*> m_DestroyArr;
};
