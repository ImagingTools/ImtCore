#pragma once


// Qt includes
#include <QtCore/QSharedPointer>
#include <QtNetwork/QSsl>
#include <QtNetwork/QSslSocket>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtcom/ISslConfigurationManager.h>


// forward declaration
class QSslKey;
class QSslCertificate;

namespace imtcom
{

class CSslConfigurationManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public ISslConfigurationManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSslConfigurationManagerComp)
		I_REGISTER_INTERFACE(ISslConfigurationManager)
	I_END_COMPONENT

	QSsl::KeyAlgorithm GetKeyAlgorithmFromParams(const iprm::IParamsSet& params) const;;
	QSsl::EncodingFormat GetEncodingFormatFromParams(const iprm::IParamsSet& params) const;
	QSsl::KeyType GetKeyTypeFromParams(const iprm::IParamsSet& params) const;
	QSsl::SslProtocol GetSslProtocolFromParams(const iprm::IParamsSet& params) const;
	QSslSocket::PeerVerifyMode GetVerifyModeFromParams(const iprm::IParamsSet& params) const;
	QSharedPointer<QSslKey> CreateSslKeyFromParams(const iprm::IParamsSet& params) const;
	QSharedPointer<QSslCertificate> CreateSslCertificateFromParams(const iprm::IParamsSet& params) const;

	// reimplemented (ISslConfigurationManager)
	virtual bool CreateSslConfiguration(const iprm::IParamsSet& params, QSslConfiguration& output) const override;
};




} // namespace imtcom


