// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/CCumulatedProgressManagerBase.h>

// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>
#include <imtbase/IProgressSessionsManager.h>


namespace imtservergql
{


class CProgressManagerPublisherComp:
			public CGqlPublisherCompBase,
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
	virtual bool CancelProgressSession(
				const QByteArray& sessionId,
				const QString& description = QString(),
				bool isFailed = false) override;

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
		QString m_description;
		bool m_isFailed;
		bool m_isCompleted;
	};

	typedef std::shared_ptr<ProgressManagerSession> ProgressManagerSessionPtr;

private:
	QRecursiveMutex m_mutex;

	QMap<QByteArray, ProgressManagerSessionPtr> m_progressSessions;
};


} // namespace imtservergql


