// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoUploadSink.h>


namespace imtrestdemo
{


CDemoUploadSink::CDemoUploadSink(const QString& filePath)
	:BaseClass(filePath),
	m_hash(QCryptographicHash::Sha256)
{
}


QByteArray CDemoUploadSink::GetSha256() const
{
	return m_result;
}


// reimplemented (imtrest::IHttpRequestBodySink)

bool CDemoUploadSink::Write(const char* data, qint64 size)
{
	if (!BaseClass::Write(data, size)){
		return false;
	}

	m_hash.addData(QByteArrayView(data, qsizetype(size)));

	return true;
}


bool CDemoUploadSink::Finish()
{
	if (!BaseClass::Finish()){
		return false;
	}

	m_result = m_hash.result().toHex();

	return true;
}


} // namespace imtrestdemo
