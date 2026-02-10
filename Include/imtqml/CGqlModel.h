// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>


namespace imtqml
{


class CGqlModel: public imtbase::CTreeItemModel
{
	Q_OBJECT

public:
	typedef imtbase::CTreeItemModel BaseClass;

	Q_INVOKABLE inline bool setGqlQuery(QString query, QVariantMap headers = QVariantMap()){ return SetGqlQuery(query, headers); }

	explicit CGqlModel(QObject* parent = nullptr);
	~CGqlModel();

public Q_SLOTS:
	bool SetGqlQuery(QString query, QVariantMap headers = QVariantMap());
	void replyFinished();
	void errorOccurred(QNetworkReply::NetworkError code);

	static void SetGlobalAccessToken(const QString& accessToken);
	static void SetRefreshToken(const QString& refreshToken);

public:
	static QString s_accessToken;
	static QString s_refreshToken;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlModel*)


