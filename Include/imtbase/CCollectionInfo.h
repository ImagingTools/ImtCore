#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>

// ACF includes
#include <iser/IArchive.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


/**
	Implementation of a general collection information.
	\ingroup Collection
*/
class CCollectionInfo:
			virtual public ICollectionInfo,
			virtual public iser::ISerializable
{
public:
	int GetItemIndex(const QByteArray& id) const;

	virtual QByteArray InsertItem(const QByteArray& id, const QString& name, const QString& description, int position = -1);
	virtual void RemoveItem(const QByteArray& id);
	virtual void UpdateItem(const QByteArray& id, const QString& name, const QString& description);

	// reimplemented (ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	struct Item
	{
		Item(const QByteArray& id = QByteArray())
			:isEnabled(true)
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
			this->isEnabled = object.isEnabled;
			this->description = object.description;
		}

		bool isEnabled;
		QString name;
		QString description;
		QByteArray id;

		bool operator==(const Item &item) const
		{
			return ((isEnabled == item.isEnabled)
					&& (name == item.name)
					&& (description == item.description)
					&& (id == item.id));
		}

		bool operator!=(const Item &item) const
		{
			return !(operator==(item));
		}
	};

	typedef QVector<Item> Items;

protected:
	Items m_items;
};


} // namespace imtbase


