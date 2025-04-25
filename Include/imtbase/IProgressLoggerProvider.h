#pragma once


// ACF includes
#include <ibase/IProgressLogger.h>
#include <istd/IChangeable.h>


namespace imtbase
{


class IProgressLoggerProvider: virtual public istd::IChangeable
{
public:
	/**
		Get IDs of the available progress loggers
	*/
	virtual QByteArrayList GetProgressLoggerIds() const = 0;

	/**
		Get progress logger with given ID
	*/
	virtual ibase::IProgressLogger* GetProgressLogger(const QByteArray& progressLoggerId) const = 0;
};


} // namespace imtbase


