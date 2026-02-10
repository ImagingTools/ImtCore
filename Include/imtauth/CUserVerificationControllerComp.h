// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserVerificationController.h>


namespace imtauth
{


class CUserVerificationControllerComp: public icomp::CComponentBase, virtual public IUserVerificationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserVerificationControllerComp);
		I_REGISTER_INTERFACE(IUserVerificationController);
		I_ASSIGN(m_userVerificationCollectionCompPtr, "UserVerificationCollection", "User verification collection", false, "UserVerificationCollection");
		I_ASSIGN(m_codeValidityPeriodAttrPtr, "CodeValidityPeriod", "Code validity period (in secs)", false, 180);
	I_END_COMPONENT;

	CUserVerificationControllerComp();

	// reimplemented (imtauth::IUserVerificationController)
	virtual const imtauth::IUserVerification* GetVerificationData(const QByteArray& verificationId) const override;
	virtual bool GenerateVerificationData(const QByteArray& userId, imtauth::IUserVerification* userVerificationPtr = nullptr) override;
	virtual bool VerifyUser(const QByteArray& userId, const QByteArray& verificationCode) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	int GenerateCode(int min, int max) const;

	class CheckExpiredVerificationDataThread: public QThread
	{
	public:
		CheckExpiredVerificationDataThread(CUserVerificationControllerComp& parent);
		void Start();
	protected:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CUserVerificationControllerComp& m_parent;
	};

private:
	I_REF(imtbase::IObjectCollection, m_userVerificationCollectionCompPtr);
	I_ATTR(int, m_codeValidityPeriodAttrPtr);

private:
	CheckExpiredVerificationDataThread m_checkExpiredVerificationDataThread;
};



} // namespace imtauth


