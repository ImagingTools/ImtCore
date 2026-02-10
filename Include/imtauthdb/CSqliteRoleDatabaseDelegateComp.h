// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqliteDatabaseDocumentDelegateComp.h>


namespace imtauthdb
{


class CSqliteRoleDatabaseDelegateComp: public imtdb::CSqliteDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqliteDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CSqliteRoleDatabaseDelegateComp)
	I_END_COMPONENT
};


} // namespace imtauthdb


