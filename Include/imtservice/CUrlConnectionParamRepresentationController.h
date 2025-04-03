#pragma once


// ImtCore includes
#include <imtserverapp/IRepresentationController.h>
#include <imtservice/CUrlConnectionParam.h>


namespace imtservice
{


class CUrlConnectionParamRepresentationController: public imtserverapp::IRepresentationController
{
public:
	// reimplemented (imtrest::IRepresentationController)
	virtual QByteArray GetModelId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				imtbase::CTreeItemModel& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(
				const imtbase::CTreeItemModel& representation,
				istd::IChangeable& dataModel) const override;
};


} // namespace imtservice


