// ImtCore includes
#include <imtbase/Init.h>
#include <imtlic/IProductInfo.h>


template <class ApplicationComposite, class AppInitializer, void (*ProductFiller)(imtlic::IProductInfo&) = nullptr, bool autoInit = false>
int ProductFeatureRun(int argc, char* argv[])
{
	ApplicationComposite instance;

	imtlic::IProductInfo* productInfoPtr = instance.template GetInterface<imtlic::IProductInfo>();
	if (productInfoPtr != nullptr && ProductFiller != nullptr){
		ProductFiller(*productInfoPtr);
	}

	return Run<ApplicationComposite, AppInitializer, autoInit>(argc, argv);
}


