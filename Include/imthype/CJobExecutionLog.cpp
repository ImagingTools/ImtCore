// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CJobExecutionLog.h>


// imthype includes
#include <imthype/CJobExecutionMessage.h>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imthype
{


CJobExecutionLog::CJobExecutionLog()
{
}


void CJobExecutionLog::LogExecutionEvent(
			ExecutionEventType eventType,
			const QString& message,
			const QDateTime& timestamp)
{
	istd::CChangeNotifier changeNotifier(this);
	
	// Create a log message with appropriate category based on event type
	istd::IInformationProvider::InformationCategory category = istd::IInformationProvider::IC_INFO;
	
	switch (eventType) {
		case EET_JOB_FAILED:
			category = istd::IInformationProvider::IC_ERROR;
			break;
		case EET_JOB_CANCELLED:
			category = istd::IInformationProvider::IC_WARNING;
			break;
		case EET_JOB_COMPLETED:
			category = istd::IInformationProvider::IC_INFO;
			break;
		default:
			category = istd::IInformationProvider::IC_INFO;
			break;
	}
	
	// Create and add the execution message with event type
	ilog::IMessageConsumer::MessagePtr messagePtr(new CJobExecutionMessage(eventType, category, message, "JobExecutionLog", 0, &timestamp));
	BaseClass::AddMessage(messagePtr);
}


ilog::IMessageContainer::Messages CJobExecutionLog::GetExecutionEvents(ExecutionEventType eventType) const
{
	ilog::IMessageContainer::Messages events;
	
	// Iterate through all messages and filter by event type
	const ilog::IMessageContainer::Messages messages = BaseClass::GetMessages();
	for (const ilog::IMessageConsumer::MessagePtr& message : messages) {
		// Check if this is a CJobExecutionMessage and if it matches the requested event type
		const CJobExecutionMessage* executionMsg = dynamic_cast<const CJobExecutionMessage*>(message.GetPtr());
		if (executionMsg && executionMsg->GetEventType() == eventType) {
			events.push_back(ilog::IMessageConsumer::MessagePtr(new CJobExecutionMessage(*executionMsg)));
		}
	}
	
	return events;
}


bool CJobExecutionLog::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	
	// Serialize the message container (parent class)
	// The event types are stored within each CJobExecutionMessage, so no separate map needed
	return ilog::CMessageContainer::Serialize(archive);
}


bool CJobExecutionLog::CopyFrom(const istd::IChangeable& object, CompatibilityMode mode)
{
	const CJobExecutionLog* sourcePtr = dynamic_cast<const CJobExecutionLog*>(&object);
	if (sourcePtr) {
		istd::CChangeNotifier changeNotifier(this);
		
		// Copy the message container
		return ilog::CMessageContainer::CopyFrom(*sourcePtr, mode);
	}
	
	return false;
}


bool CJobExecutionLog::IsEqual(const istd::IChangeable& object) const
{
	const CJobExecutionLog* sourcePtr = dynamic_cast<const CJobExecutionLog*>(&object);
	if (sourcePtr) {
		// Compare the message containers
		return ilog::CMessageContainer::IsEqual(*sourcePtr);
	}
	
	return false;
}


istd::IChangeableUniquePtr CJobExecutionLog::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CJobExecutionLog);
	if (clonePtr->CopyFrom(*this, mode)) {
		return clonePtr;
	}
	
	return nullptr;
}


bool CJobExecutionLog::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);
	
	// Reset the message container
	return ilog::CMessageContainer::ResetData(mode);
}


} // namespace imthype


