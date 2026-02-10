// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtQuickWidgets/QQuickWidget>
#include <QtWidgets/QVBoxLayout>

// ACF includes
#include <ibase/TLocalizableWrap.h>
#include <icomp/CComponentBase.h>
#include <imod/TModelWrap.h>
#include <iqtgui/IVisualStatus.h>
#include <iqtgui/IGuiObject.h>
#include <iqtgui/TMakeIconProviderCompWrap.h>

// ImtCore includes
#include <imtqml/CQuickObjectCompBase.h>


namespace imtqml
{


/**
	Base class for IGuiObject implementation based on QML quick object.
*/
class CQmlGuiCompBase:
			public QObject,
			public ibase::TLocalizableWrap<iqtgui::TMakeIconProviderCompWrap<icomp::CComponentBase>>,
			virtual public iqtgui::IGuiObject
{
	Q_OBJECT
public:
	typedef ibase::TLocalizableWrap<iqtgui::TMakeIconProviderCompWrap<icomp::CComponentBase>> BaseClass;

	I_BEGIN_BASE_COMPONENT(CQmlGuiCompBase);
		I_REGISTER_INTERFACE(iqtgui::IGuiObject);
		I_REGISTER_SUBELEMENT(VisualStatus);
		I_REGISTER_SUBELEMENT_INTERFACE_T(VisualStatus, iqtgui::IVisualStatus, ExtractVisualStatus);
		I_REGISTER_SUBELEMENT_INTERFACE_T(VisualStatus, istd::IChangeable, ExtractVisualStatus);
		I_REGISTER_SUBELEMENT_INTERFACE_T(VisualStatus, imod::IModel, ExtractVisualStatus);
		I_ASSIGN(m_pathToQmlAttrPtr, "QmlFilePath", "This path used for load QML file", true, "");
		I_ASSIGN(m_defaultStatusIconPathAttrPtr, "DefaultStatusIcon", "Path of status icon used by default", false, "");
		I_ASSIGN(m_defaultStatusTextAttrPtr, "DefaultStatusText", "Status text used by default", true, "");
	I_END_COMPONENT;

	CQmlGuiCompBase();

	bool IsGuiShown() const;

	// reimplemented (iqtgui::IGuiObject)
	virtual bool IsGuiCreated() const override;
	virtual bool CreateGui(QWidget* parentPtr) override;
	virtual bool DestroyGui() override;
	virtual QWidget* GetWidget() const override;
	virtual void OnTryClose(bool* ignoredPtr = NULL) override;
	
protected:
	virtual void OnGuiDesignChanged();
	virtual void OnGuiShown();
	virtual void OnGuiHidden();
	virtual void OnRetranslate();
	virtual void OnGuiRetranslate();
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

	// reimplemented (ibase::TDesignSchemaHandlerWrap)
	virtual void OnDesignSchemaChanged(const QByteArray& themeId) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (QObject)
	virtual bool eventFilter(QObject* senderPtr, QEvent* eventPtr) override;

protected Q_SLOTS:
	virtual void OnQuickItemDestroyed();

protected:
	class VisualStatus: virtual public iqtgui::IVisualStatus
	{
	public:
		// reimplemented (iqtgui::IVisualStatus)
		virtual QIcon GetStatusIcon() const override;
		virtual QString GetStatusText() const override;

		friend class CQmlGuiCompBase;

	private:
		QIcon m_statusIcon;
		QString m_statusText;
	};

	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractVisualStatus(CQmlGuiCompBase& component)
	{
		return &component.m_visualStatus;
	}

protected:
	I_ATTR(QString, m_pathToQmlAttrPtr);
	I_ATTR(QString, m_defaultStatusIconPathAttrPtr);
	I_TEXTATTR(m_defaultStatusTextAttrPtr);

	QQuickWidget* m_quickWidget = nullptr;
	imod::TModelWrap<VisualStatus> m_visualStatus;

private:
	bool m_isGuiShown = false;
	bool m_hasPendingDesignChanges = false;
};


} // namespace imtqml


