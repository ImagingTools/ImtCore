// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtservergql
{


/**
	What the servers of products may send to this server: the permission of a server token each request needs.
	A request not declared here is closed to them.
*/
class IProductServerRequestPolicy: virtual public istd::IPolymorphic
{
public:
	/**
		Find the permission a server token needs for the request.
		\param permission	Empty when any token of a connected server may send the request.
		\return				False when the request is closed to the servers of products.
	*/
	virtual bool FindRequiredPermission(const imtgql::CGqlRequest& gqlRequest, QByteArray& permission) const = 0;
};


} // namespace imtservergql
