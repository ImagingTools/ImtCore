#include <imtbase/CDelegatedParamsSetComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// public methods

// reimplemented (iprm::IParamsSet)

iprm::IParamsSet::Ids CDelegatedParamsSetComp::GetParamIds(bool editableOnly) const
{
	iprm::IParamsSet::Ids paramIds = BaseClass::GetParamIds(editableOnly);

	if (m_paramsSetCompPtr.IsValid()){
		paramIds += m_paramsSetCompPtr->GetParamIds(editableOnly);
	}

	return paramIds;
}


const iser::ISerializable* CDelegatedParamsSetComp::GetParameter(const QByteArray& id) const
{
	const iser::ISerializable* retValPtr = BaseClass::GetParameter(id);
	if (retValPtr == nullptr){
		if (m_paramsSetCompPtr.IsValid()){
			retValPtr = m_paramsSetCompPtr->GetParameter(id);
		}
	}

	return retValPtr;
}


iser::ISerializable* CDelegatedParamsSetComp::GetEditableParameter(const QByteArray& id)
{
	iser::ISerializable* retValPtr = BaseClass::GetEditableParameter(id);
	if (retValPtr == nullptr){
		if (m_paramsSetCompPtr.IsValid()){
			retValPtr = m_paramsSetCompPtr->GetEditableParameter(id);
		}
	}

	return retValPtr;
}


// reimplemented (iser::ISerializable)

bool CDelegatedParamsSetComp::Serialize(iser::IArchive& archive)
{
	return false;
}


} // namespace imtbase


