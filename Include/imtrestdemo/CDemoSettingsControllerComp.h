// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtrestdemo/IDemoSettingsController.h>


namespace imtrestdemo
{


/**
	Component that gives the settings of the server to the servlets: it only calls the functions that the owner of the server
	has set (CDemoServer), so the servlets do not depend on the server.
*/
class CDemoSettingsControllerComp:
			public icomp::CComponentBase,
			virtual public IDemoSettingsController,
			virtual public IDemoSettingsControllerBinding
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDemoSettingsControllerComp);
		I_REGISTER_INTERFACE(IDemoSettingsController);
		I_REGISTER_INTERFACE(IDemoSettingsControllerBinding);
	I_END_COMPONENT;

	// reimplemented (IDemoSettingsControllerBinding)
	virtual void SetFunctions(const GetFunction& getFunction, const ChangeFunction& changeFunction) override;

	// reimplemented (IDemoSettingsController)
	virtual QJsonObject GetSettings() const override;
	virtual bool ChangeSettings(const QJsonObject& changes, QString& errorText, QJsonObject& result) override;

private:
	IDemoSettingsControllerBinding::GetFunction m_getFunction;
	IDemoSettingsControllerBinding::ChangeFunction m_changeFunction;
};


} // namespace imtrestdemo


