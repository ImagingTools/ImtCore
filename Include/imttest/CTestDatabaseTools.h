// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>


namespace imttest
{


/**
	Tools of the database tests, which recreate their PostgreSQL database on every run.
*/
class CTestDatabaseTools
{
public:
	/**
		\brief Drop the test database through the maintenance database of its server, closing the connections left to it.
				Only a database whose name ends with _test is dropped, so a test never drops a production database.
		\return true if the database was dropped or did not exist.
	*/
	static bool DropDatabase(const imtdb::IDatabaseLoginSettings& settings);
};


} // namespace imttest

