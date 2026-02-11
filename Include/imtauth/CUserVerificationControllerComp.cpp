// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserVerificationControllerComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDebug>
#include <QtCore/QRandomGenerator>

// ImtCore includes
#include <imtauth/CUserVerification.h>


namespace imtauth
{


// public methods

CUserVerificationControllerComp::CUserVerificationControllerComp()
	:m_checkExpiredVerificationDataThread(*this)
{
}


// reimplemented (imtauth::IUserVerificationController)

const imtauth::IUserVerification* CUserVerificationControllerComp::GetVerificationData(const QByteArray& verificationId) const
{
	if (!m_userVerificationCollectionCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userVerificationCollectionCompPtr->GetObjectData(verificationId, dataPtr)){
		if (dataPtr.IsValid()){
			istd::TUniqueInterfacePtr<const imtauth::IUserVerification> userVerificationInfoPtr;
			userVerificationInfoPtr.MoveCastedPtr(dataPtr->CloneMe());

			return userVerificationInfoPtr.PopInterfacePtr();
		}
	}

	return nullptr;
}


bool CUserVerificationControllerComp::GenerateVerificationData(const QByteArray& userId, imtauth::IUserVerification* outputUserVerificationPtr)
{
	if (m_userVerificationCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids ids = m_userVerificationCollectionCompPtr->GetElementIds();
		if (ids.contains(userId)){
			// Verification data already exists
			imtbase::ICollectionInfo::Ids elementIds;
			elementIds << userId;
			m_userVerificationCollectionCompPtr->RemoveElements(elementIds);
		}

		istd::TDelPtr<imtauth::CIdentifiableUserVerificationInfo> userVerificationInfoPtr;
		userVerificationInfoPtr.SetPtr(new imtauth::CIdentifiableUserVerificationInfo);

		userVerificationInfoPtr->SetObjectUuid(userId);
		userVerificationInfoPtr->SetUserId(userId);

		int code = GenerateCode(100000, 999999);
		userVerificationInfoPtr->SetVerificationCode(QString::number(code).toUtf8());

		QDateTime currentDate = QDateTime::currentDateTime();
		userVerificationInfoPtr->SetCreationDate(currentDate);

		QDateTime expirationDate = currentDate.addSecs(*m_codeValidityPeriodAttrPtr);
		qDebug() << "expirationDate" << expirationDate.toString();
		userVerificationInfoPtr->SetExpirationDate(expirationDate);

		m_userVerificationCollectionCompPtr->InsertNewObject("UserVerification", "", "", userVerificationInfoPtr.GetPtr(), userId);

		if (outputUserVerificationPtr != nullptr){
			outputUserVerificationPtr->CopyFrom(*userVerificationInfoPtr.GetPtr());
		}

		if (!m_checkExpiredVerificationDataThread.isRunning()){
			m_checkExpiredVerificationDataThread.Start();
		}

		return true;
	}

	return false;
}


bool CUserVerificationControllerComp::VerifyUser(const QByteArray& userId, const QByteArray& verificationCode) const
{
	istd::TDelPtr<const imtauth::IUserVerification> userVerificationInfoPtr = GetVerificationData(userId);
	if (userVerificationInfoPtr.IsValid()){
		QByteArray currentUserId = userVerificationInfoPtr->GetUserId();
		if (currentUserId == userId && !userVerificationInfoPtr->IsExpired()){
			QByteArray currentVerificationCode = userVerificationInfoPtr->GetVerificationCode();
			if (currentVerificationCode == verificationCode){
				return true;
			}
		}
	}

	return false;
}


// reimplemented (icomp::CComponentBase)

void CUserVerificationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CUserVerificationControllerComp::OnComponentDestroyed()
{
	m_checkExpiredVerificationDataThread.requestInterruption();
	m_checkExpiredVerificationDataThread.wait();

	BaseClass::OnComponentDestroyed();
}

// private methods

int CUserVerificationControllerComp::GenerateCode(int min, int max) const
{
	return QRandomGenerator::global()->bounded(min, max);
}


// public methods of the embedded class CheckExpiredVerificationDataThread

CUserVerificationControllerComp::CheckExpiredVerificationDataThread::CheckExpiredVerificationDataThread(CUserVerificationControllerComp& parent)
	:m_parent(parent)
{
}


void CUserVerificationControllerComp::CheckExpiredVerificationDataThread::Start()
{
	QThread::start();
}


// protected methods of the embedded class CheckExpiredVerificationDataThread

// reimplemented (QThread)

void CUserVerificationControllerComp::CheckExpiredVerificationDataThread::run()
{
	int interval = 5;
	if (!m_parent.m_userVerificationCollectionCompPtr.IsValid()){
		return;
	}

	while (!isInterruptionRequested()){
		imtbase::ICollectionInfo::Ids ids = m_parent.m_userVerificationCollectionCompPtr->GetElementIds();
		if (ids.isEmpty()){
			return;
		}

		for (const imtbase::ICollectionInfo::Id& id : ids){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_parent.m_userVerificationCollectionCompPtr->GetObjectData(id, dataPtr)){
				const imtauth::IUserVerification* userVerificationInfoPtr = dynamic_cast<const imtauth::IUserVerification*>(dataPtr.GetPtr());
				if (userVerificationInfoPtr != nullptr){
					if (userVerificationInfoPtr->IsExpired()){
						imtbase::ICollectionInfo::Ids elementIds;
						elementIds << id;
						m_parent.m_userVerificationCollectionCompPtr->RemoveElements(elementIds);
					}
				}
			}
		}

		int count = interval;
		while (count-- > 0){
			if (isInterruptionRequested()){
				return;
			}

			sleep(1);
		}
	}
}


} // namespace imtauth

