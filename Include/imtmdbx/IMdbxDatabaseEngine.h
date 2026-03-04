// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// libmdbx includes
#include <mdbx.h++>


namespace imtmdbx
{


class IMdbxDatabaseEngine
{

public:
	virtual mdbx::env_managed& GetEnv() = 0;
};


}//namespace imtmdbx
