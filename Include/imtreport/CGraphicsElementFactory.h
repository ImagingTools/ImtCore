#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtreport/IGraphicsElement.h>

namespace imtreport
{


/**
	 factory for 2D-objects.
*/
class CGraphicsElementFactory: virtual public istd::TIFactory<IGraphicsElement>
{
public:
	// reimplemented (istd::TIFactory)
	IGraphicsElement* CreateInstance(const QByteArray& keyId = "") const override;

	// reimplemented (istd::IFactoryInfo)
	KeyList GetFactoryKeys() const override;
};


} // namespace imtreport


