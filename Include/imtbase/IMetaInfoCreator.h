#pragma once


// ACF includes
#include <istd/TSmartPtr.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtbase
{


/**
	Interface for creating meta-informations for a data object.
	\ingroup DataManagement
*/
class IMetaInfoCreator: virtual public istd::IPolymorphic
{
public:
	typedef QList<QByteArray> TypeIds;
	typedef istd::TSmartPtr<idoc::IDocumentMetaInfo> MetaInfoPtr;

	/**
		Get list of supported type-IDs for the meta-information creation.
	*/
	virtual TypeIds GetSupportedTypeIds() const = 0;

	/**
		Create meta informations for the data object of the given type.
		\note if the dataPtr is \c null, an empty meta-info object will be created.
	*/
	virtual bool CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, MetaInfoPtr& metaInfoPtr) const = 0;
};


} // namespace imtbase


