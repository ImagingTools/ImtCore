// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CFileIO.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDebug>


namespace imtqml
{


QByteArray FileIO::source()
{
	return m_source;
}


void FileIO::setSource(QByteArray source)
{
	m_source = source;
}


QByteArray FileIO::read()
{
	if(m_source.isEmpty()){
		return QByteArray();
	}
	QFile file(m_source);
	if(!file.exists()){
		qWarning() << "Does not exits: " << m_source;
		return QByteArray();
	}

	QByteArray data;
	if(file.open(QIODevice::ReadOnly)){
		data = file.readAll();
	}

	return data;
}


bool FileIO::write(QByteArray data)
{
	if(m_source.isEmpty()){
		return false;
	}
	QFile file(m_source);
	if(file.open(QIODevice::WriteOnly)){
		file.write(data);

		return true;
	}

	return false;
}


} // namespace imtqml


