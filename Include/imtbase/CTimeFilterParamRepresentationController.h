#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IRepresentationController.h>


namespace imtbase
{


class CTimeFilterParamRepresentationController: virtual public imtbase::IRepresentationController
{
public:
	// reimplemented (IRepresentationController)
	virtual QByteArray GetModelId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				CTreeItemModel& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(
				const CTreeItemModel& representation,
				istd::IChangeable& dataModel) const override;
};


} // namespace imtbase


