#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes


namespace imtstyle
{

class IDesignTokenProcessor
{
public:
	virtual int Exec() = 0;

};


} // namespace imtstyle
