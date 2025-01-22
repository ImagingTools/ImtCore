#pragma once


// Qt includes
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtauth/IJsonWebToken.h>


namespace imtauth
{


class CJsonWebToken: virtual public IJsonWebToken
{
public:
	// reimplemented (imtauth::IJsonWebToken)
	virtual QJsonObject GetHeader() const override;
	virtual void SetHeader(const QJsonObject& header) override;
	virtual QJsonObject GetPayload() const override;
	virtual void SetPayload(const QJsonObject& payload) override;
	virtual QByteArray GetSignature() const override;
	virtual void SetSignature(const QByteArray& signature) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QJsonObject m_header;
	QJsonObject m_payload;
	QByteArray m_signature;
};


} // namespace imtauth


