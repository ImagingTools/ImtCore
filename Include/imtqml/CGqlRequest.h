// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>


namespace imtqml
{


class CGqlRequest: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString state READ State WRITE SetState NOTIFY stateChanged)
	Q_PROPERTY(QString json READ GetJson WRITE SetJson NOTIFY jsonChanged)

public:
	typedef QObject BaseClass;

	Q_INVOKABLE inline bool setGqlQuery(QString query, QVariantMap headers = QVariantMap()){ return SetGqlQuery(query, headers); }

	explicit CGqlRequest(QObject* parent = nullptr);
	~CGqlRequest();

	bool SetGqlQuery(QString query, QVariantMap headers = QVariantMap());

public Q_SLOTS:
	void replyFinished();
	void errorOccurred(QNetworkReply::NetworkError code);
	void onSslErrors(const QList<QSslError>& errors);

Q_SIGNALS:
	void stateChanged(const QString& state);
	void jsonChanged(const QString& state);

private:
	const QString &State() const;
	void SetState(const QString& newState);

	QString GetJson() const;
	void SetJson(const QString& json);

	QString m_json;
	QString m_state;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlRequest*)


