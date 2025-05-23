/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


#include <aculatask/CTaskParamsComp.h>


namespace aculatask
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
	if (paramPtr != NULL){
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


} // namespace aculatask


