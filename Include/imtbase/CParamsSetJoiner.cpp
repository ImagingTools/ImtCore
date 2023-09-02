#include <imtbase/CParamsSetJoiner.h>


namespace imtbase
{



// public methods

CParamsSetJoiner::CParamsSetJoiner()
	:m_paramsSet1(nullptr),
	m_paramsSet2(nullptr)
{
}


CParamsSetJoiner::CParamsSetJoiner(iprm::IParamsSet* paramsSet1, iprm::IParamsSet* paramsSet2)
	:m_paramsSet1(paramsSet1),
	m_paramsSet2(paramsSet2)
{
}


CParamsSetJoiner::CParamsSetJoiner(const iprm::IParamsSet* paramsSet1, const iprm::IParamsSet* paramsSet2)
	:m_paramsSet1(const_cast<iprm::IParamsSet*>(paramsSet1)),
	m_paramsSet2(const_cast<iprm::IParamsSet*>(paramsSet2))
{
}


void CParamsSetJoiner::SetParams(iprm::IParamsSet* paramsSet1, iprm::IParamsSet* paramsSet2)
{
	Q_ASSERT(paramsSet1 != paramsSet2);

	m_paramsSet1 = paramsSet1;
	m_paramsSet2 = paramsSet2;
}


void CParamsSetJoiner::SetParams(const iprm::IParamsSet* paramsSet1, const iprm::IParamsSet* paramsSet2)
{
	SetParams(const_cast<iprm::IParamsSet*>(paramsSet1), const_cast<iprm::IParamsSet*>(paramsSet2));
}


// reimplemented (iprm::IParamsSet)

iprm::IParamsSet::Ids CParamsSetJoiner::GetParamIds(bool editableOnly) const
{
	iprm::IParamsSet::Ids retVal;
	
	if (m_paramsSet1 != nullptr){
		retVal = m_paramsSet1->GetParamIds(editableOnly);
	}

	if (m_paramsSet2 != nullptr){
		retVal += m_paramsSet2->GetParamIds(editableOnly);
	}

	return retVal;
}


const iser::ISerializable* CParamsSetJoiner::GetParameter(const QByteArray& id) const
{
	if (m_paramsSet1 != nullptr){
		const iser::ISerializable* parameterPtr = m_paramsSet1->GetParameter(id);
		if (parameterPtr != nullptr){
			return parameterPtr;
		}
	}

	if (m_paramsSet2 != nullptr){
		const iser::ISerializable* parameterPtr = m_paramsSet2->GetParameter(id);
		if (parameterPtr != nullptr){
			return parameterPtr;
		}
	}

	return nullptr;
}


iser::ISerializable* CParamsSetJoiner::GetEditableParameter(const QByteArray& id)
{
	if (m_paramsSet1 != nullptr){
		iser::ISerializable* parameterPtr = m_paramsSet1->GetEditableParameter(id);
		if (parameterPtr != nullptr){
			return parameterPtr;
		}
	}

	if (m_paramsSet2 != nullptr){
		iser::ISerializable* parameterPtr = m_paramsSet2->GetEditableParameter(id);
		if (parameterPtr != nullptr){
			return parameterPtr;
		}
	}

	return nullptr;
}


// reimplement (iser::ISerializable)

bool CParamsSetJoiner::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


} // namespace imtbase


