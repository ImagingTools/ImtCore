#pragma once


// Qt includes
#include <QtCore/QAbstractListModel>

// ACF includes
#include <iser/ISerializable.h>
#include <imod/CModelUpdateBridge.h>
#include <imod/CModelBase.h>


namespace imtbase
{


/**
	Universal data controller for UI representations
	\ingroup LicenseManagement
*/
class CTreeItemModel: public QAbstractListModel, public imod::CModelBase, virtual public iser::ISerializable
{
	Q_OBJECT
	Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)

public:
	explicit CTreeItemModel(QObject* parent = nullptr);
	~CTreeItemModel();

	const QString &state() const;
	void setState(const QString &newState);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

public Q_SLOTS:
	int InsertNewItem();
	CTreeItemModel* AddTreeModel(const QByteArray &key, int index = 0);
	bool SetData(const QByteArray &key, const QVariant &value, int index = 0, const ChangeInfoMap& infoMap = ChangeInfoMap());
	QVariant GetData(const QByteArray &key, int index = 0) const;
	bool IsTreeModel(const QByteArray &key, int index = 0) const;
	CTreeItemModel* GetTreeItemModel(const QByteArray &key, int index = 0) const;
	int GetItemsCount() const;
	void GetKeys(QList<QByteArray>& keys, int index = 0);

	// reimplemented (QAbstractListModel)
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
	virtual QHash<int, QByteArray> roleNames() const override;

	// pseudo-reimplemented (istd::IChangeable)
//	virtual int GetSupportedOperations() const;
//	virtual istd::IChangeable* CloneMe(istd::IChangeable::CompatibilityMode mode = istd::IChangeable::CM_WITHOUT_REFS) const;
	virtual void BeginChanges(const istd::IChangeable::ChangeSet& changeSet);
	virtual void EndChanges(const istd::IChangeable::ChangeSet& changeSet);
	virtual void BeginChangeGroup(const istd::IChangeable::ChangeSet& changeSet);
	virtual void EndChangeGroup(const istd::IChangeable::ChangeSet& changeSet);

Q_SIGNALS:
	void stateChanged(const QString& state);

protected:
	virtual bool SerializeRecursive(iser::IArchive& archive, const QByteArray &tagName);
	virtual void subModelChanged(const CTreeItemModel* model, istd::IChangeable::ChangeSet& changeSet);
	// reimplemented (imod::CModelBase)
	virtual void OnBeginGlobalChanges();
	virtual void OnEndGlobalChanges(const istd::IChangeable::ChangeSet& changeSet);

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
	imod::CModelUpdateBridge m_parentUpdateBridge;


	QString m_state;
};


} // namespace imtbase

