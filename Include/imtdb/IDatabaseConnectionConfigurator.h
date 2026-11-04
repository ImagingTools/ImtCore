// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QSqlDatabase>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


/**
	Sets up a connection to a database that the engine has just opened (see CDatabaseEngineComp, the reference \c ConnectionConfigurator),
	for example the settings of a concrete database. The engine itself knows nothing about the databases that it works with.
*/
class IDatabaseConnectionConfigurator: virtual public istd::IPolymorphic
{
public:
	/**
		Set up the connection. The engine has a connection for every thread, and it calls this function every time a connection is
		opened, from the thread of the connection and at the same time from several threads, so the function must not change the state of
		the configurator.
		\param	databaseConnection	The connection that is just opened.
		\param	errorMessage		The reason if the connection is not set up (empty if it is). The engine writes it to its log.
		\return	\c true only if the connection is set up as the configurator needs it. Otherwise the engine does not use the connection:
				a connection that is thought to be set up but is not (for example, it has no WAL mode) makes the database slow or locked
				without any sign of the reason.
	*/
	virtual bool ConfigureConnection(QSqlDatabase& databaseConnection, QString& errorMessage) const = 0;
};


} // namespace imtdb


