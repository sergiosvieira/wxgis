/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog main header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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

#include "wxgis/datasource/datasource.h"
#include "wx/filename.h"

#define NOTFIRESELID    1010


enum wxGISEnumSaveObjectResults
{
	enumGISSaveObjectNone = 0x0000,
	enumGISSaveObjectAccept = 0x0001,
	enumGISSaveObjectExists = 0x0002,
	enumGISSaveObjectDeny = 0x0004
};

class IGxObject;
DEFINE_SHARED_PTR(IGxObject);
DEFINE_WEAK_PTR(IGxObject);

typedef std::vector<IGxObjectSPtr> GxObjectArray;

class IGxObjectFactory;
DEFINE_SHARED_PTR(IGxObjectFactory);
typedef std::vector<IGxObjectFactorySPtr> GxObjectFactoryArray;

class IGxCatalog
{
public:
	IGxCatalog(void) : m_pConf(NULL){};
	virtual ~IGxCatalog(void){};
	virtual wxString ConstructFullName(const IGxObject* pObject) = 0;
	virtual bool GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray) = 0;
	virtual GxObjectArray* const GetDisabledRootItems(void){return &m_aRootItems;};
    virtual GxObjectFactoryArray* const GetObjectFactories(void){return &m_ObjectFactoriesArray;};
	virtual void ObjectAdded(IGxObjectWPtr pObject) = 0;
	virtual void ObjectChanged(IGxObjectWPtr pObject) = 0;
	virtual void ObjectDeleted(IGxObjectWPtr pObject) = 0;
	virtual void ObjectRefreshed(IGxObjectWPtr pObject) = 0;
	virtual IGxObjectSPtr ConnectFolder(wxString sPath, bool bSelect = true) = 0;
	virtual void DisconnectFolder(CPLString sPath, bool bSelect = true) = 0;
    virtual IGISConfig* const GetConfig(void){return m_pConf;};
    virtual void EnableRootItem(IGxObjectSPtr pRootItem, bool bEnable) = 0;
	virtual bool GetShowHidden(void){return m_bShowHidden;};
	virtual bool GetShowExt(void){return m_bShowExt;};
	virtual void SetShowHidden(bool bShowHidden){m_bShowHidden = bShowHidden;};
	virtual void SetShowExt(bool bShowExt){m_bShowExt = bShowExt;};
protected:
    IGISConfig* m_pConf;
    GxObjectArray m_aRootItems;
    GxObjectFactoryArray m_ObjectFactoriesArray;
	bool m_bShowHidden, m_bShowExt;
};
DEFINE_SHARED_PTR(IGxCatalog);
DEFINE_WEAK_PTR(IGxCatalog);

class IGxObject
{
public:
	virtual ~IGxObject(void){};
	virtual bool Attach(IGxObjectWPtr pParent, IGxCatalogWPtr pCatalog)
	{
		m_pParent = pParent;
		m_pCatalog = pCatalog;
		return true;
	};
	virtual void Detach(void)
	{
		m_pParent = NULL;
		m_pCatalog = NULL;
	};
	virtual wxString GetName(void) = 0;
	virtual wxString GetBaseName(void) = 0; //the name without ext
	virtual wxString GetFullName(void)
	{
		if(m_pCatalog)
			return m_pCatalog->ConstructFullName(this);
		else
			return wxEmptyString;
	};
	virtual CPLString GetInternalName(void) = 0;
	virtual wxString GetCategory(void) = 0;
	virtual IGxObjectWPtr GetParent(void){return m_pParent;};
	virtual void Refresh(void){};
protected:
	IGxObjectWPtr m_pParent;
	IGxCatalogWPtr m_pCatalog;
};

/** \class IGxObjectEdit catalog.h
    \brief A GxObject edit interface.
*/
class IGxObjectEdit
{
public:
	virtual ~IGxObjectEdit(void){};
	virtual bool Delete(void){return false;};
	virtual bool CanDelete(void){return false;};
	virtual bool Rename(wxString NewName){return false;};
	virtual bool CanRename(void){return false;};
};

class IGxObjectContainer;
DEFINE_SHARED_PTR(IGxObjectContainer);

