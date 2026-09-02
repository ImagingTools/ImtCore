// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateCompBase.h>


namespace imtdb
{


class CSqlDatabaseDocumentDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CSqlDatabaseDocumentDelegateComp)
	I_END_COMPONENT

protected:
	virtual bool WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const override;
	virtual bool ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const override;
};


} // namespace imtdb
