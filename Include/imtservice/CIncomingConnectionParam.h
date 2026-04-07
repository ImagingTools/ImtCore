// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtservice
{


class CIncomingConnectionParam: virtual public iser::ISerializable
{
public:
	QString GetHost() const;
	void SetHost(const QString& host);

	QString GetDescription() const;
	void SetDescription(const QString& description);

	int GetWsPort() const;
	void SetWsPort(int wsPort);

	int GetHttpPort() const;
	void SetHttpPort(int httpPort);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_host = QStringLiteral("localhost");
	QString m_description;
	int m_wsPort = -1;
	int m_httpPort = -1;
};


} // namespace imtservice


