#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QByteArray>


namespace imtqml
{


class FileIO : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QByteArray source READ source WRITE setSource NOTIFY sourceChanged)

public:
	QByteArray source();
	void setSource(QByteArray source);
	Q_INVOKABLE QByteArray read();
	Q_INVOKABLE bool write(QByteArray data);

Q_SIGNALS:
	void sourceChanged();

private:
	QByteArray m_source;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::FileIO*)


