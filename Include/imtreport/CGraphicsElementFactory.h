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
	virtual i2d::IObject2d* CreateInstance(const QByteArray& keyId = "") const override;

	// reimplemented (istd::IFactoryInfo)
	virtual KeyList GetFactoryKeys() const override;
};


} // namespace imtreport


