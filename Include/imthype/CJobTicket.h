// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Acula includes
#include <imthype/IJobTicket.h>
#include <imthype/CStandardJobOutput.h>

// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>


namespace imthype
{


class CJobTicket: virtual public IJobTicket
{
public:
	CJobTicket();

	// reimplemented (imthype::IJobTicket)
	virtual QByteArray GetTypeId() const override;
	virtual void SetTypeId(const QByteArray& typeId) override;
	virtual QByteArray GetUuid() const override;
	virtual void SetUuid(const QByteArray& uuid) override;
	virtual QString GetJobName() const override;
	virtual void SetJobName(const QString& name) override;
	virtual QByteArray GetContextId() const override;
	virtual void SetContextId(const QByteArray& contextId) override;
	virtual iprm::IParamsSetSharedPtr GetParams() const override;
	virtual iprm::IParamsSetSharedPtr CreateParams() const override;
	virtual void SetParams(const iprm::IParamsSetSharedPtr& paramsPtr) override;
	virtual imthype::IJobQueueManager::ProcessingStatus GetProcessingStatus() const override;
	virtual void SetProcessingStatus(imthype::IJobQueueManager::ProcessingStatus status) override;
	virtual double GetProgress() const override;
	virtual void SetProgress(double progress) override;
	virtual const imthype::IJobOutput* GetResults() const override;
	virtual void SetResults(const imthype::IJobOutput& results) override;
	virtual const imtbase::IReferenceCollection* GetInput() const override;
	virtual void SetInput(const imtbase::IReferenceCollection& input) override;

	virtual void SetParamsFactory(const ParamsFactoryFunction& factory) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_typeId;
	QByteArray m_uuid;
	QString m_name;
	QByteArray m_contextId;
	double m_progress;
	imthype::IJobQueueManager::ProcessingStatus m_processingStatus;
	CStandardJobOutput m_results;
	imtbase::CSimpleReferenceCollection m_input;
	iprm::IParamsSetSharedPtr m_paramsPtr;
	ParamsFactoryFunction m_paramsFactory;
};

typedef imtbase::TIdentifiableWrap<CJobTicket> CIdentifiableJobInfo;


} // namespace imthype


