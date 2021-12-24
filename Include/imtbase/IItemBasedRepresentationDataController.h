#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Base interface for representation data controllers
*/
class IItemBasedRepresentationDataController: virtual public istd::IPolymorphic
{
public:
	virtual void UpdateRepresentationFromBaseModel(
				istd::IChangeable* baseModel,
				istd::IChangeable* representationModel,
				const istd::IChangeable::ChangeSet& changeSet) = 0;

	virtual void UpdateBaseModelFromRepresentation(
				istd::IChangeable* baseModel,
				istd::IChangeable* representationModel,
				const istd::IChangeable::ChangeSet& changeSet) = 0;
};


} // namespace imtbase


