// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlic
{


/**
	Component for providing of features.
*/
class CFeatureInfoProviderComp:
			public icomp::CComponentBase,
			virtual public IFeatureInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureInfoProviderComp)
		I_REGISTER_INTERFACE(IFeatureInfoProvider);
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
	I_END_COMPONENT

	// reimplemented (IFeatureInfoProvider)
	virtual imtlic::IFeatureInfoSharedPtr GetFeatureInfo(const QByteArray& featureId) const override;
	virtual const imtbase::ICollectionInfo& GetFeatureList() const override;

protected:
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
};


} // namespace imtlic


