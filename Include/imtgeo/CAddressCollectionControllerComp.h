// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>


namespace imtgeo
{


class CAddressCollectionControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	using BaseClass = imtservergql::CObjectCollectionControllerCompBase;

	I_BEGIN_COMPONENT(CAddressCollectionControllerComp);
		I_ASSIGN(m_addressTypeCollectionPtr, "AddressTypeComponent", "Collection containing address type declarations", false, "AddressTypeCollection");
	I_END_COMPONENT;

protected:
	I_REF(imtbase::IObjectCollection, m_addressTypeCollectionPtr);

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual bool SetupGqlItem(
				const imtgql::CGqlRequest& gqlRequest,
				QJsonObject& itemObj,
				const imtbase::IObjectCollectionIterator* objectCollectionIterator,
				QString& errorMessage) const override;
};


} // namespace imtgeo



