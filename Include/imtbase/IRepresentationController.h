#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <iprm/IParamsSet.h>


// ImtCore includes
#include <imtbase/CTreeItemModel.h>


namespace imtbase
{


class IRepresentationController: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetModelId() const = 0;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const = 0;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const = 0;
};


} // namespace imtbase


