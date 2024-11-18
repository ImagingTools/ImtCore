#pragma once


// std includes
#include <memory>

// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


class CSdlType: virtual public iser::ISerializable
{

public:
	CSdlType();

	[[nodiscard]] QString GetName() const;
	void SetName(const QString& name);

	[[nodiscard]] SdlFieldList GetFields() const;
	void SetFields(const SdlFieldList& fields);

	/// \todo remove it \deprecated. Build namespace from \c m_schemaParamsPtr instead \sa GetSchemaParams
	[[nodiscard]] QString GetNamespace() const;
	void SetNamespace(const QString& aNamespace);

	[[nodiscard]] QString GetTargetHeaderFile() const;
	void SetTargetHeaderFile(const QString& headerFile);

	[[nodiscard]] QString GetSchemaFile() const;
	void SetSchemaFile(const QString& schemaFile);

	[[nodiscard]] bool IsExternal() const;
	void SetExternal(bool isExternal);

	[[nodiscard]] QString GetQmlImportDeclaration() const;
	void SetQmlImportDeclaration(const QString& qmlImportDeclaration);

	[[nodiscard]] const iprm::IParamsSet& GetSchemaParams() const;
	void SetSchemaParamsPtr(const std::shared_ptr<iprm::IParamsSet>& schemaParamsPtr);

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// operators
	[[nodiscard]] bool operator==(const CSdlType& other) const;
	[[nodiscard]] bool operator!=(const CSdlType& other) const { return !(operator==(other)); }

private:
	QString m_name;
	SdlFieldList m_fields;
	QString m_namespace;
	QString m_targetHeaderFile;
	QString m_schemaFile;
	bool m_isExternal;
	QString m_qmlImportDeclaration;
	std::shared_ptr<iprm::IParamsSet> m_schemaParamsPtr;
};


}
