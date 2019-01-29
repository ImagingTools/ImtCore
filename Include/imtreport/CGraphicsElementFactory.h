#pragma once


// ACF includes
#include <iser/IObject.h>
#include <i2d/IObject2d.h>


namespace imtreport
{


/**
	 factory for 2D-objects.
*/
class CGraphicsElementFactory: virtual public istd::TIFactory<i2d::IObject2d>
{
public:
	// reimplemented (istd::TIFactory)
   i2d::IObject2d* CreateInstance(const QByteArray& keyId = "") const override;

	// reimplemented (istd::IFactoryInfo)
	KeyList GetFactoryKeys() const override;
};


} // namespace imtreport


