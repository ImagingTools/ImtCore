// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


class IDatabaseServerConnectionChecker: virtual public istd::IPolymorphic
{
public:
	/**
		Check if the database server is connected.
	*/
	virtual bool CheckDatabaseConnection(QString& errorMessage) const = 0;
};


} // namespace imtdb


