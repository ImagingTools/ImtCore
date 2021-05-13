#include <imtbase/CTextParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ITextParam.h>


namespace imtbase
{


// public methods

// reimplemented (IParamRepresentationController)

bool CTextParamRepresentationControllerComp::CreateTextRepresentation(const iser::ISerializable& param, QString& textRepresentation) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&param);
	if (textParamPtr != nullptr){
		textRepresentation = textParamPtr->GetText();

		return true;
	}

	return false;
}


} // namespace imtbase


