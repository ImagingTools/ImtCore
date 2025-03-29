#include <imtrest/CIntegerParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ITextParam.h>


namespace imtrest
{


// protected methods

// reimplemented (imtrest::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0>)

QByteArray CIntegerParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::IntegerParam.toUtf8();
}


bool CIntegerParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);
	if (textParamPtr != nullptr){
		return true;
	}
	
	return false;
}


bool CIntegerParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);
	Q_ASSERT(textParamPtr != nullptr);
	if (textParamPtr == nullptr){
		return false;
	}
	
	bool ok = false;
	QString text = textParamPtr->GetText();
	sdlRepresentation.value = text.toInt(&ok, 10);
	if (!ok){
		return false;
	}
	
	return true;
}


bool CIntegerParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0& sdlRepresentation) const
{
	iprm::ITextParam* textParamPtr = dynamic_cast<iprm::ITextParam*>(&dataModel);
	Q_ASSERT(textParamPtr != nullptr);
	if (textParamPtr == nullptr){
		return false;
	}
	
	if (!sdlRepresentation.value){
		return false;
	}
	
	QString text = QString::number(*sdlRepresentation.value);
	textParamPtr->SetText(text);
	
	return true;
}


} // namespace imtrest


