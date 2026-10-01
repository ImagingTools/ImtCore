// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCollectionInfoAdapterComp.h>


namespace imtbase
{


// public methods

// reimplemented (imtbase::ICollectionInfo)

int CCollectionInfoAdapterComp::GetElementsCount(
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	if (!m_optionsListCompPtr.IsValid()){
		return 0;
	}

	return m_optionsListCompPtr->GetOptionsCount();
}


ICollectionInfo::Ids CCollectionInfoAdapterComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	Ids retVal;

	if (!m_optionsListCompPtr.IsValid()){
		return retVal;
	}

	int optionsCount = m_optionsListCompPtr->GetOptionsCount();
	if (offset < 0 || offset >= optionsCount){
		return retVal;
	}

	int lastIndex = (count >= 0) ? qMin(offset + count, optionsCount) : optionsCount;
	for (int i = offset; i < lastIndex; i++){
		retVal += m_optionsListCompPtr->GetOptionId(i);
	}

	return retVal;
}


bool CCollectionInfoAdapterComp::GetSubsetInfo(
			ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CCollectionInfoAdapterComp::GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	int index = GetOptionIndex(elementId);
	if (index < 0){
		return QVariant();
	}

	switch (infoType){
	case EIT_NAME:
		return QVariant(m_optionsListCompPtr->GetOptionName(index));

	case EIT_DESCRIPTION:
		return QVariant(m_optionsListCompPtr->GetOptionDescription(index));

	case EIT_ENABLED:
		return QVariant(m_optionsListCompPtr->IsOptionEnabled(index));

	default:
		break;
	}

	return QVariant();
}


idoc::MetaInfoPtr CCollectionInfoAdapterComp::GetElementMetaInfo(const Id& /*elementId*/, ilog::IMessageConsumer* /*logPtr*/) const
{
	return idoc::MetaInfoPtr();
}


bool CCollectionInfoAdapterComp::SetElementName(const Id& /*elementId*/, const QString& /*name*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CCollectionInfoAdapterComp::SetElementDescription(const Id& /*elementId*/, const QString& /*description*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CCollectionInfoAdapterComp::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

void CCollectionInfoAdapterComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	istd::CChangeNotifier notifier(this);
}


// reimplemented (icomp::CComponentBase)

void CCollectionInfoAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_optionsListModelCompPtr.IsValid()){
		RegisterModel(m_optionsListModelCompPtr.GetPtr());
	}
}


void CCollectionInfoAdapterComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private methods

int CCollectionInfoAdapterComp::GetOptionIndex(const Id& elementId) const
{
	if (!m_optionsListCompPtr.IsValid()){
		return -1;
	}

	int optionsCount = m_optionsListCompPtr->GetOptionsCount();
	for (int i = 0; i < optionsCount; i++){
		if (m_optionsListCompPtr->GetOptionId(i) == elementId){
			return i;
		}
	}

	return -1;
}


} // namespace imtbase

