// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtgeo/IAddressElementInfo.h>

#undef GetObject

namespace imtgeo
{



class CAddressControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAddressControllerComp);
        I_ASSIGN(m_addressInfoFactCompPtr, "AddressFactory", "Factory used for creation of the new address element", true, "AddressFactory");
		I_ASSIGN(m_addressTypeCollectionPtr, "AddressTypeComponent", "The string address type collection", false, "AddressTypeCollection");
	I_END_COMPONENT;

protected:
	virtual QJsonObject GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual QJsonObject InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		QByteArray& objectId,
		QString& errorMessage) const override;
	virtual QJsonObject UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_FACT(imtgeo::IAddressElementInfo, m_addressInfoFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_addressTypeCollectionPtr);
};


} // namespace npgql


