#include <imtbase/CObjectRepresentationJoinerComp.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CObjectRepresentationJoinerComp::IsModelSupported(const istd::IChangeable& /*dataModel*/) const
{
	return false;
}


bool CObjectRepresentationJoinerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& /*dataModel*/,
			CTreeItemModel& /*representation*/,
			const iprm::IParamsSet* /*paramsPtr*/) const
{

	return false;
}


bool CObjectRepresentationJoinerComp::GetDataModelFromRepresentation(
			const CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{

	return false;
}


} // namespace imtbase


