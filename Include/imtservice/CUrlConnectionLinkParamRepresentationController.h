#pragma once


// ImtCore includes
#include <imtbase/IRepresentationController.h>
#include <imtservice/CUrlConnectionLinkParam.h>


namespace imtservice
{


class CUrlConnectionLinkParamRepresentationController: public imtbase::IRepresentationController
{
public:
	// reimplemented (imtbase::IRepresentationController)
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


