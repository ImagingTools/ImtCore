#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QVector>
#include <QtCore/QVariant>

// ACF includes
#include <ilog/IMessageConsumer.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/IParamsSet.h>
#include <istd/IChangeable.h>
#include <istd/TSmartPtr.h>


namespace imtbase
{


/**
	Interface for non-iterable colllection info.
	\ingroup Collection
*/
class ICollectionInfo: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;
	typedef QVector<Id> Ids;

	struct NotifierInfo
	{
		QByteArray elementId;
	};

	static const QByteArray CN_ALL_CHANGED;

	static const QByteArray CN_ELEMENT_INSERTED;
	typedef NotifierInfo ElementInsertInfo;

	static const QByteArray CN_ELEMENT_UPDATED;
	typedef NotifierInfo ElementUpdateInfo;

	static const QByteArray CN_ELEMENT_REMOVED;
	typedef NotifierInfo ElementRemoveInfo;

	/**
		Change notification flags.
	*/
	enum ChangeFlags
	{
		/**
			Element was added.
		*/
		CF_ADDED = 10000,

		/**
			Element was changed.
		*/
		CF_UPDATED,

		/**
			Element was removed.
		*/
		CF_REMOVED
	};

	/**
		Type of the element information.
	*/
	enum ElementInfoType
	{
		/**
			Name of the element given as a QString.
		*/
		EIT_NAME,

		/**
			Human-readable description of the element given as a QString.
		*/
		EIT_DESCRIPTION,

		/**
			Enabled/Disabled status of the element as a boolean.
		*/
		EIT_ENABLED,

		/**
			Start value for user-defined info types.
		*/
		EIT_USER = 1000
	};

	/**
		Type of the meta-informations supported by the collection.
	*/
	enum MetaInfoType
	{
		/**
			Name of the user who has added the element.
		*/
		MIT_INSERTION_USER = idoc::IDocumentMetaInfo::MIT_USER + 10000,

		/**
			Timestamp of the element inserting into the collection.
		*/
		MIT_INSERTION_TIME,

		/**
			Name of the user who has modified the element.
		*/
		MIT_LAST_OPERATION_USER,

		/**
			Timestamp of the last operation on the element in the collection.
		*/
		MIT_LAST_OPERATION_TIME,

		/**
			Tumbnail for the item preview.
		*/
		MIT_PREVIEW_THUMBNAIL,

		/**
			Current element revision.
		*/
		MIT_REVISION = idoc::IDocumentMetaInfo::MIT_USER + 20000
	};


	/**
		Get number of elements in the collection
	*/
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const = 0;

	/**
		Get IDs of the elements inside the collection for parent.
		\param offset				[optional] Index offset of the first element
		\param count				[optional] If positive, the number of elements should be returned.
		\param selectionParamsPtr	[optional] Additional parameters for filtering/ordering elements.
		\param parentId				[optional] Parent element ID. Entire filtering operation applies to the childs of this element only.
	*/
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const = 0;
	/**
		Get information about the subset of the whole collection according to the given filtering/sorting parameters.
		\param subsetInfo		collection information for the data subset.
		\param offset				[optional] Index offset of the first element
		\param count				[optional] If positive, the number of elements should be returned.
		\param selectionParamsPtr	[optional] Additional parameters for filtering/ordering elements.
		\param parentId				[optional] Parent element ID. Entire filtering operation applies to the childs of this element only.
	*/
	virtual bool GetSubsetInfo(
				ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const = 0;

	/**
		Get information about a given element.
	*/
	virtual QVariant GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const = 0;

	/**
		Get meta-information of the element in the collection.
		\param elementId		ID of the element in the collection.
		\param metaInfo			Meta-info to be filled by this method.
		\return \c true if the operation was successful, and \c false if no information could be provided.
	*/
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const = 0;

	/**
		Set name of the element with the given ID.
	*/
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) = 0;

	/**
		Set description of the element with the given ID.
	*/
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) = 0;

	/**
		Enable/Disable element with the given ID.
	*/
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) = 0;
};


} // namespace imtbase


Q_DECLARE_METATYPE(imtbase::ICollectionInfo::NotifierInfo);


