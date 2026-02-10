// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imthype/IJobExecutionLog.h>

// ACF includes
#include <ilog/CMessageContainer.h>
#include <iser/ISerializable.h>


namespace imthype
{


/**
	Concrete implementation of IJobExecutionLog.
	
	Stores system-level job controller events and lifecycle tracking.
*/
class CJobExecutionLog:
	public ilog::CMessageContainer,
	virtual public IJobExecutionLog,
	virtual public iser::ISerializable
{
public:
	typedef ilog::CMessageContainer BaseClass;

	CJobExecutionLog();
	
	// reimplemented (IJobExecutionLog)
	virtual void LogExecutionEvent(
		ExecutionEventType eventType,
		const QString& message,
		const QDateTime& timestamp = QDateTime::currentDateTime()) override;
	virtual ilog::IMessageContainer::Messages GetExecutionEvents(ExecutionEventType eventType) const override;
	
	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;
	
	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
};


typedef istd::TSharedInterfacePtr<CJobExecutionLog> CJobExecutionLogSharedPtr;


} // namespace imthype


