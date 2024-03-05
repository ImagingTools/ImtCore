#include <imtservice/CUrlConnectionLinkParamRepresentationController.h>


namespace imtservice
{


// reimplemented (imtbase::IRepresentationController)

QByteArray CUrlConnectionLinkParamRepresentationController::GetModelId() const
{
	return QByteArray();
}


bool CUrlConnectionLinkParamRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const CUrlConnectionLinkParam* urlConnectionParamPtr = dynamic_cast<const CUrlConnectionLinkParam*>(&dataModel);
	if (urlConnectionParamPtr != nullptr) {
		return true;
	}

	return false;
}


bool CUrlConnectionLinkParamRepresentationController::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const CUrlConnectionLinkParam* urlConnectionParamPtr = dynamic_cast<const CUrlConnectionLinkParam*>(&dataModel);
	if (urlConnectionParamPtr == nullptr) {
		return false;
	}

	QByteArray dependantServiceConnectionId = urlConnectionParamPtr->GetDependantServiceConnectionId();
	QString serviceTypeName = urlConnectionParamPtr->GetServiceTypeName();
	QByteArray usageId = urlConnectionParamPtr->GetUsageId();

	representation.SetData("Id", usageId);
	representation.SetData("UsageId", usageId);
	representation.SetData("DependantConnectionId", dependantServiceConnectionId);
	representation.SetData("ServiceTypeName", serviceTypeName);
	representation.SetData("DisplayUrl", "");

	return true;
}


bool CUrlConnectionLinkParamRepresentationController:: GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& representation,
			istd::IChangeable& dataModel) const
{
	return false;
}


} // namespace imtservice


