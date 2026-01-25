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
		MIT_CONTEXT_ID
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
	virtual void SetParams(const iprm::IParamsSetSharedPtr& paramsPtr) = 0;

	virtual const IJobOutput* GetResults() const = 0;
	virtual void SetResults(const IJobOutput& results) = 0;

	virtual const imtbase::IReferenceCollection* GetInput() const = 0;
	virtual void SetInput(const imtbase::IReferenceCollection& input) = 0;

	virtual void SetParamsFactory(const ParamsFactoryFunction& factory) = 0;
};


typedef istd::TSharedInterfacePtr<IJobTicket> IJobTicketSharedPtr;


} // namespace imthype


