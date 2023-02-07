#include <imtbase/CObjectRepresentationJoinerComp.h>


// ACF includes
#include <iprm/ITextParam.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CObjectRepresentationJoinerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	return true;
}


bool CObjectRepresentationJoinerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{

	return true;
}


bool CObjectRepresentationJoinerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{

	return false;
}


} // namespace imtbase


