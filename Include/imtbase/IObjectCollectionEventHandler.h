#pragma once


// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


class IObjectCollectionEventHandler: virtual public istd::IPolymorphic
{
public:
	/**
		New objects was inserted into collection
	*/
	virtual void OnInsert(const ICollectionInfo::Id& objectId) = 0;

	/**
		Objects was changed
	*/
	virtual void OnUpdate(const ICollectionInfo::Id& objectId) = 0;

	/**
		Objects was removed
	*/
	virtual void OnRemove(const ICollectionInfo::Id& objectId) = 0;

	/**
		Objects was renamed
	*/
	virtual void OnRename(const ICollectionInfo::Id& objectId) = 0;

	/**
		Description of objects was changed
	*/
	virtual void OnDescriptionUpdate(const ICollectionInfo::Id& objectId) = 0;

	/**
		Objects enable state was changed
	*/
	virtual void OnEnableStateChange(const ICollectionInfo::Id& objectId) = 0;
};


} // namespace imtbase


