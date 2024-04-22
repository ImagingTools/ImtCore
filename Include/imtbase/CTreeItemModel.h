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
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Hierarchical representation of the data model.
*/
class CTreeItemModel: public QAbstractListModel, virtual public istd::IChangeable
{
	Q_OBJECT
	Q_PROPERTY(QString state READ State WRITE SetState NOTIFY stateChanged)

public:
	typedef QAbstractListModel BaseClass2;

	explicit CTreeItemModel(QObject* parent = nullptr);
	~CTreeItemModel();

	const QString &State() const;
	void SetState(const QString &newState);

	virtual bool SerializeModel(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)

public Q_SLOTS:
	void SetParent(QObject *parent);
	bool Copy(const CTreeItemModel* object);
	imtbase::CTreeItemModel* CopyMe() const;
	bool IsEqualWithModel(CTreeItemModel* modelPtr) const;
	void InsertNewItemWithParameters(int index, const QVariantMap& map);
	int InsertNewItem();
	int InsertNewItem(int index);
	int RemoveItem(int index);
	imtbase::CTreeItemModel* AddTreeModel(const QByteArray &key, int index = 0);
	bool SetExternTreeModel(const QByteArray &key, CTreeItemModel *externTreeModel, int index = 0);
	bool CopyItemDataFromModel(int index, const CTreeItemModel *externTreeModel, int externIndex = 0);
	bool CopyItemDataFromModel(int index, CTreeItemModel *externTreeModel, int externIndex = 0);
	bool CopyItemDataToModel(int index, CTreeItemModel *externTreeModel, int externIndex = 0) const;
	bool SetData(const QByteArray &key, const QVariant &value, int index = 0);
	bool RemoveData(const QByteArray &key, int index = 0);
	QVariant GetData(const QByteArray &key, int index = 0) const;
	imtbase::CTreeItemModel* GetParent() const;
	bool IsTreeModel(const QByteArray &key, int index = 0) const;
	bool ContainsKey(const QByteArray &key, int index = 0) const;
	bool IsValidData(const QByteArray &key, int index = 0) const;
	imtbase::CTreeItemModel* GetTreeItemModel(const QByteArray &key, int index = 0) const;
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
	QMap<QByteArray, QByteArray> &GetQueryParams();
	void ClearQueryParams(const QByteArray& key);
	void SetUpdateEnabled(bool updateEnabled);
	void OnDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

	void Refresh();

	QString ToJson();

	// reimplemented (QAbstractListModel)
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	virtual QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
	void stateChanged(const QString& state);
	void needsReload();
	
protected:
	bool SerializeRecursive(iser::IArchive& archive, const QByteArray& tagName);

private:
	class Item
	{
	public:
		Item() {}
		~Item() {}

		void SetValue(const QByteArray& key, const QVariant& value) {
			if (!m_map.contains(key)){
				m_keys.append(key);
			}
			m_map.insert(key,value);
		}
		void RemoveValue(const QByteArray& key) {
			m_map.remove(key); m_keys.removeAll(key);
		}
		QVariant Value(const QByteArray& key) {
			QVariant res;
			if(m_map.contains(key))
				res = m_map.value(key);
			return res;
		}
		void GetKeys(QList<QByteArray>& keys) { keys = m_keys; }
		bool Contains(const QByteArray &key) { return m_map.contains(key); }
		bool ContainsKey(const QByteArray &key) { return m_keys.contains(key); }
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
	bool m_isTransaction;

protected:
	virtual int GetKeyRole(const QByteArray& key) const;
	virtual bool ParseRecursive(const QJsonObject &jsonObject, int index = 0);
};


typedef istd::TSmartPtr<imtbase::CTreeItemModel> CHierarchicalItemModelPtr;


} // namespace imtbase


Q_DECLARE_METATYPE(imtbase::CTreeItemModel*)


