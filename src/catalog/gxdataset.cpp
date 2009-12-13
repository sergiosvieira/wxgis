/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDataset class.
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

#include "../../art/shape_icon16.xpm"
#include "../../art/shape_icon48.xpm"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/carto/featuredataset.h"
#include "wxgis/carto/rasterdataset.h"
#include "wxgis/framework/messagedlg.h"
#include "wxgis/framework/application.h"

//--------------------------------------------------------------
//class wxGxDataset
//--------------------------------------------------------------

wxGxDataset::wxGxDataset(wxString Path, wxString Name, wxGISEnumDatasetType Type)
{
	m_ImageListSmall.Create(16, 16);
	m_ImageListSmall.Add(wxBitmap(shape_icon16_xpm));
	m_ImageListLarge.Create(48, 48);
	m_ImageListLarge.Add(wxBitmap(shape_icon48_xpm));

	m_type = Type;

	m_sName = Name;
	m_sPath = Path;

	m_pwxGISDataset = NULL;
    m_Encoding = wxFONTENCODING_DEFAULT;
}

wxGxDataset::~wxGxDataset(void)
{
	wsDELETE(m_pwxGISDataset);
}

wxIcon wxGxDataset::GetLargeImage(void)
{
////	return wxIcon(m_ImageListLarge.GetIcon(2));
//	switch(m_type)
//	{
//	case enumGISFeatureDataset:
//		return wxIcon(m_ImageListLarge.GetIcon(0));
//	case enumGISRasterDataset:
//		return wxIcon(m_ImageListLarge.GetIcon(2));
//	case enumGISTableDataset:
//	default:
		return wxIcon(m_ImageListLarge.GetIcon(1));
	//}
}

wxIcon wxGxDataset::GetSmallImage(void)
{
	return wxIcon(m_ImageListSmall.GetIcon(1));
}

bool wxGxDataset::Delete(void)
{
	return true;
}

bool wxGxDataset::Rename(wxString NewName)
{
	m_sName = NewName; 
	return true;
}

void wxGxDataset::EditProperties(wxWindow *parent)
{
}

wxGISDataset* wxGxDataset::GetDataset(void)
{
	if(m_pwxGISDataset == NULL)
	{		
		m_pwxGISDataset = new wxGISFeatureDataset(m_sPath, m_Encoding);
		//for storing internal pointer
		m_pwxGISDataset->Reference();
	}
	//for outer pointer
	m_pwxGISDataset->Reference();
	return m_pwxGISDataset;
}

void wxGxDataset::SetEncoding(wxFontEncoding Encoding)
{
    m_Encoding = Encoding;
}

//--------------------------------------------------------------
//class wxGxShapefileDataset
//--------------------------------------------------------------

wxGxShapefileDataset::wxGxShapefileDataset(wxString Path, wxString Name, wxGISEnumShapefileDatasetType Type)
{
	m_ImageListSmall.Create(16, 16);
	m_ImageListSmall.Add(wxBitmap(shape_icon16_xpm));
	m_ImageListLarge.Create(48, 48);
	m_ImageListLarge.Add(wxBitmap(shape_icon48_xpm));

	m_type = Type;

	m_sName = Name;
	m_sPath = Path;

	m_pwxGISDataset = NULL;
}

wxGxShapefileDataset::~wxGxShapefileDataset(void)
{
	wsDELETE(m_pwxGISDataset);
}

wxString wxGxShapefileDataset::GetCategory(void)
{
	return wxString(_("Feature class"));
}

wxIcon wxGxShapefileDataset::GetLargeImage(void)
{
	switch(m_type)
	{
	case enumESRIShapefile:
		return wxIcon(m_ImageListLarge.GetIcon(0));
	case enumMapinfoTabfile:
	default:
		return wxIcon();
	}
}

wxIcon wxGxShapefileDataset::GetSmallImage(void)
{
	switch(m_type)
	{
	case enumESRIShapefile:
		return wxIcon(m_ImageListSmall.GetIcon(0));
	case enumMapinfoTabfile:
	default:
		return wxIcon();
	}
}

bool wxGxShapefileDataset::Delete(void)
{
	return true;
}

bool wxGxShapefileDataset::Rename(wxString NewName)
{
	m_sName = NewName; 
	return true;
}

void wxGxShapefileDataset::EditProperties(wxWindow *parent)
{
}

wxGISDataset* wxGxShapefileDataset::GetDataset(void)
{
	if(m_pwxGISDataset == NULL)
	{		
		m_pwxGISDataset = new wxGISFeatureDataset(m_sPath, m_Encoding);
		//for storing internal pointer
		m_pwxGISDataset->Reference();
	}
	//for outer pointer
	m_pwxGISDataset->Reference();
	return m_pwxGISDataset;
}

void wxGxShapefileDataset::SetEncoding(wxFontEncoding Encoding)
{
    m_Encoding = Encoding;
}

//--------------------------------------------------------------
//class wxGxRasterDataset
//--------------------------------------------------------------

wxGxRasterDataset::wxGxRasterDataset(wxString Path, wxString Name, wxGISEnumRasterDatasetType Type)
{
	m_ImageListSmall.Create(16, 16);
	m_ImageListSmall.Add(wxBitmap(shape_icon16_xpm));
	m_ImageListLarge.Create(48, 48);
	m_ImageListLarge.Add(wxBitmap(shape_icon48_xpm));

	m_type = Type;

	m_sName = Name;
	m_sPath = Path;

	m_pwxGISDataset = NULL;
}

wxGxRasterDataset::~wxGxRasterDataset(void)
{
	wsDELETE(m_pwxGISDataset);
}

