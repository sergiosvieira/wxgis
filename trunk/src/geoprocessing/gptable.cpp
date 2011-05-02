/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  main table analysis functions.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/geoprocessing/gptable.h"

wxGISTableSPtr CreateTable(CPLString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *poFields, wxGISEnumTableDatasetType nType, char ** papszDataSourceOptions, char ** papszLayerOptions)
{
    CPLErrorReset();
    poFields->Reference();
    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( wgWX2MB(sDriver) );
    if(poDriver == NULL)
    {
        wxString sErr = wxString::Format(_("The driver '%s' is not available! OGR error: "), sDriver.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_FileIO, sFullErr );
        return wxGISTableSPtr();
    }

    CPLString sFullPath = CPLFormFilename(sPath, sName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
    OGRDataSource *poDS = poDriver->CreateDataSource(sFullPath, papszDataSourceOptions );
    if(poDS == NULL)
    {
        wxString sErr = wxString::Format(_("Error create the output file '%s'! OGR error: "), sName.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        return wxGISTableSPtr();
    }

    sName.Replace(wxT("."), wxT("_"));
    sName.Replace(wxT(" "), wxT("_"));
    sName.Replace(wxT("&"), wxT("_"));
	if(wxIsdigit(sName[0]))
		sName.Prepend(_("Layer_"));
    sName.Truncate(27);
    CPLString szName = CPLString(sName.mb_str());

	OGRLayer *poLayerDest = poDS->CreateLayer(szName, NULL, wkbUnknown, papszLayerOptions );
    if(poLayerDest == NULL)
    {
        wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sName.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        return wxGISTableSPtr();
    }

    for(size_t i = 0; i < poFields->GetFieldCount(); ++i)
    {
        OGRFieldDefn *pField = poFields->GetFieldDefn(i);
        if( poLayerDest->CreateField( pField ) != OGRERR_NONE )
        {
            wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sName.c_str());
            CPLString sFullErr(sErr.mb_str());
            sFullErr += CPLGetLastErrorMsg();
            CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
            return wxGISTableSPtr();
        }
    }


    poFields->Release();

    wxGISTableSPtr pDataSet = boost::make_shared<wxGISTable>(sFullPath, nType, poLayerDest, poDS);
    return pDataSet;
}

bool MeanValByColumn(wxGISTableSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    if(!pFilter || !pDSet)
        return false;

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();
    int nNewSubType = pFilter->GetSubType();

    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("Calculate mean values for columns. Source dataset %s. Destination dataset %s"), pDSet->GetName().c_str(), sName.c_str()), -1, enumGISMessageTitle);

    OGRFeatureDefn *pNewDef = new OGRFeatureDefn("mean_vals");
	pNewDef->AddFieldDefn( new OGRFieldDefn("date_time", OFTDate) );
	pNewDef->AddFieldDefn( new OGRFieldDefn("TMean", OFTReal) );
	pNewDef->AddFieldDefn( new OGRFieldDefn("TMax", OFTReal ) );
	pNewDef->AddFieldDefn( new OGRFieldDefn("TMin", OFTReal ) );
	pNewDef->AddFieldDefn( new OGRFieldDefn("RRR", OFTReal ) );

	const char *apszOptions[3] = { "CREATE_CSVT=YES", "SEPARATOR=SEMICOLON", NULL};
    wxGISTableSPtr pNewDSet = CreateTable(sPath, sName, sExt, sDriver, pNewDef, (wxGISEnumTableDatasetType)nNewSubType, NULL, (char **)apszOptions);
    if(!pNewDSet)
    {
        wxString sErr(_("Error creating new dataset! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(wgMB2WX(sFullErr), -1, enumGISMessageErr);
        return false;
    }

 //   //OGRFeatureDefn *pDef = pDSet->GetDefiniton();
 //   //if(!pDef)
 //   //{
 //   //    wxString sErr(_("Error read dataset definition"));
 //   //    CPLError( CE_Failure, CPLE_AppDefined, sErr.mb_str() );
 //   //    if(pTrackCancel)
 //   //        pTrackCancel->PutMessage(sErr, -1, enumGISMessageErr);
 //   //    return false;
 //   //}

	//OGRFeatureSPtr poFeature = pNewDSet->CreateFeature();
	//OGRFeatureDefn *pNewDefmmm = poFeature->GetDefnRef();
	//int nC = pNewDefmmm->GetFieldCount();
	//int nD = pNewDefmmm->GetFieldIndex("TMean");
	//poFeature->SetFID(1);
	//poFeature->SetField(1, 0.5);
	//poFeature->SetField(2, 0.75);
	//poFeature->SetField(3, 1.5);
	//OGRErr eErr = pNewDSet->StoreFeature(poFeature);

	//int nCounter(0);
	//long nFidCounter(0);
	struct Val
	{
		double sum; 
		int count;
	};
	struct Vals
	{
		struct Val mean_temp;
		double min_temp;
		double max_temp;
		double max_rrr;
	};

	std::map<wxDateTime,Vals> maped_data;
 	OGRFeatureSPtr poFeature;
	pDSet->Reset();
    while((poFeature = pDSet->Next()) != NULL)	
    {
        if(pTrackCancel && !pTrackCancel->Continue())
			return false;

		double dT = poFeature->GetFieldAsDouble(1);
		double dTMin = poFeature->GetFieldAsDouble(13);
		double dTMax = poFeature->GetFieldAsDouble(14);
		double dRRR = poFeature->GetFieldAsDouble(22);
		CPLString sDate = poFeature->GetFieldAsString(0);
		wxDateTime dt;
		dt.ParseFormat(wxString(sDate, wxConvLocal), wxT("%d.%m.%Y"));// %H:%M
		if(!dt.IsValid())
		    continue;

		maped_data[dt].mean_temp.sum += dT;
		maped_data[dt].mean_temp.count++;
		if(dTMin != 0 && (maped_data[dt].min_temp == 0 || maped_data[dt].min_temp > dTMin))
			maped_data[dt].min_temp = dTMin;
		if(dTMax != 0 && (maped_data[dt].max_temp == 0 || maped_data[dt].max_temp < dTMin))
			maped_data[dt].max_temp = dTMax;
		if(dRRR != 0 && (maped_data[dt].max_rrr == 0 || maped_data[dt].max_rrr < dRRR))
			maped_data[dt].max_rrr = dRRR;
		

		//int year, mon, day, hour, min, sec, flag;
		//if(poFeature->GetFieldAsDateTime(0, &year, &mon, &day, &hour, &min, &sec, &flag) == TRUE)
  //      {
		//    wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
  //          if(!dt.IsValid())
		//	    continue;
		//	dt.SetHour(0);
		//	dt.SetMinute(0);
		//	dt.SetSecond(0);
		//	//dt.SetMillisecond(0);
		//	//maped_data[dt].Vals[0].sum += 1;
		//	//maped_data[dt].Vals[0].count += 1;
  //      }
		//else continue;

	//	if(!GeometryVerticesToPointsDataset(poFeature->GetFID(), pGeom, pNewDSet, poCT, nFidCounter, pTrackCancel))
	//	{
	//		if(pTrackCancel)
	//			pTrackCancel->PutMessage(wxString(_("Unsupported geometry type")), -1, enumGISMessageWarning);
	//	}
	//	else
	//	{
	//		if(pTrackCancel)
	//			pTrackCancel->PutMessage(wxString::Format(_("Geometry No. %d added"), nCounter), -1, enumGISMessageInfo);
	//	}
	}

	IProgressor* pProgress(NULL);
    if(pTrackCancel)
		pProgress = pTrackCancel->GetProgressor();
	if(pProgress)
		pProgress->SetRange(maped_data.size());
	int nCounter(0);
	for(std::map<wxDateTime,Vals>::iterator it = maped_data.begin(); it != maped_data.end(); ++it)
	{
		if(pProgress)
			pProgress->SetValue(nCounter);

		OGRFeatureSPtr poFeature = pNewDSet->CreateFeature();
		poFeature->SetField(0, it->first.GetYear(), it->first.GetMonth() + 1, it->first.GetDay());
		poFeature->SetField(1, double(it->second.mean_temp.sum) / it->second.mean_temp.count);
		poFeature->SetField(2, it->second.max_temp);
		poFeature->SetField(3, it->second.min_temp);
		poFeature->SetField(4, it->second.max_rrr);
		OGRErr eErr = pNewDSet->StoreFeature(poFeature);

		nCounter++;
	}

	return true;
}