// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CObserverQmlComp.h>


// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


namespace imtqml
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CObserverQmlComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	
	if (!m_quickObjectCompPtr.IsValid()){
		return;
	}
	
	if (!m_settingsRepresentationControllerCompPtr.IsValid()){
		return;
	}
	
	QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
	if (quickItem == nullptr){
		return;
	}

	connect(quickItem, SIGNAL(saveSettings(QString)), this, SLOT(OnSettingsChanged(QString)));
	
	QJsonObject jsonRepresentation;
	if (m_settingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*m_settingsCompPtr, jsonRepresentation)){
		QJsonDocument jsonDocument(jsonRepresentation);
		quickItem->setProperty("localSettings", jsonDocument.toJson(QJsonDocument::Compact));
	}
}


void CObserverQmlComp::OnSettingsChanged(QString jsonData)
{
	if (!m_settingsRepresentationControllerCompPtr.IsValid()){
		return;
	}
	
	if (!m_settingsCompPtr.IsValid()){
		return;
	}
	
	if (!m_quickObjectCompPtr.IsValid()){
		return;
	}

	QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
	if (quickItem == nullptr){
		return;
	}
	
	QJsonParseError jsonParseError;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8(), &jsonParseError);
	if (jsonParseError.error != QJsonParseError::NoError){
		qDebug() << jsonParseError.errorString();
		return;
	}
	
	if (m_settingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(jsonDocument.object(), *m_settingsCompPtr)){
		QMetaObject::invokeMethod(quickItem, "settingsSaved");
	}
	else{
		QMetaObject::invokeMethod(quickItem, "settingsSaveFailed");
	}
}


} // namespace imtqml


