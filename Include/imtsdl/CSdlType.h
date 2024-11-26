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
#include <imtsdl/CSdlEntryBase.h>


namespace imtsdl
{


class CSdlType: virtual public CSdlEntryBase
{

public:
	typedef CSdlEntryBase BaseClass;

	CSdlType();

	[[nodiscard]] QString GetName() const;
	void SetName(const QString& name);

	[[nodiscard]] SdlFieldList GetFields() const;
	void SetFields(const SdlFieldList& fields);

	/// \todo remove it \deprecated. Build namespace from \c m_schemaParamsPtr instead. \sa GetSchemaParams
	[[nodiscard]] QString GetNamespace() const;
	void SetNamespace(const QString& aNamespace);

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// operators
	[[nodiscard]] bool operator==(const CSdlType& other) const;
	[[nodiscard]] bool operator!=(const CSdlType& other) const { return !(operator==(other)); }

private:
	QString m_name;
	SdlFieldList m_fields;
	QString m_namespace;
};


}
