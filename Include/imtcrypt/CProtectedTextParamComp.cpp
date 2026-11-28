// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcrypt/CProtectedTextParamComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>


namespace imtcrypt
{


// reimplemented (iser::ISerializable)

bool CProtectedTextParamComp::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag protectedTextTag("ProtectedText", "Text protected by the secret protector, Base64", iser::CArchiveTag::TT_LEAF);

	const ISecretProtector* protectorPtr = m_secretProtectorCompPtr.GetPtr();

	if (archive.IsStoring()){
		QByteArray encoded;

		QByteArray secret = GetText().toUtf8();
		if (!secret.isEmpty()){
			QByteArray protectedSecret;
			if ((protectorPtr != nullptr) && protectorPtr->Protect(secret, protectedSecret)){
				encoded = protectedSecret.toBase64();
			}
			else{
				SendErrorMessage(0, QStringLiteral("The secret could not be protected and is not stored"), "ProtectedTextParam");
			}
		}

		bool retVal = archive.BeginTag(protectedTextTag);
		retVal = retVal && archive.Process(encoded);
		retVal = retVal && archive.EndTag(protectedTextTag);

		return retVal;
	}

	QByteArray encoded;
	bool retVal = archive.BeginTag(protectedTextTag);
	retVal = retVal && archive.Process(encoded);
	retVal = retVal && archive.EndTag(protectedTextTag);
	if (!retVal){
		return false;
	}

	QString text;
	if (!encoded.isEmpty()){
		QByteArray secret;
		if ((protectorPtr != nullptr) && protectorPtr->Unprotect(QByteArray::fromBase64(encoded), secret)){
			text = QString::fromUtf8(secret);
		}
		else{
			SendErrorMessage(0, QStringLiteral("The stored secret could not be unprotected; it is ignored"), "ProtectedTextParam");
		}
	}

	SetText(text);

	return true;
}


} // namespace imtcrypt


