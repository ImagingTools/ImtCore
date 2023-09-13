#include <imtbase/CTextParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::CObjectRepresentationControllerCompBase)

bool CTextParamRepresentationControllerComp::GetRepresentationFromValue(
			const istd::IChangeable& dataModel,
			CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);
	Q_ASSERT(textParamPtr != nullptr);

	QString textParam = textParamPtr->GetText();

	representation.SetData("Value", textParam);

	return true;
}


// reimplemented (IRepresentationController)

bool CTextParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);
	if (textParamPtr != nullptr) {
		return true;
	}

	return false;
}


bool CTextParamRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	iprm::ITextParam* textParamPtr = dynamic_cast<iprm::ITextParam*>(&dataModel);
	Q_ASSERT(textParamPtr != nullptr);

	if (representation.ContainsKey("Value")){
		QString text = representation.GetData("Value").toString();

		textParamPtr->SetText(text);

		return true;
	}

	return false;
}


} // namespace imtbase


