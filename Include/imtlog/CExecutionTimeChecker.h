#pragma once


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QDateTime>

namespace imtlog
{


class CExecutionTimeChecker
{
public:
	CExecutionTimeChecker(const QString& text);
	~CExecutionTimeChecker();

private:
	QString m_text;
	QTime m_time;
};


} // namespace imtlog


