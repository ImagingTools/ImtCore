// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CPasswordObfuscator.h>


namespace imtdb
{


const char* CPasswordObfuscator::s_prefix = "{enc}";

// Application-level obfuscation key (not a cryptographic secret - prevents casual reading)
const quint8 CPasswordObfuscator::s_key[] = {
	0x4D, 0x67, 0x2A, 0x51, 0x78, 0x1F, 0x6B, 0x3E,
	0x59, 0x0C, 0x44, 0x73, 0x28, 0x65, 0x1A, 0x5F
};

const int CPasswordObfuscator::s_keyLength = sizeof(s_key);


// public static methods

QString CPasswordObfuscator::Obfuscate(const QString& password)
{
	if (password.isEmpty()){
		return password;
	}

	QByteArray utf8Data = password.toUtf8();
	QByteArray transformed = Transform(utf8Data);
	QByteArray encoded = transformed.toBase64();

	return QString::fromLatin1(s_prefix) + QString::fromLatin1(encoded);
}


QString CPasswordObfuscator::Deobfuscate(const QString& value)
{
	if (!IsObfuscated(value)){
		return value;
	}

	QString encoded = value.mid(static_cast<int>(strlen(s_prefix)));
	QByteArray transformed = QByteArray::fromBase64(encoded.toLatin1());
	QByteArray utf8Data = Transform(transformed);

	return QString::fromUtf8(utf8Data);
}


bool CPasswordObfuscator::IsObfuscated(const QString& value)
{
	return value.startsWith(QString::fromLatin1(s_prefix));
}


// private static methods

QByteArray CPasswordObfuscator::Transform(const QByteArray& data)
{
	QByteArray result(data.size(), Qt::Uninitialized);

	for (int i = 0; i < data.size(); ++i){
		result[i] = static_cast<char>(static_cast<quint8>(data.at(i)) ^ s_key[i % s_keyLength]);
	}

	return result;
}


} // namespace imtdb

