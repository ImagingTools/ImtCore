#pragma once


// Qt includes
#include <QtCore/QAbstractListModel>

// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


/**
	Universal data controller for UI representations
	\ingroup LicenseManagement
*/
class CTreeItemModel: public QAbstractListModel, virtual public iser::ISerializable
{
	Q_OBJECT
	Q_PROPERTY(QString state READ GetState WRITE SetState NOTIFY stateChanged)

public:
	explicit CTreeItemModel(QObject* parent = nullptr);

	const QString& GetState() const;
	void SetState(const QString &newState);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

public Q_SLOTS:
	int InsertNewItem();
	void AddTreeModel(const QByteArray &key, int index = 0);
	bool SetData(const QByteArray &key, const QVariant &value, int index = 0);
	QVariant GetData(const QByteArray &key, int index = 0) const;
	bool IsTreeModel(const QByteArray &key, int index = 0) const;
	CTreeItemModel* GetTreeItemModel(const QByteArray &key, int index = 0) const;
	int GetItemsCount() const;

	// reimplemented (QAbstractListModel)
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
	virtual QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
	void stateChanged(const QString& state);

protected:
	virtual bool SerializeRecursive(iser::IArchive& archive, const QByteArray &tagName);

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
	private:
		QMap<QByteArray,QVariant> m_map;
		QList<QByteArray> m_keys;
	};

	QList<Item*> m_items;
	QHash<int, QByteArray> m_roleNames;

	QString m_state;
};


} // namespace imtbase

