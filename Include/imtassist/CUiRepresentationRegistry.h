// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QPair>

// ImtCore includes
#include <imtassist/IUiRepresentationRegistry.h>
#include <imtassist/CParameterSchema.h>


namespace imtassist
{


/**
	Registry of UI representation types with their data schemas.

	\ingroup imtassist
*/
class CUiRepresentationRegistry: virtual public IUiRepresentationRegistry
{
public:
	/**
		Register a representation type. The type and version are taken from the schema.
		\return False if the schema has no ID or version, or the pair is already registered.
	*/
	bool RegisterRepresentation(const CParameterSchema& dataSchema);

	// reimplemented (imtassist::IUiRepresentationRegistry)
	virtual bool ValidateRepresentation(
				const QByteArray& type,
				int version,
				const QJsonObject& data,
				QString* errorPtr = nullptr) const override;

private:
	typedef QPair<QByteArray, int> TypeKey;

	QMap<TypeKey, CParameterSchema> m_schemas;
};


} // namespace imtassist


