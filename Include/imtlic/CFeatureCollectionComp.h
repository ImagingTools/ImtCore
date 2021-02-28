#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtbase/CCollectionInfo.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CFeatureCollection.h>

namespace imtlic
{


class CFeatureCollectionComp: public icomp::TMakeComponentWrap<CFeatureCollection>
{
public:
	typedef icomp::TMakeComponentWrap<CFeatureCollection> BaseClass;

	I_BEGIN_COMPONENT(CFeatureCollectionComp);
		I_REGISTER_INTERFACE(IFeatureDependenciesProvider);
		I_REGISTER_INTERFACE(IFeatureInfoProvider);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_ownerCollectionCompPtr, "OwnerCollection", "Collection which contains this object", true, "OwnerCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtbase::IObjectCollection, m_ownerCollectionCompPtr);
};


} // namespace imtlic


