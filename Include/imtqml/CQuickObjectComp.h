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
		I_ASSIGN(m_pathToQmlPtr, "PathToQmlPtr", "If enabled, this path used for load Qml", true, "");
	I_END_COMPONENT;

	CQuickObjectComp();

	// reimplemented (imtgui::IQuickObject)
	virtual bool IsItemCreated() const;
	virtual bool CreateItem(QQuickItem* parentPtr);
	virtual bool CreateItem(QQmlEngine* engine);
	virtual bool DestroyItem();
	virtual QQuickItem* GetItem() const;
	virtual void OnTryClose(bool* ignoredPtr = NULL);

protected:
	I_ATTR(QString, m_pathToQmlPtr);

	QQuickItem* m_quickItemPtr;
};


} // namespace imtqml



