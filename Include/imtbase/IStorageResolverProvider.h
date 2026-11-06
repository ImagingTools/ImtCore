// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/IStorageResolver.h>


namespace imtbase
{


/**
	Interface for getting of the storage resolvers by the logical storage ID.
	It provides the late binding of the storage links: the link keeps only the logical storage ID,
	the concrete resolver is chosen by the application configuration.
	\ingroup Collection
*/
class IStorageResolverProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get resolver for the given logical storage ID.
		An empty storage ID addresses the default resolver.
		\return Pointer to the resolver or \c nullptr, if no resolver is registered for the ID.
	*/
	virtual IStorageResolver* GetStorageResolver(const QByteArray& storageId = QByteArray()) const = 0;
};


} // namespace imtbase


