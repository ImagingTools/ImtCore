// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtassist
{


/**
	Registry of the UI representation types known to the application.

	A representation is a type name, a version and data. The UI renders it with a
	registered component; the data must match the registered schema. No QML,
	JavaScript, HTML or other executable content is ever accepted.

	\ingroup imtassist
*/
class IUiRepresentationRegistry: virtual public istd::IPolymorphic
{
public:
	/**
		Validate a representation.
		\param errorPtr Optional technical description of the violation.
		\return False if the type and version are not registered or the data does not match.
	*/
	virtual bool ValidateRepresentation(
				const QByteArray& type,
				int version,
				const QJsonObject& data,
				QString* errorPtr = nullptr) const = 0;
};


} // namespace imtassist


