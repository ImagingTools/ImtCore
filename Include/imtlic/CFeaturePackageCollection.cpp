#include <imtlic/CFeaturePackageCollection.h>


// ImtCore includes
#include <imtlic/CFeaturePackage.h>


namespace imtlic
{


// public methods

CFeaturePackageCollection::CFeaturePackageCollection()
{
	typedef istd::TSingleFactory<istd::IChangeable, imtlic::CFeaturePackage> FactoryImpl;

	RegisterFactory<FactoryImpl>("FeaturePackage");
}


} // namespace imtlic


