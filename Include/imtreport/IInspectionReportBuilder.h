#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QImage>

// ACF includes
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtreport/CInspectionReportStatus.h>


namespace imtreport
{


class IReportDocument;


/**
*/
class IInspectionReportBuilder: virtual public istd::IPolymorphic
{
public:

	enum ErrorClass
	{
		EC_NONE,
		EC_GAP,
		EC_GLUE_NARROW,
		EC_GLUE_WIDE,
	};

	struct InspectionRegionResult
	{
		ErrorClass errorClass;
		istd::IInformationProvider::InformationCategory status;
		double length;
		double value;
		double tolerance;
		double diff;
	};

	typedef QVector<InspectionRegionResult> Results;

	struct InspectionRegion
	{
		QString name;
		Results results;
	};

	typedef QVector<InspectionRegion> Regions;

	struct InspectionInfo
	{
		QString name;
		QImage image;
		istd::IInformationProvider::InformationCategory status;
		Regions regions;
	};

	typedef QVector<InspectionInfo> Inspections;

	struct ReportInputData
	{
		QString companyName;
		QString appVersion;
		QString productName;
		QDateTime time;
		QImage productOverview;
		QString partSerialNumber;
		istd::IInformationProvider::InformationCategory partStatus;
		Inspections inspections;
	};

	virtual bool CreateReport(const ReportInputData& inputData, IReportDocument& reportDocument, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtreport


