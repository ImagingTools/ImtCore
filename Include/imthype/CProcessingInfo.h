// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/CMessageContainer.h>

// Acula includes
#include <imthype/IProcessingInfo.h>


namespace imthype
{


class CProcessingInfo: virtual public IProcessingInfo
{
public:
	typedef ilog::CMessageContainer BaseClass;

	CProcessingInfo();

	/**
		Set job's start time.
	*/
	void SetStartTime(const QDateTime& startTime);

	/**
		Set job's finish time.
	*/
	void SetFinishedTime(const QDateTime& finishedTime);

	/**
		Set result category of the job processing.
	*/
	void SetResultCategory(istd::IInformationProvider::InformationCategory resultCategory);

	/**
		Sets the information description.
	*/
	void SetInformationDescription(const QString& informationDescription);

	/**
		Sets the job processing log.
	*/
	void SetProcessingLog(const ilog::IMessageContainer& processingLog);

	// reimplemented (IJobOutput)
	virtual ProcessingInfo GetProcessingInfo() const override;
	virtual const ilog::IMessageContainer& GetProcessingLog() const override;

	// reimplemented (istd::IInformationProvider)
	virtual QDateTime GetInformationTimeStamp() const override;
	virtual InformationCategory GetInformationCategory() const override;
	virtual int GetInformationId() const override;
	virtual QString GetInformationDescription() const override;
	virtual QString GetInformationSource() const override;
	virtual int GetInformationFlags() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	istd::IInformationProvider::InformationCategory m_resultCategory;
	QDateTime m_startedAt;
	QDateTime m_resultTimeStamp;
	QString m_informationDescription;

	ilog::CMessageContainer m_processingLog;
};


} // namespace imthype


