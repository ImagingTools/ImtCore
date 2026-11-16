// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IClientProductCatalog.h>
#include <imtauth/IConnectedProduct.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Client products are the products connected to this server.
*/
class CConnectedClientProductCatalogComp:
			public icomp::CComponentBase,
			virtual public IClientProductCatalog
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CConnectedClientProductCatalogComp)
		I_REGISTER_INTERFACE(IClientProductCatalog);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Catalog of the connected products", true, "ProductCollection");
		I_ASSIGN(m_productFactCompPtr, "ProductFactory", "Factory of the catalog entries", true, "ProductFactory");
	I_END_COMPONENT

	// reimplemented (IClientProductCatalog)
	virtual bool IsClientProduct(const QByteArray& productId, bool* isReadPtr = nullptr) const override;
	virtual bool AddClientProduct(const QByteArray& productId, const QString& name, QString& errorMessage, bool* isAddedPtr = nullptr) override;

private:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_FACT(IConnectedProduct, m_productFactCompPtr);

	// The check and the insertion of a product are one step
	QMutex m_addMutex;
};


} // namespace imtauth


