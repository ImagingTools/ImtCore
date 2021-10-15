#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>


namespace imtdesign
{

class IDesignTokenProcessor
{
public:
	/**
		\brief Processing the file
		\return result of processing
		\retval 0 - success \retval !0 - fail
	 */
	virtual int Exec() = 0;
	virtual QByteArray GetHelpString()const = 0;

};


} // namespace imtdesign
