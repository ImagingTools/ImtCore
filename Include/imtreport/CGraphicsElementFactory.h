// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <i2d/IObject2d.h>


namespace imtreport
{


/**
	Factory for drawable 2D-objects.
*/
class CGraphicsElementFactory: virtual public istd::TIFactory<i2d::IObject2d>
{
public:
	// reimplemented (istd::TIFactory)
	virtual istd::TUniqueInterfacePtr<i2d::IObject2d> CreateInstance(const QByteArray& keyId = "") const override;

	// reimplemented (istd::IFactoryInfo)
	virtual KeyList GetFactoryKeys() const override;
};


} // namespace imtreport


