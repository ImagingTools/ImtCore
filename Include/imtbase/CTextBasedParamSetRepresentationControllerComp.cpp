#include <imtbase/CTextBasedParamSetRepresentationControllerComp.h>


// ACF includes
#include <iprm/IParamsSet.h>


namespace imtbase
{


// public methods

// reimplemented (ITextBasedRepresentationController)

bool CTextBasedParamSetRepresentationControllerComp::CreateTextRepresentation(const istd::IChangeable& object, QString& textRepresentation) const
{
	const iprm::IParamsSet*  paramSetPtr = dynamic_cast<const iprm::IParamsSet*>(&object);
	if (paramSetPtr != nullptr){
		QString retVal;

		int idCount = qMin(m_paramIdAttrPtr.GetCount(), m_paramRepresentationControllerCompPtr.GetCount());
		idCount = qMin(idCount, m_paramNameAttrPtr.GetCount());

		iprm::IParamsSet::Ids paramSetIds = paramSetPtr->GetParamIds();
		for (int i = 0; i < idCount; i++){
			if (paramSetIds.contains(m_paramIdAttrPtr[i])){
				if (m_paramRepresentationControllerCompPtr[i] != nullptr){
					const iser::ISerializable* paramPtr = paramSetPtr->GetParameter(m_paramIdAttrPtr[i]);
					if (paramPtr == nullptr){
						return false;
					}

					QString paramTextRepresentation;
					if (!m_paramRepresentationControllerCompPtr[i]->CreateTextRepresentation(*paramPtr, paramTextRepresentation)){
						 return false;
					}

					if (m_paramNameAttrPtr[i] != nullptr){
						retVal += m_paramNameAttrPtr[i] + "=" + paramTextRepresentation + ";";
					}
					else{
						retVal += paramTextRepresentation + "; ";
					}
				}
				else{
					return false;
				}
			}
			else{
				return false;
			}
		}

		retVal.chop(2);
		textRepresentation = retVal;

		return true;
	}

	return false;
}


} // namespace imtbase


