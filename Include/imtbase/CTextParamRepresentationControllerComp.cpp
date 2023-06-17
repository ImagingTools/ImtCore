#include <imtbase/CTextParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ITextParam.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CTextParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);
	if (textParamPtr != nullptr) {
		return true;
	}

	return false;
}


bool CTextParamRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);
	if (textParamPtr == nullptr) {
		return false;
	}

	representation.SetData("Id", *m_paramIdAttrPtr);
	representation.SetData("Name", *m_paramNameAttrPtr);

	if (m_qmlPathAttrPtr.IsValid()){
		representation.SetData("Source", *m_qmlPathAttrPtr);
	}

	QString textParam = textParamPtr->GetText();

	representation.SetData("Value", textParam);

	return true;
}


bool CTextParamRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	iprm::ITextParam* textParamPtr = dynamic_cast<iprm::ITextParam*>(&dataModel);
	if (textParamPtr == nullptr) {
		return false;
	}

	if (representation.ContainsKey("Value")){
		QString text = representation.GetData("Value").toString();

		textParamPtr->SetText(text);

		return true;
	}

	return false;
}


} // namespace imtbase


