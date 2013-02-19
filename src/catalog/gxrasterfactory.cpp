/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactory class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2013  Bishop
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
#include "wxgis/catalog/gxrasterfactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/sysop.h"

//------------------------------------------------------------------------------
// wxGxRasterFactory
//------------------------------------------------------------------------------
static const char *raster_add_exts[] = {
    "aux", "rrd", "ovr", "w", "wld", "bpw", "bmpw", "gifw", "jpgw", "jpegw", "pngw", "pngw", "mgrd", "sgrd", "vrtw", "vtw", "tilw", "tlw", "sdatw", "stw", NULL
};

typedef struct _rformat
{
    const char* sExt;
    wxGISEnumRasterDatasetType eType;
    const char* sDriverName;
} RFORMAT;

static const RFORMAT raster_exts[] = {
    { "bmp",    enumRasterBmp,  "BMP"   },
    { "jpg",    enumRasterJpeg, "JPEG"  },
    { "jpeg",   enumRasterJpeg, "JPEG"  },
    { "img",    enumRasterImg,  "HFA"   },
    { "gif",    enumRasterGif,  "GIF"   },
    { "sdat",   enumRasterSAGA, "SAGA"  },
    { "tif",    enumRasterTiff, "GTiff" },
    { "tiff",   enumRasterTiff, "GTiff" },
    { "png",    enumRasterPng,  "PNG"   },
    { "til",    enumRasterTil,  "TIL"   },
    { "vrt",    enumRasterVRT,  "VRT"   }
};

IMPLEMENT_DYNAMIC_CLASS(wxGxRasterFactory, wxGxObjectFactory)

wxGxRasterFactory::wxGxRasterFactory(void)
{
}

wxGxRasterFactory::~wxGxRasterFactory(void)
{
}

bool wxGxRasterFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;
        bool bContinue(false);

        unsigned int j;
        for(j = 0; j < sizeof(raster_exts) / sizeof(raster_exts[0]); ++j)
        {
            if(wxGISEQUAL(szExt, raster_exts[j].sExt) && GDALGetDriverByName(raster_exts[j].sDriverName))
		    {
                pGxObj = GetGxObject(pParent, GetConvName(pFileNames[i]), pFileNames[i], raster_exts[j].eType);
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
                if(pGxObj != NULL)
                    pChildrenIds.Add(pGxObj->GetId());
                bContinue = true;
                break;
		    }
        }

        if(bContinue)
            continue;


        if(wxGISEQUAL(szExt, "prj"))
        {
			if(pFileNames)
			{
                unsigned int j;
                for(j = 0; j < sizeof(raster_exts) / sizeof(raster_exts[0]); ++j)
                {
    				szPath = (char*)CPLResetExtension(pFileNames[i], raster_exts[j].sExt);
	    			if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                    {
		    			pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
                        bContinue = true;
                        break;
                    }
                }
            }
        }

        if(bContinue)
            continue;
        
        for( j = 0; raster_add_exts[j] != NULL; ++j )
        {
            if(wxGISEQUAL(szExt, raster_add_exts[j]))
            {
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
                break;
            }
        }
    }
	return true;
}

wxGxObject* wxGxRasterFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumRasterDatasetType type)
{
    wxGxRasterDataset* pDataset = new wxGxRasterDataset(type, pParent, soName, szPath);
    return wxStaticCast(pDataset, wxGxObject);
}
