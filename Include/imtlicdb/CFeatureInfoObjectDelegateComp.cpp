#include <imtlicdb/CFeatureInfoObjectDelegateComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::IDatabaseObjectDelegate)

istd::IChangeable* CFeatureInfoObjectDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
{
	istd::TDelPtr<imtlic::CFeatureInfo> featureInfoPtr = new imtlic::CFeatureInfo;
	QByteArray featureId;
	QString featureName;

	if (record.contains("Id")){
		featureId = record.value("Id").toByteArray();
	}
	
	if (record.contains("Name")){
		featureName = record.value("Name").toString();
	}

	if (!featureId.isEmpty() && !featureName.isEmpty()){
		return featureInfoPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtlicdb


