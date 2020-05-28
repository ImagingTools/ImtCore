#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectExtractor.h>
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


class CCollectionObjectExtractorComp: public icomp::CComponentBase, virtual public IObjectExtractor
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionObjectExtractorComp);
		I_REGISTER_INTERFACE(IObjectExtractor);
	I_END_COMPONENT;

	// reimplemented (IObjectExtractor)
	virtual QByteArrayList GetSupportedObjectIds(const istd::IChangeable* compositePtr) const override;
	virtual const istd::IChangeable* ExtractObject(const istd::IChangeable* compositePtr, const QByteArray& objectId) const override;
};


} // namespace imtbase


