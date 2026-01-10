#include <imtbase/CDelegatedParamsSetComp.h>


namespace imtbase
{


// public methods

// reimplemented (iprm::IParamsSet)

iprm::IParamsSet::Ids CDelegatedParamsSetComp::GetParamIds(bool editableOnly) const
{
	if (m_paramsSetCompPtr.IsValid()){
		return m_paramsSetCompPtr->GetParamIds(editableOnly);
	}

	return iprm::IParamsSet::Ids();
}


const iser::ISerializable* CDelegatedParamsSetComp::GetParameter(const QByteArray& id) const
{
	if (m_paramsSetCompPtr.IsValid()){
		return m_paramsSetCompPtr->GetParameter(id);
	}

	return nullptr;
}


iser::ISerializable* CDelegatedParamsSetComp::GetEditableParameter(const QByteArray& id)
{
	if (m_paramsSetCompPtr.IsValid()){
		return m_paramsSetCompPtr->GetEditableParameter(id);
	}

	return nullptr;
}


// reimplemented (iser::ISerializable)

bool CDelegatedParamsSetComp::Serialize(iser::IArchive& archive)
{
	if (m_paramsSetCompPtr.IsValid()){
		return false;
	}

	return m_paramsSetCompPtr->Serialize(archive);
}


} // namespace imtbase


