#include <imtbase/CObjectCollectionMetaInfo.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


// public methods

QByteArray CObjectCollectionMetaInfo::GetMetaInfoId(int metaInfoType) const
{
	switch (metaInfoType){
	case imtbase::IObjectCollection::MIT_INSERTION_TIME:
		return "Added";

	case imtbase::IObjectCollection::MIT_REVISION:
		return "Revision";
	}

	return BaseClass::GetMetaInfoId(metaInfoType);
}


QString CObjectCollectionMetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case imtbase::IObjectCollection::MIT_INSERTION_TIME:
		return tr("Added");

	case imtbase::IObjectCollection::MIT_REVISION:
		return tr("Revision");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


QString CObjectCollectionMetaInfo::GetMetaInfoDescription(int metaInfoType) const
{
	switch (metaInfoType){
	case imtbase::IObjectCollection::MIT_INSERTION_TIME:
		return tr("Time when the object was added to the collection");

	case imtbase::IObjectCollection::MIT_REVISION:
		return tr("Item revision");
	}

	return BaseClass::GetMetaInfoDescription(metaInfoType);
}


} // namespace imtbase


