// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CLegacyObjectCollectionControllerCompBase.h>


namespace imtgeo
{


class CAddressCollectionControllerComp: public imtservergql::CLegacyObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CLegacyObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAddressCollectionControllerComp);
		I_ASSIGN(m_addressTypeCollectionPtr, "AddressTypeComponent", "The string address type collection", false, "AddressTypeCollection");
	I_END_COMPONENT;

protected:
	I_REF(imtbase::IObjectCollection, m_addressTypeCollectionPtr);

	// reimplemented (imtservergql::CLegacyObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtgeo



