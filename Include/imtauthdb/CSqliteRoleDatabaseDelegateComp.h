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


