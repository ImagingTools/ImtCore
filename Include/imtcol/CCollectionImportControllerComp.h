#pragma once


// ACF includes
#include <ifile/ITempFileManager.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IProgressSessionsManager.h>
#include <imtbase/IProgressLoggerProvider.h>
#include <imtcol/ICollectionImportController.h>
#include <imthype/IJobQueueManager.h>
#include <imtbase/TModelUpdateBinder.h>


namespace imtcol
{


class CCollectionImportControllerComp:
	public icomp::CComponentBase,
	virtual public ICollectionImportController
{
	Q_DECLARE_TR_FUNCTIONS(CCollectionImportControllerComp)
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionImportControllerComp);
		I_REGISTER_SUBELEMENT(UploadProgressLoggerProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(UploadProgressLoggerProvider, imtbase::IProgressLoggerProvider, ExtractProgressLoggerProvider);
		I_ASSIGN(m_tempFileManagerCompPtr, "TempFileManager", "Temporary file manager for uploaded files", true, "TempFileManager");
		I_ASSIGN(m_progressSessionManagerCompPtr, "ProgressSessionManager", "ProgressSessionManager", true, "ProgressSessionManager");
		I_ASSIGN(m_jobQueueManagerCompPtr, "JobQueueManager", "Job queue manager", true, "JobQueueManager");
	I_END_COMPONENT;

	// reimplemented (imtcol::ICollectionImportController)
	virtual bool BeginCollectionImportTransaction(const ICollectionImportController::TransactionInfo& transactionInfo, QString& errorMessage) override;
	virtual bool CancelCollectionImportTransaction(const QByteArray& transactionId, QString& errorMessage) override;

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;

private:
	class ProgressLogger: ibase::IProgressLogger
	{
	public:
		void SetParent(CCollectionImportControllerComp& parent);

		// reimplamanted (ibase::IProgressLogger)
		virtual void OnProgress(double currentProgress) override;
		virtual bool IsCanceled() const override;

	public:
		QByteArray transactionId;
		QByteArray fileId;
		std::unique_ptr<ibase::IProgressManager> progressManagerPtr;
		std::unique_ptr<ibase::IProgressLogger> progressLoggerPtr;
		double m_progress = 0;

		CCollectionImportControllerComp* parentPtr = nullptr;
	};
	typedef std::shared_ptr<ProgressLogger> ProgressLoggerPtr;

	struct FileInfo: ICollectionImportController::FileInfo
	{
		QByteArray tempFileId;
		ProgressLoggerPtr uploadProgressLoggerPtr;
	};

	struct TransactionInfo
	{
		QByteArray transactionId;
		QByteArray collectionId;
		QMap<QByteArray, FileInfo> files;

		QByteArray tempFileSessionId;
		ibase::IProgressManager* mainProgressManagerPtr = nullptr;
		std::unique_ptr<ibase::IProgressManager> uploadProgressManagerPtr;
		std::unique_ptr<ibase::IProgressManager> fileProcessingProgressManagerPtr;
		std::unique_ptr<ibase::IProgressManager> insertionProgressManagerPtr;
	};

	class UploadProgressLoggerProvider : virtual public imtbase::IProgressLoggerProvider
	{
	public:
		void SetParent(CCollectionImportControllerComp& parent);

		// reimplamanted (imtbase::IProgressLoggerProvider)
		virtual QByteArrayList GetProgressLoggerIds() const override;
		virtual ibase::IProgressLogger* GetProgressLogger(const QByteArray& progressLoggerId) const override;

	private:
		CCollectionImportControllerComp* m_parentPtr = nullptr;
	};

private:
	template <typename Interface>
	static Interface* ExtractProgressLoggerProvider(CCollectionImportControllerComp& component)
	{
		return &component.m_progressLoggerProvider;
	}

	bool PrepareProgressManager(TransactionInfo& transaction);
	void UploadProgressChanged(QByteArray transactionId, QByteArray fileId);
	bool StartImportJob(TransactionInfo& transaction);

private:
	I_REF(ifile::ITempFileManager, m_tempFileManagerCompPtr);
	I_REF(imtbase::IProgressSessionsManager, m_progressSessionManagerCompPtr);
	I_REF(imthype::IJobQueueManager, m_jobQueueManagerCompPtr);

	QMap<QByteArray, std::shared_ptr<TransactionInfo>> m_transactions;

	UploadProgressLoggerProvider m_progressLoggerProvider;

	QMutex m_mutex;
};


} // namespace imtcol


