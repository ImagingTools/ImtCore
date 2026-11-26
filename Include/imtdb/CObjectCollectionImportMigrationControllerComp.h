// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/CMigrationControllerCompBase.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtdb/IObjectImportConverter.h>


namespace imtdb
{


/**
	Migration step that imports the active objects of another collection - for example a collection of another application's
	database - into a table of this database. The objects keep their IDs, so the references to them stay valid.

	The objects are written through the delegate of the target table, as a new object would be: the meta info is created and
	no organization owns them. An object already in the target table is left as it is, so a repeated step changes nothing.
	Without a converter every object is imported unchanged.

	The step fails if the source cannot be read or an object cannot be written. With the optional SourceDatabase, the database the
	source collection lives in, a missing source database means there is nothing to import: the step passes, as on an installation
	that never had the source. Any other failure fails the step, so the revision is not recorded and the import is repeated.
*/
class CObjectCollectionImportMigrationControllerComp: public imtdb::CMigrationControllerCompBase
{
public:
	typedef imtdb::CMigrationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionImportMigrationControllerComp);
		I_ASSIGN(m_sourceCollectionCompPtr, "SourceCollection", "Collection the objects are imported from", true, "SourceCollection");
		I_ASSIGN(m_targetDelegateCompPtr, "TargetDelegate", "Delegate of the table the objects are imported into", true, "TargetDelegate");
		I_ASSIGN(m_converterCompPtr, "ObjectConverter", "Selects and converts the imported objects; without it every object is imported unchanged", false, "ObjectConverter");
		I_ASSIGN(m_sourceDatabaseCompPtr, "SourceDatabase", "Database of the source collection: if it does not exist, there is nothing to import", false, "SourceDatabase");
	I_END_COMPONENT;

protected:
	// reimplemented (imtdb::IMigrationController)
	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange = istd::CIntRange()) const override;

private:
	bool IsInTarget(const QByteArray& objectId, bool& isInTarget) const;
	bool ImportObject(const QByteArray& objectId, bool& isImported) const;

private:
	I_REF(imtbase::IObjectCollection, m_sourceCollectionCompPtr);
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_targetDelegateCompPtr);
	I_REF(imtdb::IObjectImportConverter, m_converterCompPtr);
	I_REF(imtdb::IDatabaseServerConnectionChecker, m_sourceDatabaseCompPtr);
};


} // namespace imtdb


