// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CUiRepresentationRegistry.h>


namespace imtassist
{


bool CUiRepresentationRegistry::RegisterRepresentation(const CParameterSchema& dataSchema)
{
	TypeKey key(dataSchema.GetSchemaId(), dataSchema.GetVersion());
	if (key.first.isEmpty() || (key.second <= 0) || m_schemas.contains(key)){
		return false;
	}

	m_schemas.insert(key, dataSchema);

	return true;
}


// reimplemented (imtassist::IUiRepresentationRegistry)

bool CUiRepresentationRegistry::ValidateRepresentation(
			const QByteArray& type,
			int version,
			const QJsonObject& data,
			QString* errorPtr) const
{
	QMap<TypeKey, CParameterSchema>::const_iterator iter = m_schemas.constFind(TypeKey(type, version));
	if (iter == m_schemas.constEnd()){
		if (errorPtr != nullptr){
			*errorPtr = QString("Unknown representation type '%1' version %2").arg(QString::fromUtf8(type.left(64))).arg(version);
		}

		return false;
	}

	return iter.value().Validate(data, errorPtr);
}


} // namespace imtassist


