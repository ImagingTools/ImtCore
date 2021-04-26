#pragma once

// Qt includes
#include <QtCore/QAbstractListModel>
#include <QtCore/QJsonArray>

// ACF includes
#include <iser/ISerializable.h>


namespace imtauthgui
{


/**
	Universal data controller for UI representations
	\ingroup LicenseManagement
*/


class CTreeItemModel : public QAbstractListModel, virtual public iser::ISerializable
{
	Q_OBJECT

public:
	explicit CTreeItemModel(QObject* parent = nullptr);
	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

public slots:
	int addItem();
	void addTreeModel(const QByteArray &key, int index = 0);
	bool setData(const QByteArray &key, const QVariant &value, int index = 0);
	QVariant getData(const QByteArray &key, int index = 0) const;
	bool isTreeModel(const QByteArray &key, int index = 0);
	CTreeItemModel* getTreeItemModel(const QByteArray &key, int index = 0);
	int ItemsCount();

	// reimplemented (QAbstractListModel)
	int rowCount(const QModelIndex & parent = QModelIndex()) const override;

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

	// reimplemented (QAbstractListModel)
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;


};


} // namespace imtauthgui

