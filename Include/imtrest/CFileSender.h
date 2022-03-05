#pragma once


// Qt includes
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>



namespace imtrest
{


/**
    Universal data controller for UI representations
    \ingroup LicenseManagement
*/
class CFileSender: public QObject
{
    Q_OBJECT

public:
    explicit CFileSender(QObject* parent = nullptr);
    ~CFileSender();

public Q_SLOTS:
    bool SendFile(QString query);
    void replyFinished();
};


} // namespace imtrest

Q_DECLARE_METATYPE(imtrest::CFileSender*)
