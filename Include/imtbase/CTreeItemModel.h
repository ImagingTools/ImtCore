#pragma once


// Qt includes
#include <QtCore/QAbstractListModel>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QtDebug>

// ACF includes
#include <iser/ISerializable.h>
#include <imod/CModelUpdateBridge.h>
#include <imod/CModelBase.h>


namespace imtbase
{


/**
	Universal data controller for UI representations
*/
class CTreeItemModel: public QAbstractListModel, public imod::CModelBase, virtual public iser::ISerializable
{
	Q_OBJECT
	Q_PROPERTY(QString state READ State WRITE SetState NOTIFY stateChanged)

public:
	explicit CTreeItemModel(QObject* parent = nullptr);
	~CTreeItemModel();

	const QString &State() const;
	void SetState(const QString &newState);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

public Q_SLOTS:
	int InsertNewItem();
	int RemoveItem(int index, const ChangeInfoMap& infoMap = ChangeInfoMap());
	imtbase::CTreeItemModel* AddTreeModel(const QByteArray &key, int index = 0);
	bool SetExternTreeModel(const QByteArray &key, CTreeItemModel *externTreeModel, int index = 0);
	bool CopyItemDataFromModel(int index, CTreeItemModel *externTreeModel, int externIndex = 0);
	bool SetData(const QByteArray &key, const QVariant &value, int index = 0, const ChangeInfoMap& infoMap = ChangeInfoMap());
	bool RemoveData(const QByteArray &key, int index = 0, const ChangeInfoMap& infoMap = ChangeInfoMap());
	QVariant GetData(const QByteArray &key, int index = 0) const;
	imtbase::CTreeItemModel* GetParent() const;
	bool IsTreeModel(const QByteArray &key, int index = 0) const;
	bool ContainsKey(const QByteArray &key, int index = 0) const;
	imtbase::CTreeItemModel* GetTreeItemModel(const QByteArray &key, int index = 0) const;
	int GetItemsCount() const;
	void GetKeys(QList<QByteArray>& keys, int index = 0);
	void Clear();
	bool IsArray();
	void SetIsArray(const bool& isArray);
	bool Parse(const QByteArray& data);

	void SetQueryParam(const QByteArray& key, const QByteArray& value);
	QByteArray GetQueryParam(const QByteArray& key);
	QByteArray TakeQueryParam(const QByteArray& key);
	QMap<QByteArray, QByteArray> &GetQueryParams();
	void ClearQueryParams(const QByteArray& key);

	void Refresh();

	// reimplemented (QAbstractListModel)
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	virtual QHash<int, QByteArray> roleNames() const override;

	// pseudo-reimplemented (istd::IChangeable)
	virtual void BeginChanges(const istd::IChangeable::ChangeSet& changeSet);
	virtual void EndChanges(const istd::IChangeable::ChangeSet& changeSet);
	virtual void BeginChangeGroup(const istd::IChangeable::ChangeSet& changeSet);
	virtual void EndChangeGroup(const istd::IChangeable::ChangeSet& changeSet);

Q_SIGNALS:
	void stateChanged(const QString& state);
	void needsReload();

private:
	class Item
	{
	public:
		Item() {  }
		~Item() {  }

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
	imod::CModelUpdateBridge m_parentUpdateBridge;
	QMap<QByteArray,QByteArray> m_queryParams;
	bool m_isArray;

	QString m_state;

protected:
	virtual bool SerializeRecursive(iser::IArchive& archive, const QByteArray &tagName);
	virtual bool ParseRecursive(const QJsonObject &jsonObject, int index = 0);
	virtual void subModelChanged(const CTreeItemModel* model, istd::IChangeable::ChangeSet& changeSet);

	// reimplemented (imod::CModelBase)
	virtual void OnBeginGlobalChanges();
	virtual void OnEndGlobalChanges(const istd::IChangeable::ChangeSet& changeSet);
};


} // namespace imtbase


Q_DECLARE_METATYPE(imtbase::CTreeItemModel*)


