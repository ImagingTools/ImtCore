// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QAbstractListModel>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QtDebug>
#include <QtCore/QVariantMap>

// ACF includes
#include <iser/ISerializable.h>
#include <istd/TSmartPtr.h>


namespace imtbase
{


/**
	Hierarchical representation of the data model.
*/
class CTreeItemModel: public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(QString state READ State WRITE SetState NOTIFY stateChanged)

public:
	typedef QAbstractListModel BaseClass2;

	Q_INVOKABLE inline bool beginChanges(){ return BeginChanges(); }
	Q_INVOKABLE inline bool endChanges(){ return EndChanges(); }
	Q_INVOKABLE inline bool copy(const CTreeItemModel* object){ return Copy(object); }
	Q_INVOKABLE inline void setParent(QObject* parent){ SetParent(parent); }
	Q_INVOKABLE inline imtbase::CTreeItemModel* copyMe(){ return CopyMe(); }
	Q_INVOKABLE inline bool isEqualWithModel(CTreeItemModel* modelPtr){ return IsEqualWithModel(modelPtr); }
	Q_INVOKABLE inline void insertNewItemWithParameters(int index, const QVariantMap& map){ InsertNewItemWithParameters(index, map); }
	Q_INVOKABLE inline int insertNewItem(){ return InsertNewItem(); }
	Q_INVOKABLE inline int insertNewItem(int index){ return InsertNewItem(index); }
	Q_INVOKABLE inline int removeItem(int index){ return RemoveItem(index); }
	Q_INVOKABLE inline bool swapItems(int index1, int index2){ return SwapItems(index1, index2); }
	Q_INVOKABLE inline imtbase::CTreeItemModel* addTreeModel(const QByteArray& key, int index = 0){ return AddTreeModel(key, index); }
	Q_INVOKABLE inline bool setExternTreeModel(const QByteArray& key, CTreeItemModel* externTreeModel, int index = 0){ return SetExternTreeModel(key, externTreeModel, index); }
	Q_INVOKABLE inline bool copyItemDataFromModel(int index, const CTreeItemModel* externTreeModel, int externIndex = 0){ return CopyItemDataFromModel(index, externTreeModel, externIndex); }
	Q_INVOKABLE inline bool copyItemDataFromModel(int index, CTreeItemModel* externTreeModel, int externIndex = 0){ return CopyItemDataFromModel(index, externTreeModel, externIndex); }
	Q_INVOKABLE inline bool copyItemDataToModel(int index, CTreeItemModel* externTreeModel, int externIndex = 0) const{ return CopyItemDataToModel(index, externTreeModel, externIndex); }
	Q_INVOKABLE inline bool setData(const QByteArray& key, const QVariant& value, int index = 0){ return SetData(key, value, index); }
	Q_INVOKABLE inline bool removeData(const QByteArray& key, int index = 0){ return RemoveData(key, index); }
	Q_INVOKABLE inline QVariant getData(const QByteArray& key, int index = 0) const{ return GetData(key, index); }
	Q_INVOKABLE inline imtbase::CTreeItemModel* getParent() const{ return GetParent(); }
	Q_INVOKABLE inline bool isTreeModel(const QByteArray& key, int index = 0) const{ return IsTreeModel(key, index); }
	Q_INVOKABLE inline bool containsKey(const QByteArray& key, int index = 0) const{ return ContainsKey(key, index); }
	Q_INVOKABLE inline bool isValidData(const QByteArray& key, int index = 0) const{ return IsValidData(key, index); }
	Q_INVOKABLE inline imtbase::CTreeItemModel* getTreeItemModel(const QByteArray& key, int index = 0) const{ return GetTreeItemModel(key, index); }
	Q_INVOKABLE inline imtbase::CTreeItemModel* getModelFromItem(int itemIndex = 0) const{ return GetModelFromItem(itemIndex); }
	Q_INVOKABLE inline int getItemsCount() const{ return GetItemsCount(); }
	Q_INVOKABLE inline void getKeys(QList<QByteArray>& keys, int index = 0) const{ return GetKeys(keys, index); }
	Q_INVOKABLE inline QList<QString> getKeys(int index = 0) const{ return GetKeys(index); }
	Q_INVOKABLE inline void clear(){ Clear(); }
	Q_INVOKABLE inline bool isArray(){ return IsArray(); }
	Q_INVOKABLE inline void setIsArray(const bool& isArray){ SetIsArray(isArray); }
	Q_INVOKABLE inline bool createFromJson(const QByteArray& jsonContent){ return CreateFromJson(jsonContent); }
	Q_INVOKABLE inline void setQueryParam(const QByteArray& key, const QByteArray& value){ SetQueryParam(key, value); }
	Q_INVOKABLE inline QByteArray getQueryParam(const QByteArray& key){ return GetQueryParam(key); }
	Q_INVOKABLE inline QByteArray takeQueryParam(const QByteArray& key){ return TakeQueryParam(key); }
	Q_INVOKABLE inline QMap<QByteArray, QByteArray>& getQueryParams(){ return GetQueryParams(); }
	Q_INVOKABLE inline void clearQueryParams(const QByteArray& key){ ClearQueryParams(key); }
	Q_INVOKABLE inline void setUpdateEnabled(bool updateEnabled){ SetUpdateEnabled(updateEnabled); }
	Q_INVOKABLE inline void refresh(){ Refresh(); }
	Q_INVOKABLE inline QString toJson(){ return ToJson(); }

