// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/CMessage.h>

// imthype includes
#include <imthype/IJobExecutionLog.h>


namespace imthype
{


/**
	Extended message class for job execution events.
	
	This message type extends ilog::CMessage to include the ExecutionEventType,
	ensuring the message and its type information are always synchronized.
*/
class CJobExecutionMessage: virtual public ilog::CMessage
{
public:
	typedef ilog::CMessage BaseClass;

	enum MessageId
	{
		MI_JOB_EXECUTION_EVENT = 200000000
	};

	CJobExecutionMessage();
	CJobExecutionMessage(
				IJobExecutionLog::ExecutionEventType eventType,
				istd::IInformationProvider::InformationCategory category,
				const QString& text,
				const QString& source = QString(),
				int flags = 0,
				const QDateTime* timeStampPtr = nullptr);

	IJobExecutionLog::ExecutionEventType GetEventType() const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

public:
	static QByteArray GetTypeId();

private:
	IJobExecutionLog::ExecutionEventType m_eventType;
};


} // namespace imthype


