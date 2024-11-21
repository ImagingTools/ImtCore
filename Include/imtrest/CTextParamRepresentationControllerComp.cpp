#include <imtrest/CTextParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ITextParam.h>

// ImtCore includes
#include <iqt/iqt.h>


namespace imtrest
{


// protected methods

// reimplemented (imtrest::CObjectRepresentationControllerCompBase)

bool CTextParamRepresentationControllerComp::GetRepresentationFromValue(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
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
	if (textParamPtr != nullptr){
		return true;
	}

	return false;
}


bool CTextParamRepresentationControllerComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
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


} // namespace imtrest


