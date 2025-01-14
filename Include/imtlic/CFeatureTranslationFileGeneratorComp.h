#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtlic/IProductInfo.h>


namespace imtlic
{



class CFeatureTranslationFileGeneratorComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureTranslationFileGeneratorComp)
		I_ASSIGN(m_featureContainerCompPtr, "FeatureContainer", "Feature container", true, "FeatureContainer");
	I_END_COMPONENT

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(imtlic::IProductInfo, m_featureContainerCompPtr);
};


} // namespace imtlic


