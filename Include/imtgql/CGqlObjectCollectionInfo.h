#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>

// ACF includes
#include <iprm/COptionsManager.h>
#include <iser/IArchive.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/TIStructuredCollectionInfo.h>
#include <imtgql/IGqlClient.h>
#include <imtgql/IGqlObjectCollectionDelegate.h>


namespace imtgql
{


/**
	Implementation of a general collection information.
	\ingroup Collection
*/
class CGqlObjectCollectionInfo: virtual public imtbase::IStructuredObjectCollectionInfo
{
public:
	CGqlObjectCollectionInfo();

	int GetItemIndex(const QByteArray& id) const;

	void SetGqlClient(const IGqlClient* gqlClientPtr);
	void SetGqlDelegate(const IGqlObjectCollectionDelegate* gqlClientPtr);
	void SetNodeId(const QByteArray& nodeId);

	// reimplemented (imtbase::TIStructuredCollectionInfo)
	virtual ElementType GetElementType(const QByteArray& elementId) const override;
	virtual QByteArrayList GetElementBasePath(const QByteArray& elementId) const override;

	// reimplemented (imtbase::IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const Id& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual bool GetSubsetInfo(
				imtbase::ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	struct Item
	{
		Item(const QByteArray& id = QByteArray())
		{
			if (id.isEmpty()){
				this->id = QUuid::createUuid().toByteArray();
			}
			else{
				this->id = id;
			}
		}

		Item(const Item& object)
		{
			this->id = object.id;
			this->name = object.name;
			this->description = object.description;
			this->typeId= object.typeId;
		}

		bool operator==(const Item &item) const
		{
			return ((id == item.id)
					&& (name == item.name)
					&& (description == item.description)
					&& (typeId == item.typeId));
		}

		bool operator!=(const Item &item) const
		{
			return !(operator==(item));
		}

		QByteArray id;
		QString name;
		QString description;
		QByteArray typeId;
		idoc::MetaInfoPtr dataMetaInfoPtr;
		idoc::MetaInfoPtr elementMetaInfoPtr;
	};

	typedef QVector<Item> Items;

protected:
	Item* FindItemById(const Id& elementId) const;
	void UpdateCache();

protected:
	Items m_items;

	const IGqlClient* m_clientPtr;
	const IGqlObjectCollectionDelegate* m_delegatePtr;
	QByteArray m_nodeId;
	QByteArrayList m_basePath;
	iprm::COptionsManager m_typeIds;
};


} // namespace imtgql


