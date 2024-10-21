#pragma once


// ACF includes
#include <iproc/TSyncProcessorCompBase.h>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>


namespace imtgql
{


class CComplexCollectionFilterProcessor : public iproc::CSyncProcessorCompBase
{
public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CComplexCollectionFilterProcessor);
	I_END_COMPONENT;

	// reimplemented (iproc::IProcessor)
	virtual int DoProcessing(
		const iprm::IParamsSet* paramsPtr,
		const istd::IPolymorphic* inputPtr,
		istd::IChangeable* outputPtr,
		ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool ProcessGroupFilter(
		const sdl::imtbase::ComplexCollectionFilter::V1_0::CGroupFilter& source,
		imtbase::IComplexCollectionFilter::GroupFilter& target);

	bool ProcessFieldFilter(
		const sdl::imtbase::ComplexCollectionFilter::V1_0::CFieldFilter& source,
		imtbase::IComplexCollectionFilter::FieldFilter& target);
};


} // namespace imtgql


