// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


class IDatabaseServerConnectionChecker: virtual public istd::IPolymorphic
{
public:
	enum DatabaseState
	{
		/**
			The server is connected and the database exists.
		*/
		DS_EXISTS,
		/**
			The server is connected, the database does not exist.
		*/
		DS_MISSING,
		/**
			The server cannot be connected or asked about the database.
		*/
		DS_NOT_CONNECTED
	};

	/**
		Check the connection to the database server and whether the database exists there.
		\param errorMessage	The reason of \c DS_NOT_CONNECTED.
	*/
	virtual DatabaseState CheckDatabaseConnection(QString& errorMessage) const = 0;
};


} // namespace imtdb


