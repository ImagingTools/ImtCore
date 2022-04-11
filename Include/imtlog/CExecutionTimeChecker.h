#pragma once


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>


namespace imtlog
{


class CExecutionTimeChecker
{
public:
	CExecutionTimeChecker(const QString& text);
	~CExecutionTimeChecker();

private:
	QString m_text;
	QElapsedTimer m_time;
};


} // namespace imtlog


