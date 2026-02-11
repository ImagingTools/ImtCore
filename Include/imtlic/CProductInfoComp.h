// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CProductInfo.h>


namespace imtlic
{


class CProductInfoComp: public icomp::CComponentBase, virtual public CIdentifiableProductInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductInfoComp);
		I_REGISTER_INTERFACE(imtlic::IProductInfo);
		I_REGISTER_INTERFACE(iprm::INameParam);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_featureInfoProviderCompPtr, "FeatureInfoProvider", "Feature info provider", false, "FeatureInfoProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(imtlic::IFeatureInfoProvider, m_featureInfoProviderCompPtr);
};


} // namespace imtlic


