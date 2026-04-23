// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtgql/CGqlRequest.h>


namespace imtqml
{


/**
	\brief icomp/QObject hybrid that exposes the imtclientgql GraphQL
	pipeline to the imtqml controllers.

	\details
	The bridge is created by icomp (so that the \c IGqlClient
	reference can be injected via the standard \c I_REF mechanism)
	and resolved internally through \c Instance(). The
	bridge is intentionally NOT exposed to QML — controllers find it
	on their own, so there is no \c apiClient property to wire from
	QML.

	\note Because \c TClientRequestManagerCompWrap::SendModelRequest is a
	blocking call, callers MUST invoke \c SendSdlRequest() from a worker
	thread (typically through \c QtConcurrent::run). The result is
	returned in-place; callers are responsible for marshalling Qt
	signals back to the GUI thread (e.g. via \c
	QMetaObject::invokeMethod with \c Qt::QueuedConnection).
*/
class CGqlClientBridge:
			public QObject,
			public imtclientgql::TClientRequestManagerCompWrap<icomp::CComponentBase>
{
	Q_OBJECT

public:
	typedef imtclientgql::TClientRequestManagerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CGqlClientBridge)
	I_END_COMPONENT;

	explicit CGqlClientBridge(QObject* parent = nullptr);
	~CGqlClientBridge() override;

	/**
		\brief The most recently created bridge instance, or \c nullptr
		if none has been created yet (or it has been destroyed).
		\details Used by \c CStaticQmlTypeRegistratorComp to expose the
		bridge as a QML singleton.
	*/
	static CGqlClientBridge* Instance();

	/**
		\brief Build a GQL request from \c arguments using the SDL
		generated request class \c SdlRequest, send it through
		\c SendModelRequest and return the parsed payload. Blocking.
		\param[in]  arguments     Arguments structure produced by the SDL
		                          generator (e.g. \c CGetObjectDataInput
		                          wrapper).
		\param[out] payload       Parsed payload (only valid when the
		                          method returns \c true).
		\param[out] errorMessage  Empty on success, otherwise carries the
		                          textual diagnostic.
		\returns \c true if the request completed successfully and
		         \c payload is filled in.
	*/
	template<class Arguments, class Payload, class SdlRequest>
	bool SendSdlRequest(const Arguments& arguments, Payload& payload, QString& errorMessage) const
	{
		errorMessage.clear();

		imtgql::CGqlRequest gqlRequest;
		if (!SdlRequest::SetupGqlRequest(gqlRequest, arguments)){
			errorMessage = QStringLiteral("Failed to set up GQL request");
			return false;
		}

		payload = SendModelRequest<Payload>(gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			return false;
		}

		if (!payload.Version_1_0.HasValue()){
			errorMessage = QStringLiteral("Payload is empty");
			return false;
		}

		return true;
	}

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	static CGqlClientBridge* s_instancePtr;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlClientBridge*)
