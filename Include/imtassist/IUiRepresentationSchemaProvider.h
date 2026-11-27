// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QList>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtassist/CParameterSchema.h>


namespace imtassist
{


/**
	Provider of application-specific UI representation types.
	The schema ID is the type name, the schema version the representation version.
	The application must register a QML component for every provided type.

	\ingroup imtassist
*/
class IUiRepresentationSchemaProvider: virtual public istd::IPolymorphic
{
public:
	typedef QList<CParameterSchema> Schemas;

	virtual Schemas GetRepresentationSchemas() const = 0;
};


} // namespace imtassist


