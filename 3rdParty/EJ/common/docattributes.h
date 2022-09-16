#ifndef DOCATTRIBUTES_H
#define DOCATTRIBUTES_H

#include <QObject>
#include <QAbstractListModel>
#include "common_global.h"

class TaskProperty;

class COMMONSHARED_EXPORT DocAttributes : public QObject
{
    Q_OBJECT
public:
    enum AttributesRoles {
        NUM = 1,
        DATE,
        COMMENT,
        DELIVERY,

        REPORTER_CATEGORY = 50,
        REPORTER_NAME,
        REPORTER_SURNAME,
        REPORTER_NUM,
        REPORTER_DATE,
        REPORTER_SIGNATURES,
        REPORTER_CITY,
        REPORTER_POSTINDEX,
        REPORTER_ADDRESS,
        REPORTER_PHONE,
        REPORTER_INN,
        REPORTER_SNILS,
        REPORTER_SEX,
        REPORTER_PASSPORT,
        REPORTER_COMMENT
    };
    Q_ENUMS(AttributesRoles)

    enum ReportersRoles {
        Category = 1,
        Name,
        SurName,
        City,
        PostIndex,
        Address,
        Phone,
        INN,
        SNILS,
        Sex,
        NumPassport,
        Comment
    };

//    enum ModelsRoles {
//        NameRole = Qt::UserRole + 1,
//        DataText
//    }

    explicit DocAttributes(QObject *parent = nullptr);
    Q_INVOKABLE QString roleName(AttributesRoles role) const;
    Q_INVOKABLE QString getData(AttributesRoles role) const;
    Q_INVOKABLE void setData(AttributesRoles role, QString text);
    void clear();
    void setProperty(TaskProperty *property);
    QList<TaskProperty*> m_lNewProps;
    QMap<AttributesRoles, TaskProperty*> m_Properties;
//    void sort();
signals:

public slots:
protected:
//    QMap<int, QString> m_nameOfData;


};

#endif // DOCATTRIBUTES_H
