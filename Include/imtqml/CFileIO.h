#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>


namespace imtqml
{


class FileIO : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

public:
	QString source();
	void setSource(QString source);
	Q_INVOKABLE QString read();
	Q_INVOKABLE bool write(QString text);

Q_SIGNALS:
	void sourceChanged();

private:
	QString m_source;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::FileIO*)


