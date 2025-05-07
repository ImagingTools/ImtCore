#pragma once


// ACF includes
#include <ibase/CCumulatedProgressManagerBase.h>

// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>
#include <imtbase/IProgressSessionsManager.h>


namespace imtservergql
{


class CProgressManagerPublisherComp : public CGqlPublisherCompBase,
									  virtual public imtbase::IProgressSessionsManager
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CProgressManagerPublisherComp)
		I_REGISTER_INTERFACE(imtbase::IProgressSessionsManager);
	I_END_COMPONENT;

	// reimplemented (imtbase::IProgressSessionsManager)
	virtual ibase::IProgressManager* BeginProgressSession(const QByteArray& sessionId, const QString& description) override;
	virtual bool EndProgressSession(const QByteArray& sessionId) override;
	virtual bool CancelProgressSession(const QByteArray& sessionId) override;

private:
	bool PublishSession(const QByteArray& sessionId);

private:
	class ProgressManagerSession : public ibase::CCumulatedProgressManagerBase
	{
		friend CProgressManagerPublisherComp;

	public:
		ProgressManagerSession(
			CProgressManagerPublisherComp& parent, const QByteArray& sessionId, const QString& description);

	protected:
		// reimplemented (CCumulatedProgressManagerBase)
		void OnProgressChanged(double cumulatedValue) override;

	protected:
		CProgressManagerPublisherComp& m_parent;
		const QByteArray m_sessionId;
		const QString m_description;
	};

	typedef std::shared_ptr<ProgressManagerSession> ProgressManagerSessionPtr;

private:
	QRecursiveMutex m_mutex;

	QMap<QByteArray, ProgressManagerSessionPtr> m_progressSessions;
};


} // namespace imtservergql