wxIcon wxGxRasterDataset::GetLargeImage(void)
{
	switch(m_type)
	{
	case enumRasterUnknown:
	default:
		return wxIcon(m_ImageListLarge.GetIcon(2));
	}
}

wxIcon wxGxRasterDataset::GetSmallImage(void)
{
	switch(m_type)
	{
	case enumRasterUnknown:
	default:
		return wxIcon(m_ImageListSmall.GetIcon(2));
	}
}

bool wxGxRasterDataset::Delete(void)
{
	return true;
}

bool wxGxRasterDataset::Rename(wxString NewName)
{
	m_sName = NewName; 
	return true;
}

void wxGxRasterDataset::EditProperties(wxWindow *parent)
{
}

wxString wxGxRasterDataset::GetCategory(void)
{
	switch(m_type)
	{
	case enumRasterUnknown:
	default:
		return wxString(_("Raster"));
	}
}

typedef struct OvrProgressData
{
    IStatusBar* pStatusBar;
    wxGISProgressor* pProgressor;
    wxString sMessage;
} *LPOVRPROGRESSDATA;

int CPL_STDCALL OvrProgress( double dfComplete, const char *pszMessage, void *pData)
{
    LPOVRPROGRESSDATA pOvrData = (LPOVRPROGRESSDATA)pData;
    if( pszMessage != NULL )
        pOvrData->pStatusBar->SetMessage(wgMB2WX(pszMessage));
    else if( pOvrData != NULL && !pOvrData->sMessage.IsEmpty() )
        pOvrData->pStatusBar->SetMessage(pOvrData->sMessage);
    else
        pOvrData->pStatusBar->SetMessage(_("Building overviews"));

    if(pOvrData->pProgressor)
        pOvrData->pProgressor->SetValue((int) (dfComplete*100));

    bool bKeyState = wxGetKeyState(WXK_ESCAPE);
    return bKeyState == true ? 0 : 1;
}

wxGISDataset* wxGxRasterDataset::GetDataset(void)
{
	if(m_pwxGISDataset == NULL)
	{	
        wxGISRasterDataset* pwxGISRasterDataset = new wxGISRasterDataset(m_sPath);

		m_pwxGISDataset = static_cast<wxGISDataset*>(pwxGISRasterDataset);
        //open (ask for overviews)
        if(!pwxGISRasterDataset->Open())
			return NULL;
        //pyramids
        if(!pwxGISRasterDataset->HasOverviews())
        {
	        CPLSetConfigOption( "USE_RRD", "NO" );//YES
	        CPLSetConfigOption( "HFA_USE_RRD", "YES" );
	        //CPLSetConfigOption( "COMPRESS_OVERVIEW", "LZW" );//DEFLATE

        	bool bAskCreateOvr = true;
            wxString name, ext;
            wxFileName::SplitPath(m_sPath, NULL, NULL, &name, &ext);
            wxString sFileName = name + wxT(".") + ext;
            IGISConfig*  pConfig = m_pCatalog->GetConfig();
            bool bCreateOverviews = true;
            if(pConfig)
            {
                wxXmlNode* pNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/raster")));
                if(pNode)
                    bAskCreateOvr = wxAtoi(pNode->GetPropVal(wxT("create_ovr"), wxT("1")));
                else
                {
                    pNode = pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/raster")), true);
                    pNode->AddProperty(wxT("create_ovr"), wxT("1"));
                }
                if(bAskCreateOvr)
                {
                    //show ask dialog
                    wxGISMessageDlg dlg(NULL, wxID_ANY, wxString::Format(_("Create pyramids for %s (%d x %d)"), sFileName.c_str(),pwxGISRasterDataset->GetWidth(), pwxGISRasterDataset->GetHeight()), wxString(_("This raster data source does not have pyramids. Pyramids allow for rapid display at varying resolutions.")), wxString(_("Pyramid buildin may take a few moments.\nWould you like to create pyramids?")), wxDefaultPosition, wxSize( 400,160 ));
                    if(dlg.ShowModal() == wxID_NO)
                    {
                        bCreateOverviews = false;
                    }
                    if(!dlg.GetShowInFuture())
                    {
                        pNode->DeleteProperty(wxT("create_ovr"));
                        pNode->AddProperty(wxT("create_ovr"), wxT("0"));
                    }
                }
            }

	        if(bCreateOverviews)
	        {
		        int anOverviewList[5] = { 4, 8, 16, 32, 64 };
                wxString sProgressMsg = wxString::Format(_("Creating pyramids for : %s (%d bands)"), sFileName.c_str(), pwxGISRasterDataset->GetRaster()->GetRasterCount());
                IApplication* pApp = ::GetApplication();
                IStatusBar* pStatusBar = pApp->GetStatusBar();  
                wxGISProgressor* pProgressor = dynamic_cast<wxGISProgressor*>(pStatusBar->GetProgressor());
                if(pProgressor)
                    pProgressor->Show(true);

                OvrProgressData Data = {pStatusBar, pProgressor, sProgressMsg}; 
		        CPLErr err = pwxGISRasterDataset->GetRaster()->BuildOverviews( "GAUSS", 5, anOverviewList, 0, NULL, /*GDALDummyProgress*/OvrProgress, (void*)&Data );
		        //"NEAREST", "GAUSS", "CUBIC", "AVERAGE", "MODE", "AVERAGE_MAGPHASE" or "NONE" 
                if(pProgressor)
                    pProgressor->Show(false);
                pStatusBar->SetMessage(_("Done"));
	        }
        }
		//for storing internal pointer
		m_pwxGISDataset->Reference();
	}
	//for outer pointer
	m_pwxGISDataset->Reference();
	return m_pwxGISDataset;
}
