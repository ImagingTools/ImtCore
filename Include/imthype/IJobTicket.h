// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <functional>

// ImtCore includes
#include <imtbase/IReferenceCollection.h>

// Acula includes
#include <imthype/IJobQueueManager.h>


namespace imthype
{


class IJobTicket: virtual public iser::ISerializable
{
public:
	enum MetaInfoTypes
	{
		MIT_TYPE_ID,
		MIT_UUID,
		MIT_JOB_NAME,
		MIT_CONTEXT_ID,
		MIT_PROGRESS,
		MIT_PROCESSING_STATUS
	};

	typedef std::function<iprm::IParamsSetSharedPtr(const QByteArray& contextId, const QByteArray& typeId)> ParamsFactoryFunction;

	virtual QByteArray GetTypeId() const = 0;
	virtual void SetTypeId(const QByteArray& typeId) = 0;

	virtual QByteArray GetUuid() const = 0;
	virtual void SetUuid(const QByteArray& uuid) = 0;

	virtual QString GetJobName() const = 0;
	virtual void SetJobName(const QString& name) = 0;

	virtual QByteArray GetContextId() const = 0;
	virtual void SetContextId(const QByteArray& contextId) = 0;

	virtual iprm::IParamsSetSharedPtr GetParams() const = 0;
	virtual iprm::IParamsSetSharedPtr CreateParams() const = 0;
	virtual void SetParams(const iprm::IParamsSetSharedPtr& paramsPtr) = 0;

	virtual IJobQueueManager::ProcessingStatus GetProcessingStatus() const = 0;
	virtual void SetProcessingStatus(IJobQueueManager::ProcessingStatus status) = 0;

	virtual double GetProgress() const = 0;
	virtual void SetProgress(double progress) = 0;

	virtual const IJobOutput* GetResults() const = 0;
	virtual void SetResults(const IJobOutput& results) = 0;

	virtual const imtbase::IReferenceCollection* GetInput() const = 0;
	virtual void SetInput(const imtbase::IReferenceCollection& input) = 0;
	// JobStatus - Progress, ProcassingStatus, MessageContainer log

	virtual void SetParamsFactory(const ParamsFactoryFunction& factory) = 0;
};


typedef istd::TSharedInterfacePtr<IJobTicket> IJobTicketSharedPtr;


} // namespace imthype


