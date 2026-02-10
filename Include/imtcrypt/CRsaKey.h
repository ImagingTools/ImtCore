// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtcrypt
{


class CRsaKey
{
public:
	void GenerateRsaKeys();
	QByteArray GetPrivateKey() const;
	QByteArray GetPublicKey() const;
	void SetPrivateKey(const QByteArray &key);
	void SetPublicKey(const QByteArray &key);

protected:
	QByteArray m_privateKey;
	QByteArray m_publicKey;
};


} //namespace imtcrypt


