#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtauth/IJwtSessionInfo.h>


namespace imtauth
{


class CJwtSessionInfo: virtual public IJwtSessionInfo
{
public:
	CJwtSessionInfo();

	// reimplemented (imtauth::IJwtSessionInfo)
	virtual const imtauth::IJsonWebToken* GetAccessToken() const override;
	virtual void SetAccessToken(const imtauth::IJsonWebToken* accessToken) override;
	virtual QByteArray GetRefreshToken() const override;
	virtual void SetRefreshToken(const QByteArray& refreshToken) override;
	virtual QDateTime GetCreationDate() const override;
	virtual void SetCreationDate(const QDateTime& creationDate) override;
	virtual int GetAccessTokenLifeTime() const override;
	virtual void SetAccessTokenLifeTime(int lifeTime) override;
	virtual int GetRefreshTokenLifeTime() const override;
	virtual void SetRefreshTokenLifeTime(int lifeTime) override;
	virtual bool IsAccessTokenAlive() const override;
	virtual bool IsRefreshTokenAlive() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	int m_accessTokenLifeTime;
	int m_refreshTokenLifeTime;
	QDateTime m_creationDate;
	QByteArray m_refreshToken;
	std::unique_ptr<imtauth::IJsonWebToken> m_jwtTokenPtr;
};


} // namespace imtauth


