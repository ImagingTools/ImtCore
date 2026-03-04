// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CParamsSetJoiner.h>


namespace imtbase
{



// public methods

CParamsSetJoiner::CParamsSetJoiner()
	:m_paramsSet1(nullptr),
	m_paramsSet2(nullptr),
	m_infoProvider(this)
{
}


CParamsSetJoiner::CParamsSetJoiner(iprm::IParamsSet* paramsSet1, iprm::IParamsSet* paramsSet2)
	:m_paramsSet1(paramsSet1),
	m_paramsSet2(paramsSet2),
	m_infoProvider(this)
{
}


CParamsSetJoiner::CParamsSetJoiner(const iprm::IParamsSet* paramsSet1, const iprm::IParamsSet* paramsSet2)
	:m_paramsSet1(const_cast<iprm::IParamsSet*>(paramsSet1)),
	m_paramsSet2(const_cast<iprm::IParamsSet*>(paramsSet2)),
	m_infoProvider(this)
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


const iprm::IParamsInfoProvider* CParamsSetJoiner::GetParamsInfoProvider() const
{
	return &m_infoProvider;
}


// reimplement (iser::ISerializable)

bool CParamsSetJoiner::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// ParamsInfoProviderJoiner implementation

CParamsSetJoiner::ParamsInfoProviderJoiner::ParamsInfoProviderJoiner(const CParamsSetJoiner* joiner)
	:m_joiner(joiner)
{
}


std::unique_ptr<iprm::IParamsInfoProvider::ParamInfo> CParamsSetJoiner::ParamsInfoProviderJoiner::GetParamInfo(const QByteArray& paramId) const
{
	// Check if param belongs to m_paramsSet1
	if (m_joiner->m_paramsSet1 != nullptr){
		const iser::ISerializable* paramPtr = m_joiner->m_paramsSet1->GetParameter(paramId);
		if (paramPtr != nullptr){
			const iprm::IParamsInfoProvider* infoProviderPtr = m_joiner->m_paramsSet1->GetParamsInfoProvider();
			if (infoProviderPtr != nullptr){
				return infoProviderPtr->GetParamInfo(paramId);
			}
			return nullptr;
		}
	}

	// Check if param belongs to m_paramsSet2
	if (m_joiner->m_paramsSet2 != nullptr){
		const iser::ISerializable* paramPtr = m_joiner->m_paramsSet2->GetParameter(paramId);
		if (paramPtr != nullptr){
			const iprm::IParamsInfoProvider* infoProviderPtr = m_joiner->m_paramsSet2->GetParamsInfoProvider();
			if (infoProviderPtr != nullptr){
				return infoProviderPtr->GetParamInfo(paramId);
			}
			return nullptr;
		}
	}

	return nullptr;
}


} // namespace imtbase


