#include <imtqml/CFileIO.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>


namespace imtqml
{


QUrl FileIO::source()
{
	return m_source;
}


void FileIO::setSource(QUrl source)
{
	m_source = source;
}


QString FileIO::read()
{
	if(m_source.isEmpty()) {
		return QString();
	}
	QFile file(m_source.toLocalFile());
	if(!file.exists()) {
		qWarning() << "Does not exits: " << m_source.toLocalFile();
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
	QFile file(m_source.toLocalFile());
	if(file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);
		stream << text;

		return true;
	}

	return false;
}


} // namespace imtqml


