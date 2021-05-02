#pragma once

// Qt includes
#include <QtCore/QtGlobal>
#include <QtQuick/QQuickItem>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imtqml/IQuickObject.h>


namespace imtqml
{


/**
	Component wrapper for the QML object.
 */
class CQuickObjectCompBase:
			public icomp::CComponentBase,
			virtual public imtqml::IQuickObject
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CQuickObjectCompBase);
		I_REGISTER_INTERFACE(imtqml::IQuickObject);
		I_ASSIGN(m_pathToQmlAttrPtr, "QmlFilePath", "If enabled, this path used for load QML file", true, "QmlFilePath");
	I_END_COMPONENT;

	CQuickObjectCompBase();

	// reimplemented (imtgui::IQuickObject)
	virtual bool IsItemCreated() const override;
	virtual bool CreateQuickItem(QQuickItem* parentPtr) override;
	virtual bool DestroyQuickItem() override;
	virtual QQuickItem* GetQuickItem() const override;
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

protected:
	I_ATTR(QString, m_pathToQmlAttrPtr);

	QQuickItem* m_quickItemPtr;
};


} // namespace imtqml



