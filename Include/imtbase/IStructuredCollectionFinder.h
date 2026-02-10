// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


class IStructuredCollectionFinder: virtual public istd::IPolymorphic
{
public:
	virtual QByteArrayList FindObjectParentNodes(const QByteArray& objectId) const = 0;
};


} // namespace imtbase


