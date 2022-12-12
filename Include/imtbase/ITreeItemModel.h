#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


/**
	Universal interface of data controller for UI representations
*/
class ITreeItemModel: virtual public iser::ISerializable
{
public:
	/**
		Add item to model
	*/
	virtual int InsertNewItem() = 0;

	/**
		Remove item from model
	*/
	virtual int RemoveItem(int index, const ChangeInfoMap& infoMap = ChangeInfoMap()) = 0;

	/**
		Add child item model to main model
	*/
	virtual imtbase::ITreeItemModel* AddTreeModel(const QByteArray &key, int index = 0) = 0;

	/**
		Add child extern item model to main model
	*/
	virtual bool SetExternTreeModel(const QByteArray &key, ITreeItemModel *externTreeModel, int index = 0) = 0;

	/**
		Copy item data from model
	*/
	virtual bool CopyItemDataFromModel(int index, ITreeItemModel *externTreeModel, int externIndex = 0) = 0;

	/**
		Set data to item
	*/
	virtual bool SetData(const QByteArray &key, const QVariant &value, int index = 0, const ChangeInfoMap& infoMap = ChangeInfoMap()) = 0;

	/**
		Remove data from item
	*/
	virtual bool RemoveData(const QByteArray &key, int index = 0, const ChangeInfoMap& infoMap = ChangeInfoMap()) = 0;

	/**
		Get data from item
	*/
	virtual QVariant GetData(const QByteArray &key, int index = 0) const = 0;

	/**
		Get parent model for current model
	*/
	virtual imtbase::ITreeItemModel* GetParent() const = 0;

	/**
		Checking that the item tree model
	*/
	virtual bool IsTreeModel(const QByteArray &key, int index = 0) const = 0;

	/**
		Checking that the model contains a key
	*/
	virtual bool ContainsKey(const QByteArray &key, int index = 0) const = 0;

	/**
		Get tree item model by key
	*/
	virtual imtbase::ITreeItemModel* GetRepresentation(const QByteArray &key, int index = 0) const = 0;

	/**
		Get items count for model
	*/
	virtual int GetItemsCount() const = 0;

	/**
		Get keys for model
	*/
	virtual void GetKeys(QList<QByteArray>& keys, int index = 0) = 0;

	/**
		Get keys list for model
	*/
	virtual QList<QByteArray> GetKeys(int index = 0) = 0;

	/**
		Clear all items from model
	*/
	virtual void Clear() = 0;

	/**
		Checking that the item array
	*/
	virtual bool IsArray() = 0;

	/**
		Make item an array
	*/
	virtual void SetIsArray(const bool& isArray) = 0;

	/**
		Parse model data from byte array
	*/
	virtual bool Parse(const QByteArray& data) = 0;

	/**
		Parse model data to JSON
	*/
	virtual QString ToJSON() = 0;
};


} // namespace imtbase


