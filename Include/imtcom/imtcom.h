#pragma once

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QSharedPointer>


class QSslConfiguration;
class QSslKey;
class QSslCertificate;

/**
	Namespace containing the implementation of REST-based communication
	This package is system independent.
*/
namespace imtcom
{

class ISslConfiguration;
class ISslKey;
class ISslCertificate;


QSharedPointer<QSslKey> CreateSslKey(const ISslKey& sslKey);
QSharedPointer<QSslCertificate> CreateSslCertificate(const ISslCertificate& sslCertificate);
void CreateSslConfiguration(const ISslConfiguration& input, QSslConfiguration& output);
void AddSslKeyToConfiguration(const ISslConfiguration& input, QSslConfiguration& output);


} // namespace imtcom


