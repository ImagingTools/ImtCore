// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoSettingsControllerComp.h>


namespace imtrestdemo
{


// public methods

void CDemoSettingsControllerComp::SetFunctions(const GetFunction& getFunction, const ChangeFunction& changeFunction)
{
	m_getFunction = getFunction;
	m_changeFunction = changeFunction;
}


// reimplemented (IDemoSettingsController)

QJsonObject CDemoSettingsControllerComp::GetSettings() const
{
	return m_getFunction ? m_getFunction() : QJsonObject();
}


bool CDemoSettingsControllerComp::ChangeSettings(const QJsonObject& changes, QString& errorText, QJsonObject& result)
{
	if (!m_changeFunction){
		errorText = QStringLiteral("The settings cannot be changed");

		return false;
	}

	return m_changeFunction(changes, errorText, result);
}


} // namespace imtrestdemo


