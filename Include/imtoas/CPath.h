// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QList>

// ImtCore includes
#include <imtoas/CParameter.h>
#include <imtoas/COperation.h>
#include <imtoas/CServer.h>


namespace imtoas
{


class CPath
{
public:
	[[nodiscard]]QString GetId() const;
	void SetId(const QString& id);

	[[nodiscard]] QString GetReference() const;
	void SetReference(const QString& reference);

	[[nodiscard]] QString GetSummary() const;
	void SetSummary(const QString& summary);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] COperation GetGet() const;
	void SetGet(const COperation& get);

	[[nodiscard]] COperation GetPut() const;
	void SetPut(const COperation& put);

	[[nodiscard]] COperation GetPost() const;
	void SetPost(const COperation& post);

	[[nodiscard]] COperation GetDelete() const;
	void SetDelete(const COperation& deleted);

	[[nodiscard]] COperation GetOptions() const;
	void SetOptions(const COperation& options);

	[[nodiscard]] COperation GetHead() const;
	void SetHead(const COperation& head);

	[[nodiscard]] COperation GetPatch() const;
	void SetPatch(const COperation& patch);

	[[nodiscard]] COperation GetTrace() const;
	void SetTrace(const COperation& trace);

	[[nodiscard]] QList<CServer> GetServers() const;
	void SetServers(const QList<CServer>& servers);

	[[nodiscard]] QList<CParameter> GetParameters() const;
	void SetParameters(const QList<CParameter>& parameters);

	[[nodiscard]] static bool ReadFromJsonObject(CPath& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CPath& other) const;
	bool operator!=(const CPath& other) const { return !(operator==(other)); }


private:
	QString m_id;
	QString m_reference;
	QString m_summary;
	QString m_description;
	COperation m_get;
	COperation m_put;
	COperation m_post;
	COperation m_delete;
	COperation m_options;
	COperation m_head;
	COperation m_patch;
	COperation m_trace;
	QList<CServer> m_servers;
	QList<CParameter> m_parameters;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CPath);


