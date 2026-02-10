// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imthype/IJobStatus.h>

// ACF includes
#include <iser/ISerializable.h>


namespace imthype
{


/**
	Concrete implementation of IJobStatus interface.
*/
class CJobStatus: virtual public IJobStatus, virtual public iser::ISerializable
{
public:
	CJobStatus();
	explicit CJobStatus(const QByteArray& jobId);

	// reimplemented (imthype::IJobStatus)
	virtual IJobQueueManager::ProcessingStatus GetProcessingStatus() const override;
	virtual void SetProcessingStatus(IJobQueueManager::ProcessingStatus status) override;
	virtual double GetProgress() const override;
	virtual void SetProgress(double progress) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode) const override;
	virtual bool ResetData(CompatibilityMode mode) override;

private:
	QByteArray m_jobId;
	IJobQueueManager::ProcessingStatus m_processingStatus;
	double m_progress;
};


typedef istd::TSharedInterfacePtr<CJobStatus> CJobStatusSharedPtr;


} // namespace imthype


