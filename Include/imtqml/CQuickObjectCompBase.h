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
	QML object.
 */
class CQuickObjectComp:
			public icomp::CComponentBase,
			virtual public imtqml::IQuickObject
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CQuickObjectComp);
		I_REGISTER_INTERFACE(imtqml::IQuickObject);
		I_ASSIGN(m_pathToQmlAttrPtr, "QmlFilePath", "If enabled, this path used for load QML file", true, "QmlFilePath");
	I_END_COMPONENT;

	CQuickObjectComp();

	// reimplemented (imtgui::IQuickObject)
	virtual bool IsItemCreated() const override;
	virtual bool CreateItem(QQuickItem* parentPtr) override;
	virtual bool DestroyItem() override;
	virtual QQuickItem* GetItem() const override;
	virtual void OnTryClose(bool* ignoredPtr = nullptr) override;

protected:
	virtual QQuickItem* CreateItem(QQmlEngine* enginePtr);
	virtual QQuickItem* CreateItem(QQmlEngine* enginePtr, const QVariantMap& initialProperties);

	virtual void OnItemCreated(QQuickItem& item);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_ATTR(QString, m_pathToQmlAttrPtr);

	QQuickItem* m_quickItemPtr;
};


} // namespace imtqml



