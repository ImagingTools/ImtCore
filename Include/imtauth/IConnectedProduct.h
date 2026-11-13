// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>
#include <istd/TInterfacePtr.h>


namespace imtauth
{


/**
	A product connected to the authorization server: the catalog entry its server tokens refer to.
	The token stays the only identity of a server.
*/
class IConnectedProduct: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetProductId() const = 0;
	virtual void SetProductId(const QByteArray& productId) = 0;
	virtual QString GetProductName() const = 0;
	virtual void SetProductName(const QString& name) = 0;
	virtual QString GetProductDescription() const = 0;
	virtual void SetProductDescription(const QString& description) = 0;
};


typedef istd::TUniqueInterfacePtr<IConnectedProduct> IConnectedProductUniquePtr;


} // namespace imtauth
