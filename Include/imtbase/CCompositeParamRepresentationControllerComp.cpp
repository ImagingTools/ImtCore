#include <imtbase/CCompositeParamRepresentationControllerComp.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CCompositeParamRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!m_paramRepresentationControllersCompPtr.IsValid()){
		return false;
	}

	bool retVal = true;
	for (int i = 0; i < m_paramRepresentationControllersCompPtr.GetCount(); ++i){
		IRepresentationController* representationPtr = m_paramRepresentationControllersCompPtr[i];
		if (representationPtr != nullptr){
			CTreeItemModel subRepresentation;
			representationPtr->GetRepresentationFromDataModel(dataModel, subRepresentation);
		}
		else{
			return false;
		}
	}

	return retVal;
}


bool CCompositeParamRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	return false;
}


} // namespace imtbase


