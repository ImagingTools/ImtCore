#include <imtqml/CFileIO.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>


namespace imtqml
{


QString FileIO::source()
{
	return m_source;
}


void FileIO::setSource(QString source)
{
	m_source = source;
}


QString FileIO::read()
{
	if(m_source.isEmpty()) {
		return QString();
	}
	QFile file(m_source);
	if(!file.exists()) {
		qWarning() << "Does not exits: " << m_source;
		return QString();
	}

	QString text;
	if(file.open(QIODevice::ReadOnly)) {
		QTextStream stream(&file);
		text = stream.readAll();
	}

	return text;
}


bool FileIO::write(QString text)
{
	if(m_source.isEmpty()) {
		return false;
	}
	QFile file(m_source);
	if(file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);
		stream << text;

		return true;
	}

	return false;
}


} // namespace imtqml


