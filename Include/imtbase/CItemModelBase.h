#pragma once

#include <QObject>
#include <QVariant>


namespace imtbase
{


class CItemModelBase : public QObject
{
	Q_OBJECT
public:
	explicit CItemModelBase(QObject* parent = nullptr);

	Q_INVOKABLE virtual void beginChanges();
	Q_INVOKABLE virtual void endChanges();
	Q_INVOKABLE virtual QString toJson() const;
	Q_INVOKABLE virtual bool createFromJson(const QString& json);
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject); // copyFrom ???
	Q_INVOKABLE virtual QString toGraphQL() const;
	Q_INVOKABLE virtual bool isEqualWithModel(CItemModelBase* other) const;
	Q_INVOKABLE virtual CItemModelBase* copyMe();
	Q_INVOKABLE virtual bool copyFrom(CItemModelBase* other);
	Q_INVOKABLE virtual bool copyTo(CItemModelBase* other);
	Q_INVOKABLE virtual QObject* CreateObject(const QString& key);
	Q_INVOKABLE virtual QStringList getProperties() const;
	Q_INVOKABLE virtual QString getJSONKeyForProperty(const QString& propertyName) const;

signals:
	void modelChanged(const QVariantList& changes);
	void finished();

public slots:
	void OnInternalModelChanged();
	void OnModelChanged(const QVariantList& changes);

protected:
	virtual CItemModelBase* CreateItemModel(const QString& jsonKey);

private:
	bool m_enableNotifications = true;
	CItemModelBase* m_owner;
	bool m_isTransaction = false;
	int m_changeCount = 0;
	QVariantList m_changeList;

	void emitChange(const QString& name);
};


} // namespace imtbase


