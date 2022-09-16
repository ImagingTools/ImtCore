#include "docattributes.h"
//#include <note.h>

DocAttributes::DocAttributes(QObject *parent) : QObject(parent)
{

}

QString DocAttributes::roleName(AttributesRoles role) const
{
    switch (role) {
    case NUM:
        return tr("Num");
    case DATE:
        return tr("Date");
    case COMMENT:
        return tr("Comment");
    case DELIVERY:
        return tr("Delivery");
    case REPORTER_CATEGORY:
        return tr("Category");
    case REPORTER_NAME:
        return tr("Name");
    case REPORTER_SURNAME:
        return tr("Surname");
    case REPORTER_NUM:
        return tr("Num");
    case REPORTER_DATE:
        return tr("Date");
    case REPORTER_SIGNATURES:
        return tr("Signatures");
    case REPORTER_CITY:
        return tr("City");
    case REPORTER_ADDRESS:
        return tr("Address");
    case REPORTER_POSTINDEX:
        return tr("Postindex");
    case REPORTER_PHONE:
        return tr("Phone");
    case REPORTER_INN:
        return tr("INN");
    case REPORTER_SNILS:
        return tr("SNILS");
    case REPORTER_SEX:
        return tr("sex");
    case REPORTER_PASSPORT:
        return tr("Passport");
    case REPORTER_COMMENT:
        return tr("Comment");

    }



//    roles[Num] = "num";
//    roles[Data] = "Data";
//    roles[Content] = "Content";
//    roles[Delivery] = "Delivery";
//    roles[ReporterName] = "ReporterName";
//    roles[ReporterNum] = "ReporterNum";
//    roles[ReporterData] = "ReporterData";
//    roles[ReporterSignatures] = "ReporterSignatures";

    return QString();
}

QString DocAttributes::getData(DocAttributes::AttributesRoles role) const
{
    QString res;
    TaskProperty *property = NULL;
    property = m_Properties.value(role, NULL);
    if(property)
        res = property->strData;
    return res;
}

void DocAttributes::setData(DocAttributes::AttributesRoles role, QString text)
{
    TaskProperty *cur_prop = m_Properties.value(role,NULL);
    if(cur_prop)
    {
        if(cur_prop->strData != text)
        {
            cur_prop->uiData = role;
            cur_prop->strData = text;
            if(text.isEmpty())
                cur_prop->m_status = TaskProperty::REMOVED;
            else
                cur_prop->m_status = TaskProperty::ON;
            m_lNewProps.append(cur_prop);
        }
    }
    else
    {
        cur_prop = new TaskProperty(TaskProperty::DOC_ATTRIBUTES);
        cur_prop->uiData = role;
        cur_prop->strData = text;
        cur_prop->m_status = TaskProperty::ON;
        m_Properties.insert(role,cur_prop);
        m_lNewProps.append(cur_prop);
    }
}


void DocAttributes::clear()
{
    qDeleteAll(m_Properties);
    m_Properties.clear();
}

void DocAttributes::setProperty(TaskProperty *property)
{
    delete m_Properties.take((AttributesRoles)property->uiData);
    if(property->m_status != TaskProperty::REMOVED)
        m_Properties.insert((AttributesRoles)property->uiData,property);
}