	explicit CTreeItemModel(QObject* parent = nullptr);
	~CTreeItemModel();

	const QString& State() const;
	void SetState(const QString& newState);

	virtual bool SerializeModel(iser::IArchive& archive);

	bool BeginChanges();
	bool EndChanges();

	void SetParent(QObject* parent);
	bool Copy(const CTreeItemModel* object);
	imtbase::CTreeItemModel* CopyMe() const;
	bool IsEqualWithModel(CTreeItemModel* modelPtr) const;
	bool IsEqualWithModel(const CTreeItemModel* modelPtr) const;
	void InsertNewItemWithParameters(int index, const QVariantMap& map);
	int InsertNewItem();
	int InsertNewItem(int index);
	int RemoveItem(int index);
	bool SwapItems(int index1, int index2);
	imtbase::CTreeItemModel* AddTreeModel(const QByteArray& key, int index = 0);
	bool SetExternTreeModel(const QByteArray& key, CTreeItemModel* externTreeModel, int index = 0);
	bool CopyItemDataFromModel(int index, const CTreeItemModel* externTreeModel, int externIndex = 0);
	bool CopyItemDataFromModel(int index, CTreeItemModel* externTreeModel, int externIndex = 0);
	bool CopyItemDataToModel(int index, CTreeItemModel* externTreeModel, int externIndex = 0) const;
	bool SetData(const QByteArray&key, const QVariant& value, int index = 0);
	bool RemoveData(const QByteArray& key, int index = 0);
	QVariant GetData(const QByteArray& key, int index = 0) const;
	imtbase::CTreeItemModel* GetParent() const;
	bool IsTreeModel(const QByteArray& key, int index = 0) const;
	bool ContainsKey(const QByteArray& key, int index = 0) const;
	bool IsValidData(const QByteArray& key, int index = 0) const;
	imtbase::CTreeItemModel* GetTreeItemModel(const QByteArray& key, int index = 0) const;
	imtbase::CTreeItemModel* GetModelFromItem(int itemIndex = 0) const;
	int GetItemsCount() const;
	void GetKeys(QList<QByteArray>& keys, int index = 0) const;
	QList<QString> GetKeys(int index = 0) const;
	void Clear();
	bool IsArray();
	void SetIsArray(const bool& isArray);
	bool CreateFromJson(const QByteArray& jsonContent);
	void SetQueryParam(const QByteArray& key, const QByteArray& value);
	QByteArray GetQueryParam(const QByteArray& key);
	QByteArray TakeQueryParam(const QByteArray& key);
	QMap<QByteArray, QByteArray>& GetQueryParams();
	void ClearQueryParams(const QByteArray& key);
	void SetUpdateEnabled(bool updateEnabled);
	void Refresh();
	QString ToJson();

	// reimplemented (QAbstractListModel)
	virtual int rowCount(const QModelIndex&  parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex&  index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	virtual QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
	void OnModelChanged();

Q_SIGNALS:
	void stateChanged(const QString& state);
	void needsReload();
	void modelChanged();

protected:
	bool SerializeRecursive(iser::IArchive& archive, const QByteArray& tagName);

private:
	class Item
	{
	public:
		Item(){}
		~Item(){}

		void SetValue(const QByteArray& key, const QVariant& value){
			if (!m_map.contains(key)){
				m_keys.append(key);
			}
			m_map.insert(key,value);
		}
		void RemoveValue(const QByteArray& key){
			m_map.remove(key); m_keys.removeAll(key);
		}
		QVariant Value(const QByteArray& key){
			QVariant res;
			if(m_map.contains(key))
				res = m_map.value(key);
			return res;
		}
		void GetKeys(QList<QByteArray>& keys){ keys = m_keys; }
		bool Contains(const QByteArray& key){ return m_map.contains(key); }
		bool ContainsKey(const QByteArray& key){ return m_keys.contains(key); }
	private:
		QMap<QByteArray,QVariant> m_map;
		QList<QByteArray> m_keys;
	};

	QList<Item*> m_items;
	QHash<int, QByteArray> m_roleNames;
	QMap<QByteArray,QByteArray> m_queryParams;
	bool m_isArray;

	QString m_state;
	bool m_isUpdateEnabled;

	int m_countTransaction;
	int m_countChanges;

protected:
	virtual int GetKeyRole(const QByteArray& key) const;
	virtual bool ParseRecursive(const QJsonObject& jsonObject, int index = 0);
};


typedef istd::TSmartPtr<imtbase::CTreeItemModel> CHierarchicalItemModelPtr;


} // namespace imtbase


Q_DECLARE_METATYPE(imtbase::CTreeItemModel*)


