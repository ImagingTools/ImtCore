#include <imtlic/CFeaturePackageCollection.h>


// ImtCore includes
#include <imtlic/CFeaturePackage.h>


namespace imtlic
{


// public methods

CFeaturePackageCollection::CFeaturePackageCollection()
{
	RegisterFactory(new istd::TSingleFactory<istd::IChangeable, imtlic::CFeaturePackage>("FeaturePackage"), true);
}


} // namespace imtlic


