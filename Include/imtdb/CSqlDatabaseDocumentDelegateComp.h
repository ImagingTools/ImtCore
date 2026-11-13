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
		I_ASSIGN(m_serializeHeaderAttrPtr, "SerializeHeader", "If true, each document holds the ACF header with the version infos; switch off for documents that do not depend on versions, e.g. log messages", true, true);
	I_END_COMPONENT

protected:
	virtual bool WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const override;
	virtual bool ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const override;

private:
	bool IsHeaderSerialized() const;

private:
	I_ATTR(bool, m_serializeHeaderAttrPtr);
};


} // namespace imtdb
