#include <imtbase/CObjectRepresentationControllerCompBase.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

QByteArray CObjectRepresentationControllerCompBase::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


bool CObjectRepresentationControllerCompBase::IsModelSupported(const istd::IChangeable& dataModel) const
{
	return true;
}


bool CObjectRepresentationControllerCompBase::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation) const
{
	return true;
}


bool CObjectRepresentationControllerCompBase::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	return true;
}


} // namespace imtbase


