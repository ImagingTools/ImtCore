// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CFilterableSelectControllerComp.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>


namespace imtservergql
{


bool CFilterableSelectControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtbase::FilterableSelect::CFilterableSelectListGqlRequest& /*listRequest*/,
			sdl::imtbase::FilterableSelect::CSelectableItemData::V1_0& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Object collection is not set");
		SendErrorMessage(0, errorMessage, "CFilterableSelectControllerComp");
		return false;
	}

	const QByteArray objectId = objectCollectionIterator.GetObjectId();

	representationObject.id = objectId;
	representationObject.name = objectCollectionIterator.GetObjectName();

	return true;
}


} // namespace imtservergql
