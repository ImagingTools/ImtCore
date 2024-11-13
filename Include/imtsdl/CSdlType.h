#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


class CSdlType: public iser::ISerializable
{

public:
	CSdlType();

	[[nodiscard]] QString GetName() const;
	void SetName(const QString& name);

	[[nodiscard]] SdlFieldList GetFields() const;
	void SetFields(const SdlFieldList& fields);

	[[nodiscard]] QString GetNamespace() const;
	void SetNamespace(const QString& aNamespace);

	[[nodiscard]] QString GetTargetHeaderFile() const;
	void SetTargetHeaderFile(const QString& headerFile);

	[[nodiscard]] QString GetSchemaFile() const;
	void SetSchemaFile(const QString& schemaFile);

	[[nodiscard]] bool IsExternal() const;
	void SetExternal(bool isExternal);

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
};


}
