#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QUrl>


namespace imtqml
{


class FileIO : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

public:
	QUrl source();
	void setSource(QUrl source);
	Q_INVOKABLE QString read();
	Q_INVOKABLE bool write(QString text);

Q_SIGNALS:
	void sourceChanged();

private:
	QUrl m_source;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::FileIO*)


