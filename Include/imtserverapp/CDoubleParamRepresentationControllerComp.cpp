#include <imtserverapp/CDoubleParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ITextParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CDoubleParam::V1_0>)

QByteArray CDoubleParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::DoubleParam.toUtf8();
}


bool CDoubleParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);
	if (textParamPtr != nullptr){
		return true;
	}
	
	return false;
}


bool CDoubleParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CDoubleParam::V1_0& sdlRepresentation,
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

	sdlRepresentation.value = text.toDouble(&ok);
	if (!ok){
		return false;
	}
	
	return true;
}


bool CDoubleParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CDoubleParam::V1_0& sdlRepresentation) const
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


} // namespace imtserverapp


