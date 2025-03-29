#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QString>

// ACF includes
#include <iser/ISerializable.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlEntryBase.h>


namespace imtsdl
{


class CSdlUnion: virtual public CSdlEntryBase
{

public:
	typedef CSdlEntryBase BaseClass;

	CSdlUnion();

	[[nodiscard]] QString GetName() const;
	void SetName(const QString& name);

	[[nodiscard]] QList<QPair<QString, bool>> GetTypes() const;
	void SetTypes(const QList<QPair<QString, bool>>& types);
	void AddType(const QPair<QString, bool>& value);

	bool HasContainsCustom() const;

	/// \todo remove it \deprecated. Build namespace from \c m_schemaParamsPtr instead. \sa GetSchemaParams
	[[nodiscard]] QString GetNamespace() const;
	void SetNamespace(const QString& aNamespace);

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// operators
	[[nodiscard]] bool operator==(const CSdlUnion& other) const;
	[[nodiscard]] bool operator!=(const CSdlUnion& other) const { return !(operator==(other)); }

private:
	QString m_name;
	QList<QPair<QString /*name*/, bool /*isCustom*/>> m_types;
	QString m_namespace;
};


}
