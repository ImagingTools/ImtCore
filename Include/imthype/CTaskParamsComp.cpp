// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CTaskParamsComp.h>


namespace imthype
{


// reimplemented (iprm::IParamsSet)

iprm::IParamsSet::Ids CTaskParamsComp::GetParamIds(bool editableOnly) const
{
	Ids retVal = BaseClass::GetParamIds(editableOnly);

	if (!editableOnly){
		retVal += "TaskId";
		retVal += "TaskInputId";
	}

	return retVal;
}


const iser::ISerializable* CTaskParamsComp::GetParameter(const QByteArray& id) const
{
	const iser::ISerializable* paramPtr = BaseClass::GetParameter(id);
	if (paramPtr != nullptr){
		return paramPtr;
	}

	if (id == "TaskId"){
		return &m_taskIdParam;
	}

	if (id == "TaskInputId"){
		return &m_taskInputIdParam;
	}

	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CTaskParamsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_taskIdParam.SetCollectionContext(m_collectionContextCompPtr.GetPtr());
	m_taskInputIdParam.SetCollectionContext(m_collectionContextCompPtr.GetPtr());
}


// public methods of the embedded class TaskIdParam

CTaskParamsComp::TaskIdParam::TaskIdParam()
	:m_contextPtr(nullptr)
{
}


void CTaskParamsComp::TaskIdParam::SetCollectionContext(const ITaskCollectionContext* contextPtr)
{
	m_contextPtr = contextPtr;
}


// reimplemented (iprm::IIdParam)

QByteArray CTaskParamsComp::TaskIdParam::GetId() const
{
	if (m_contextPtr != nullptr){
		return m_contextPtr->GetTaskId();
	}

	return QByteArray();
}


void CTaskParamsComp::TaskIdParam::SetId(const QByteArray& /*id*/)
{
	Q_ASSERT(false);
}


// reimplemented (iser::ISerializable)

bool CTaskParamsComp::TaskIdParam::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// public methods of the embedded class TaskInputIdParam

// reimplemented (iprm::IIdParam)

QByteArray CTaskParamsComp::TaskInputIdParam::GetId() const
{
	if (m_contextPtr != nullptr){
		return m_contextPtr->GetTaskInputId();
	}

	return QByteArray();
}


} // namespace imthype


