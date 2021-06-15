#pragma once

#ifndef _In_
#define _In_
#endif
#ifndef _In_opt_
#define _In_opt_
#endif
#ifndef _Inout_
#define _Inout_
#endif
#ifndef _Inout_opt_
#define _Inout_opt_
#endif
#ifndef _Out_
#define _Out_
#endif
#ifndef _Out_opt_
#define _Out_opt_
#endif
#ifndef _Outptr_
#define _Outptr_
#endif
#ifndef _Outptr_opt_
#define _Outptr_opt_
#endif

// Qt includes
#include <QtSql>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{
	
//class QSqlDatabase;
	   
/**
	Interfaces and basic implementations for general database management.
	This package is system independent.
*/
class IDatabaseEngine : virtual public istd::IPolymorphic
{
public:
	virtual	QString ConnectionName() const = 0;
	virtual QSqlDatabase GetDatabase() const = 0;
	virtual bool OpenDataBase() = 0;
	virtual void CloseDataBase() = 0;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecSqlQuery(_In_ const QByteArray& queryString, _Outptr_opt_ QSqlError* sqlError = nullptr) const = 0 ;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecSqlQuery(_In_ const QByteArray& queryString, _In_ const QVariantMap& bindValues, _Outptr_opt_ QSqlError* sqlError = nullptr) const = 0;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecSqlQueryFromFile(_In_ const QByteArray& filePath, _Outptr_opt_ QSqlError* sqlError = nullptr) const = 0;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecSqlQueryFromFile(_In_ const QByteArray& filePath, _In_ const QVariantMap& bindValues, _Outptr_opt_ QSqlError* sqlError = nullptr) const = 0;
};


} // namespace imtdb
