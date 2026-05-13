// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CTextParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ITextParam.h>


namespace imtserverapp
{


// protected methods


// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::Settings::CTextParam::V1_0>)

QByteArray CTextParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::TextParam.toUtf8();
}


bool CTextParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);

	return textParamPtr != nullptr;
}


bool CTextParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CTextParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);
	Q_ASSERT(textParamPtr != nullptr);
	if (textParamPtr == nullptr){
		return false;
	}

	QString text = textParamPtr->GetText();
	sdlRepresentation.text = text;

	return true;
}


bool CTextParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CTextParam::V1_0& sdlRepresentation) const
{
	iprm::ITextParam* textParamPtr = dynamic_cast<iprm::ITextParam*>(&dataModel);
	Q_ASSERT(textParamPtr != nullptr);
	if (textParamPtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.text){
		return false;
	}

	QString text = *sdlRepresentation.text;
	textParamPtr->SetText(text);

	return true;
}


} // namespace imtserverapp


