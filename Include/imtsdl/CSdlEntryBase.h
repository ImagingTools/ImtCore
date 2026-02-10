// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// std includes
#include <memory>

// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>


namespace iprm {

class IParamsSet;

} // namespace iprm



namespace imtsdl
{


class CSdlEntryBase: virtual public iser::ISerializable
{

public:
	CSdlEntryBase();

	[[nodiscard]] virtual QString GetName() const;
	virtual void SetName(const QString& name);

	[[nodiscard]] virtual QString GetTargetHeaderFilePath() const;
	virtual void SetTargetHeaderFilePath(const QString& headerFile);

	[[nodiscard]] virtual QString GetSchemaFilePath() const;
	virtual void SetSchemaFilePath(const QString& schemaFile);

	[[nodiscard]] virtual bool IsExternal() const;
	virtual void SetExternal(bool isExternal);

	[[nodiscard]] virtual QString GetQmlImportDeclaration() const;
	virtual void SetQmlImportDeclaration(const QString& qmlImportDeclaration);

	[[nodiscard]] virtual const iprm::IParamsSet& GetSchemaParams() const;
	virtual void SetSchemaParamsPtr(const std::shared_ptr<iprm::IParamsSet>& schemaParamsPtr);

protected:
	QString m_name;
	QString m_targetHeaderFilePath;
	QString m_schemaFilePath;
	bool m_isExternal;
	QString m_qmlImportDeclaration;
	std::shared_ptr<iprm::IParamsSet> m_schemaParamsPtr;
};


}
