#pragma once

// Qt includes
#include <QtCore/QtGlobal>
#include <QtQuick/QQuickItem>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imtqml/IQuickObject.h>
#include <imtbase/CTreeItemModel.h>


namespace imtqml
{


/**
	Component wrapper for the QML object.
 */
class CQuickObjectCompBase:
			public QObject,
			public icomp::CComponentBase,
			virtual public imtqml::IQuickObject
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CQuickObjectCompBase);
		I_REGISTER_INTERFACE(imtqml::IQuickObject);
		I_ASSIGN(m_pathToQmlAttrPtr, "QmlFilePath", "This path used for load QML file", true, GetPathToQml());
		I_ASSIGN(m_baseUrlAttrPtr, "BaseUrl", "BaseUrl for AccessManager", false, "");
		I_ASSIGN_MULTI_0(m_modelIdsAttrPtr, "ModelIdsAttr", "If enabled, this Id's used for register models", false);
		I_ASSIGN_MULTI_0(m_modelQueriesAttrPtr, "ModelQueries", "If enabled, this Queries used for get datas", false);
	I_END_COMPONENT;

	CQuickObjectCompBase();

	static QString GetPathToQml();

	// reimplemented (imtgui::IQuickObject)
	virtual bool IsItemCreated() const override;
	virtual bool CreateQuickItem(QQuickItem* parentPtr) override;
	virtual bool DestroyQuickItem() override;
	virtual QQuickItem* GetQuickItem() const override;
	virtual void SetBaseUrl(const QString& baseUrl) const override;
	virtual void OnTryClose(bool* ignoredPtr = nullptr) override;

protected:
	/**
		Internal creation of the quick item using QML-engine.
	*/
	virtual QQuickItem* CreateItem(QQmlEngine* enginePtr) const;

	/**
		Internal creation of the quick item using QML-engine using some properties for the engine initialization.
	*/
	virtual QQuickItem* CreateItem(QQmlEngine* enginePtr, const QVariantMap& initialProperties) const;

	/**
		Callback method after the quick item was created.
		Default implementation does nothing.
	*/
	virtual void OnItemCreated();

	/**
		Callback method before the quick item will be destroyed.
		Default implementation does nothing.
	*/
	virtual void OnItemDestroyed();

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

public Q_SLOTS:
	void OnModelNeedsReload(imtbase::CTreeItemModel *itemModelPtr = nullptr);

protected:
	I_ATTR(QString, m_pathToQmlAttrPtr);
	I_ATTR(QString, m_baseUrlAttrPtr);
	I_MULTIATTR(QByteArray, m_modelIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_modelQueriesAttrPtr);

	QQuickItem* m_quickItemPtr;
	QList<imtbase::CTreeItemModel*> m_models;
};


} // namespace imtqml


