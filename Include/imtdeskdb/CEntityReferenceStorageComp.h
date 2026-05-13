// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdesk/IEntityReferenceStorage.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtdeskdb
{


class CEntityReferenceStorageComp:
		public icomp::CComponentBase,
		virtual public imtdesk::IEntityReferenceStorage
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CEntityReferenceStorageComp)
		I_REGISTER_INTERFACE(imtdesk::IEntityReferenceStorage);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for entity reference storage", false, "DatabaseEngine");
	I_END_COMPONENT

	// reimplemented (imtdesk::IEntityReferenceStorage)
	virtual QByteArray CreateEntityReference(
				const QString& entityType,
				const QByteArray& entityId,
				const QString& displayName,
				const QString& entityUrl) override;
	virtual bool GetEntityReference(
				const QByteArray& refId,
				QString& entityType,
				QByteArray& entityId,
				QString& displayName,
				QString& entityUrl) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


} // namespace imtdeskdb
