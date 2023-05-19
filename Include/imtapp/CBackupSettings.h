#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtapp/IBackupSettings.h>


namespace imtapp
{


class CBackupSettings: virtual public IBackupSettings
{
public:
	// reimplemented (ISchedulerParams)
	virtual const QDateTime& GetStartTime() const override;
	virtual void SetStartTime(const QDateTime& startTime) override;
	virtual int GetInterval() const override;
	virtual void SetInterval(int interval) override;

	// reimplemented (ifile::IFileNameParam)
	virtual int GetPathType() const override;
	virtual const QString& GetPath() const override;
	virtual void SetPath(const QString& path) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QString m_backupFolderPath;
	QDateTime m_startTime;
	int m_interval;
};


} // namespace imtapp


