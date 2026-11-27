// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CStandartSplashScreenQuickComp.h>


// Qt includes
#include <QtCore/QThread>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickWindow>


namespace imtqml
{


// public methods

CStandartSplashScreenQuickComp::CStandartSplashScreenQuickComp()
	:m_quickItemPtr(nullptr)
{
}


// reimplemented (imtqml::IQuickObject)

bool CStandartSplashScreenQuickComp::IsItemCreated() const
{
	return (m_quickItemPtr != nullptr);
}


bool CStandartSplashScreenQuickComp::CreateQuickItem(QQuickItem* parentPtr)
{
	if (parentPtr == nullptr || m_quickItemPtr != nullptr){
		return false;
	}

	QQmlEngine* enginePtr = qmlEngine(parentPtr);
	if (enginePtr == nullptr && parentPtr->window() != nullptr){
		enginePtr = qmlEngine(parentPtr->window());
	}
	if (enginePtr == nullptr){
		return false;
	}

	enginePtr->addImportPath(QStringLiteral("qrc:/qml"));

	QQmlComponent component(enginePtr, QUrl(QStringLiteral("qrc:/qml/imtgui/Application/StandartSplashScreen.qml")));
	m_quickItemPtr = qobject_cast<QQuickItem*>(component.create(enginePtr->rootContext()));
	if (m_quickItemPtr == nullptr){
		qWarning() << "Unable to create the splash screen item:" << component.errors();

		return false;
	}

	m_quickItemPtr->setParentItem(parentPtr);

	UpdateItemProperties();

	return true;
}


bool CStandartSplashScreenQuickComp::DestroyQuickItem()
{
	if (m_quickItemPtr == nullptr){
		return false;
	}

	m_quickItemPtr->deleteLater();
	m_quickItemPtr = nullptr;

	return true;
}


QQuickItem* CStandartSplashScreenQuickComp::GetQuickItem() const
{
	return m_quickItemPtr;
}


void CStandartSplashScreenQuickComp::OnTryClose(bool* /*ignoredPtr*/)
{
}


// reimplemented (ilog::IMessageConsumer)

bool CStandartSplashScreenQuickComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CStandartSplashScreenQuickComp::AddMessage(const MessagePtr& messagePtr)
{
	QQuickItem* itemPtr = m_quickItemPtr;
	if (itemPtr == nullptr || !messagePtr.IsValid()){
		return;
	}

	const QString text = messagePtr->GetInformationDescription();

	QMetaObject::invokeMethod(itemPtr, [itemPtr, text](){
		itemPtr->setProperty("progressText", text);
	});

	// The main thread is busy with the initialization, so the splash must be repainted explicitly:
	if (QThread::currentThread() == itemPtr->thread()){
		QGuiApplication::processEvents();
	}
}


// private methods

void CStandartSplashScreenQuickComp::UpdateItemProperties() const
{
	Q_ASSERT(m_quickItemPtr != nullptr);

	QString productName;
	QString applicationName;
	QString applicationSubname;
	QString applicationType;
	QString legalCopyright;
	QString mainVersionText;
	QString versionKindText;
	QStringList versionDescriptions;
	QStringList versionTexts;

	if (m_applicationInfoCompPtr.IsValid()){
		const int mainVersionId = m_applicationInfoCompPtr->GetMainVersionId();

		versionKindText = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_VERSION_KIND);

		if (mainVersionId >= 0){
			const iser::IVersionInfo& versionInfo = m_applicationInfoCompPtr->GetVersionInfo();

			quint32 mainVersionNumber;
			if (versionInfo.GetVersionNumber(mainVersionId, mainVersionNumber)){
				mainVersionText = versionInfo.GetEncodedVersionName(mainVersionId, mainVersionNumber);
			}
		}

		AddVersionTexts(mainVersionId, versionDescriptions, versionTexts);

		if (*m_showProductNameAttrPtr){
			productName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_PRODUCT_NAME);
		}

		if (*m_showApplicationNameAttrPtr){
			applicationName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_NAME);
		}

		if (*m_showApplicationSubnameAttrPtr){
			applicationSubname = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_SUBNAME);
		}

		if (*m_showProductTypeAttrPtr){
			applicationType = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_TYPE);
		}

		if (*m_showLegalCopyrightAttrPtr){
			legalCopyright = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_LEGAL_COPYRIGHT);
		}
	}

	QString applicationTypeText = applicationSubname + applicationType;
	if (!applicationSubname.isEmpty() && !applicationType.isEmpty()){
		applicationTypeText = applicationSubname + '(' + applicationType + ')';
	}

	// make the application version kind (e.g. "Beta Version") visible in the UI
	if (!versionKindText.isEmpty()){
		mainVersionText = mainVersionText.isEmpty() ? versionKindText : mainVersionText + QStringLiteral(" - ") + versionKindText;
	}

	if (m_backgroundImagePathAttrPtr.IsValid() && !(*m_backgroundImagePathAttrPtr).isEmpty()){
		m_quickItemPtr->setProperty("backgroundSource", QUrl::fromUserInput(*m_backgroundImagePathAttrPtr));
	}

	m_quickItemPtr->setProperty("productName", productName);
	m_quickItemPtr->setProperty("applicationName", applicationName);
	m_quickItemPtr->setProperty("applicationType", applicationTypeText);
	m_quickItemPtr->setProperty("mainVersion", mainVersionText);
	m_quickItemPtr->setProperty("versionDescriptions", versionDescriptions);
	m_quickItemPtr->setProperty("versionTexts", versionTexts);
	m_quickItemPtr->setProperty("copyright", legalCopyright);
}


void CStandartSplashScreenQuickComp::AddVersionTexts(int mainVersionId, QStringList& descriptions, QStringList& versions) const
{
	const iser::IVersionInfo& versionInfo = m_applicationInfoCompPtr->GetVersionInfo();

	if (m_versionIdsAttrPtr.IsValid()){
		const int versionsCount = m_versionIdsAttrPtr.GetCount();

		for (int i = 0; i < versionsCount; ++i){
			const int versionId = m_versionIdsAttrPtr[i];

			quint32 version;
			if (versionInfo.GetVersionNumber(versionId, version)){
				descriptions << ((i < m_versionNamesAttrPtr.GetCount()) ? m_versionNamesAttrPtr[i] : versionInfo.GetVersionIdDescription(versionId));
				versions << versionInfo.GetEncodedVersionName(versionId, version);
			}
		}

		return;
	}

	const iser::IVersionInfo::VersionIds ids = versionInfo.GetVersionIds();
	for (int versionId : ids){
		quint32 version;
		if ((versionId != mainVersionId) && versionInfo.GetVersionNumber(versionId, version)){
			descriptions << versionInfo.GetVersionIdDescription(versionId);
			versions << versionInfo.GetEncodedVersionName(versionId, version);
		}
	}
}


} // namespace imtqml


