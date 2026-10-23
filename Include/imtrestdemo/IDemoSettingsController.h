// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrestdemo
{


/**
	Access of the servlets of the demo to the settings of the server that works: the servlet \c /admin/settings reads and
	changes the settings through this interface, it does not know the server.
	The methods are called from the threads of the handlers.
*/
class IDemoSettingsController: virtual public istd::IPolymorphic
{
public:
	/**
		Get the settings of the server that works as JSON (see CDemoServer::GetSettingsJson()).
	*/
	virtual QJsonObject GetSettings() const = 0;

	/**
		Check the changes and, if they are valid and something is changed, schedule the restart of the server with the new
		settings. The restart is made later, so the response of the request that has asked for it is delivered.
		\param changes	Names and new values of the settings, only the changed ones must be set.
		\param errorText	The reason of the refusal.
		\param result	Description of what will be done: \c restartScheduled, \c restartDelayMs, \c port and \c token that will be used.
		\return \c false if the changes are not valid, nothing is changed in this case.
	*/
	virtual bool ChangeSettings(const QJsonObject& changes, QString& errorText, QJsonObject& result) = 0;
};


/**
	Setup of the controller of the settings: the owner of the server (CDemoServer) gives the functions that are called by the servlet.
	The server is assembled by the composition (ImtRestDemoServer.acc), the owner gets this interface from the component and sets the functions.
*/
class IDemoSettingsControllerBinding: virtual public istd::IPolymorphic
{
public:
	typedef std::function<QJsonObject()> GetFunction;
	typedef std::function<bool(const QJsonObject&, QString&, QJsonObject&)> ChangeFunction;

	virtual void SetFunctions(const GetFunction& getFunction, const ChangeFunction& changeFunction) = 0;
};


} // namespace imtrestdemo