class IGxObjectContainer :
	public IGxObject
{
public:
	virtual ~IGxObjectContainer(void){};
	virtual bool AddChild(IGxObjectSPtr pChild)
	{
		if(pChild == NULL)
			return false;
		if(!pChild->Attach(this, m_pCatalog))
			return false;
		m_Children.push_back(pChild);
		return true;
	};
	virtual bool DeleteChild(IGxObjectSPtr pChild)
	{
		if(pChild == NULL)
			return false;
		GxObjectArray::iterator pos = std::find(m_Children.begin(), m_Children.end(), pChild);
		if(pos != m_Children.end())
		{
			pChild->Detach();
			m_Children.erase(pos);
		}
		return true;
	};
	virtual bool AreChildrenViewable(void) = 0;
	virtual bool HasChildren(void) = 0;
	virtual GxObjectArray* const GetChildren(void){return &m_Children;};
	virtual IGxObjectSPtr SearchChild(wxString sPath)
	{
		wxString sTestPath = sPath;
		if(GetFullName().CmpNoCase(sPath) == 0)//GetName
			return boost::dynamic_pointer_cast<IGxObject>(this);
		for(size_t i = 0; i < m_Children.size(); i++)
		{
			wxString sTestedPath = m_Children[i]->GetFullName();
			if(sTestedPath.CmpNoCase(sPath) == 0)
				return m_Children[i];
            bool bHavePart = sPath.MakeLower().Find(sTestedPath.MakeLower()) != wxNOT_FOUND;
			if(bHavePart )
			{
				IGxObjectContainerSPtr pContainer = boost::dynamic_pointer_cast<IGxObjectContainer>(m_Children[i]);
				if(pContainer && pContainer->HasChildren())
				{
					IGxObjectSPtr pFoundChild = pContainer->SearchChild(sPath);
					if(pFoundChild)
						return pFoundChild;
				}
			}
		}
		return IGxObjectSPtr();
	}
    virtual bool CanCreate(long nDataType, long DataSubtype){return false;}; 
protected:
	GxObjectArray m_Children;
};

class IGxObjectFactory
{
public:
	IGxObjectFactory(void) : m_bIsEnabled(true){};
	virtual ~IGxObjectFactory(void){};
    //pure virtual
	virtual bool GetChildren(CPLString sParentDir, char** pFileNames, GxObjectArray &ObjArray) = 0;
    virtual void Serialize(wxXmlNode* pConfig, bool bStore) = 0;
    virtual wxString GetClassName(void) = 0;
    virtual wxString GetName(void) = 0;
    //
    virtual bool GetEnabled(void){return m_bIsEnabled;};
    virtual void SetEnabled(bool bIsEnabled){m_bIsEnabled = bIsEnabled;};
	virtual void PutCatalogRef(IGxCatalog* pCatalog){m_pCatalog = pCatalog;};
protected:
	IGxCatalogWPtr m_pCatalog;
    bool m_bIsEnabled;
};

class IGxCatalogEvents
{
public:
	virtual ~IGxCatalogEvents(void){};
	virtual void OnObjectAdded(IGxObjectWPtr object) = 0;
	virtual void OnObjectChanged(IGxObjectWPtr object) = 0;
	virtual void OnObjectDeleted(IGxObjectWPtr object) = 0;
	virtual void OnObjectRefreshed(IGxObjectWPtr object) = 0;
	virtual void OnRefreshAll(void) = 0;
};


class IGxDataset
{
public:
	virtual ~IGxDataset(void){};
	virtual wxGISDatasetSPtr GetDataset(bool bReadOnly) = 0;
	virtual wxGISEnumDatasetType GetType(void) = 0;
	virtual int GetSubType(void) = 0;
};

class IGxFile
{
public:
	virtual ~IGxFile(void){};
	//virtual bool Open(void) = 0;
	//virtual bool Close(bool bSaveEdits) = 0;
	//virtual void Edit(void) = 0;
	//virtual void New(void) = 0;
	//virtual bool Save(void) = 0;
};

class IGxRootObjectProperties
{
public:
	virtual ~IGxRootObjectProperties(void){};
	virtual void Init(wxXmlNode* pConfigNode) = 0;
	virtual wxXmlNode* const GetProperties(void) = 0;
    virtual bool GetEnabled(void){return m_bEnabled;};
    virtual void SetEnabled(bool bEnabled){m_bEnabled = bEnabled;};
protected:
    bool m_bEnabled;
};

class IGxObjectSort
{
public:
	virtual ~IGxObjectSort(void){};
	virtual bool IsAlwaysTop(void) = 0;
	virtual bool IsSortEnabled(void) = 0;
};

class IGxObjectFilter
{
public:
	virtual ~IGxObjectFilter(void){};
	virtual bool CanChooseObject( IGxObjectWPtr pObject ) = 0;//, esriDoubleClickResult* result
	virtual bool CanDisplayObject( IGxObjectWPtr pObject ) = 0;
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObjectWPtr pLocation, wxString sName ) = 0;
	virtual wxString GetName(void) = 0;
    virtual wxString GetExt(void) = 0;
    virtual wxString GetDriver(void) = 0;
    virtual int GetSubType(void) = 0;
};

typedef std::vector<IGxObjectFilter*> OBJECTFILTERS, *LPOBJECTFILTERS;

//
//class IGxObjectFactories
//{
//public:
//	virtual ~IGxObjectFactories(void){};
//	virtual GxObjectFactoryArray* GetEnabledGxObjectFactories() = 0;
//};

static wxString GetConvName(wxString sPath, wxFileName &FName)
{
    //name conv cp866 if zip
    wxString name;
    if(sPath.Find(wxT("/vsizip/")) != wxNOT_FOUND)
    {
        wxString str(FName.GetFullName().mb_str(*wxConvCurrent), wxCSConv(wxT("cp-866")));
        name = str;
    }
    else
        name = FName.GetFullName();
	return name;
}



