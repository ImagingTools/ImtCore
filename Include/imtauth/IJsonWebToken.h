#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtauth
{


class IJsonWebToken: virtual public iser::ISerializable
{
public:
	virtual QJsonObject GetHeader() const = 0;
	virtual void SetHeader(const QJsonObject& header) = 0;
	virtual QJsonObject GetPayload() const = 0;
	virtual void SetPayload(const QJsonObject& payload) = 0;
	virtual QByteArray GetSignature() const = 0;
	virtual void SetSignature(const QByteArray& signature) = 0;
};


} // namespace imtauth


