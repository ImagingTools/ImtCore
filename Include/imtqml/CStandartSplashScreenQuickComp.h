// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>
#include <icomp/CComponentBase.h>
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtqml/IQuickObject.h>


namespace imtqml
{


/**
	Splash screen component based on the QML object.
	The QML file (imtgui/Application/StandartSplashScreen.qml) shows the background image from the resources together with the product, application and version information.
	The same information is shown as in the iqtgui::CSplashScreenGuiComp of the ACF.
	Messages sent to the component are shown as the progress text of the splash screen.
	The path to the QML file is fixed, so the component implements imtqml::IQuickObject directly and has no 'QmlFilePath' attribute.
*/
class CStandartSplashScreenQuickComp:
			public icomp::CComponentBase,
			virtual public imtqml::IQuickObject,
			virtual public ilog::IMessageConsumer
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStandartSplashScreenQuickComp);
		I_REGISTER_INTERFACE(imtqml::IQuickObject);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN(m_backgroundImagePathAttrPtr, "BackgroundImagePath", "Path (URL) of the background image, e.g. from the resources", true, "qrc:/qml/imtgui/Resources/Images/StandartSplashScreen.png");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Version management for application", false, "ApplicationInfo");
		I_ASSIGN(m_showProductNameAttrPtr, "ShowProductName", "If true, product name will be shown, disable it, if it was painted on background image", true, true);
		I_ASSIGN(m_showApplicationNameAttrPtr, "ShowApplicationName", "If true, application name will be shown, disable it, if it was painted on background image", true, true);
		I_ASSIGN(m_showApplicationSubnameAttrPtr, "ShowApplicationSubName", "If true, application subname will be shown, disable it, if it was painted on background image", true, false);
		I_ASSIGN(m_showProductTypeAttrPtr, "ShowProductType", "Additional product description shown over version info", true, false);
		I_ASSIGN(m_showLegalCopyrightAttrPtr, "ShowLegalCopyright", "If enabled legal copyrights will be shown", true, true);
		I_ASSIGN_MULTI_1(m_versionIdsAttrPtr, "VersionIds", "List of version ID's for version information should be displayed", false, 0);
		I_ASSIGN_MULTI_1(m_versionNamesAttrPtr, "VersionNames", "List of version names will be used as version description", false, "Version");
	I_END_COMPONENT;

	CStandartSplashScreenQuickComp();

	// reimplemented (imtqml::IQuickObject)
	virtual bool IsItemCreated() const override;
	virtual bool CreateQuickItem(QQuickItem* parentPtr) override;
	virtual bool DestroyQuickItem() override;
	virtual QQuickItem* GetQuickItem() const override;
	virtual void OnTryClose(bool* ignoredPtr = nullptr) override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

private:
	void UpdateItemProperties() const;
	void AddVersionTexts(int mainVersionId, QStringList& descriptions, QStringList& versions) const;

private:
	QQuickItem* m_quickItemPtr;

	I_TEXTATTR(m_backgroundImagePathAttrPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_ATTR(bool, m_showProductNameAttrPtr);
	I_ATTR(bool, m_showApplicationNameAttrPtr);
	I_ATTR(bool, m_showApplicationSubnameAttrPtr);
	I_ATTR(bool, m_showProductTypeAttrPtr);
	I_ATTR(bool, m_showLegalCopyrightAttrPtr);
	I_MULTIATTR(int, m_versionIdsAttrPtr);
	I_MULTITEXTATTR(m_versionNamesAttrPtr);
};


} // namespace imtqml


