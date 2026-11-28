// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CUiRepresentationRegistryComp.h>


// ImtCore includes
#include <imtassist/CStandardRepresentations.h>


namespace imtassist
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CUiRepresentationRegistryComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_registerStandardAttrPtr.IsValid() || *m_registerStandardAttrPtr){
		RegisterRepresentation(CStandardRepresentations::CreateSelectableListSchema());
	}

	int count = m_schemaProvidersCompPtr.GetCount();
	for (int i = 0; i < count; ++i){
		const IUiRepresentationSchemaProvider* providerPtr = m_schemaProvidersCompPtr[i];
		if (providerPtr == nullptr){
			continue;
		}

		for (const CParameterSchema& schema: providerPtr->GetRepresentationSchemas()){
			if (!RegisterRepresentation(schema)){
				SendErrorMessage(
							0,
							QStringLiteral("UI representation '%1' version %2 was rejected: missing type or version, or already registered")
										.arg(QString::fromUtf8(schema.GetSchemaId()))
										.arg(schema.GetVersion()),
							"UiRepresentationRegistry");
			}
		}
	}
}


} // namespace imtassist


