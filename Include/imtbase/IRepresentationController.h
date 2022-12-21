#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class CTreeItemModel;


class IRepresentationController: virtual public istd::IPolymorphic
{
public:
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation) const = 0;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const = 0;
};


} // namespace imtbase


