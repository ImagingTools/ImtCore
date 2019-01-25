#pragma once


// Qt includes
#include <QtCore/QtCore>

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
		EC_GLUE_GAP,
		EC_GLUE_NARROW,
		EC_GLUE_WIDE,
	};

	enum ResultFlags
	{
		RF_NONE = 0x0,
		RF_SHOW_IN_SUMMARY = 0x1
	};

	struct InspectionRegionResult
	{
		QString regionName;
		ErrorClass errorClass;
		istd::IInformationProvider::InformationCategory status;
		double length;
		double value;
		double tolerance;
		double diff;
		int flags;
	};

	typedef QVector<InspectionRegionResult> Results;

	struct InspectionRegion
	{
		QString name;
		Results results;
	};

	typedef QVector<InspectionRegion> Regions;

	struct Inspection
	{
		QString name;
		QString imagePath;
		istd::IInformationProvider::InformationCategory status;
		Regions regions;
	};

	typedef QVector<Inspection> Inspections;

	struct ReportInputData
	{
		QString companyName;
		QString appVersion;
		QString productName;
		QDateTime time;
		QString imagePath;
		QString partSerialNumber;
		istd::IInformationProvider::InformationCategory partStatus;
		Inspections inspections;
	};

	virtual bool CreateReport(const ReportInputData& inputData, IReportDocument& reportDocument, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtreport


