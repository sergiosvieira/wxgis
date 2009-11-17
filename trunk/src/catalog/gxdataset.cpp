#include "wxgis/catalog/gxdataset.h"
#include "../../art/shape_icon16.xpm"
#include "../../art/shape_icon48.xpm"
#include "wxgis/carto/featuredataset.h"
#include "wxgis/carto/rasterdataset.h"

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
		m_pwxGISDataset = new wxGISFeatureDataset(m_sPath);
		//for storing internal pointer
		m_pwxGISDataset->Reference();
	}
	//for outer pointer
	m_pwxGISDataset->Reference();
	return m_pwxGISDataset;
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
		m_pwxGISDataset = new wxGISFeatureDataset(m_sPath);
		//for storing internal pointer
		m_pwxGISDataset->Reference();
	}
	//for outer pointer
	m_pwxGISDataset->Reference();
	return m_pwxGISDataset;
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


wxGISDataset* wxGxRasterDataset::GetDataset(void)
{
	if(m_pwxGISDataset == NULL)
	{		
		m_pwxGISDataset = new wxGISRasterDataset(m_sPath);
		//for storing internal pointer
		m_pwxGISDataset->Reference();
	}
	//for outer pointer
	m_pwxGISDataset->Reference();
	return m_pwxGISDataset;
}
