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
#include <istd/TChangeDelegator.h>
#include <istd/TSmartPtr.h>


namespace imtbase
{


/**
	Universal data controller for UI representations
*/
class CTreeItemModel: public QAbstractListModel, public istd::CChangeDelegator, public iser::ISerializable
{
	Q_OBJECT
	Q_PROPERTY(QString state READ State WRITE SetState NOTIFY stateChanged)

public:
	typedef istd::CChangeDelegator BaseClass;
	typedef QAbstractListModel BaseClass2;

	explicit CTreeItemModel(QObject* parent = nullptr);
	~CTreeItemModel();

	const QString &State() const;
	void SetState(const QString &newState);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual void OnEndChanges(const ChangeSet& changeSet) override;
	virtual void OnBeginChanges() override;

public Q_SLOTS:
	void SetParent(QObject *parent);
	bool Copy(CTreeItemModel* object);
	void InsertNewItemWithParameters(int index, const QVariantMap& map);
	int InsertNewItem();
	int RemoveItem(int index, const ChangeInfoMap& infoMap = ChangeInfoMap());
	imtbase::CTreeItemModel* AddTreeModel(const QByteArray &key, int index = 0);
	bool SetExternTreeModel(const QByteArray &key, CTreeItemModel *externTreeModel, int index = 0);
	bool CopyItemDataFromModel(int index, const CTreeItemModel *externTreeModel, int externIndex = 0);
	bool CopyItemDataFromModel(int index, CTreeItemModel *externTreeModel, int externIndex = 0);
	bool CopyItemDataToModel(int index, CTreeItemModel *externTreeModel, int externIndex = 0) const;
	bool SetData(const QByteArray &key, const QVariant &value, int index = 0, const ChangeInfoMap& infoMap = ChangeInfoMap());
	bool RemoveData(const QByteArray &key, int index = 0, const ChangeInfoMap& infoMap = ChangeInfoMap());
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

	void Refresh();

	QString toJSON();

	// reimplemented (QAbstractListModel)
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	virtual QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
	void stateChanged(const QString& state);
	void needsReload();
	void modelChanged(const ChangeSet& changeSet);

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

protected:
	virtual bool SerializeRecursive(iser::IArchive& archive, const QByteArray &tagName);
	virtual bool ParseRecursive(const QJsonObject &jsonObject, int index = 0);
	virtual void subModelChanged(const CTreeItemModel* model, istd::IChangeable::ChangeSet& changeSet);
};


typedef istd::TSmartPtr<imtbase::CTreeItemModel> CHierarchicalItemModelPtr;


} // namespace imtbase


Q_DECLARE_METATYPE(imtbase::CTreeItemModel*)


