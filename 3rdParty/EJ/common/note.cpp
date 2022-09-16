#include "note.h"
#ifndef WIN32
//#include <uuid/uuid.h>
//#include <external/e2fsprogs/lib/uuid/uuid.h>
#endif
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
//#include "difft.h"
#include "simplecrypt.h"
#include "storage.h"
#include "tags.h"
//#include "ejdb.hh"
#include "filter.h"
#include "reply.h"

QByteArray autorData;
quint64 keyData;



QUuid GenerateGUID()//генерирует номер объекта по GUID
{
    //    qint64 isfNO;
    QUuid res = QUuid::createUuid();
    //    res.data1 = 25;
    //    QByteArray ba(16,1);
//    res = QUuid::createUuidV5(res,autorData);
    return res;

    //#ifndef WIN32
    //    uuid_t out;
    //    char *chr = (char*)out;
    //    uuid_generate(out);
    //    QByteArray ba(chr,16);
    //    res = QUuid::fromRfc4122(ba);
    ////    isfNO = *(qint64*)(out+8);
    //#else
    //    GUID G3;
    //    CoCreateGuid(&G3);
    ////    isfNO = *(qint64*)(&(G3.Data4));
    //#endif

    //    return res;
}

quint64 getKey(quint64 key,char* data)
{
    char *chr = (char *)&key;
    quint64 res;
    char *res_chr = (char*)&res;
    res_chr[0] = chr[0] | data[0];
    res_chr[1] = chr[1] | data[3];
    res_chr[2] = chr[2] | data[4];
    res_chr[3] = chr[3] | data[7];
    res_chr[4] = chr[4] | data[10];
    res_chr[5] = chr[5] | data[11];
    res_chr[6] = chr[6] | data[13];
    res_chr[7] = chr[7] | data[14];
    return res;

}

quint64 getKey_min(quint64 key, char *data)
{
    char *chr = (char *)&key;
    quint64 res = key;
    char *res_chr = (char*)&res;
    res_chr[0] = chr[0] | data[0];
    res_chr[5] = chr[5] | data[1];
    return res;
}



bool caseNoteMoreThan(const Task *task1, const Task *task2)
{
    return task1->dt_modify > task2->dt_modify;
}

bool caseGroupMoreThan(const QObject *obj1, const QObject *obj2)
{
    Group *group1 = (Group*)obj1;
    Group *group2 = (Group*)obj2;
//    Group *group;
    if(group1->id < 2 || group2->id < 2) {
        return group1->id < group2->id;
//    if(group1->id == 0) return true;
//    if(group2->id == 0) return false;
    }
    if(group1->parentGroup != group2->parentGroup)
    {
        if(group1->level != group2->level)
        {
            if(group1->level < group2->level)
            {
                if(group2->parentGroup == group1)
                    return true;
//                group = group2;
                for(int i = 0; i < 10; i++)
                {
                    if(group2->parentGroup && group2->parentGroup != group1->parentGroup) {
                        group2 = group2->parentGroup;
                        if(group2->parentGroup == group1)
                            return true;
                        if(group1->level == group2->level)
                            break;
                    }
                }

            }
            else if(group2->level < group1->level)
            {
                if(group1->parentGroup == group2)
                    return false;
                for(int i = 0; i < 10; i++)
                {
                    if(group1->parentGroup && group2->parentGroup != group1->parentGroup) {
                        group1 = group1->parentGroup;
                        if(group1->parentGroup == group2)
                            return false;
                        if(group1->level == group2->level)
                            break;
                    }
                }

            }
//            else
            {
                for(int i = 0; i < 10; i++)
                {
                    if(group1->parentGroup && group2->parentGroup && group2->parentGroup != group1->parentGroup)
                    {
                        group1 = group1->parentGroup;
                        group2 = group2->parentGroup;
                    }
                }
            }


        }
        else if(group1->parentGroup && group2->parentGroup)
        {
            return caseGroupMoreThan(group1->parentGroup, group2->parentGroup);
        }
    }
    return group1->name < group2->name;
}

bool caseTagMoreThan(const QObject *obj1, const QObject *obj2)
{
//    Tag *tag1 = (Tag*)obj1;
//    Tag *tag2 = (Tag*)obj2;
    Tag const *tag1 = dynamic_cast<Tag const*>(obj1);
    Tag const *tag2 = dynamic_cast<Tag const*>(obj2);
    return tag1->name < tag2->name;
}

bool caseCheckTagMoreThan(const QObject *obj1, const QObject *obj2)
{
    Tag const *tag1 = dynamic_cast<Tag const*>(obj1);
    Tag const *tag2 = dynamic_cast<Tag const*>(obj2);
    return tag1->time_checked > tag2->time_checked;
}

bool caseUserMoreThan(const QObject *obj1, const QObject *obj2)
{
    EJUser *user1 = (EJUser*)obj1;
    EJUser *user2 = (EJUser*)obj2;
    if(user1->id == 0)
        return true;
    if(user2->id == 0)
        return false;
    return user1->name < user2->name;
}

bool caseUserMoreThanUnreg(const QObject *obj1, const QObject *obj2)
{
    EJUser *user1 = (EJUser*)obj1;
    EJUser *user2 = (EJUser*)obj2;
    return user1->time_modify < user2->time_modify;
}

Group::Group(QObject *parent) : QObject(parent)
{
//    QUuid ful_key = GenerateGUID();
//    key = *((quint32*)ful_key.data4);
    key = GenerateGUID();
    rootKey = key;
    offset = 0;
    ver = 0;
    is_modify = 0;
    time_modify = 0;
    id = 0;
    level = 0;
    parentGroup = nullptr;
    m_isOpen = false;
    m_childsEnabled = false;
    m_isOpenChilds = false;
    m_access = 0;
}

bool Group::isOpen() const
{
    return m_isOpen;
}

bool Group::childsEnabled() const
{
    return m_childsEnabled;
}

bool Group::isOpenChilds() const
{
    return m_isOpenChilds;
}

int Group::access() const
{
    return m_access;
}

QString Group::shortName() const
{
    if(m_shortName.isEmpty())
        return name;
    return m_shortName;
}

Group *Group::rootGroup()
{
    Group *retVal = this;
    while(retVal->parentGroup != nullptr && retVal != retVal->parentGroup)
        retVal = retVal->parentGroup;
    return retVal;
}

void Group::setIsOpen(bool isOpen)
{
    if (m_isOpen == isOpen)
        return;

    m_isOpen = isOpen;
    emit isOpenChanged(m_isOpen);
}

void Group::setChildsEnabled(bool childsEnabled)
{
    if (m_childsEnabled == childsEnabled)
        return;

    m_childsEnabled = childsEnabled;
    emit childsEnabledChanged(m_childsEnabled);
}

void Group::setIsOpenChilds(bool isOpenChilds)
{
    if (m_isOpenChilds == isOpenChilds)
        return;

    m_isOpenChilds = isOpenChilds;
    emit isOpenChildsChanged(m_isOpenChilds);
}

void Group::setAccess(int access)
{
    if (m_access == access)
        return;

    m_access = access;
    emit accessChanged(m_access);
}

void Group::setShortName(QString shortName)
{
    if (m_shortName == shortName)
        return;

    m_shortName = shortName;
    emit shortNameChanged(m_shortName);
}


Tag::Tag(QObject *parent) : QObject(parent)
{
    //    QUuid ful_key = GenerateGUID();
    //    key = *((quint32*)ful_key.data4);
    key = GenerateGUID();
    offset = 0;
    ver = 0;
    is_modify = 0;
    time_modify = 0;
    id = 0;
    vid = WHAT;
}

void Tag::setVid(Tag::VidTag source)
{
    if(vid != source)
    {
        vid = source; emit vidChanged();
        is_modify = true;
        ext_storage->saveTag(this);
    }
}

QString Tag::shortName() const
{
    if(m_shortName.isEmpty())
        return name;
    return m_shortName;
}

void Tag::setShortName(QString shortName)
{
    if (m_shortName == shortName)
        return;

    m_shortName = shortName;
    emit shortNameChanged(m_shortName);
}



void Task::clear()
{
    //    Block *curBlock;
    //    int k = 0;
    //    while (lBlocks.count() > 0) {
    //        curBlock = lBlocks.takeFirst();
    //        delete curBlock;
    //    }
 //   qDeleteAll(lBlocks);
    if(m_doc)
        m_doc->clear();
    qDeleteAll(lChilds);
    lChilds.clear();
    qDeleteAll(m_newProperties);
    m_newProperties.clear();
    qDeleteAll(m_signatures);
    m_signatures.clear();

//    qDeleteAll(m_lNotes);
//    m_lNotes.clear();
    qDeleteAll(m_lTransferTimes);
    m_lTransferTimes.clear();

}

bool Task::isContainsTags(QSet<qint32> &filter_tags)
{
	QSet<qint32>tags = QSet(m_tags.begin(),m_tags.end());
    tags.intersect(filter_tags);
    return !tags.isEmpty();
}

void Task::loadLinks(bool force)
{
    Task task;
    quint32 lastOffset;
    qint16 lastKey;
    quint16 lastVer;
    LinkProp *curLink;

    delete task.m_doc;
    task.m_doc = nullptr;

    for(int i = 0; i < m_inputLinks.count(); i++)
    {
        Task *task = m_inputLinks[i];
//        task->id = EjDb::Instance()->getTaskId(task->key);
//        if(EjDb::Instance()->loadTask(task))
//        {
//            ext_storage->loadTasksBody(task, lastOffset, lastKey, lastVer);
//        }
    }
    for(int i = 0; i < m_outputLinks.count(); i++)
    {
        Task *task = m_outputLinks[i];
//        task->id = EjDb::Instance()->getTaskId(task->key);
//        if(EjDb::Instance()->loadTask(task))
//        {
//            ext_storage->loadTasksBody(task, lastOffset, lastKey, lastVer);
//        }
    }
    if(!m_doc->m_attrProp)
        return;
    for(int i = 0; i < m_doc->m_attrProp->m_lLinks.count(); i++) {
        curLink = m_doc->m_attrProp->m_lLinks.at(i);
  //      if(i == 0)
  //      {
  //          curLink->m_doc = this->doc;
  //      }
  //      else
        {
            if(curLink->m_extTask && !force)
                continue;
//            if(!curLink->m_extDoc)
//                curLink->m_extDoc = new Document(curLink->keyUuid());
            if(!curLink->m_extTask)
                curLink->m_extTask = new Task();
            curLink->m_extTask->key = curLink->keyUuid();
            curLink->m_extDoc = curLink->m_extTask->m_doc;
//            curLink->m_extTask->id = EjDb::Instance()->getTaskId(curLink->m_extTask->key);
//            if(EjDb::Instance()->loadTask(curLink->m_extTask))
//            {
//                ext_storage->loadTasksBody(curLink->m_extTask, lastOffset, lastKey, lastVer);
//            }
//            task.m_doc = nullptr;
        }
    }

}

void Task::reloadLink(QUuid key)
{
    quint32 lastOffset;
    qint16 lastKey;
    quint16 lastVer;
    LinkProp *curLink;
    QList<LinkProp*>lLinks;
    QList<Task*>lTaskLinks;
    QList<Reply*>lReplies;
    Task *task;
    findLink(key, lLinks);
    for(int i = 0; i < lLinks.count(); i++)
    {
        curLink = lLinks[i];
        if(curLink)
        {
            if(!curLink->m_extTask)
                curLink->m_extTask = new Task();
            curLink->m_extTask->key = curLink->keyUuid();
            curLink->m_extDoc = curLink->m_extTask->m_doc;
//            curLink->m_extTask->id = EjDb::Instance()->getTaskId(curLink->m_extTask->key);
//            if(EjDb::Instance()->loadTask(curLink->m_extTask))
//            {
//                ext_storage->loadTasksBody(curLink->m_extTask, lastOffset, lastKey, lastVer);
//            }
            emit curLink->m_extTask->titleChanged();
            emit curLink->m_extTask->repliesChanged();
        }
    }
    findTaskLink(key, lTaskLinks, lReplies);
    for(int i = 0; i < lTaskLinks.count(); i++)
    {
        task = lTaskLinks[i];
        if(task)
        {
//            if(task->id < 0)
//                task->id = EjDb::Instance()->getTaskId(task->key);
//            if(EjDb::Instance()->loadTask(task))
//            {
//                ext_storage->loadTasksBody(task, lastOffset, lastKey, lastVer);
//            }
            emit task->workStateChanged();
            emit task->titleChanged();
            emit task->repliesChanged();

        }
    }

    for(int i = 0; i < lReplies.count(); i++)
    {
        Reply *reply = lReplies[i];
        emit reply->commentChanged();
        emit reply->workStateChanged();
        emit reply->subtasksChanged();
    }
    for(int i = 0; i < m_outputLinks.count(); i++)
    {
        if(!m_outputLinks[i]->isDocument())
        {
            loadSubtasks();
            break;
        }
    }
    emit repliesChanged();
}

void Task::sign(int status, QString comment)
{
    TaskProperty *task_prop = new TaskProperty(TaskProperty::SIGNED2, this);
    task_prop->uiData = ext_storage->myId();
    task_prop->uiData2 = 0;
    task_prop->m_status = (TaskProperty::e_status)status;
    task_prop->strData = comment;
    m_newProperties.append(task_prop);
    for(int j = 0; j < m_signatures.count(); j++)
    {
        if(m_signatures.at(j)->uiData == task_prop->uiData)
        {
            task_prop->uiData2 = m_signatures.at(j)->uiData2;
            delete m_signatures.takeAt(j);
            j--;
        }
    }
    m_signatures.append(task_prop);
    ext_storage->saveTasksBody(this);
    m_newProperties.clear();

    emit signaturesChanged();
    emit countAgreedChanged();
    foreach(JotInterface *plugin,ext_plugins)
    {
        plugin->docSigned(m_doc, status, comment);
    }
}

void Task::addReply(int status, QString comment)
{
    int signId = ext_storage->myId();
//    QList<QUuid> keys = ext_storage->getSettings()->mAuthorities.values(EJSettings::A_SIGN_RESOLUTION);
//    for(int i = 0; i < keys.count(); i++)
//    {
//        signId = ext_storage->getUserId(keys[i]);
//    }
    TaskProperty *task_prop = new TaskProperty(TaskProperty::REPLY, this);
    task_prop->uiData = signId;
    task_prop->uiData2 = 0;
    task_prop->m_status = status;
    task_prop->strData = comment;
    m_newProperties.append(task_prop);
    Reply *reply = nullptr;
    for(int j = 0; j < m_replies.count(); j++)
    {
        if(m_replies[j]->m_author.first == task_prop->uiData)
        {
            reply = m_replies[j];
            task_prop->uiData2 = m_replies[j]->m_author.second;
            break;
//            delete m_replies.takeAt(j);
//            j--;
        }
    }
    if(!reply)
    {
        reply = new Reply();
        reply->m_author.first = task_prop->uiData;
        reply->m_author.second = task_prop->uiData2;
        m_replies.append(reply);
    }
    reply->m_comment = comment;
    reply->m_workState = (Task::WorkState)status;

    if(responsible() == signId && m_workState != WS_CLOSED)
    {
        setWorkState(status, false);
    }
    //reply->m_reply = task_prop;
    int securityBak = this->security;
    this->security = EDIT;
    ext_storage->saveTasksBody(this);
    this->security = securityBak;
    qDeleteAll(m_newProperties);
    m_newProperties.clear();

    emit repliesChanged();

}

void Task::setDocDefaults(DocLayout *docLayout, DocMargings *docMargings)
{
    int securityBak = this->security;
    this->security = EDIT;
    AttrProp *attrProp = this->document()->attributes();
    attrProp->setDocLayout(docLayout);
    attrProp->setDocMargings(docMargings);
    ext_storage->saveTasksBody(this);
    this->security = securityBak;
}

Reply* Task::myReply()
{
    Reply *reply = nullptr;
    for(int j = 0; j < m_replies.count(); j++)
    {
        if(ext_storage->isForMy(m_replies[j]->m_author.first))
        {
            reply = m_replies[j];
            break;
        }
    }
    return reply;
}

void Task::setChat(Document *doc)
{
    if(!doc)
        return;
    quint32 offset;
//    EjDb::Instance()->setTaskEASKeyToCrypter(id);
    offset = ext_storage->saveChat(ext_storage->myId(),*doc->lBlocks,data);
    if(offset > 0) {
        TaskProperty *task_prop = new TaskProperty(TaskProperty::CHAT, this);
        task_prop->uiData = offset;
        task_prop->m_status = TaskProperty::ON; // (TaskProperty::e_status)status;
//        task_prop->strData = comment;
        m_newProperties.append(task_prop);
        ext_storage->saveTasksBody(this);
        m_newProperties.clear();
        m_chats.append(task_prop);
        delete task_prop;
//        getTask();
        emit chatsChanged();
    }

//    emit signaturesChanged();
//    emit countAgreedChanged();
}

void Task::setResponsible(qint32 id)
{
//    if(id != responsible())
//    {
//        TaskProperty *task_prop = new TaskProperty(TaskProperty::SIGNED);
//        task_prop->uiData = id;
//        lProperties.append(task_prop);
//        emit responsibleChanged();
//    }
}

qint32 Task::responsible()
{
    return m_responsible.first;
    //    return ext_storage->getUserName(res);
}

QString Task::nameResponsible()
{
    return ext_storage->userNameInGroup(m_responsible.first, m_responsible.second);
}

QString Task::shortNameResponsible()
{
    return ext_storage->shortUserNameInGroup(m_responsible.first, m_responsible.second);
}

QString Task::nameAuthor()
{
    QString retVal;
    if(m_author.first > -1)
        retVal = ext_storage->userNameInGroup(m_author.first, m_author.second);
    return retVal;
}

QString Task::shortNameAuthor()
{
    QString retVal;
    if(m_author.first > -1)
        retVal = ext_storage->shortUserNameInGroup(m_author.first, m_author.second);
    return retVal;
}

QString Task::fromWhom()
{
    QString retVal;
    if(m_from > -1)
        retVal = ext_storage->getUserName(m_from);
    else
        retVal = ext_storage->getUserName(creator_id);
    return retVal;
}



QDateTime Task::fromTime()
{
	return QDateTime::fromSecsSinceEpoch(m_fromTime);
}

QDateTime Task::registryTime()
{
	return QDateTime::fromSecsSinceEpoch(m_registryTime);
}

//void Task::getTask(bool isFromId)
//{
//    QString sql;
//    QSqlQuery q;
//    SimpleCrypt crypto;
////    quint64 key2 = Q_UINT64_C(0x3847afdb4cb9e20d);
//    quint64 key2 = Q_UINT64_C(0x4947afdb4cb9e20d);

//    quint64 new_key;

////    sql = QString("select task_key, patch_key, offset, flag, state, is_modify, time_modify, time_history, title, data, ver, ver_patch, user_key from notes where id=%1").arg(id);
////    if(!q.exec(sql))
////    {
////        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
////        return;
////    }
//    if(isFromId)
//    {
//        if (!q.prepare(QLatin1String("select task_key, patch_key, offset, flag, state, is_modify, time_modify, time_history, title, data, ver, ver_patch, user_key, id, users from tasks where id=:id")))
//        {
//            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//            return;
//        }
//        q.bindValue(":id",id);

//    }
//    else
//    {
//        if (!q.prepare(QLatin1String("select task_key, patch_key, offset, flag, state, is_modify, time_modify, time_history, title, data, ver, ver_patch, user_key, id, users from tasks where task_key=:task_key")))
//        {
//            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//            return;
//        }
//        q.bindValue(":task_key",key.toRfc4122());
//    }
//    if(!q.exec())
//        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();

//    q.next();

//    key = QUuid::fromRfc4122(q.value(0).toByteArray());
////    patch_key = q.value(1).toInt();
//    offset_history = q.value(2).toInt();
//    flag = q.value(3).toInt();
//    state = q.value(4).toInt();
//    is_modify = q.value(5).toInt();
//    dt_modify = QDateTime::fromSecsSinceEpoch(q.value(6).toUInt());
//    dt_history = QDateTime::fromSecsSinceEpoch(q.value(7).toUInt());


//    data = q.value(9).toLongLong();
//    state = data - (data/10*10 +1);
//    data -= state;
////    new_key = keyData ^ q.value(9).toLongLong();
//    new_key = keyData ^ data;
//    key2 = key2 ^ new_key;
//    crypto.setKey(::getKey(key2,q.value(0).toByteArray().data()));
//    m_title = crypto.decryptToString(q.value(8).toByteArray());

//    //    data = data/10*10+1;

//    ver = q.value(10).toInt();
////    ver_patch = q.value(11).toInt();
//    author_key = q.value(12).toInt();
//    id = q.value(13).toInt();
//    if(author_key == (uint)ext_storage->userKey())
//        state = 4;
//    count_users = q.value(14).toUInt();

//    sql = QString("select tag_id from tasks_tags where task_id=%1").arg(id);
//    if(!q.exec(sql))
//    {
//        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//        return;
//    }
//    while(q.next())
//    {
//        lKeyTags.append(q.value(0).toInt());
//    }

//    //        sql = QString("select user_key from notes_users where id_note=%1").arg(id);
//    //        if(!q.exec(sql))
//    //        {
//    //            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//    //            return;
//    //        }
//    //        while(q.next())
//    //        {
//    //            lKeyUsers.append(q.value(0).toUInt());
//    //        }

//}

QVariant Task::signatures()
{
    QList<QObject*> dataList;
//    QMap<quint32,TaskProperty*> mSignatures;
//    foreach (TaskProperty *task_prop, lProperties) {
//        if(task_prop->type == TaskProperty::SIGNED)
//            mSignatures.insert(task_prop->uiData,task_prop);
//    }
    foreach (TaskProperty *task_prop, m_signatures) {
        dataList.append(task_prop);
    }
    //    qSort(dataList.begin(), dataList.end(), caseTagMoreThan);
    return QVariant::fromValue(dataList);
}

QVariant Task::replies()
{
    QList<QObject*> dataList;
    foreach (Reply *reply, m_replies) {
        dataList.append(reply);
    }
    return QVariant::fromValue(dataList);
}

QVariant Task::chats()
{
    QList<QObject*> dataList;
    foreach (TaskProperty *task_prop, m_chats) {
        dataList.append(task_prop);
    }
    //    qSort(dataList.begin(), dataList.end(), caseTagMoreThan);
    return QVariant::fromValue(dataList);
}



QVariant Task::attachments(int authorId)
{
//        m_attachmentModel.setParent(this);
    return m_attachmentModel.getAttachments(authorId);
}

QList<QObject*> Task::getAttachAndLinks(int authorId)
{
    QList<QObject*> dataList;
    loadLinks(true);
    for(int i = 0; i < m_attachmentModel.l_attachments.count(); i++)
    {
        Attachment *attachment = m_attachmentModel.l_attachments[i];
        bool isForReply = false;
        bool isAuthorTaskReply = false;
        for(int j = 0; j < m_replies.count(); j++)
        {
            if(m_replies[j]->author() == attachment->authorId())
            {
                if((attachment->authorId() == authorId && authorId > -1) || authorId < 0)
                    isForReply = true;
                if((this->m_author.first == attachment->authorId() || this->creator_id == attachment->authorId())
                    && (authorId < 0 || attachment->authorId() == authorId || attachment->authorId() == authorId ) )
                       isAuthorTaskReply = true;
            }
        }
//        if((authorId < 0 && !isForReply) || (authorId >= 0 && isForReply) )
        if((authorId < 0 && (!isForReply && !isAuthorTaskReply)) || (authorId > -1 && (isForReply || isAuthorTaskReply)) )
        {
            attachment->setObjectName("attach");
            dataList.append(attachment);
        }
    }

    for(int i = 0; i < m_inputLinks.count(); i++)
    {
        Task *task = m_inputLinks[i];
        bool isForReply = false;
        bool isAuthorTaskReply = false;
        for(int j = 0; j < m_replies.count(); j++)
        {
            if(m_replies[j]->author() == task->m_author.first)
            {
//                if((task->m_author.first == authorId && authorId > -1) || authorId < 0)
                if((task->m_author.first == authorId && authorId > -1))
                   isForReply = true;
                if((this->m_author.first == task->m_author.first || this->creator_id == task->creator_id )
                    && (authorId < 0 || task->m_author.first == authorId || task->creator_id == authorId ) )
                    isForReply = false;
//                       isAuthorTaskReply = true;
//                break;
            }
        }
        if((authorId < 0 && (!isForReply && !isAuthorTaskReply)) || (authorId > -1 && (isForReply || isAuthorTaskReply)) )
        {
            task->setObjectName("link_in");
            dataList.append(task);
        }
    }

    for(int i = 0; i < m_outputLinks.count(); i++)
    {
        Task *task = m_outputLinks[i];
        bool isForReply = false;
        bool isAuthorTaskReply = false;
        for(int j = 0; j < m_replies.count(); j++)
        {
            if(m_replies[j]->author() == task->m_author.first)
            {
//                if((task->m_author.first == authorId && authorId > -1) || authorId < 0)
//                    isForReply = true;
//                break;
                if((task->m_author.first == authorId && authorId > -1) || authorId < 0)
                    isForReply = true;
                if((this->m_author.first == task->m_author.first || this->creator_id == task->m_author.first)
                    && (authorId < 0 || task->m_author.first == authorId || task->m_author.first == authorId ) )
                       isAuthorTaskReply = true;
            }
        }
//        if((authorId < 0 && !isForReply) || (authorId >= 0 && isForReply) )
        if((authorId < 0 && (!isForReply && !isAuthorTaskReply)) || (authorId > -1 && (isForReply || isAuthorTaskReply)) )
        {
            if(task->isDocument())
            {
                task->setObjectName("link_out");
                dataList.append(task);
            }
        }
    }
//    return QVariant::fromValue(dataList);
    return dataList;
}


QVariant Task::inputDocs()
{
    QList<QObject*> dataList;
    loadLinks(true);
    for(int i = 0; i < m_inputLinks.count(); i++)
    {
        dataList.append(m_inputLinks[i]);
    }
//    LinkProp *curLink;

//    if(!m_doc->m_attrProp)
//        return QVariant();
//    loadLinks(true);
//    for(int i = 0; i < m_doc->m_attrProp->m_lLinks.count(); i++) {
//        curLink = m_doc->m_attrProp->m_lLinks.at(i);
////        if(curLink->m_extTask && curLink->typeLink() == LinkProp::LINK_INPUT && curLink->m_extTask->m_repeatModels.activeCategory() == 0)
//        if(curLink->m_extTask && curLink->typeLink() == LinkProp::LINK_INPUT)
//        {
////            if(curLink->m_extTask->title() == "" && curLink->m_extTask->offset_history > 0)
////            {
////                quint32 lastOffset;
////                qint16 lastKey;
////                quint16 lastVer;
////                ext_storage->loadTasksBody(curLink->m_extTask, lastOffset, lastKey, lastVer);
////                ext_storage->createTitle(curLink->m_extTask);
////            }
//            dataList.append(curLink->m_extTask);
//        }
//    }
//    //    qSort(dataList.begin(), dataList.end(), caseTagMoreThan);
    return QVariant::fromValue(dataList);
    
}

QVariant Task::outputDocs()
{
    QList<QObject*> dataList;
    loadLinks(true);
    for(int i = 0; i < m_outputLinks.count(); i++)
    {
        if(m_outputLinks[i]->isDocument())
            dataList.append(m_outputLinks[i]);
    }
//    LinkProp *curLink;
//    if(!m_doc->m_attrProp)
//        return QVariant();
//    loadLinks(true);
//    for(int i = 0; i < m_doc->m_attrProp->m_lLinks.count(); i++) {
//        curLink = m_doc->m_attrProp->m_lLinks.at(i);
////        if(curLink->m_extTask && curLink->typeLink() == LinkProp::LINK_OUTPUT && curLink->m_extTask->m_repeatModels.activeCategory() == 0)
//        if(curLink->m_extTask && curLink->typeLink() == LinkProp::LINK_OUTPUT)
//            dataList.append(curLink->m_extTask);
//    }
//    //    qSort(dataList.begin(), dataList.end(), caseTagMoreThan);
    return QVariant::fromValue(dataList);
}

QVariant Task::subtasks()
{
    QList<QObject*> dataList;
    loadLinks(true);
    for(int i = 0; i < m_outputLinks.count(); i++)
    {
        if(!m_outputLinks[i]->isDocument())
        {
            dataList.append(m_outputLinks[i]);
        }
    }
//    LinkProp *curLink;

//    if(!m_doc->m_attrProp)
//        return QVariant();
//    loadLinks(true);
//    for(int i = 0; i < m_doc->m_attrProp->m_lLinks.count(); i++) {
//        curLink = m_doc->m_attrProp->m_lLinks.at(i);
////        if(curLink->m_extTask && curLink->typeLink() == LinkProp::LINK_OUTPUT && curLink->m_extTask->m_repeatModels.activeCategory() > 0)
//          if(curLink->m_extTask && curLink->typeLink() == LinkProp::LINK_OUTPUT_TASK)
//              dataList.append(curLink->m_extTask);
//    }
//    //    qSort(dataList.begin(), dataList.end(), caseTagMoreThan);
    return QVariant::fromValue(dataList);
}

void Task::loadSubtasks()
{
    loadLinks(true);
    bool isChangeReplies = false;
    for(int i = 0; i < m_outputLinks.count(); i++)
    {
        if(!m_outputLinks[i]->isDocument())
        {
            if(m_outputLinks[i]->m_author.first < 0)
            {
                emit getFullTask(m_outputLinks[i]);
            }
            else
            {
                Task *task = m_outputLinks.takeAt(i);
                task->dt_current = task->dt_start;
                task->dt_end_interval = task->dt_end;
                Reply *reply = nullptr;
                for(int j = 0; j < m_replies.count(); j++)
                {
                    if(task->m_author.first == m_replies[j]->m_author.first)
                    {
                        reply = m_replies[j];
                        break;
                    }
                }
                if(!reply)
                {
                    reply = new Reply();
                    reply->m_author = task->m_author;
                    m_replies.append(reply);
                }
                reply->m_outTasks.append(task);
                reply->subtasksChanged();
                i--;
                isChangeReplies = true;
            }
        }
    }
    if(isChangeReplies)
        emit repliesChanged();

}

void Task::copyBaseData(Task *source)
{
    creator_id = source->creator_id;
    key = source->key;
    offset_history = source->offset_history;
    offset_model = source->offset_model;
    offset_tags = source->offset_tags;
    offset_title = source->offset_title;
//    note.dt_title_modify = new_note->dt_title_modify;
    prev_key = source->prev_key;
    prev_ver = source->prev_ver;
    patch_key = source->patch_key;
    ver_patch = source->ver_patch;
    ver = source->ver;
    data = source->data;
    type = source->type;
    id = source->id;
    m_title = source->m_title;
    m_regNum = source->m_regNum;
    dt_start = source->dt_start;
    dt_end = source->dt_end;
    dt_end_interval = source->dt_end_interval;
    dt_current = source->dt_current;
    dt_modify = source->dt_modify;
    dt_history = source->dt_history;
    dt_prev = source->dt_prev;
    dt_next = source->dt_next;
    flag = source->flag;
    isEditable = source->isEditable;
    is_modify = source->is_modify;
    m_author = source->m_author;
    m_index = source->m_index;
    m_from = source->m_from;
    m_fromTime = source->m_fromTime;
    m_registryTime = source->m_registryTime;
    m_isHaveReply = source->m_isHaveReply;
    m_isLocalTransfer = source->m_isLocalTransfer;
    m_isOverdue = source->m_isOverdue;
    m_interval = source->m_interval;
    m_notify = source->m_notify;
    m_workState = source->m_workState;
    security = source->security;

}

void Task::setTitle(QString source)
{
    m_title = source;
    emit titleChanged();
}



//QDataStream & operator << (QDataStream &dataStream, const Note* src)
//{
//    qint32 size = src->lBlocks.size();
//    qint8 count_prop = 4;
//    dataStream << count_prop << src->key << src->Title << src->dt_modify << src->lTags << size;
//    for(int i = 0; i < size; i++)
//    {
//        dataStream << src->lBlocks[i]->type << src->lBlocks[i]->style;
////        dataStream << lBlocks[i]->page << lBlocks[i]->width << lBlocks[i]->height;
//        switch (src->lBlocks[i]->type) {
//        case TEXT:
//            dataStream << static_cast<TextBlock*>(src->lBlocks[i])->text;
//            break;
//        default:
//            break;
//        }

//    }
//    size = src->lParagraph.size();
//    dataStream << size;
//    for(int i = 0; i < size; i++)
//    {
//        dataStream << *src->lParagraph[i];
//    }
//    return dataStream;
//}

//QDataStream & operator>>(QDataStream &dataStream, Note* src)
//{
//    qint32 size;
//    qint8 type;
//    qint8 style;
//    Block *curBlock;
//    Paragraph *curParagraph;
//    qint8 count_prop;
//    if(!src)
//        src = new Note();
//    dataStream >> count_prop >> src->key >> src->Title >> src->dt_modify >> src->lTags >> size;
//    for(int i = 0; i < size; i++)
//    {
////        dataStream << lBlocks[i]->type; << lBlocks[i]->style;
//        dataStream >> type >> style;
//        switch (type) {
//        case TEXT:
//            curBlock = new TextBlock();
//            dataStream >> static_cast<TextBlock*>(curBlock)->text;
//            break;
//        default:
//            curBlock = new Block();
//            break;
//        }
//        curBlock->type = type; curBlock->style = style;
//        src->lBlocks.append(curBlock);

//    }

//    dataStream >> size;
//    for(int i = 0; i < size; i++)
//    {
//        curParagraph = new Paragraph;
//        dataStream >> *curParagraph;
//        src->lParagraph.append(curParagraph);
//    }
//    return dataStream;
//}
bool SubTask::isOverdue()
{
   return m_workState != CLOSED && dt_task.date() < QDate::currentDate();
}

void SubTask::setDeleted()
{
    flag = DELETED;
//    is_modify = true;
//    isEditable = true;
    ext_storage->saveFlag(this);

}

void SubTask::setWorkState(bool source)
{

//    if(source)
//    {
//        flag = CLOSED;
//    }
//    else
//        flag = ACTIVE;
////    is_modify = true;
////    isEditable = true;
//    ext_storage->saveFlag(this);
//    emit workStateChanged();
}

//NotesFilterModel::NotesFilterModel(QObject *parent)
//    : QAbstractListModel(parent)
//{
//    m_rowCount = 0;
//    m_activeTask = 0;
//    m_selectedFilter = 0;
//    isFiltering = false;
//    m_viewNewNote = false;
//}

//int NotesFilterModel::rowCount(const QModelIndex &parent) const
//{
//    Q_UNUSED(parent);
////    return 10000;
//    if(m_viewNewNote)
//        return m_rowCount+1;
//    else
//        return m_rowCount;
//    //    return m_filterNotes.count();
//}

//QVariant NotesFilterModel::data(const QModelIndex &index, int role) const
//{
//    //    if (index.row() < 0 || index.row() >= m_filterNotes.count())
//    //        return QVariant();

//    //    const Note *cur_note = m_filterNotes[index.row()];
//    //    if (role == TitleRole)
//    //        return cur_note->Title;
//    //    else if (role == TimeRole)
//    //        return cur_note->dt_modify;
//    //    return QVariant();

//    if (index.row() < 0 || index.row() >= m_rowCount + (m_viewNewNote > 0 ? 1 : 0))
//        return QVariant();
//    QSqlQuery q;
//    QString sql;
////    quint64 key = Q_UINT64_C(0x3847afdb4cb9e20d);
//    quint64 key = Q_UINT64_C(0x4947afdb4cb9e20d);

//    quint64 new_key;
//    SimpleCrypt crypto;
//    quint8 state;
//    int id;
//    if(m_viewNewNote) index.row() > 0 ? id = getId(index.row() -1) : id = -1;
//    else id = getId(index.row());
//    sql = QString("select title,time_modify,task_key,data,state,flag from tasks where id=%1").arg(id);
//    if(id != -1)
//    {
//        if(!q.exec(sql))
//        {
//            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//            return QVariant();
//        }
//        q.next();
//        if (role == TitleRole)
//        {
////            new_key = keyData ^ q.value(3).toLongLong();
////            key = key ^ new_key;
//            new_key = q.value(3).toLongLong();
//            state = new_key - (new_key/10*10+1);
//            new_key -= state;
//            new_key = keyData ^ new_key;
//            key = key ^ new_key;

//            crypto.setKey(getKey(key,q.value(2).toByteArray().data()));
//            return crypto.decryptToString(q.value(0).toByteArray());
//        }
//        else if (role == TimeRole)
//            return QDateTime::fromSecsSinceEpoch(q.value(1).toUInt());
//        else if(role == StateRole)
//        {
//            return q.value(4).toInt();
//        }
//        else if(role == FlagRole)
//            return q.value(5).toInt();
//    }
//    else
//    {
//        if (role == TitleRole)
//        {
//            return m_activeTask->m_title;
//        }
//        else if (role == TimeRole)
//            return m_activeTask->dt_modify;
//        else if (role == StateRole)
//            return m_activeTask->state;
//        else if (role == FlagRole)
//            return m_activeTask->flag;

//    }
//    return QVariant();
//}

//int NotesFilterModel::getId(int row) const
//{
//    int id = 0;
//    QString sql;
//    QSqlQuery q;
//    QString sql_tags;
//    if(row < m_cursor || row > m_cursor + 1000 || m_idNotes.count() == 0)
//    {
//        m_cursor = row - 500;
//        if(m_cursor < 0) m_cursor = 0;
//        sql = QString("select id from tasks where flag != 3 and repeat_state = 0 order by time_modify desc limit 1000 offset %1").arg(m_cursor);
//        if(isFiltering)
//        {
//            for(int i = 0;i < m_selectedFilter->count(); i++)
//            {
//                if(i == 0)
//                    sql_tags += QString("%1").arg(m_selectedFilter->at(i));
//                else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
//            }
//            sql = QString("select id from tasks where repeat_state = 0 and exists ( select task_id from tasks_tags where tag_id in ( %0 ) and task_id = tasks.id )  order by time_modify desc limit 1000 offset %1 ")
//                    .arg(sql_tags).arg(m_cursor);

//            //            sql = QString("select id from notes_tags,notes where id_note = id and (%0) order by time_modify desc limit 1000 offset %1 ")
//            //                    .arg(sql_tags).arg(m_cursor);
//        }
//        if(!q.exec(sql))
//        {
//            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//            return 0;
//        }
//        m_idNotes.clear();
//        while(q.next())
//        {
//            id = q.value(0).toInt();
//            //            if(!m_idNotes.contains(id))
//            m_idNotes.append(id);
//        }

//    }

//    if(row - m_cursor > m_idNotes.count()-1)
//        id = 0;
//    else id = m_idNotes[row - m_cursor];

//    return id;
//}

//Note *NotesFilterModel::getNote(int index, Note *source)
//{
////    QString sql;
////    QSqlQuery q;
////    SimpleCrypt crypto;
////    quint64 key = Q_UINT64_C(0x3847afdb4cb9e20d);
////    quint64 new_key;
//    if(m_viewNewNote && source)
//    {
//        source->m_title = m_activeTask->m_title;
//        source->id = m_activeTask->id;
//        source->lKeyTags = m_activeTask->lKeyTags;
//        source->offset_history = m_activeTask->offset_history;
//        return source;
//    }
//    int id = 0;
//    int row = index - m_cursor;
//    Note *res = NULL;
//    if(row >= 0 && row < m_idNotes.count() )
//    {

//        if(!source)
//        {
//            res = m_activeTask;
//            m_activeTask = new Note();
//            delete res;
//            res = m_activeTask;
//        }
//        else res = source;

//        id = m_idNotes[row];
//        res->id = id;
//        res->getNote();

//    }
//    return res;
//}

//Note *NotesFilterModel::getNoteById(int idNote)
//{
//    delete m_activeTask;
//    m_activeTask = new Note();
//    m_activeTask->id = idNote;
//    m_activeTask->getNote();
//    return m_activeTask;
//}

//Note *NotesFilterModel::addNote()
//{
//    delete m_activeTask;
//    m_activeTask = new Note();
//    m_activeTask->dt_history = m_activeTask->dt_modify = QDateTime::currentDateTime();
//    m_viewNewNote = true;
//    m_activeId = -1;
//    return m_activeTask;

//}

//void NotesFilterModel::refresh()
//{
//    QSqlQuery q;
//    QString sql = "select count(id) from tasks where flag != 3 and repeat_state = 0 ";
//    QString sql_tags;

//    m_cursor = 0;

//    if(isFiltering)
//    {
//        for(int i = 0;i < m_selectedFilter->count(); i++)
//        {
//            if(i == 0)
//                sql_tags += QString("%1").arg(m_selectedFilter->at(i));
//            else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
//        }
//        //        q.prepare(QString("select :id from notes_tags,notes where id_note = id and (%0) order by time_modify desc limit 1000").arg(sql_tags));
//        //        sql = QString("select count(id) from notes_tags,notes where id_note = id and (%0) ").arg(sql_tags);
//        sql = QString("select count(id) from tasks where flag != 3 and repeat_state = 0 and exists ( select task_id from tasks_tags where tag_id in ( %0 ) and task_id = tasks.id ) ").arg(sql_tags);

//    }
//    //    else sql = "select count(id) from notes";
//    //    else q.prepare("select count(id) from notes order by time_modify desc limit 1000");
//    //    q.bindValue(":id","count(*)");

//    if(!q.exec(sql))
//    {
//        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
////        qDebug() << sql;
//        return;
//    }
//    q.next();
//    m_rowCount = q.value(0).toInt();
//    m_idNotes.clear();

//    //    q.bindValue(":id","id");

//    //    if(!q.exec())
//    //    {
//    //        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//    //        return;
//    //    }
//    //    while(q.next())
//    //    {
//    //        m_idNotes.append(q.value(0).toInt());
//    //    }
//    beginResetModel();
//    endResetModel();
//}

//int NotesFilterModel::indexOf(Note *note)
//{
//    //    return m_filterNotes.indexOf(note);
//    if(!note)
//        return 0;
//    if(m_activeId == -1 && m_viewNewNote)
//        return 0;
//    int res = m_idNotes.indexOf(note->id);
////    if(res == -1)
//    return res;
//}

////Note *NotesFilterModel::getNote(int index)
////{
////    if(index >=0 && index < m_filterNotes.count())
////    {
////        return m_filterNotes.at(index);
////    }
////    else
////        return 0;
////}

//QHash<int, QByteArray> NotesFilterModel::roleNames() const
//{
//    QHash<int, QByteArray> roles;
//    roles[TitleRole] = "title";
//    roles[TimeRole] = "time";
//    roles[StateRole] = "stateNote";
//    roles[FlagRole] = "flag";
//    return roles;
//}




//Block *Block::makeCopy()
//{
//    Block *res = new Block();
//    copyData(res);
//    return res;
//}

//void Block::copyData(Block *result)







Task::Task(QObject *parent) :
    QObject(parent),
    m_repeatModels(this),
    m_notifyModel(this),
    m_attachmentModel(this),
    m_docAttributes(this)
{
    //    QUuid ful_key = GenerateGUID();
    //    key = *((quint32*)ful_key.data4);
//    key = GenerateGUID();
//    QTime time;
    id = -1;
    offset_history = 0;
    offset_model = 0;
    offset_tags = 0;
    offset_title = 0;
    flag = 0;
    security = Task::FULL;
//    key =  QUuid::createUuid(); //GenerateGUID();
////    key.data1 = ext_storage->myId();
////    QUuid ful_key = GenerateGUID();
//    data = *((quint64*)key.data4);
//    data = data/10*10+1;
    //  data = keyData ^ data;
    isEditable = false;
    is_modify = false;
    ver = 0;
    ver_patch = prev_ver = last_ver = 0;
    prevTask = 0;
    type = NOTE;
    patch_key = prev_key = 0;
    count_users = 1;
    creator_id = 0;

    is_modify = 0;
    m_notify = 0;
    flag = 0;
//    m_repeat = 0;
    m_interval = 0;
    type = TASK;
    m_transfer_status = TransferTime::NO_TRANSFER;
    m_transferTime = NULL;
//    m_lTransferTimes = NULL;
//    dt_close = QDateTime::fromSecsSinceEpoch(0);
//    dt_start = QDateTime::currentDateTime();
//    time = dt_start.time();
//    time.setHMS(time.hour(),0,0);
//    dt_start.setTime(time);
//    dt_start = dt_end = dt_current = dt_start.addSecs(60);
    m_isLocalTransfer = false;
    m_isOverdue = false;
//    m_assigned = -1;
    m_workState = false;
    m_isHaveReply = false;
//    m_repeatModels.active_category = RepeatModels::NO_REPEAT;
    m_responsible = m_author = QPair<qint32,qint32>(-1,-1);
    m_from = -1;
    m_fromTime = 0;
    m_registryTime = 0;
    m_newChats = 0;
    m_doc = new Document();
    m_index = -1;
    baseTask = NULL;

}

Task::~Task()
{
//    qDebug() << __FILE__ << __LINE__ << "Delete task:" << id;
    try
    {
        clear();
        delete m_doc;
    }
    catch(...)
    {
        qDebug() << "Error for del";
    }
}

int Task::interval()
{
//    int res;
//    m_interval = dt_end.toSecsSinceEpoch() - dt_start.toSecsSinceEpoch();
    return m_interval;
}

void Task::setInterval(int source)
{
//    dt_end = dt_start;
//    dt_end = dt_end.addSecs(source);
//    emit endTimeChanged();
    m_interval = source;
    emit intervalChanged();
}

void Task::setWorkState(int source, bool saved)
{
    if(m_workState == source)
        return;
    m_workState = source;
    ext_storage->saveIsClosed(this, saved);
//    if(source)
//    {
//        dt_close = QDateTime::currentDateTime();
//    }
//    else
//    {
//        dt_close = QDateTime::fromSecsSinceEpoch(0);
//    }
    emit workStateChanged();

}

void Task::setIsReading(bool source)
{
    m_isReading = source;
//    if(source && id > 0)
//    {
//        EjDb *ejdb = EjDb::Instance();
//        ejdb->saveTask(this);
//        ejdb->setIsSendReading(id - 1, true);
////        ejdb->setIsForSending(id - 1, true);
//        ejdb->saveDocuments();
//    }
    emit isReadingChanged();
}

bool Task::isAllDay()
{
    bool res = true;
    int d = (dt_end_interval.time().msecsSinceStartOfDay() -  dt_current.time().msecsSinceStartOfDay())/1000;
//    if(d != (23*3600 + 59*60 + 59))
//            res = false;
    if(d < (23*3600 + 55*60))
        res = false;
    return res;
}

void Task::setIsAllDay(bool source)
{
//    if(source)
//    {
//        if(m_repeatModels.active_category == RepeatModels::NO_REPEAT)
//        {
//            dt_start.setTime(QTime(0,0,0));
//            dt_end.setTime(QTime(23,59,59));
//            dt_current = dt_start;
//            dt_end_interval = dt_end;
//            m_interval = dt_end.toSecsSinceEpoch() - dt_start.toSecsSinceEpoch();
//        }
//        else
//        {
//            dt_current.setTime(QTime(0,0,0));
//            dt_end_interval.setTime(QTime(23,59,59));
//            m_interval = dt_end_interval.toSecsSinceEpoch() - dt_current.toSecsSinceEpoch();
//        }
//    }
//    else
//    {
////        dt_start.setTime(QTime(0,0,0));
////        dt_end = dt_current = dt_end_interval = dt_start;
////        m_interval = 0;
//    }
//    emit startTimeChanged();
//    emit endTimeChanged();
//    emit currentTimeChanged();
    //    emit isAllDayChanged();
}

bool Task::isOverdue()
{
    bool isOverdue = m_workState < WS_FINISH && dt_end_interval < QDateTime::currentDateTime();

    return  isOverdue;
}

void Task::setIsHaveReply(bool source)
{
    m_isHaveReply = source;
    emit isHaveReplyChanged();
}

void Task::setStartTime(QDateTime source)
{
    //    int len = (dt_end.toMSecsSinceEpoch() - dt_start.toMSecsSinceEpoch()) / 1000;
    if(dt_start == source)
        return;
    dt_start = source;
    //    if(m_interval > -1)
    //        setInterval(m_interval);
    if(dt_end < dt_start)
    {
        dt_end = dt_start;
        emit endTimeChanged();
    }
//    m_interval = dt_end.toSecsSinceEpoch() - dt_start.toSecsSinceEpoch();

    emit startTimeChanged();
    emit isAllDayChanged();
    emit intervalChanged();
}

void Task::setEndTime(QDateTime source)
{
    if(dt_end == source)
        return;
    dt_end = source;
    if(dt_end < dt_start)
    {
//        dt_end = dt_start;
        dt_start = dt_end;
        emit startTimeChanged();
    }
//    m_interval = dt_end.toSecsSinceEpoch() - dt_start.toSecsSinceEpoch();
    emit endTimeChanged();
    emit isAllDayChanged();
    emit intervalChanged();
}

//QVariant Task::notes()
//{
////    QList<QObject*> dataList;
////    foreach (Note *note, m_lNotes) {
////        dataList.append(note);
////    }
////    //    qSort(dataList.begin(), dataList.end(), caseTagMoreThan);
////    return QVariant::fromValue(dataList);
//}

QVariant Task::getSubTasks(bool isAll)
{
    QList<QObject*> dataList;
    foreach (SubTask *subTask, m_lSubTasks) {
        if(isAll || (subTask->dt_task >= dt_current && subTask->dt_task <= dt_end_interval))
            dataList.append(subTask);
        else if(dt_current.date() == QDate::currentDate() && subTask->flag != CLOSED
                && subTask->dt_task.date() < QDate::currentDate())
            dataList.append(subTask);
    }
    //    qSort(dataList.begin(), dataList.end(), caseTagMoreThan);
    return QVariant::fromValue(dataList);

}

QVariant Task::getRepeatDates()
{
//    quint32 time;
//    QList<QObject*> dataList;
//    QDateTime *date_time;
//    QSqlQuery q;
//    QString sql = QString("select task_id, interval, state, is_closed, time_start from repeats where task_id = %1").arg(id);

//    if(!q.exec(sql))
//    {
//        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError() << sql;
//        return QVariant;
//    }
//    //    qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError() << sql;
//    while(q.next())
//    {
//        time = q.value("time_start").toUInt();
//        date_time = new QDateTime();
//        date_time->setTime_t(time);
//        dataList.append(date_time);
//    }
    //    return QVariant::fromValue(dataList);
    return QVariant();
}

int Task::countAgreed()
{
    int res = 0;
    for(int i = 0; i < m_signatures.count(); i++)
    {
        if(m_signatures.at(i)->m_status == TaskProperty::ON)
            res++;
    }
    return res;
}

SubTask* Task::addSubTask()
{
    SubTask *subTask = new SubTask();
    subTask->id = -1;
    subTask->dt_task = dt_current;
    subTask->isEditable = true;
    subTask->creator_id = creator_id;
    isEditable = true;
    m_lSubTasks.append(subTask);
    emit subTasksChanged();
    return subTask;
}

QVariant Task::getTransferAllTimes()
{
    QVariant res;
    QList<QObject*> lRes;
//    QList<TransferTime*> lTransferTimes;
    bool isAddCurrent = false;

//    if(m_transferTime.dt_start.isNull())
//    {
//        m_transferTime.dt_start =  m_transferTime.dt_start_new = curStartTime;
//        m_transferTime.dt_end = m_transferTime.dt_end_new = curStartTime.addSecs(interval);
//    }

    foreach (TransferTime *curTransfer, m_lTransferTimes) {
//        if(m_transferTime && m_transferTime->dt_start < curTransfer->dt_start && !isAddCurrent)
//        {
//           lRes.append(m_transferTime);
//           isAddCurrent = true;
//        }
        if(curTransfer->is_used && ( curTransfer->dt_end >= QDateTime::currentDateTime()
                || curTransfer->dt_end_new >= QDateTime::currentDateTime()) )
            lRes.append(curTransfer);
    }
//    if(!isAddCurrent && m_transferTime)
//        lRes.append(m_transferTime);

    res.setValue(lRes);

    return res;
}

QVariant Task::getTransferTime()
{
    QVariant res;
    QList<QObject*> lRes;
//    if(m_transferTime.dt_start.isNull())
//    {
//        m_transferTime.dt_start =  m_transferTime.dt_start_new = curStartTime;
//        m_transferTime.dt_end = m_transferTime.dt_end_new = curStartTime.addSecs(interval);
//    }
    lRes.append(m_transferTime);
    res.setValue(lRes);

    return res;
}

bool Task::isOneDay()
{
    qDebug() << __FILE__ << __LINE__ << dt_current << dt_end_interval;
    return dt_current.date() == dt_end_interval.date();
}

bool Task::isAllIntervalDay()
{
    bool res = true;
    QDateTime dt = dt_start;
    dt.setTime(dt_end.time());
    int d = (dt.time().msecsSinceStartOfDay() -  dt_start.time().msecsSinceStartOfDay())/1000;
//    if(d != (23*3600 + 59*60 + 59))
//            res = false;
    if(d < (23*3600 + 55*60))
        res = false;
    return res;
}

void Task::reload()
{
    quint32 offset;
    qint16 last_key;
    quint16 last_ver;
//    id = EjDb::Instance()->getTaskId(key);
//    EjDb::Instance()->loadTask(this);
    ext_storage->loadTasksBody(this,offset,last_key, last_ver);
    emit taskChanged();
//    emit subTasksChanged();
}

Document *Task::document()
{
    m_doc->setParent(this);
    return m_doc;
}

bool Task::attachFile(QString path)
{
    bool res = ext_storage->attachFile(this,path);
    emit attachmentsChanged();
    return res;

	//    property->uiData = task->dt_current.toSecsSinceEpoch();
}

bool Task::openAttach(int authorId, int index)
{
    bool res = false;
    QList<QObject*> attachments = getAttachAndLinks(authorId);
//    if(index > -1 && index < m_attachmentModel.l_attachments.count())
    if(index > -1 && index < attachments.count())
    {
        if(attachments[index]->objectName() == "attach")
        {
//        Attachment *attachment = m_attachmentModel.l_attachments.at(index);
            Attachment *attachment = qobject_cast<Attachment*>(attachments[index]);
            res = ext_storage->openAttach(attachment);
        }
    }
    return res;
}

bool Task::remAttach(int index)
{
    bool res = false;
    if(index > -1 && index < m_attachmentModel.l_attachments.count())
    {
        Attachment *attachment = m_attachmentModel.l_attachments.takeAt(index);
        res = ext_storage->remAttach(this, attachment);
    }
    return res;
}

bool Task::isContainsReplies()
{
    QList<QObject*> dataList;
    LinkProp *curLink;
    if(!m_doc->m_attrProp)
        return false;
    bool retVal = false;
//    loadLinks();
//    for(int i = 0; i < m_doc->m_attrProp->m_lLinks.count(); i++) {
//        curLink = m_doc->m_attrProp->m_lLinks.at(i);
////        if(curLink->m_extTask && curLink->typeLink() == LinkProp::LINK_OUTPUT && curLink->m_extTask->m_repeatModels.activeCategory() == 0)
//        if(curLink->m_extTask && curLink->typeLink() == LinkProp::LINK_OUTPUT)
//        {
//            if(!curLink->m_extTask->m_replies.isEmpty())
//                retVal = true;
//        }
//    }
    //    qSort(dataList.begin(), dataList.end(), caseTagMoreThan);
    return retVal;

}

QString Task::getWorkStateName(quint8 workState)
{
    QString retVal = "";
    switch(workState) {
    case WS_OPEN: retVal = tr("Open");
        break;
    case WS_ACCEPTED: retVal = tr("Accepted");
        break;
    case WS_IN_WORK: retVal = tr("In work");
        break;
    case WS_FINISH: retVal = tr("Finished");
        break;
    case WS_CLOSED: retVal = tr("Closed");
        break;
    case WS_REJECTED: retVal = tr("Rejected");
        break;
    case WS_DELETED: retVal = tr("Deleted");
        break;
    case WS_AGREEDING: retVal = tr("Agreeding");
        break;
    case WS_AGREED: retVal = tr("Agreed");
        break;
    }
    return retVal;

}

QString Task::getWorkStateName()
{
    if(isDocument() == true && m_workState != WS_AGREED && m_workState != WS_AGREEDING )
    {
        if(m_repeatModels.checkedEnabled())
            return tr("Control");
//        else return "";
    }
    return  getWorkStateName(static_cast<WorkState>(m_workState));
}

QColor Task::getWorkStateColor()
{
 //            color: !activeTask || (activeTask.workState < 2 && !activeTask.isOverdue)  ? "transparent" : activeTask.isOverdue ? "red" : "#bcbcbc"
//    bool isOverdue = m_workState < WS_FINISH && dt_end_interval < QDateTime::currentDateTime();
    return getWorkStateColor(m_workState);
}

QColor Task::getWorkStateColor(int workState)
{
    bool isOverdue = this->isOverdue();
    QColor retVal = QColor(Qt::transparent);
//    if(!isDocument())
//        retVal = (m_workState < WS_FINISH && !isOverdue)  ? QColor::fromRgb(0xbc,0xbc,0xbc) : isOverdue ? QColor(Qt::red) : m_workState == WS_FINISH ? ext_storage->getSettings()->baseColor : QColor::fromRgb(0xbc,0xbc,0xbc);
    retVal = (workState < WS_FINISH && !isOverdue)  ? QColor::fromRgb(0xac,0xac,0xac) : (workState < WS_FINISH && isOverdue) ? QColor(Qt::red) : workState == WS_FINISH ? ext_storage->getSettings()->baseColor : QColor::fromRgb(0xac,0xac,0xac);

    return retVal;
}

QColor Task::getWorkStateBorderColor()
{
    //            border.color: !activeTask ? "transparent" : activeTask.workState === 2 ? "#bcbcbc" : "red"
    QColor retVal = QColor(Qt::transparent);
    if(isDocument())
        retVal = QColor(Qt::red);
    else
        retVal = (m_workState == WS_CLOSED)  ? QColor::fromRgb(0xbc,0xbc,0xbc) : QColor(Qt::red);

    return retVal;
}

bool Task::isDocument()
{
    return m_repeatModels.activeCategory() == 0
            || m_workState == WS_AGREED || m_workState == WS_AGREEDING;   // NO_TIME
}

bool Task::isHaveAgreeding()
{
    return m_workState == WS_AGREED || m_workState == WS_AGREEDING;
}

bool Task::isSignResolution(int authorId)
{
    bool res = false;
    if(authorId == 0)
        res = true;
//    QList<QUuid> keys = ext_storage->getSettings()->mAuthorities.values(EJSettings::A_SIGN_RESOLUTION);
//    for(int i = 0; i < keys.count(); i++)
//    {
//        int authorityId = ext_storage->getUserId(keys[i]);
//        if(authorityId == authorId)
//        {
//            res = true;
//            break;
//        }
//    }
    return res;
}

bool Task::relpyEnabled()
{
    bool res = false;
    int signId = ext_storage->myId();
//    QList<QUuid> keys = ext_storage->getSettings()->mAuthorities.values(EJSettings::A_SIGN_RESOLUTION);
//    for(int i = 0; i < keys.count(); i++)
//    {
//        signId = ext_storage->getUserId(keys[i]);
//        break;
//    }
    for(int i = 0; i < m_replies.count(); i++)
    {
        if(signId == m_replies[i]->author())
        {
            res = true;
            break;
        }
    }
    return res;
}

QString Task::regNum() const
{
    return m_regNum;
}

void Task::setRegNum(QString regNum)
{
    if (m_regNum == regNum)
        return;

    m_regNum = regNum;
    emit regNumChanged();
}


//void Task::changeTimeSubTask(int index, QDateTime new_time)
//{
//   if(index > -1 && index < lSubTasks.count())
//   {
//        lSubTasks.at(index)->dt_task = new_time;
//   }
//}

QString Task::repeatTitle()
{
    QString res;
//    if(m_repeatModels)
    {
       res = m_repeatModels.repeatTitle(dt_current);
    }
    return res;
}



void Task::getTask()
{
	return;
    QString sql;
    QSqlQuery q;
    SimpleCrypt crypto;
    quint64 key = Q_UINT64_C(0x4947afdb4cb9e20d);
    quint64 new_key;
//    qint64 time_int;
    Task *res = this;
    Task *curTask;
    SubTask *curSubTask;
    QDateTime datetime;
    QByteArray ba_repeat_params, ba_transfer;

    bool b_insert;
//    QList<TransferTime*> lTransferTimes;
//    DaysParams *curDaysParams;
//    WeeksParams *curWeeksParams;

//    int row = index - m_cursor;
//    if(row >= 0 && row < m_idTasks.count() )
    if(id > -1)
    {
//        id = m_idTasks[row];

        sql = QString("select task_key, flag, is_modify, time_modify, time_start, time_end, repeat_state, repeat_params, notify_params, title,data, offset, ver, ver_patch, patch_key, time_history, user_id, transfer_times, users, chats from tasks where id=%1").arg(id);
//        sql = QString("select * from tasks where id=%1").arg(id);
        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//            return 0;
        }
        q.next();

//        QMapIterator<QDateTime, int> iter(m_bufferTasks);
//        for(int i = 0; i < index+1; i++)
//        {
//            if (iter.hasNext()) {
//                iter.next();
//            }
//        }
//        res->dt_current = res->dt_end = res->dt_prev = iter.key();
//        QList<QDateTime> ldateTimes = m_bufferTasks.keys(id);
//        row = ldateTimes.indexOf(res->dt_current);
//        if(row > 0)
//            res->dt_prev = ldateTimes[row - 1];
//        if(ldateTimes.count() > row + 1)
//            res->dt_next = ldateTimes[row + 1];


        res->key = QUuid::fromRfc4122(q.value(0).toByteArray());
        res->flag = q.value(1).toInt();
        res->is_modify = q.value(2).toInt();
		res->dt_modify = QDateTime::fromSecsSinceEpoch(q.value(3).toUInt());
//        time_int = q.value(4).toLongLong();
//        res->dt_start = res->dt_current = res->dt_prev = res->dt_next = QDateTime::fromMSecsSinceEpoch(time_int);
		res->dt_start = QDateTime::fromSecsSinceEpoch(q.value(4).toUInt());
		res->dt_end = QDateTime::fromSecsSinceEpoch(q.value(5).toUInt());
        res->dt_end_interval = res->dt_current;
        res->dt_end_interval = res->dt_end_interval.addSecs(res->m_interval);
        ba_repeat_params = q.value(7).toByteArray();
        res->m_repeatModels.parseParams(ba_repeat_params);
        ba_repeat_params = q.value(8).toByteArray();
        res->m_notifyModel.parseParams(ba_repeat_params);
//        res->m_interval = 0;

//        res->m_notify = q.value(8).toInt();

        //        res->m_index = index;

//        new_key = keyData ^ q.value(12).toLongLong();
//        key = key ^ new_key;
        data = q.value(10).toLongLong();
        security = data - (data/10*10 +1);
        data -= security;
    //    new_key = keyData ^ q.value(9).toLongLong();
        new_key = keyData ^ data;
        key = key ^ new_key;

        crypto.setKey(::getKey(key,q.value(0).toByteArray().data()));
        res->m_title = crypto.decryptToString(q.value(9).toByteArray());
//        res->data = q.value(12).toLongLong();
        res->offset_history = q.value(11).toInt();
        res->ver = q.value(12).toInt();
//        res->ver_patch = q.value(13).toInt();
//        res->patch_key = q.value(14).toInt();
		res->dt_history = QDateTime::fromSecsSinceEpoch(q.value(15).toUInt());
//        res->dt_close = QDateTime::fromSecsSinceEpoch(q.value(16).toUInt());
//        if(res->m_repeatModels.active_category > 0)
//            res->dt_close = QDateTime::fromSecsSinceEpoch(0);
        res->creator_id = q.value(16).toUInt();
        if(creator_id == (uint)ext_storage->myId())
            security = 4;
        ba_transfer = q.value(17).toByteArray();
        parseTransfer(ba_transfer,res->m_lTransferTimes);
        res->count_users = q.value(18).toUInt();
        res->m_newChats = q.value(19).toInt();
//        delete res->m_transferTime;
        res->m_transferTime = NULL;
            foreach (TransferTime *curTransferTime, res->m_lTransferTimes) {
                if(curTransferTime->dt_start_new == res->dt_current
						&& (int)(curTransferTime->dt_end_new.toSecsSinceEpoch()-curTransferTime->dt_start_new.toSecsSinceEpoch()) == res->m_interval)
                {
                    res->m_transfer_status = curTransferTime->m_status;
                    res->m_transferTime = curTransferTime;
                    curTransferTime->is_used = true;
                    break;
                }
            }
        if(!res->m_transferTime)
        {
            res->m_transferTime = new TransferTime(this);
            res->m_transferTime->dt_start =  res->m_transferTime->dt_start_new = res->dt_current;
            res->m_transferTime->dt_end = res->m_transferTime->dt_end_new = res->dt_current.addSecs(res->m_interval);
            res->m_transferTime->is_used = true;
            res->m_isLocalTransfer = true;

            b_insert = false;
            for(int i = 0; i < res->m_lTransferTimes.count(); i++)
            {
                if(res->m_transferTime->dt_start > res->m_lTransferTimes.at(i)->dt_start)
                {
                    res->m_lTransferTimes.insert(i,res->m_transferTime);
                    b_insert = true;
                    break;
                }
            }
            if(!b_insert)
                res->m_lTransferTimes.append(res->m_transferTime);
        }
		//        res->dt_end_interval = QDateTime::fromSecsSinceEpoch(q.value(19).toUInt());


        sql = QString("select tag_id from tasks_tags where task_id=%1 and flag != 3").arg(id);
        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//            return 0;
        }
        res->m_tags.clear();
        while(q.next())
        {
            res->m_tags.append(q.value(0).toUInt());
        }


//        sql = QString("select task_key from tasks_notes where task_id=%1").arg(id);
//        if(!q.exec(sql))
//        {
//            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
////            return 0;
//        }
//        while(q.next())
//        {
//            curTask = new Task();
//            curTask->key = QUuid::fromRfc4122(q.value(0).toByteArray());
//            curTask->getTask(false);
//            res->m_lNotes.append(curTask);
//            //            res->lKeyTags.append(q.value(0).toUInt());
//        }
        sql = QString("select id, patch_key, offset, flag, is_modify, time_modify, time_history,time_task, data, ver, user_id, ver_patch, subtask_key, users, chats from subtasks where task_id=%1 and flag != 3 order by time_task").arg(id);

        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
            return;
        }
        qint64 range_prev = res->dt_current.msecsTo(res->dt_prev) / 2;
        qint64 range_next = res->dt_current.msecsTo(res->dt_next) / 2;
        qint64 range_current;
        qDeleteAll(res->m_lSubTasks);
        res->m_lSubTasks.clear();

        while(q.next())
        {
			datetime = QDateTime::fromSecsSinceEpoch(q.value(7).toUInt());
            range_current = res->dt_current.msecsTo(datetime);
            if((range_prev == 0 && range_current <= 0) || (range_current >= range_prev && range_current <= range_next) ||
                    (range_next == 0 && range_current >= 0))
            {
            curSubTask = new SubTask(this);
            curSubTask->id = q.value(0).toUInt();
//            curSubTask->patch_key = q.value(1).toInt();
            curSubTask->offset_history = q.value(2).toInt();
            curSubTask->flag = q.value(3).toInt();
            curSubTask->is_modify = q.value(4).toInt();
			curSubTask->dt_modify = QDateTime::fromSecsSinceEpoch(q.value(5).toUInt());
			curSubTask->dt_history = QDateTime::fromSecsSinceEpoch(q.value(6).toUInt());
            curSubTask->dt_task = datetime;
            curSubTask->baseTask = this;

            curSubTask->data = q.value(8).toLongLong();
            curSubTask->security = curSubTask->data - (curSubTask->data/10*10 +1);
            curSubTask->data -= curSubTask->security;

            curSubTask->ver = q.value(9).toInt();
            curSubTask->creator_id = q.value(10).toInt();
//            curSubTask->dt_close = QDateTime::fromSecsSinceEpoch(q.value(11).toUInt());
//            curSubTask->ver_patch = q.value(12).toInt();
            curSubTask->key = QUuid::fromRfc4122(q.value(12).toByteArray());
            curSubTask->count_users = q.value(13).toInt();
            curSubTask->m_newChats = q.value(14).toInt();


            res->m_lSubTasks.append(curSubTask);
            }
        }



    }
//    return res;

}

void Task::parseTransfer(QByteArray &buffer, QList<TransferTime *> &lTransferTimes, QObject *parent)
{
//    QByteArray ba_transfer_times;
    QDataStream stream_transfer(&buffer,QIODevice::ReadOnly);
    TransferTime *curTransferTime;
    quint8 count, size8;
    quint32 time;
    stream_transfer.setVersion(QDataStream::Qt_4_5);
//    size8 = task->lTransferTimes.count();
    qDeleteAll(lTransferTimes);
    lTransferTimes.clear();
    stream_transfer >> count;
    for(int i = 0; i < count; i++)
    {
        stream_transfer >> size8;
        curTransferTime = new TransferTime(parent);
//        curTransferTime = new TransferTime(this);
        curTransferTime->m_status = (TransferTime::e_status)size8;
        stream_transfer >> time;
		curTransferTime->dt_start = QDateTime::fromSecsSinceEpoch(time);
        stream_transfer >> time;
		curTransferTime->dt_end = QDateTime::fromSecsSinceEpoch(time);
        stream_transfer >> time;
		curTransferTime->dt_start_new = QDateTime::fromSecsSinceEpoch(time);
        stream_transfer >> time;
		curTransferTime->dt_end_new = QDateTime::fromSecsSinceEpoch(time);
//        size8 = 0; // new parametrs;
        stream_transfer >> size8;
        lTransferTimes.append(curTransferTime);
    }

}

bool Task::getRepeatProperties()
{
    QSqlQuery q;
    QByteArray ba_repeat_params;
    QByteArray ba_transfer;
    QString sql;

    sql = QString("select repeat_params, transfer_times from tasks where id=%1").arg(id);
//        sql = QString("select * from tasks where id=%1").arg(id);
    if(!q.exec(sql))
    {
        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
            return false;
    }
    q.next();
//    m_repeatModels = new RepeatModels();
    ba_repeat_params = q.value(0).toByteArray();
    m_repeatModels.parseParams(ba_repeat_params);

    m_lTransferTimes.clear();
    ba_transfer = q.value(1).toByteArray();
    parseTransfer(ba_transfer,m_lTransferTimes);
    return true;
}

void Task::findLink(const QUuid &key, QList<LinkProp *> &lLinks)
{
    LinkProp *curLink;
    if(m_doc && m_doc->m_attrProp)
    {
        for(int i = 0; i < m_doc->m_attrProp->m_lLinks.count(); i++) {
            curLink = m_doc->m_attrProp->m_lLinks.at(i);
            if(curLink->keyUuid() == key)
            {
                lLinks.append(curLink);
            }
            if(curLink->m_extTask)
                curLink->m_extTask->findLink(key,lLinks);
        }
    }
}

void Task::findTaskLink(const QUuid &key, QList<Task *> &lLinks, QList<Reply *> &lReplies)
{
    Task *task;
    for(int i = 0; i < m_inputLinks.count(); i++)
    {
        task = m_inputLinks[i];
        if(task->key == key)
            lLinks.append(task);
        task->findTaskLink(key,lLinks,lReplies);
    }
    for(int i = 0; i < m_outputLinks.count(); i++)
    {
        task = m_outputLinks[i];
        if(task->key == key)
            lLinks.append(task);
        task->findTaskLink(key,lLinks, lReplies);
    }

    for(int i = 0; i < m_replies.count(); i++)
    {
        Reply *reply = m_replies[i];
        for(int j = 0; j < reply->m_outTasks.count(); j++)
        {
            task = reply->m_outTasks[j];
            if(task->key == key)
            {
                lLinks.append(task);
                lReplies.append(reply);
            }
            task->findTaskLink(key,lLinks, lReplies);
        }
    }
}



TasksFilterModel::TasksFilterModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_activeTask = 0;
    m_selectedFilter = 0;
    m_activeModelIndex = -1;
    m_viewMode = TO_DO;
    m_viewNewTask = false;
    isFiltering = false;
//    m_templateFilter.append(8);
    m_templateFilter.append(1);
    m_selectedFilter = &m_templateFilter;
    m_rowCount = 0;
    //   getId(0);
}

TasksFilterModel::~TasksFilterModel()
{
    delete m_activeTask;
    m_activeTask = NULL;
}

int TasksFilterModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    int count = m_rowCount;
//    QMapIterator<QDateTime, int> iter(m_bufferTasks);
//    while (iter.hasNext()) {
//        iter.next();
//        if(iter.key().date() <= QDate::currentDate().addDays(7))
//            count++;
//        else if(count < 70 && iter.key().date() <= QDate::currentDate().addDays(14))
//            count++;
//        else
//            break;

//        //                cout << i.key() << ": " << i.value() << endl;
//    }

    if(m_viewNewTask)
         count++;
//    qDebug() << "row count:" << count;
        return count;
}

QVariant TasksFilterModel::data(const QModelIndex &index, int role) const
{
    //    if (index.row() < 0 || index.row() >= m_filterTasks.count())
    //        return QVariant();
    //    if(index.row() >= 0 && index.row() < m_filterTasks.count())
    //    {
    //        const Task *cur_task = m_filterTasks[index.row()];
    //        if (role == TitleRole)
    //            return cur_task->title;
    //        else if (role == TimeRole)
    //            return cur_task->dt_start;
    //        else if (role == TimeDateRole)
    //            return QDateTime(cur_task->dt_start.date());
    //        else if (role == StatusRole)
    //            return cur_task->flag;
    //    }
    //    return QVariant();
//    if(role == TimeRole)
//        return QDateTime::currentDateTime();
//    else if(role == TitleRole)
//        return QVariant("Task 93");

    if (index.row() < 0 || index.row() >= m_rowCount + (m_viewNewTask ? 1:0))
        return QVariant();
    QSqlQuery q;
    QString sql;
    QString str, str2;
    QDateTime dt_start, dt_end;
    quint64 key = Q_UINT64_C(0x4947afdb4cb9e20d);
    quint64 new_key;
    SimpleCrypt crypto;
    quint8 state;
    QLocale locale = QLocale::system();
    int id = -1;
//    QMapIterator<QDateTime, int> iter(m_bufferTasks);
    int n;
    quint32 time;

//    if(m_viewNewTask) index.row() > 0 ? id = getId(index.row() -1) : id = -1;
//    else id = getId(index.row());
//    qDebug() << "TasksFilterModel data(...) role: " << role << " id: " << id;
//    qDebug() << __FILE__ << __LINE__ << "row:" << index.row() << "role:" << role;
    if(m_viewNewTask) index.row() > 0 ? n = index.row() -1 : n = -1;
    else n = index.row();
////    for(int i = 0; i < n+1; i++)
////    {
////        if (iter.hasNext()) {
////            iter.next();
////            //                cout << i.key() << ": " << i.value() << endl;
////        }
////    }

//    if(n > -1) id = m_lTasksParams.at(n).id;
    if(m_viewNewTask) index.row() > 0 ? id = getId(index.row() -1) : id = -1;
    else id = getId(index.row());
    n -= m_cursor;
    sql = QString("select title,time_start,task_key,flag,data,state, time_end, repeat_state, offset from tasks where id=%1").arg(id);
    if(role == TaskId)
        return id;
    if(id != -1 && id != 0)
    {
        if(role == IsInSearch)
        {
            if(m_searchString.isEmpty())
                return true;
        }
        if(role == TitleRole || role == StatusRole || role == SecurityRole || role == WorkState || role == TimeStringRole || role == IsInSearch)
        {

            if(!q.exec(sql))
            {
                qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
                return QVariant();
            }
            q.next();
        }
        //        dt_start = QDateTime::fromMSecsSinceEpoch(q.value(1).toLongLong());
        //        if(dt_start.date() < QDate::currentDate())
        //            dt_start.setDate(QDate::currentDate());
        if(role == IsInSearch)
        {
            bool res = false;
            Task task;
            quint32 offset;
            qint16 last_key;
            quint16 last_ver;
            Block *cur_block;
            task.id = id;
            task.offset_history = offset;
            task.getTask();
            ext_storage->loadTasksBody(&task, offset, last_key, last_ver);
//            return true;
            foreach (TaskProperty *property, task.m_docAttributes.m_Properties) {
                if(property->strData.contains(m_searchString, Qt::CaseInsensitive))
                {
                    res = true;
                    break;
                }
            }
            if(res == false)
            {
                for(int i = 0; i < task.m_doc->lBlocks->count(); i++)
                {
                    cur_block = task.m_doc->lBlocks->at(i);
                    if(cur_block->type == TEXT)
                    {
                        if(((TextBlock*)cur_block)->text.contains(m_searchString, Qt::CaseInsensitive))
                        {
                            res = true;
                            break;
                        }
                    }
                }
            }
            return res;
        }
        if (role == TitleRole)
        {
//            new_key = keyData ^ q.value(4).toLongLong();
//            key = key ^ new_key;
            new_key = q.value(4).toLongLong();
            state = new_key - (new_key/10*10+1);
            new_key -= state;
            new_key = keyData ^ new_key;
            key = key ^ new_key;

            crypto.setKey(getKey(key,q.value(2).toByteArray().data()));
            QByteArray res = q.value(0).toByteArray();
            QString resStr = crypto.decryptToString(res);
            return resStr;
        }

//        if(m_bufferTasks.count() > 0) dt_start = iter.key();
        if(n > -1)  dt_start =  m_lTasksParams.at(n).dt_current;
        if (role == TimeRole)
        {
            if(m_viewMode == LAST_MODIFY || m_viewMode == NOTES_AND_TASKS || m_viewMode == ONLY_NOTES)
                return dt_start;
            else
                return m_lTasksParams.at(n).dt_overdue;
        }
        else if (role == TimeDateRole)
			return dt_start;
        if (role == TimeStringRole)
        {
            str = "";
            if(q.value(7).toInt() > 0)
            {
				dt_start = QDateTime::fromSecsSinceEpoch(q.value(1).toUInt());
				dt_end = QDateTime::fromSecsSinceEpoch(q.value(6).toUInt());
                str2 = "dd MMM hh:mm";
                if(dt_start.date().year() != QDate::currentDate().year())
                    str2 = "dd MMM yyyy hh:mm";
                if(dt_start.date() == dt_end.date())
                {
                    if(dt_start == dt_end)
                    {
                        str = dt_start.toString(str2);
                    }
                    else
                        str = dt_start.toString(str2) +  " → " + dt_end.toString("hh:mm");
                }
                else
                   str = dt_start.toString(str2) +  " → " + dt_end.toString(str2);
            }
            return str;
        }
        else if (role == StatusRole)
            return q.value(3).toInt();
        else if (role == WorkState)
        {
//            time = q.value(5).toUInt();
//            return time > 1;
            if(n > -1)
                return  m_lTasksParams.at(n).work_state;
            else
                return 0;
        }
        else if (role == SecurityRole)
            return q.value(5).toInt();
        else if(role == SectionRole)
        {
//            qDebug() << "SectionRole";

            if(m_viewMode == TO_DO)
            {
                return dt_start.date();
//                if(dt_start.date() == QDate::currentDate() )
//                    str = tr("today");
//                else if(dt_start.date() == QDate::currentDate().addDays(1))
//                    str = tr("tomorrow");
//                else str = locale.dayName(dt_start.date().dayOfWeek());
//                return str;
            }
            if(m_viewMode == ALL_TASKS)
            {
                return dt_start.date();
//                return dt_start.toString("dd MMM yyyy");
//              return m_lTasksParams.at(n).dt_current.toString("dd MM yyyy");
            }
            else
                return QString();
//            if(dt_start.date() == QDate::currentDate() || m_lTasksParams.at(n).dt_overdue != dt_start)
//                return "today";
//            else if(dt_start.date() < QDate::currentDate())
//                return "before";
//            else if(dt_start.date() == QDate::currentDate().addDays(1))
//                return "tomorrow";
//            else if(dt_start.date() > QDate::currentDate().addDays(1) && dt_start.date() < QDate::currentDate().addDays(8))
//                return "nearest";
//            else return "sometime";

        }
        else if(role == IsRepeate)
        {
            bool res = false;
            if(n > -1 && m_lTasksParams.at(n).id == id)
            {
                res = true;
            }
            return res;
        }
        else if(role == TransferStatus)
        {
//            return m_lTasksParams.at(iter.value()).transfer_status;
            if(n > -1)
                return  m_lTasksParams.at(n).transfer_status;
            else
                return -1;
        }
        else if(role == IsOverdue)
        {
            if(n > -1 && m_viewMode != LAST_MODIFY && m_viewMode != NOTES_AND_TASKS && m_viewMode != ONLY_NOTES)
                return  m_lTasksParams.at(n).dt_overdue !=  m_lTasksParams.at(n).dt_current;
            else if(n > -1)
//                return false;
                return  m_lTasksParams.at(n).work_state < 2 && m_lTasksParams.at(n).dt_overdue <  QDateTime::currentDateTime();
            else return false;
        }
        else if(role == IsCloseVisible)
        {
            sql = QString("select repeat_params, repeat_state from tasks where id=%1").arg(id);
            if(!q.exec(sql))
            {
                qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
                return QVariant();
            }
            if(q.next())
            {
                RepeatModels repeatModels;
                QByteArray ba = q.value(0).toByteArray();
                repeatModels.parseParams(ba);
                return repeatModels.checkedEnabled() && (repeatModels.activeCategory() == RepeatModels::NO_REPEAT || repeatModels.activeCategory() == RepeatModels::NO_TIME);
            }
            return  QVariant();
        }
    }
    else if(id == 0)
    {
        if(role == SectionRole)
        {
            if(n > -1)  dt_start =  m_lTasksParams.at(n).dt_start;
            return dt_start.date();

        //            qDebug() << "SectionRole";
//            if(m_viewMode == TO_DO)
//            {
//                if(n > -1)  dt_start =  m_lTasksParams.at(n).dt_current;
//                if(dt_start.date() == QDate::currentDate() )
//                    str = tr("today");
//                else if(dt_start.date() == QDate::currentDate().addDays(1))
//                    str = tr("tomorrow");
//                else str = locale.dayName(dt_start.date().dayOfWeek());
//                return str;
//            }
        }
    }
    else if(m_activeTask)
    {
        if (role == TitleRole)
        {
            return m_activeTask->m_title;
        }
        else if (role == RegNumRole)
        {
            return m_activeTask->m_regNum;
        }
        else if (role == TimeRole)
            return m_activeTask->dt_start;
        else if (role == TimeDateRole)
			return QDateTime(m_activeTask->dt_start.date(), QTime());
        else if (role == StatusRole)
            return m_activeTask->flag;
        else if(role == TransferStatus)
        {
            return (int)m_activeTask->m_transfer_status;
        }
        else if (role == SecurityRole)
            return m_activeTask->security;
        else if (role == WorkState)
            return m_activeTask->m_workState == Task::CLOSED;
//            return m_activeTask->dt_close.toSecsSinceEpoch() > 1;


    }
    else if (role == TitleRole)
    {
        return "";
    }
    return QVariant();

}

void TasksFilterModel::refresh()
{
    if(m_viewMode == ALL_TASKS)
        return;
    QSqlQuery q;
    QString sql;
    QString sql_tags;
    QString sql_overdues;
    quint32 time;
	QDateTime dt_current(QDate::currentDate(),QTime());
	time = dt_current.toSecsSinceEpoch();



    m_cursor = 0;

    if(isFiltering)
    {
        for(int i = 0;i < m_selectedFilter->count(); i++)
        {
            if(i == 0)
                sql_tags += QString("%1").arg(m_selectedFilter->at(i));
            else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
        }
    }

    if(isFiltering && !m_selectedFilter->isEmpty())
    {
//        sql = QString("select * from tasks where time_end < %0 and repeat_state > 0 and exists ( select task_id from tasks_tags where tag_id in ( %1 ) and task_id = tasks.id )  order by time_start ")
//                                .arg(dt_current.toSecsSinceEpoch()).arg(sql_tags);
//        for(int i = 0;i < m_selectedFilter->count(); i++)
//        {
//            sql_tags += QString(" and tag_id = %1").arg(m_selectedFilter->at(i));
//        }

        sql = QString("select * from tasks where time_end < %0 and repeat_state > 0 and ( select count(task_id) from tasks_tags where tag_id in ( %1 ) and task_id = tasks.id ) = %2 order by time_start ")
				.arg(dt_current.toSecsSinceEpoch()).arg(sql_tags).arg(m_selectedFilter->count());
    }
    else
//        sql = QString("select * from tasks where time_end < %0 and time_close = 0 and repeat_state > 0 order by time_start ")
        sql = QString("select * from tasks where time_end < %0 and repeat_state > 0 order by time_start ")
				.arg(dt_current.toSecsSinceEpoch());
    if(!q.exec(sql))
    {
        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError() << sql;
        return;
    }
    while(q.next())
    {
        if(!sql_overdues.isEmpty())
            sql_overdues += ",";
        sql_overdues += q.value("id").toString();
    }


//    if(isFiltering && m_selectedFilter->count() > 0)
    if(isFiltering  && !m_selectedFilter->isEmpty())
    {

//        if(m_viewMode == ALL_TASKS)
//            sql = QString("select count() from repeats where exists ( select task_id from tasks_tags where tag_id in (%0) and task_id = repeats.task_id )  ")
//                    .arg(sql_tags);
//        else if(m_viewMode == TO_DO)
//            sql = QString("select count() from repeats where time_start >= %0 and time_start < %1 and exists ( select task_id from tasks_tags where tag_id in (%2) and task_id = repeats.task_id )  ")
//                    .arg(time).arg(dt_current.addDays(7).toSecsSinceEpoch()).arg(sql_tags);
//        else if(m_viewMode == NOTES_AND_TASKS)
//            sql = QString("select count(id) from tasks where flag != 3 and exists ( select task_id from tasks_tags where tag_id in (%1) and task_id = tasks.id )")
//                    .arg(sql_tags);
//        else
//            sql = QString("select count(id) from tasks where flag != 3 and repeat_state > 0 and exists ( select task_id from tasks_tags where tag_id in (%1) and task_id = tasks.id )")
//                    .arg(sql_tags);
        if(m_viewMode == ALL_TASKS)
            sql = QString("select count() from repeats where exists ( select task_id from tasks_tags where tag_id in (%0) and task_id = repeats.task_id )  ")
                    .arg(sql_tags);
        else if(m_viewMode == TO_DO)
            sql = QString("select count() from repeats where time_start >= %0 and time_start < %1 and exists ( select task_id from tasks_tags where tag_id in (%2) and task_id = repeats.task_id )  ")
					.arg(time).arg(dt_current.addDays(7).toSecsSinceEpoch()).arg(sql_tags);
        else if(m_viewMode == NOTES_AND_TASKS)
            sql = QString("select count(id) from tasks where flag != 3 and ( select count(task_id) from tasks_tags where tag_id in ( %1 ) and task_id = tasks.id ) = %2")
                    .arg(sql_tags).arg(m_selectedFilter->count());
        else
            sql = QString("select count(id) from tasks where flag != 3 and repeat_state = 0 and exists ( select task_id from tasks_tags where tag_id in (%1) and task_id = tasks.id )")
                    .arg(sql_tags);


        //            sql = QString("select id from notes_tags,notes where id_note = id and (%0) order by time_modify desc limit 1000 offset %1 ")
        //                    .arg(sql_tags).arg(m_cursor);
    }
    else if(m_viewMode == ALL_TASKS)
        sql = QString("select count() from repeats");
    else if(m_viewMode == TO_DO)
//        sql = QString("select count(distinct task_id) from repeats where case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end >= %1 ")
//                .arg(sql_overdues).arg(dt_current.toSecsSinceEpoch());
        sql = QString("select count() from repeats where time_start >= %0 and time_start < %1 or task_id in ( %2 )")
				.arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(7).toSecsSinceEpoch()).arg(sql_overdues);
    else if(m_viewMode == NOTES_AND_TASKS)
        sql = QString("select count(id) from tasks where flag != 3");
    else
//        sql = QString("select count(id) from tasks where flag != 3 and repeat_state > 0 ");
        sql = QString("select count(id) from tasks where flag != 3 and repeat_state = 0 ");

    if(m_viewMode != TO_DO)
    {
        qDebug() << __FILE__ << __LINE__ << "SQL " << sql;
        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
            return;
        }
        q.next();
        m_rowCount = q.value(0).toInt();
    }
    //    m_idTasks.clear();
    m_lTasksParams.clear();

    getId(0);

    if(m_viewMode == TO_DO)
    {
        m_rowCount = m_lTasksParams.count();
    }

    if(m_activeId == -1 && m_activeTask)
    {
        if (!q.prepare(QLatin1String("select id from tasks where task_key=:task_key and flag != 3" )))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
        }
        q.bindValue(":task_key",m_activeTask->key.toRfc4122());
        if(!q.exec())
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
        }
        else
        {
            if(q.next())
                m_activeId = q.value(0).toInt();
        }
    }

//    m_rowCount = m_lTasksParams.count();
    beginResetModel();
    endResetModel();
}

QVariantMap TasksFilterModel::getData(int row)
{
    //    if(index >=0 && index < m_filterTasks.count())
    //    {
    //        return m_filterTasks.at(index);
    //    }
    //    else
    //        return 0;
    //    QVariantMap get(int row) {
    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;
    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
        //cout << i.key() << ": " << i.value() << endl;
    }
    return res;
}





int TasksFilterModel::getId(int row) const
{
    if(m_viewMode == ALL_TASKS)
        return -1;
    int id = 0;
    QString sql;
    QSqlQuery q;
    QSqlQuery q2;
    QString sql_tags, sql_overdues;
    TaskParams task_params;
    quint32 time;
	QDateTime dt_back = QDateTime(QDate(2038,1,1), QTime());
    QDateTime dt_current;
    dt_current.setDate(QDate::currentDate());
//    time = dt_current.toSecsSinceEpoch();

    if(row < m_cursor || row >= m_cursor + 100 || m_lTasksParams.count() == 0)
    {
        m_cursor = row - 50;
        if(m_cursor < 0) m_cursor = 0;
        m_lTasksParams.clear();

        if(isFiltering)
        {
            for(int i = 0;i < m_selectedFilter->count(); i++)
            {
                if(i == 0)
                    sql_tags += QString("%1").arg(m_selectedFilter->at(i));
                else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
            }
        }

//        if (m_viewMode == TO_DO && m_cursor == 0)
        {
            if(isFiltering && !m_selectedFilter->isEmpty())
            {
//                sql = QString("select * from tasks where time_end < %0 and time_close = 0 and repeat_state > 0 and exists ( select task_id from tasks_tags where tag_id in ( %1 ) and task_id = tasks.id )  order by time_start ")
                sql = QString("select * from tasks where time_end < %0 and repeat_state > 0 and ( select count(task_id) from tasks_tags where tag_id in ( %1 ) and task_id = tasks.id ) = %2  order by time_start ")
										.arg(dt_current.toSecsSinceEpoch()).arg(sql_tags).arg(m_selectedFilter->count());
            }
            else
//                sql = QString("select * from tasks where time_end < %0 and time_close = 0 and repeat_state > 0 order by time_start ")
                sql = QString("select * from tasks where time_end < %0 and repeat_state > 0 order by time_start ")
						.arg(dt_current.toSecsSinceEpoch());
            if(!q.exec(sql))
            {
                qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError() << sql;
                return 0;
            }
            while(q.next())
            {
                if(!sql_overdues.isEmpty())
                    sql_overdues += ",";
                sql_overdues += q.value("id").toString();
            }
                //            task_params.is_overdue = true;
//            while(q.next())
//            {
//                task_params.id = q.value("id").toInt();
//                task_params.interval = q.value("time_start").toInt() - q.value("time_end").toInt();
//                task_params.transfer_status = TransferTime::NO_TRANSFER;
//                time = q.value("time_start").toUInt();
//                task_params.dt_current = QDateTime::fromSecsSinceEpoch(time);
//                m_lTasksParams.append(task_params);
//            }
        }

        if(isFiltering && !m_selectedFilter->isEmpty())
        {
            if (m_viewMode == ALL_TASKS)
                sql = QString("select task_id, interval, state, time_start , case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end as time from repeats where exists ( select task_id from tasks_tags where tag_id in ( %2 ) and task_id = repeats.task_id ) order by time, time_start limit 100 offset %3")
						.arg(sql_overdues).arg(dt_current.toSecsSinceEpoch()).arg(sql_tags).arg(m_cursor);
            //                sql = QString("select * from repeats where exists ( select task_id from tasks_tags where tag_id in (%0) and task_id = repeats.task_id )  order by time_start limit 100 offset %1 ")
            //                        .arg(sql_tags).arg(m_cursor);
            else if (m_viewMode == TO_DO)
                sql = QString("select task_id, interval, state, time_start , case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end as time from repeats where time >= %1 and time < %2 and exists ( select task_id from tasks_tags where tag_id in ( %3 ) and task_id = repeats.task_id ) order by time, time_start limit 100 offset %4")
						.arg(sql_overdues).arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(7).toSecsSinceEpoch()).arg(sql_tags).arg(m_cursor);

            //                sql = QString("select * from repeats where time_start >= %0 and time_start <= %1 and exists ( select task_id from tasks_tags where task_id in (%2) and task_id = repeats.task_id )  order by time_start limit 100 offset %2 ")
			//                        .arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(30).toSecsSinceEpoch()).arg(sql_tags).arg(m_cursor);
            else if (m_viewMode == NOTES_AND_TASKS)
                sql = QString("select id, time_modify from tasks where flag != 3 and ( select count(task_id) from tasks_tags where tag_id in ( %0 ) and task_id = tasks.id ) = %1 order by time_modify desc limit 100 offset %2")
                        .arg(sql_tags).arg(m_selectedFilter->count()).arg(m_cursor);
            else if (m_viewMode == ONLY_NOTES)
                sql = QString("select id, time_modify from tasks where flag != 3 and repeat_state = 0 and exists ( select task_id from tasks_tags where tag_id in (%0) and task_id = tasks.id ) order by time_modify desc limit 100 offset %1")
                        .arg(sql_tags).arg(m_cursor);
            else // LAST_MODIFY
                sql = QString("select id, time_modify from tasks where flag != 3 and repeat_state > 0 and exists ( select task_id from tasks_tags where tag_id in (%0) and task_id = tasks.id ) order by time_modify desc limit 100 offset %1")
                        .arg(sql_tags).arg(m_cursor);


//            if (m_viewMode == ALL_TASKS)
//                sql = QString("select task_id, interval, state, time_start , case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end as time from repeats where exists ( select task_id from tasks_tags where tag_id in ( %2 ) and task_id = repeats.task_id ) order by time, time_start limit 100 offset %3")
//                              .arg(sql_overdues).arg(dt_current.toSecsSinceEpoch()).arg(sql_tags).arg(m_cursor);
////                sql = QString("select * from repeats where exists ( select task_id from tasks_tags where tag_id in (%0) and task_id = repeats.task_id )  order by time_start limit 100 offset %1 ")
////                        .arg(sql_tags).arg(m_cursor);
//            else if (m_viewMode == TO_DO)
//                sql = QString("select task_id, interval, state, time_start , case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end as time from repeats where time >= %1 and time < %2 and exists ( select task_id from tasks_tags where tag_id in ( %3 ) and task_id = repeats.task_id ) order by time, time_start limit 100 offset %4")
//                              .arg(sql_overdues).arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(7).toSecsSinceEpoch()).arg(sql_tags).arg(m_cursor);

//                //                sql = QString("select * from repeats where time_start >= %0 and time_start <= %1 and exists ( select task_id from tasks_tags where task_id in (%2) and task_id = repeats.task_id )  order by time_start limit 100 offset %2 ")
////                        .arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(30).toSecsSinceEpoch()).arg(sql_tags).arg(m_cursor);
//            else if (m_viewMode == NOTES_AND_TASKS)
//                sql = QString("select id, time_modify from tasks where flag != 3 and exists ( select task_id from tasks_tags where tag_id in (%0) and task_id = tasks.id ) order by time_modify desc limit 100 offset %1")
//                        .arg(sql_tags).arg(m_cursor);
//            else if (m_viewMode == ONLY_NOTES)
//                sql = QString("select id, time_modify from tasks where flag != 3 and repeat_state = 0 and exists ( select task_id from tasks_tags where tag_id in (%0) and task_id = tasks.id ) order by time_modify desc limit 100 offset %1")
//                        .arg(sql_tags).arg(m_cursor);
//            else // LAST_MODIFY
//                sql = QString("select id, time_modify from tasks where flag != 3 and repeat_state > 0 and exists ( select task_id from tasks_tags where tag_id in (%0) and task_id = tasks.id ) order by time_modify desc limit 100 offset %1")
//                        .arg(sql_tags).arg(m_cursor);
        }
        else if (m_viewMode == ALL_TASKS)
            sql = QString("select task_id, interval, state, time_start , case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end as time from repeats order by time, time_start limit 100 offset %2")
						  .arg(sql_overdues).arg(dt_current.toSecsSinceEpoch()).arg(m_cursor);
//        sql = QString("select task_id, interval, state, time_start , case  when exists ( select * from tasks where time_close = 0 and time_end < %0 and tasks.id = repeats.task_id )  then %0 else repeats.time_start end as time from repeats order by time limit 100 offset %1")
//                      .arg(dt_current.toSecsSinceEpoch()).arg(m_cursor);
        else if (m_viewMode == TO_DO)
            sql = QString("select task_id, interval, state, time_start , case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end as time from repeats where time >= %1 and time < %2 order by time, time_start limit 100 offset %3")
						  .arg(sql_overdues).arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(7).toSecsSinceEpoch()).arg(m_cursor);
//            sql = QString("select (select count(0) from repeats r1 where r1.time_start <= r2.time_start and r1.task_id = r2.task_id and case  when r1.task_id in ( %0 )  then %1 else r1.time_start end  >= %1) as num, task_id, interval, state, time_start , case  when r2.task_id in ( %0 )  then %1 else r2.time_start end as time from repeats r2  where num = 1  order by time, time_start limit 100 offset %2;")
//                          .arg(sql_overdues).arg(dt_current.toSecsSinceEpoch()).arg(m_cursor);

//        select (select count(0) from repeats r1 where r1.time_start <= r2.time_start and r1.task_id = r2.task_id and case  when exists ( select * from tasks where time_close = 0 and time_end < 1463162400 and tasks.id = r1.task_id )  then 1463162400 else r1.time_start end  >= 1463162400) as num, task_id, interval, state, time_start , case  when exists ( select * from tasks where time_close = 0 and time_end < 1463162400 and tasks.id = r2.task_id )  then 1463162400 else r2.time_start end as time from repeats r2  where num = 1  order by time, time_start limit 100 offset 0;


//            sql = QString("select distinct task_id,interval,state,time_start from repeats limit 100 offset %1")
//                    .arg(m_cursor);
        else if (m_viewMode == NOTES_AND_TASKS)
            sql = QString("select id, time_modify from tasks where flag != 3 order by time_modify desc limit 100 offset %1")
//            sql = QString("select id, time_modify from tasks where flag != 3 order by time_history desc limit 100 offset %1")
                    .arg(m_cursor);
        else if (m_viewMode == ONLY_NOTES)
            sql = QString("select id, time_modify from tasks where flag != 3 and repeat_state = 0  order by time_modify desc limit 100 offset %1")
                    .arg(m_cursor);
        else // LAST_MODIFY
            sql = QString("select id, time_modify from tasks where flag != 3 and repeat_state > 0 order by time_modify desc limit 100 offset %1")
                    .arg(m_cursor);

//        qDebug() << "SQL " << __FILE__ << __LINE__ << ": " << sql;

        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
            return 0;
        }

//        task_params.is_overdue = false;
        if(m_viewMode == TO_DO)
        {
			dt_back = QDateTime(QDate::currentDate(), QTime());
            task_params.id = 0;
            task_params.dt_start = task_params.dt_current = task_params.dt_current = dt_back;
            task_params.interval = 0;
            m_lTasksParams.append(task_params);
        }

        while(q.next())
        {
            if (m_viewMode == LAST_MODIFY || m_viewMode == NOTES_AND_TASKS || m_viewMode == ONLY_NOTES )
            {
                task_params.id = q.value("id").toInt();
                task_params.interval = 0; //q.value("interval").toInt();
                task_params.transfer_status = TransferTime::NO_TRANSFER; // q.value("state").toInt();
                time = q.value("time_modify").toUInt();
				task_params.dt_current = QDateTime::fromSecsSinceEpoch(time);
                if(!q2.exec(QString("select * from repeats where task_id = %1 limit 1").arg(task_params.id)))
                {
                    qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
                }
                if(q2.next())
                {
                    task_params.work_state = q2.value("is_closed").toInt();
                    time = q2.value("time_start").toUInt();
					task_params.dt_start = QDateTime::fromSecsSinceEpoch(time);
                    time += q2.value("interval").toUInt();
					task_params.dt_overdue = QDateTime::fromSecsSinceEpoch(time);
                }

            }
            else if(m_viewMode == TO_DO)
            {
                time = q.value("time_start").toUInt();
//                if(dt_back.date() > QDateTime::fromSecsSinceEpoch(time).date())
//                {
//                    dt_back = QDateTime(QDateTime::fromSecsSinceEpoch(time).date());
//                }
				while( dt_back.date() < QDateTime::fromSecsSinceEpoch(time).date())
                {
                    dt_back = dt_back.addDays(1);
                    task_params.id = 0;
                    task_params.dt_current = task_params.dt_current = task_params.dt_current = dt_back;
                    task_params.interval = 0;
//                    task_params.is_overdue = false;
                    m_lTasksParams.append(task_params);
                }
                task_params.id = q.value("task_id").toInt();
                task_params.interval = q.value("interval").toInt();
                task_params.transfer_status = (TransferTime::e_status)(q.value("state").toInt() % 10);
                task_params.work_state = q.value("state").toInt() / 10;
                time = q.value("time").toUInt();
				task_params.dt_current = QDateTime::fromSecsSinceEpoch(time);
                time = q.value("time_start").toUInt();
				task_params.dt_overdue = QDateTime::fromSecsSinceEpoch(time);

            }
            else
            {
                task_params.id = q.value("task_id").toInt();
                task_params.interval = q.value("interval").toInt();
                task_params.work_state = q.value("state").toInt() / 10;
                task_params.transfer_status = (TransferTime::e_status)(q.value("state").toInt() % 10);
                time = q.value("time").toUInt();
				task_params.dt_current = QDateTime::fromSecsSinceEpoch(time);
                time = q.value("time_start").toUInt();
				task_params.dt_overdue = QDateTime::fromSecsSinceEpoch(time);
            }
            //            if(!m_idNotes.contains(id))
            m_lTasksParams.append(task_params);
        }

        if(m_viewMode == TO_DO)
        {
            time = q.value("time_start").toUInt();
            if(dt_back.date() > QDate::currentDate().addDays(6))
            {
				dt_back = QDateTime(QDate::currentDate(), QTime());
            }
            while( dt_back.date() < QDate::currentDate().addDays(6))
            {
                dt_back = dt_back.addDays(1);
                task_params.id = 0;
                task_params.dt_current = task_params.dt_current = task_params.dt_current = dt_back;
                task_params.interval = 0;
    //                    task_params.is_overdue = false;
                m_lTasksParams.append(task_params);
            }
        }

    }



    if(row - m_cursor > m_lTasksParams.count()-1)
        id = 0;
    else id = m_lTasksParams[row - m_cursor].id;

    return id;

}

Task *TasksFilterModel::getTask(int index, Task *source)
{
    if(m_viewNewTask && !source)
        return m_activeTask;
    if(m_viewNewTask)
        return source;
//    QString sql;
//    QSqlQuery q;
//    SimpleCrypt crypto;
//    quint64 key = Q_UINT64_C(0x4947afdb4cb9e20d);
//    quint64 new_key;
//    qint64 time_int;
    Task *res = NULL, *res2 = NULL;
//    Task *curTask;
//    SubTask *curSubTask;
//    QDateTime datetime;
    QList<QObject*> dataobj_tmp;
    QSqlQuery q;
    int id = 0;
    int row = index - m_cursor;

    if(m_viewMode == ALL_TASKS)
    {
        if(m_dateTasks.contains(m_activeModelIndex))
        {
            dataobj_tmp =m_dateTasks.value(m_activeModelIndex);
            if(index >= 0 && index < dataobj_tmp.count())
                res = (Task*)dataobj_tmp.at(index);

        }
        if(res)
        {
            if(!source)
            {
                res2 = m_activeTask;
                m_activeTask = new Task();
                delete res2;
                res2 = m_activeTask;
            }
            else res2 = source;
            res2->id = res->id;
            res2->m_index = index;
//            res2->m_index = res->m_index;
            res2->m_interval = res->m_interval;
            res2->m_isOverdue = res->m_isOverdue;
            res2->m_workState = res->m_workState;
            //        res->m_lTransferTimes = &m_lTasksParams.at(iter.value()).lTransferTimes;
    //        res->m_lTransferTimes = m_lTasksParams.at(iter.value()).lTransferTimes;
    //        res->m_repeatModels = m_lTasksParams.at(iter.value()).repeatModels;

            res2->dt_current = res2->dt_end = res2->dt_end_interval = res->dt_current;
            res2->getTask();
            res = res2;

        }
        return res;
    }

    if(m_lTasksParams.count() == 0)
        getId(0);
    if(row >= 0 && row < m_lTasksParams.count())
    {
        //        id = m_idTasks[row];

        //        sql = QString("select task_key, flag, is_modify, time_modify, time_start, time_end, repeat, repeat_interval, repeat_params, notify, notify_params, title,data, offset, ver, patch_key, time_history from tasks where id=%1").arg(id);
        //        if(!q.exec(sql))
        //        {
        //            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
        //            return 0;
        //        }
        //        q.next();
        if(!source)
        {
            res = m_activeTask;
            m_activeTask = new Task();
            delete res;
            res = m_activeTask;
        }
        else res = source;

//        QMapIterator<QDateTime, int> iter(m_bufferTasks);
//        for(int i = 0; i < index+1; i++)
//        {
//            if (iter.hasNext()) {
//                iter.next();
//                //                cout << i.key() << ": " << i.value() << endl;
//            }
//        }
        id = m_lTasksParams.at(index - m_cursor).id;
        //        m_bufferTasks.values()
        res->id = id;
        res->m_index = index;
        res->m_interval = m_lTasksParams.at(index - m_cursor).interval;
//        res->m_isOverdue = m_lTasksParams.at(index - m_cursor).dt_overdue != m_lTasksParams.at(index - m_cursor).dt_current;
        qDebug() << __FILE__ << __LINE__ << m_lTasksParams.at(index - m_cursor).dt_overdue;
        res->m_isOverdue = m_lTasksParams.at(index - m_cursor).dt_overdue < QDateTime::currentDateTime() && m_lTasksParams.at(index - m_cursor).work_state < 2;
        res->m_workState = m_lTasksParams.at(index - m_cursor).work_state;
        //        res->m_lTransferTimes = &m_lTasksParams.at(iter.value()).lTransferTimes;
//        res->m_lTransferTimes = m_lTasksParams.at(iter.value()).lTransferTimes;
//        res->m_repeatModels = m_lTasksParams.at(iter.value()).repeatModels;
        if(m_viewMode == NOTES_AND_TASKS)
        {
//            if(!q.exec(QString("select is_closed from repeats where task_id = %1 limit 1").arg(id)))
//            {
//                qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//            }
//            if(q.next())
//            {
//                res->m_isClosed = q.value(0).toInt();
//            }
        }

        res->dt_current = res->dt_end = res->dt_end_interval = res->dt_prev = m_lTasksParams.at(index - m_cursor).dt_current;
        //        QList<QDateTime> ldateTimes = m_bufferTasks.keys(id);
        QList<QDateTime> ldateTimes;

        for(int i = 0; i < m_lTasksParams.count(); i++)
        {

            if(m_lTasksParams.at(i).id == id)
                ldateTimes.append(m_lTasksParams.at(i).dt_current);
            //                cout << i.key() << ": " << i.value() << endl;
        }


        row = ldateTimes.indexOf(res->dt_current);
        if(row > 0)
            res->dt_prev = ldateTimes[row - 1];
        if(ldateTimes.count() > row + 1)
            res->dt_next = ldateTimes[row + 1];

        res->getTask();
        if(m_viewMode == LAST_MODIFY || m_viewMode == NOTES_AND_TASKS)
        {
			res->m_interval = res->endTime().toSecsSinceEpoch() - res->startTime().toSecsSinceEpoch();
            res->dt_end_interval = res->dt_end;
//            res->dt_current = res->dt_start;
//            res->m_isOverdue = false;
        }
        m_activeId = id;
    }
    return res;

}

Task *TasksFilterModel::getFullTask(int index)
{
    quint32 offset;
    qint16 last_key;
    quint16 last_ver;
    getTask(index);
    if(m_activeTask && m_activeTask->m_doc->lBlocks->size() == 0)
        ext_storage->loadTasksBody(m_activeTask, offset, last_key, last_ver);
    if(m_activeTask)
    {
        foreach (SubTask *curSubTask, m_activeTask->m_lSubTasks) {
            ext_storage->loadTasksBody(curSubTask,offset,last_key, last_ver);
        }
    }
    return m_activeTask;
}

//Task *TasksFilterModel::getTask(int index, Task *source)
//{
//    QString sql;
//    QSqlQuery q;
//    SimpleCrypt crypto;
//    quint64 key = Q_UINT64_C(0x4947afdb4cb9e20d);
//    quint64 new_key;
//    qint64 time_int;
//    Task *res = NULL;
//    Note *curNote;
//    SubTask *curSubTask;
//    QDateTime datetime;
//    int id = 0;
//    int row = index - m_cursor;
//    if(m_lTasksParams.count() == 0)
//        getId(0);
//    if(row >= 0 && row < m_lTasksParams.count() )
//    {
////        id = m_idTasks[row];

////        sql = QString("select task_key, flag, is_modify, time_modify, time_start, time_end, repeat, repeat_interval, repeat_params, notify, notify_params, title,data, offset, ver, patch_key, time_history from tasks where id=%1").arg(id);
////        if(!q.exec(sql))
////        {
////            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
////            return 0;
////        }
////        q.next();
//        if(!source)
//        {
//            res = m_activeTask;
//            m_activeTask = new Task();
//            delete res;
//            res = m_activeTask;
//        }
//        else res = source;

//        QMapIterator<QDateTime, int> iter(m_bufferTasks);
//        for(int i = 0; i < index+1; i++)
//        {
//            if (iter.hasNext()) {
//                iter.next();
//                //                cout << i.key() << ": " << i.value() << endl;
//            }
//        }
//        id = m_lTasksParams.at(iter.value()).id;
////        m_bufferTasks.values()
//        res->id = id;
//        res->m_index = index;
//        res->m_interval = m_lTasksParams.at(iter.value()).interval;
////        res->m_lTransferTimes = &m_lTasksParams.at(iter.value()).lTransferTimes;
//        res->m_lTransferTimes = m_lTasksParams.at(iter.value()).lTransferTimes;
//        res->m_repeatModels = m_lTasksParams.at(iter.value()).repeatModels;

//        res->dt_current = res->dt_end = res->dt_end_interval = res->dt_prev = iter.key();
////        QList<QDateTime> ldateTimes = m_bufferTasks.keys(id);
//        QList<QDateTime> ldateTimes;
//        iter.toFront();

//        while (iter.hasNext()) {
//            iter.next();
//            if(m_lTasksParams.at(iter.value()).id == id)
//                ldateTimes.append(iter.key());
//            //                cout << i.key() << ": " << i.value() << endl;
//        }


//        row = ldateTimes.indexOf(res->dt_current);
//        if(row > 0)
//            res->dt_prev = ldateTimes[row - 1];
//        if(ldateTimes.count() > row + 1)
//            res->dt_next = ldateTimes[row + 1];

//        res->getTask();
//        m_activeId = id;

////        res->key = QUuid::fromRfc4122(q.value(0).toByteArray());
////        //        m_activeTask->offset_history = q.value(2).toInt();
////        res->flag = q.value(1).toInt();
////        res->is_modify = q.value(2).toInt();
////        res->dt_modify = QDateTime::fromMSecsSinceEpoch(q.value(3).toLongLong());
////        time_int = q.value(4).toLongLong();
////        res->dt_start = QDateTime::fromMSecsSinceEpoch(time_int);
////        res->dt_end = QDateTime::fromMSecsSinceEpoch(q.value(5).toLongLong());
////        res->m_repeat = q.value(6).toInt();
////        res->m_repeatInterval = q.value(7).toInt();
////        res->m_repeatParams = q.value(8).toString();
////        res->m_notify = q.value(9).toInt();
////        res->m_index = index;
////        res->id = id;
////        //        m_activeTask->stlNotifyParams = q.value(11).toStringList();
////        new_key = keyData ^ q.value(12).toLongLong();
////        key = key ^ new_key;
////        crypto.setKey(getKey(key,q.value(0).toByteArray().data()));
////        res->m_title = crypto.decryptToString(q.value(11).toByteArray());
////        res->data = q.value(12).toLongLong();
////        res->offset_history = q.value(13).toInt();
////        res->ver = q.value(14).toInt();
////        res->patch_key = QUuid::fromRfc4122(q.value(15).toByteArray());
////        res->dt_history = QDateTime::fromMSecsSinceEpoch(q.value(16).toLongLong());

////        sql = QString("select tag_id from tasks_tags where task_id=%1").arg(id);
////        if(!q.exec(sql))
////        {
////            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
////            return 0;
////        }
////        while(q.next())
////        {
////            res->lKeyTags.append(q.value(0).toUInt());
////        }


////        sql = QString("select id_note from tasks_notes where task_id=%1").arg(id);
////        if(!q.exec(sql))
////        {
////            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
////            return 0;
////        }
////        while(q.next())
////        {
////            curNote = new Note();
////            curNote->id = q.value(0).toUInt();
////            curNote->getNote();
////            res->lNotes.append(curNote);
////            //            res->lKeyTags.append(q.value(0).toUInt());
////        }
////        sql = QString("select id, patch_key, offset, flag, is_modify, time_modify, time_history,time_task, data, ver, user_key from subtasks where task_id=%1").arg(id);

////        if(!q.exec(sql))
////        {
////            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
////            return 0;
////        }
////        qint64 range_prev = res->dt_current.msecsTo(res->dt_prev) / 2;
////        qint64 range_next = res->dt_current.msecsTo(res->dt_next) / 2;
////        qint64 range_current;

////        while(q.next())
////        {
////            datetime = QDateTime::fromMSecsSinceEpoch(q.value(7).toLongLong());
////            range_current = res->dt_current.msecsTo(datetime);
////            if((range_prev == 0 && range_current <= 0) || (range_current >= range_prev && range_current <= range_next) ||
////                    (range_next == 0 && range_current >= 0))
////            {
////            curSubTask = new SubTask();
////            curSubTask->id = q.value(0).toUInt();
////            //            key = QUuid::fromRfc4122(q.value(0).toByteArray());
////            curSubTask->patch_key = QUuid::fromRfc4122(q.value(1).toByteArray());
////            curSubTask->offset_history = q.value(2).toInt();
////            curSubTask->flag = q.value(3).toInt();
////            curSubTask->is_modify = q.value(4).toInt();
////            curSubTask->dt_modify = QDateTime::fromMSecsSinceEpoch(q.value(5).toLongLong());
////            curSubTask->dt_history = QDateTime::fromMSecsSinceEpoch(q.value(6).toLongLong());
////            curSubTask->dt_task = datetime;


////            //           new_key = keyData ^ q.value(8).toLongLong();
////            //               key2 = key2 ^ new_key;
////            //            crypto.setKey(::getKey(key2,q.value(0).toByteArray().data()));
////            //            m_title = crypto.decryptToString(q.value(7).toByteArray());

////            curSubTask->data = q.value(8).toLongLong();
////            curSubTask->ver = q.value(9).toInt();
////            curSubTask->user_key = q.value(10).toInt();

////            res->lSubTasks.append(curSubTask);
////            }
////        }



//    }
//    return res;

//}

Task *TasksFilterModel::addTask(QDate date)
{
    delete m_activeTask;
    m_activeTask = new Task();
    m_activeTask->dt_modify = m_activeTask->dt_start = QDateTime::currentDateTime();
    int h = m_activeTask->dt_start.time().hour();
    m_activeTask->dt_start.setTime(QTime(h,0));
    m_activeTask->dt_start.setDate(date);
    qint64 mSec = m_activeTask->dt_start.toMSecsSinceEpoch();
    m_activeTask->dt_start.setMSecsSinceEpoch(mSec + 3600*1000);
    m_activeTask->dt_end = m_activeTask->dt_end_interval =  m_activeTask->dt_current = m_activeTask->dt_start;
    //    m_activeTask->dt_end.addSecs(3600);
    m_viewNewTask = true;
    m_activeId = -1;
    return m_activeTask;
}

int TasksFilterModel::indexOf(Task *task)
{
    int res = -1;
    bool b_find = false;

    Task  *task2 = NULL;
    QList<QObject*> dataobj_tmp;
//    int id = 0;
//    int row = index - m_cursor;

    if(m_viewMode == ALL_TASKS)
    {
        if(m_dateTasks.contains(m_activeModelIndex))
        {
            dataobj_tmp =m_dateTasks.value(m_activeModelIndex);
            while(res + 1 < dataobj_tmp.count())
            {
                res++;
                task2 = (Task*)dataobj_tmp.at(res);
                if(task && task2->id ==  task->id && task2->dt_current ==  task->dt_current)
                {
                    b_find = true;
                    break;
                }
            }

        }
    }
//    else if(m_viewMode == NOTES_AND_TASKS)
//    {
//        while(res + 1 < m_lTasksParams.count())
//        {
//            res++;
//            if(task && m_lTasksParams.at(res).id ==  task->id)
//            {
//                b_find = true;
//                break;
//            }
//        }
//    }
    else
    {
        while(res + 1 < m_lTasksParams.count())
        {
            res++;
            qDebug() << __FILE__ << __LINE__ << m_lTasksParams.at(res).dt_current <<  task->dt_current;
            if(task && m_lTasksParams.at(res).id ==  task->id && m_lTasksParams.at(res).dt_current ==  task->dt_current)
            {
                b_find = true;
                break;
            }
        }
    }
    if(!b_find)
        res = -1;
    return res;
//    if(m_activeId == -1)
//        return 0;
//    QMapIterator<QDateTime, int> iter(m_bufferTasks);
//    int count = -1;
//    while (iter.hasNext()) {
//        iter.next();
//        count++;
//        if(m_lTasksParams.at(iter.value()).id == m_activeId)
//            break;
//    }
//    return count;
//    return m_idTasks.indexOf(m_activeId);
}

int TasksFilterModel::indexOfSection(QDate section)
{
    int res = 0;
    bool b_find = false;
    while(res < m_lTasksParams.count())
    {
        if(m_lTasksParams.at(res).dt_current.date() == section && m_lTasksParams.at(res).id > 0)
        {
            b_find = true;
            break;
        }
        res++;
    }
    if(!b_find)
        res = -1;
    return res;

    //    QMapIterator<QDateTime, int> iter(m_bufferTasks);

//    while (iter.hasNext()) {
//        res++;
//        iter.next();
//        if(iter.key().date() == section)
//            break;
//        if(section == "before" )
//            break;
//        else if(section == "today" && iter.key().date() == QDate::currentDate())
//            break;
//        else if(section == "tomorrow" && iter.key().date() == QDate::currentDate().addDays(1))
//            break;
//        else if(section == "nearest" && iter.key().date() > QDate::currentDate().addDays(1) && iter.key().date() < QDate::currentDate().addDays(8))
//            break;
//        else if(section == "sometime" && iter.key().date() > QDate::currentDate().addDays(8) )
//            break;

//        //                cout << i.key() << ": " << i.value() << endl;
//    }

}

int TasksFilterModel::selectDate(QDate date)
{
//    int res = -1;
//    bool find = false;
//    QMapIterator<QDateTime, int> iter(m_bufferTasks);

//    while (iter.hasNext()) {
//        iter.next();
//        res++;
//        if(iter.key().date() == date)
//        {
//            find = true;
//            break;
//        }
//        //                cout << i.key() << ": " << i.value() << endl;
//    }
//    //    res++;
//    if(!find) res = -1;
//    return res;
    int res = -1;
    QSqlQuery q;
    QString sql;
    QString sql_tags;
    QString sql_overdues;
    quint32 time;
	QDateTime dt_current(QDate::currentDate(), QTime());
	time = dt_current.toSecsSinceEpoch();


    sql = QString("select count() from repeats where date = %0 ")
			.arg(QDateTime(date, QTime()).toSecsSinceEpoch());
//            sql = QString("select * from tasks where time_end < %0 and ( time_close = 0 or exists ( select task_id from subtasks where time_close = 0 and task_id = tasks.id and time_task < %0 ) ) order by time_start ")
//                    .arg(time);
    if(!q.exec(sql))
    {
        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
        return -1;
    }
    q.next();
    if(q.value(0).toInt() <= 0)
            return -1;


    if(isFiltering && m_selectedFilter->count() > 0)
    {
//        QString("select * from tasks where time_end < %0 and time_close = 0 and exists ( select task_id from tasks_tags where task_id in ( %1 ) and task_id = tasks.task_id )  order by time_start ")
        QString("select * from tasks where time_end < %0 and exists ( select task_id from tasks_tags where task_id in ( %1 ) and task_id = tasks.task_id )  order by time_start ")
								.arg(dt_current.toSecsSinceEpoch()).arg(sql_tags);
    }
    else
//        sql = QString("select * from tasks where time_end < %0 and time_close = 0 order by time_start ")
        sql = QString("select * from tasks where time_end < %0 order by time_start ")
				.arg(dt_current.toSecsSinceEpoch());
    if(!q.exec(sql))
    {
        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
        return -1;
    }
    while(q.next())
    {
        if(!sql_overdues.isEmpty())
            sql_overdues += ",";
        sql_overdues += q.value("id").toString();
    }


    if(isFiltering && m_selectedFilter->count() > 0)
    {
        for(int i = 0;i < m_selectedFilter->count(); i++)
        {
            if(i == 0)
                sql_tags += QString("%1").arg(m_selectedFilter->at(i));
            else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
        }
        if(m_viewMode == ALL_TASKS)
            sql = QString("select count() from repeats where exists ( select task_id from tasks_tags where task_id in (%0) and task_id = repeats.task_id )  ")
                    .arg(sql_tags);
        else if(m_viewMode == TO_DO)
            sql = QString("select count() from repeats where time_start >= %0 and time_start < %1 and exists ( select task_id from tasks_tags where task_id in (%2) and task_id = repeats.task_id )  ")
					.arg(time).arg(dt_current.addDays(7).toSecsSinceEpoch()).arg(sql_tags);
        else
            sql = QString("select count(id) from tasks where flag != 3 and exists ( select task_id from tasks_tags where task_id in (%1) and task_id = tasks.id )");


        //            sql = QString("select id from notes_tags,notes where id_note = id and (%0) order by time_modify desc limit 1000 offset %1 ")
        //                    .arg(sql_tags).arg(m_cursor);
    }
    else if(date >= QDate::currentDate())
        sql = QString("select count() from repeats where time_start < %1 or task_id in ( %2 )")
				.arg(QDateTime(date, QTime()).toSecsSinceEpoch()).arg(sql_overdues);
    else
        sql = QString("select count() from repeats where time_start < %1 and task_id not in ( %2 )")
				.arg(QDateTime(date, QTime()).toSecsSinceEpoch()).arg(sql_overdues);
//    else if(m_viewMode == TO_DO)
//        sql = QString("select count() from repeats where time_start >= %0 and time_start < %1 or task_id in ( %2 )")
//                .arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(7).toSecsSinceEpoch()).arg(sql_overdues);
//    else
//        sql = QString("select count(id) from tasks where flag != 3");


    if(!q.exec(sql))
    {
        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
        return -1;
    }
    q.next();
    res = q.value(0).toInt();
    return res;
}

int TasksFilterModel::countTasks(QDate date)
{
    return 0;
    int res = 0;
//    bool bFind = false;
//    //    qDebug() << "countTasks" << date;
//    QMapIterator<QDateTime, int> iter(m_bufferTasks);
//    while (iter.hasNext()) {
//        iter.next();

//        if(iter.key().date() == date)
//        {
//            res++;
//            bFind = true;
//        }
//        else if(bFind)
//            break;
//        //                cout << i.key() << ": " << i.value() << endl;
//    }
    QSqlQuery q(QSqlDatabase::database());
    QString sql = "select count(id) from tasks";
    QString sql_tags;
    quint32 time, time_end;
    QDateTime date_time;
    date_time.setDate(date);
	time = date_time.toSecsSinceEpoch();
    date_time.setTime(QTime(23,59,59));
	time_end = date_time.toSecsSinceEpoch();

//    QString sql; //= QString("select id from tasks where key=% ").arg(sql_tags);


    m_cursor = 0;

//    if(isFiltering && m_selectedFilter->count() > 0)
//    {
//        for(int i = 0;i < m_selectedFilter->count(); i++)
//        {
//            if(i == 0)
//                sql_tags += QString("tag_id = %1").arg(m_selectedFilter->at(i));
//            else sql_tags += QString(" or tag_id = %1").arg(m_selectedFilter->at(i));
//        }
//        sql = QString("select count(id) from tasks where exists ( select task_id from tasks_tags where (%0) and task_id = tasks.id ) ").arg(sql_tags);

//    }


    if(isFiltering)
    {
        for(int i = 0;i < m_selectedFilter->count(); i++)
        {
            if(i == 0)
                sql_tags += QString("%1").arg(m_selectedFilter->at(i));
            else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
        }

        sql = QString("select count() from repeats where time_start >= %0 and time_start <= %1 and exists ( select task_id from tasks_tags where tag_id in (%2) and task_id = repeats.task_id ) and exists (select id from tasks where id = repeats.task_id  and flag != 3)  ")
                .arg(time).arg(time_end).arg(sql_tags);
    }
    else sql = QString("select count() from repeats where time_start >= %0 and time_start <= %1 and exists (select id from tasks where id = repeats.task_id and flag != 3)").arg(time).arg(time_end);

    if(!q.exec(sql))
    {
        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
        return 0;
    }
    q.next();
    res = q.value(0).toInt();

    return res;
}

void TasksFilterModel::setViewMode(int source) {
    if(m_viewMode != source)
    {
        m_viewMode = (e_vew_mode)source;
        refresh();
    }
}

QVariant TasksFilterModel::getTasksModel(QDate date, int modelIndex)
{

    QVariant res;
    quint32 offset;
    qint16 last_key;
    quint16 last_ver;
    QList<QObject*> dataobj;
    QList<QObject*> dataobj_tmp;
    quint32 time, time_overdue;
    QString sql, sql_overdues, sql_tags;
	QDateTime dt(date, QTime());
    QSqlQuery q;
    QString sql_current_day;

//    QList<QObject*> dataobj2;

//    int k;

    Task *cur_task;

//    int index = -1;
//    bool find = false;
//    QMapIterator<QDateTime, int> iter(m_bufferTasks);

//    while (iter.hasNext()) {
//        iter.next();
//        index++;
//        if(iter.key().date() == date)
//        {
//            find = true;
//            cur_task = new Task();
////            k = m_filterTasksModel.m_idTasks.indexOf(iter.value());
//            k = iter.value();
//            if(k >= 0)
//            {
////                m_filterTasksModel.getTask(m_filterTasksModel.m_lTasksParams.at(k).id,cur_task);
//                getTask(index,cur_task);
//                ext_storage->loadNotesBody(cur_task, offset, last_key, last_ver);
//                cur_task->m_index = index;
//                dataobj.append(cur_task);
//            }
//        }
//        else if(find)
//            break;
//        //                cout << i.key() << ": " << i.value() << endl;
//    }
//    if(!find) res = -1;
//    for(int i = 0; i < m_lTasksParams.count(); i++)
//    {
//        if(m_lTasksParams.at(i).dt_current.date() == date && m_lTasksParams.at(i).id > 0)
//        {
//            cur_task = new Task(this);
//            getTask(i,cur_task);
//            ext_storage->loadNotesBody(cur_task, offset, last_key, last_ver);
//            cur_task->m_index = i;
//            dataobj.append(cur_task);

//        }
//    }
    qDebug() << "getTasksModel " << __FILE__ << __LINE__ << ": " << date;

    if(isFiltering)
    {
        for(int i = 0;i < m_selectedFilter->count(); i++)
        {
            if(i == 0)
                sql_tags += QString("%1").arg(m_selectedFilter->at(i));
            else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
        }
    }

//        if (m_viewMode == TO_DO && m_cursor == 0)
    if(date == QDate::currentDate())
    {
        if(isFiltering)
        {
//            sql = QString("select * from tasks where time_end < %0 and time_close = 0 and repeat_state > 0 and exists ( select task_id from tasks_tags where tag_id in ( %1 ) and task_id = tasks.id )  order by time_start ")
            sql = QString("select * from tasks where time_end < %0 and repeat_state > 0 and exists ( select task_id from tasks_tags where tag_id in ( %1 ) and task_id = tasks.id )  order by time_start ")
									.arg(dt.toSecsSinceEpoch()).arg(sql_tags);
        }
        else
//            sql = QString("select * from tasks where time_end < %0 and time_close = 0 and repeat_state > 0 order by time_start ")
            sql = QString("select * from tasks where time_end < %0 and repeat_state > 0 order by time_start ")
					.arg(dt.toSecsSinceEpoch());
        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError() << sql;
            return res;
        }
        while(q.next())
        {
            if(!sql_overdues.isEmpty())
                sql_overdues += ",";
            sql_overdues += q.value("id").toString();
        }
		sql_current_day = QString("(task_id > -1 and time + interval < %1 and is_closed < 2) or ").arg(dt.toSecsSinceEpoch());
    }
    sql_overdues = ""; // in future needs for subtasks
    if(isFiltering)
    {
            sql = QString("select task_id, interval, state, is_closed, time_start , case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end as time from repeats where (%5 (time >= %1 and time < %2)) and exists ( select task_id from tasks_tags where tag_id in ( %3 ) and task_id = repeats.task_id ) and exists(select id from tasks where id=repeats.task_id and flag != 3) order by time, time_start limit 100 offset %4")
						  .arg(sql_overdues).arg(dt.toSecsSinceEpoch()).arg(dt.addDays(1).toSecsSinceEpoch()).arg(sql_tags).arg(m_cursor).arg(sql_current_day);
    }
    else
        sql = QString("select task_id, interval, state, is_closed, time_start , case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end as time from repeats where (%4 (time + interval >= %1 and time < %2))  and exists(select id from tasks where id=repeats.task_id and flag != 3) order by time, time_start limit 100 offset %3")
					  .arg(sql_overdues).arg(dt.toSecsSinceEpoch()).arg(dt.addDays(1).toSecsSinceEpoch()).arg(m_cursor).arg(sql_current_day);
//        sql = QString("select task_id, interval, state, time_start , case  when repeats.task_id in ( %0 )  then %1 else repeats.time_start end as time from repeats where time >= %1 and time < %2  and exists(select id from tasks where id=repeats.task_id and flag != 3) order by time, time_start limit 100 offset %3")
//                      .arg(sql_overdues).arg(dt.toSecsSinceEpoch()).arg(dt.addDays(1).toSecsSinceEpoch()).arg(m_cursor);
    if(!q.exec(sql))
    {
        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError() << sql;
        return res;
    }
//    qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError() << sql;
    while(q.next())
    {
        time = q.value("time").toUInt();
        time_overdue = q.value("time_start").toUInt();
        cur_task = new Task(this);
//        getTask(i,cur_task);
        cur_task->id = q.value("task_id").toInt();
        cur_task->m_interval = q.value("interval").toInt();
        time_overdue += cur_task->interval();

        cur_task->m_index = dataobj.count();
        qDebug() << "state" << q.value("state").toInt();
        cur_task->m_transfer_status = (TransferTime::e_status)(q.value("state").toInt());
        cur_task->m_workState = q.value("is_closed").toInt();

//        if(cur_task->m_isOverdue)
//            cur_task->dt_current = cur_task->dt_end = cur_task->dt_end_interval = cur_task->dt_prev = QDateTime::fromSecsSinceEpoch(time_overdue);
//        else
		cur_task->dt_current = cur_task->dt_end = cur_task->dt_end_interval = cur_task->dt_prev = QDateTime::fromSecsSinceEpoch(time);

//        cur_task->m_isOverdue = time_overdue != time;
//        cur_task->m_isOverdue = (cur_task->m_isClosed < 2  && QDateTime::fromSecsSinceEpoch(time_overdue).date() < QDate::currentDate()); // dt.date();
		cur_task->m_isOverdue = (cur_task->m_workState < 2  && QDateTime::fromSecsSinceEpoch(time_overdue) < QDateTime::currentDateTime()); // dt.date();
        cur_task->getTask();
        if(m_activeTask && cur_task->id == m_activeTask->id && cur_task->dt_current == m_activeTask->dt_current)
            m_activeTask->m_index = cur_task->m_index;
//        ext_storage->loadNotesBody(cur_task, offset, last_key, last_ver);
        dataobj.append(cur_task);

//        task_params.id = q.value("task_id").toInt();
//        task_params.interval = q.value("interval").toInt();
//        task_params.transfer_status = (TransferTime::e_status)q.value("state").toInt();
//        time = q.value("time").toUInt();
//        task_params.dt_current = QDateTime::fromSecsSinceEpoch(time);
//        time = q.value("time_start").toUInt();
//        task_params.dt_overdue = QDateTime::fromSecsSinceEpoch(time);
//        m_lTasksParams.append(task_params);
    }


    res.setValue(dataobj);
    if(m_dateTasks.contains(modelIndex))
    {
      dataobj_tmp =m_dateTasks.take(modelIndex);
      qDeleteAll(dataobj_tmp);
      dataobj_tmp.clear();
      qDebug() << "clearDateObj";
    }
    m_dateTasks.insert(modelIndex,dataobj);
//    QVariant res2 = res;
//    QList<QObject*> dataobj2 = res2.value<QList<QObject*> >();
    qDebug() << "getTasksModel";


    return res;

}

void TasksFilterModel::setActiveModelIndex(int modelIndex)
{
    m_activeModelIndex = modelIndex;
}

void TasksFilterModel::clearTasksModel(int modelIndex)
{
    QList<QObject*> dataobj_tmp;
    if(m_dateTasks.contains(modelIndex))
    {
      dataobj_tmp = m_dateTasks.take(modelIndex);
      qDeleteAll(dataobj_tmp);
      dataobj_tmp.clear();
      qDebug() << "clearDateObj";
      m_activeTask = NULL;
    }
}

void TasksFilterModel::setSearchString(QString search)
{
    m_searchString = search;
    refresh();
}

void TasksFilterModel::setViewNewTask(bool source) {
    m_viewNewTask = source;
}

void TasksFilterModel::setTags(Tags *tags)
{
    m_selectedFilter = &tags->m_selectedFilter;
}


QHash<int, QByteArray> TasksFilterModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[TimeRole] = "time";
    roles[TimeDateRole] = "date";
    roles[TimeStringRole] = "timeString";
    roles[StatusRole] = "status";
    roles[SectionRole] = "section";
    roles[IsRepeate] = "isrepeate";
    roles[WorkState] = "workState";
    roles[TransferStatus] = "transferStatus";
    roles[SecurityRole] = "security";
    roles[IsOverdue] = "isoverdue";
    roles[TaskId] = "taskid";
    roles[IsInSearch] = "isinsearch";
    roles[IsCloseVisible] = "isCloseVisible";
    roles[RegNumRole] = "regNum";
    return roles;
}


DateTasksModel::DateTasksModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_curBufferTasks = NULL;
    isFiltering = false;
    m_selectedFilter = NULL;
    m_viewMode = TasksFilterModel::TO_DO;
}

int DateTasksModel::getIndexOfDay(QDate date)
{
//    QString sql;
//    QSqlQuery q;
//    QString sql_tags;
////    TaskParams task_params;
//    quint32 time;
//    int res = -1;
//    QDateTime dt_current(QDate::currentDate());
//    time = QDateTime(date).toSecsSinceEpoch();

//    if(isFiltering && m_selectedFilter->count() > 0)
//    {
//        for(int i = 0;i < m_selectedFilter->count(); i++)
//        {
//            if(i == 0)
//                sql_tags += QString("%1").arg(m_selectedFilter->at(i));
//            else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
//        }
//        sql = QString("select count(distinct date) from repeats where exists ( select task_id from tasks_tags where task_id in (%0) and date <= %1 order by date )  ")
//                .arg(sql_tags).arg(time);
//    }
//    else if (m_viewMode == TasksFilterModel::TO_DO)
//        sql = QString("select count(distinct date) from repeats where time_start >= %0 and time_start <= %1 and date <= %2 order by date ")
//                .arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(30).toSecsSinceEpoch()).arg(time);
//    else
//        sql = QString("select count(distinct date) from repeats where date <= %1 order by date").arg(time);


//    if(!q.exec(sql))
//    {
//        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//        //            return 0;
//    }
//    //        m_lTasksParams.clear();
//    if(q.next())
//    {

//        res = q.value(0).toInt() - 1;
//    }
//    return res;

   return -date.daysTo(QDate::currentDate());


}

QVariant DateTasksModel::getTasksModel(QDate date, int modelIndex)
{
//    quint32 offset;
//    qint16 last_key;
//    quint16 last_ver;
//    QList<QObject*> dataobj2;

//    int k;
//    int index = -1;
//    bool find = false;

//    QMapIterator<QDateTime, int> iter(m_filterTasksModel.m_bufferTasks);

//    while (iter.hasNext()) {
//        iter.next();
//        index++;
//        if(iter.key().date() == firstKey.date())
//        {
//            find = true;
//            cur_task = new Task();
//            k = iter.value();
//            if(k >= 0)
//            {
//                m_filterTasksModel.getTask(index,cur_task);
//                m_storage.loadNotesBody(cur_task, offset, last_key, last_ver);
//                cur_task->m_index = index;
//                dataobj.append(cur_task);
//            }
//        }
//        else if(find)
//            break;
//    }
    QList<QObject*> dataobj;
    Task *cur_task;
    quint32 offset;
    qint16 last_key;
    quint16 last_ver;

    QString sql;
    QSqlQuery q,q2;
    QString sql_tags;
//    TaskParams task_params;
    quint32 time;
    int index = 0;
    QVariant res;
	QDateTime dt_current(QDate::currentDate(), QTime());



    if(isFiltering && m_selectedFilter->count() > 0)
    {
        for(int i = 0;i < m_selectedFilter->count(); i++)
        {
            if(i == 0)
                sql_tags += QString("%1").arg(m_selectedFilter->at(i));
            else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
        }
    }

    if (m_viewMode == TasksFilterModel::TO_DO)
    {
        if(isFiltering && m_selectedFilter->count() > 0)
        {
            QString("select * from tasks where time_end < %0 and time_close = 0 and exists ( select task_id from tasks_tags where task_id in (%1) and task_id = tasks.task_id )  order by time_start ")
									.arg(dt_current.toSecsSinceEpoch()).arg(sql_tags);
        }
        else
//            sql = QString("select * from tasks where time_end < %0 and time_close = 0 order by time_start ")
            sql = QString("select * from tasks where time_end < %0 order by time_start ")
					.arg(dt_current.toSecsSinceEpoch());
        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
            return 0;
        }
//        task_params.is_overdue = true;
        while(q.next())
        {
            if(date == dt_current.date())
            {
                cur_task = new Task(this);
                cur_task->id = q.value("id").toInt();
                cur_task->m_index = index;
                cur_task->m_interval = q.value("time_start").toInt() - q.value("time_end").toInt();
                cur_task->m_transfer_status = TransferTime::NO_TRANSFER;
                cur_task->m_workState = false;
                cur_task->m_isOverdue = true;
                time = q.value("time_start").toUInt();
				cur_task->dt_current = dt_current; //QDateTime::fromSecsSinceEpoch(time);
                cur_task->getTask();
                ext_storage->loadTasksBody(cur_task, offset, last_key, last_ver);
                dataobj.append(cur_task);
            }
            index++;
        }
    }

    if(isFiltering && m_selectedFilter->count() > 0)
    {

        if (m_viewMode == TasksFilterModel::TO_DO)
            sql = QString("select * from repeats where time_start >= %0 and time_start <= %1 and exists ( select task_id from tasks_tags where task_id in (%2) and task_id = repeats.task_id )  order by time_start limit 1000 offset %2 ")
					.arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(30).toSecsSinceEpoch()).arg(sql_tags).arg(m_cursor);
        else
            sql = QString("select * from repeats where exists ( select task_id from tasks_tags where task_id in (%0) and date = %1 )  ")
                    .arg(sql_tags).arg(time);
    }
    else if (m_viewMode == TasksFilterModel::TO_DO)
//        sql = QString("select (select count(0) from repeats r1 where r1.time_start <= r2.time_start and r1.time_start >= %0 and r1.time_start <= %1) as num, task_id, interval, state, time_start from repeats r2 where date = %2 order by time_start")
//        .arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(7).toSecsSinceEpoch()).arg(QDateTime(date).toSecsSinceEpoch());
        sql = QString("select (select count(0) from repeats r1 where r1.time_start <= r2.time_start and r1.time_start >= %0 ) as num, task_id, interval, state, time_start from repeats r2 where date = %1 order by time_start")
				.arg(dt_current.toSecsSinceEpoch()).arg(QDateTime(date, QTime()).toSecsSinceEpoch());
    else
        sql = QString("select (select count(0) from repeats r1 where r1.time_start <= r2.time_start) as num, task_id, interval, state, time_start from repeats r2 where date = %1 order by time_start")
				.arg(QDateTime(date, QTime()).toSecsSinceEpoch());
//    time = QDateTime(date).toSecsSinceEpoch();

    if(!q.exec(sql))
    {
        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError() << sql;
        return 0;
    }
//    qDebug() << "sql "  << __FILE__ << __LINE__ << ": " << sql;
//        m_lTasksParams.clear();
    while(q.next())
    {

        cur_task = new Task(this);
        cur_task->id = q.value("task_id").toInt();
        cur_task->m_index = q.value("num").toInt() - 1 + index;
        cur_task->m_interval = q.value("interval").toInt();
        cur_task->m_transfer_status = (TransferTime::e_status)(q.value("state").toInt() % 10);
        cur_task->m_workState = q.value("state").toInt() / 10;
        time = q.value("time_start").toUInt();
		cur_task->dt_current = QDateTime::fromSecsSinceEpoch(time);
        cur_task->getTask();
        ext_storage->loadTasksBody(cur_task, offset, last_key, last_ver);
        dataobj.append(cur_task);
//        sql = QString("select (select count(0) from repeats r1 where r1.time_start <= r2.time_start) as id,date from repeats r2 where time_start = %1 order by time_start;");
    }


    res.setValue(dataobj);

    m_dateTasks.insert(modelIndex,dataobj);
//    QVariant res2 = res;
//    QList<QObject*> dataobj2 = res2.value<QList<QObject*> >();
    qDebug() << "getTasksModel";


    return res;

}

void DateTasksModel::clearTasksModel(int modelIndex)
{
    QList<QObject*> dataobj_tmp;
    if(m_dateTasks.contains(modelIndex))
    {
      dataobj_tmp = m_dateTasks.take(modelIndex);
      qDeleteAll(dataobj_tmp);
      dataobj_tmp.clear();
      qDebug() << "clearDateObj";
    }
}

QHash<int, QByteArray> DateTasksModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TimeRole] = "time";
    roles[TasksRole] = "tasks";
    return roles;
}

int DateTasksModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_rowCount;
}

QVariant DateTasksModel::data(const QModelIndex &index, int role) const
{
    QDate dt_item;
    //    QMapIterator<QDateTime, int> iter(*m_curBufferTasks);
    //    for(int i = 0; i < index.row()+1; i++)
    //    {
    //        if (iter.hasNext()) {
    //            iter.next();
    ////                cout << i.key() << ": " << i.value() << endl;
    //        }
    //    }
    //    if(m_curBufferTasks->count() > 0) dt_item = iter.key();
//    dt_item = m_lDates[index.row()];
    dt_item = getDate(index.row());
    if (role == TimeRole)
        return dt_item;
    else if (role == TasksRole)
        return QVariant();
    else
        return QVariant();
}

QDate DateTasksModel::getDate(int row) const
{
//    int id = 0;
    QString sql;
    QSqlQuery q;
    QString sql_tags;
//    TaskParams task_params;
    quint32 time;
    QDate date = QDate::currentDate();
    QDateTime dt_current;
    dt_current.setDate(QDate::currentDate());
//    time = dt_current.toSecsSinceEpoch();
    if(m_viewMode == TasksFilterModel::ALL_TASKS)
    {
        date = QDate(2000,1,1);
        return date.addDays(row);
    }
    else if(m_viewMode == TasksFilterModel::TO_DO)
    {
        return date.addDays(row);
    }
    if(row < m_cursor || row >= m_cursor + 100 || m_lDates.count() == 0)
    {
        m_cursor = row - 50;
        if(m_cursor < 0) m_cursor = 0;

        if(isFiltering)
        {
            for(int i = 0;i < m_selectedFilter->count(); i++)
            {
                if(i == 0)
                    sql_tags += QString("%1").arg(m_selectedFilter->at(i));
                else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
            }
            if (m_viewMode == TasksFilterModel::TO_DO)
                sql = QString("select distinct date from repeats where time_start >= %0 and time_start <= %1 and exists ( select task_id from tasks_tags where task_id in (%2) and task_id = repeats.task_id )  order by time_start limit 100 offset %2 ")
						.arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(30).toSecsSinceEpoch()).arg(sql_tags).arg(m_cursor);
            else
                sql = QString("select distinct date from repeats where exists ( select task_id from tasks_tags where task_id in (%0) and task_id = repeats.task_id )  order by date limit 100 offset %1 ")
                        .arg(sql_tags).arg(m_cursor);
        }
        else if (m_viewMode == TasksFilterModel::TO_DO)
            sql = QString("select distinct date from repeats where time_start >= %0 and time_start <= %1 order by time_start limit 100 offset %2 ")
					.arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(30).toSecsSinceEpoch()).arg(m_cursor);
        else
            sql = QString("select distinct date from repeats order by date limit 100 offset %1").arg(m_cursor);


        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//            return 0;
        }
//        m_lTasksParams.clear();
        m_lDates.clear();
        while(q.next())
        {

           time = q.value("date").toUInt();
		   date = QDateTime::fromSecsSinceEpoch(time).date();
           m_lDates.append(date);
        }

    }

    if(row - m_cursor > m_lDates.count()-1)
        date = QDate();
    else date = m_lDates[row - m_cursor];

    return date;

}

void DateTasksModel::refresh()
{

    m_lDates.clear();
    beginResetModel();
    endResetModel();
    m_rowCount = 7;
    return;




    QSqlQuery q;
    QString sql;
    QString sql_tags;
    quint32 time;
    QDateTime dt_current;
    dt_current.setDate(QDate::currentDate());
	time = dt_current.toSecsSinceEpoch();
    m_cursor = 0;
    int count_overdue = 0;
    int n_days = 30;

    if(isFiltering)
    {
        for(int i = 0;i < m_selectedFilter->count(); i++)
        {
            if(i == 0)
                sql_tags += QString("%1").arg(m_selectedFilter->at(i));
            else sql_tags += QString(", %1").arg(m_selectedFilter->at(i));
        }
    }

    if (m_viewMode == TasksFilterModel::TO_DO)
    {
        if(isFiltering)
        {
//            QString("select count(0) from tasks where time_end < %0 and time_close = 0 and exists ( select task_id from tasks_tags where task_id in (%1) and task_id = tasks.task_id ) ")
            QString("select count(0) from tasks where time_end < %0 and exists ( select task_id from tasks_tags where task_id in (%1) and task_id = tasks.task_id ) ")
									.arg(dt_current.toSecsSinceEpoch()).arg(sql_tags);
        }
        else
//            sql = QString("select count(0) from tasks where time_end < %0 and time_close = 0 ")
            sql = QString("select count(0) from tasks where time_end < %0 ")
					.arg(dt_current.toSecsSinceEpoch());
        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
            return;
        }
        if(q.next())
            count_overdue = q.value(0).toInt();
        if(count_overdue > 0)
        {
            count_overdue = 1;
            dt_current = dt_current.addDays(1);
            n_days = 29;
        }
    }

    if(isFiltering)
    {
        if (m_viewMode == TasksFilterModel::TO_DO)
            sql = QString("select cout(distinct date) from repeats where time_start >= %0 and time_start <= %1 and exists ( select task_id from tasks_tags where task_id in (%2) and task_id = repeats.task_id ) ")
					.arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(n_days).toSecsSinceEpoch()).arg(sql_tags);
        else
            sql = QString("select count(distinct date) from repeats where exists ( select task_id from tasks_tags where task_id in (%0) and task_id = repeats.task_id )  ")
                    .arg(sql_tags);
    }
    else if (m_viewMode == TasksFilterModel::TO_DO)
        sql = QString("select count(distinct date) from repeats where time_start >= %0 and time_start <= %1 order by date")
				.arg(dt_current.toSecsSinceEpoch()).arg(dt_current.addDays(n_days).toSecsSinceEpoch());
    else
        sql = QString("select count(distinct date) from repeats order by date");

    if(!q.exec(sql))
    {
        qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
        return;
    }
    if(m_viewMode == TasksFilterModel::ALL_TASKS)
    {
        m_rowCount = 10000;
    }
    else
    {
        m_rowCount = count_overdue;
        if(q.next())
            m_rowCount += q.value(0).toInt();
    }
    //    m_idTasks.clear();
//    m_lTasksParams.clear();

    m_lDates.clear();
//    getDate(0);
//    if(m_activeId == -1 && m_activeTask)
//    {
//        if (!q.prepare(QLatin1String("select id from tasks where task_key=:task_key and flag != 3" )))
//        {
//            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//        }
//        q.bindValue(":task_key",m_activeTask->key.toRfc4122());
//        if(!q.exec())
//        {
//            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
//        }
//        else
//        {
//            if(q.next())
//                m_activeId = q.value(0).toInt();
//        }
//    }

//    m_rowCount = m_lTasksParams.count();
    beginResetModel();
    endResetModel();

}




TrashFilterModel::TrashFilterModel(QObject *parent)
{
    m_rowCount = 0;
//    m_activeTask = 0;
    m_activeTask = 0;
    m_activeType = 0;
    m_activeId = 0;
    m_selectedFilter = 0;
    isFiltering = false;
}

int TrashFilterModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_rowCount;
}

QVariant TrashFilterModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_rowCount)
        return QVariant();
    QSqlQuery q;
    QString sql;
//    quint64 key = Q_UINT64_C(0x3847afdb4cb9e20d);
    quint64 key = Q_UINT64_C(0x4947afdb4cb9e20d);

    quint64 key_task = Q_UINT64_C(0x4947afdb4cb9e20d);
    quint8 state;

    quint64 new_key;
    SimpleCrypt crypto;
    int id;
    int type;
    int row = index.row();
//    if(m_viewNewNote) index.row() > 0 ? id = getId(index.row() -1) : id = -1;
//    else
    id = getId(row);
    if(row - m_cursor > m_typeTasks.count()-1)
        type = 0;
    else type = m_typeTasks[row - m_cursor];

    if(type == 0)
      sql = QString("select title,time_modify,task_key,data,state,flag from tasks where id=%1 and repeat_state = 0").arg(id);
    if(type == 1)
      sql = QString("select title,time_modify,task_key,data,state,flag from tasks where id=%1 and repeat_state > 0").arg(id);
    if(id != -1)
    {
        if(role != TypeRole)
        {
            if(!q.exec(sql))
            {
                qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
                return QVariant();
            }
            q.next();
        }
        if (role == TitleRole)
        {
            new_key = q.value(3).toLongLong();
            state = new_key - (new_key/10*10+1);
            new_key -= state;
            new_key = keyData ^ new_key;
            key = key ^ new_key;

//            new_key = keyData ^ q.value(3).toLongLong();
//            key = key ^ new_key;
            if(type == 1)
                key = key_task ^ new_key;
            crypto.setKey(getKey(key,q.value(2).toByteArray().data()));
            return crypto.decryptToString(q.value(0).toByteArray());
        }
        else if (role == TimeRole)
			return QDateTime::fromSecsSinceEpoch(q.value(1).toUInt());
        else if(role == StateRole)
        {
            return q.value(4).toInt();
        }
        else if(role == FlagRole)
            return q.value(5).toInt();
        else if(role == TypeRole)
            return type;
    }
    else
    {
        if(type == 0)
        {
            if (role == TitleRole)
            {
                return m_activeTask->m_title;
            }
            else if (role == TimeRole)
                return m_activeTask->dt_history;
            else if (role == StateRole)
                return m_activeTask->security;
            else if (role == FlagRole)
                return m_activeTask->flag;
        }
        else if(type == 1)
        {
            if (role == TitleRole)
            {
                return m_activeTask->m_title;
            }
            else if (role == TimeRole)
                return m_activeTask->dt_history;
            else if (role == StateRole)
                return m_activeTask->security;
            else if (role == FlagRole)
                return m_activeTask->flag;
        }

    }
    return QVariant();

}

int TrashFilterModel::getId(int row) const
{
    int id = 0;
    QString sql;
    QSqlQuery q;
    QString sql_tags;
    if(row < m_cursor || row > m_cursor + 1000 || m_idTasks.count() == 0)
    {
        m_cursor = row - 500;
        if(m_cursor < 0) m_cursor = 0;
//        sql = QString("select id,time_modify,0 from notes where flag=3 union all select id,time_modify,1 from tasks where flag=3 order by time_modify desc limit 1000 offset %1").arg(m_cursor);
        sql = QString("select id,time_modify,repeat_state from tasks where flag=3 order by time_modify desc limit 1000 offset %1").arg(m_cursor);
        if(isFiltering)
        {
            for(int i = 0;i < m_selectedFilter->count(); i++)
            {
                if(i == 0)
                    sql_tags += QString("tag_id = %1").arg(m_selectedFilter->at(i));
                else sql_tags += QString(" or tag_id = %1").arg(m_selectedFilter->at(i));
            }
//            sql = QString("select id,time_modify,0 from notes where exists ( select id_note from notes_tags where (%0) and id_note = notes.id ) union all "
//                          "select id,time_modify,0 from tasks where exists ( select task_id from tasks_tags where (%0) and task_id = tasks.id ) "
//                          " order by time_history desc limit 1000 offset %1 ")
            sql = QString( "select id,time_modify,repeat_state from tasks where exists ( select task_id from tasks_tags where (%0) and task_id = tasks.id ) "
                          " order by time_history desc limit 1000 offset %1 ")
                    .arg(sql_tags).arg(m_cursor);

            //            sql = QString("select id from notes_tags,notes where id_note = id and (%0) order by time_modify desc limit 1000 offset %1 ")
            //                    .arg(sql_tags).arg(m_cursor);
        }
        if(!q.exec(sql))
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
            return 0;
        }
        m_idTasks.clear();
        m_typeTasks.clear();
        while(q.next())
        {
            id = q.value(0).toInt();
            //            if(!m_idNotes.contains(id))
            m_idTasks.append(id);
            id = q.value(2).toInt();
            if(id > 0)
                id = 1;
            m_typeTasks.append(id);
        }

    }

    if(row - m_cursor > m_idTasks.count()-1)
        id = 0;
    else id = m_idTasks[row - m_cursor];

    return id;

}

//Task *TrashFilterModel::getTask(int index)
//{
//    int id = 0;
//    int row = index - m_cursor;
//    if(row >= 0 && row < m_idTasks.count() )
//    {
//        id = m_idTasks[row];
//        delete m_activeTask;
//        m_activeTask = new Task();
//        m_activeTask->id = id;
//        m_activeTask->getTask();

//    }
//    return m_activeTask;
//}

int TrashFilterModel::getType(int index)
{
    int res = -1;
    int row = index - m_cursor;
    if(row >= 0 && row < m_typeTasks.count() )
    {
        res = m_typeTasks[row];
    }
    return res;
}

Task *TrashFilterModel::getTaskById(int idTask)
{
    delete m_activeTask;
    m_activeTask = new Task();
    m_activeTask->id = idTask;
    m_activeTask->getTask();
    return m_activeTask;
}

void TrashFilterModel::refresh()
{
    QSqlQuery q;

    m_cursor = 0;

    m_idTasks.clear();
    getId(0);
    if(m_activeId == -1)
    {
        if(m_activeTask && m_activeType == 1)
        {
            if (!q.prepare(QLatin1String("select id from tasks where task_key=:task_key" )))
            {
                qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
            }
            q.bindValue(":task_key",m_activeTask->key.toRfc4122());
        }
        else if(m_activeTask && m_activeType == 0)
        {
            if (!q.prepare(QLatin1String("select id from notes where task_key=:task_key" )))
            {
                qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
            }
            q.bindValue(":task_key",m_activeTask->key.toRfc4122());
        }
        if(!q.exec())
        {
            qDebug() << "SQL_Err " << __FILE__ << __LINE__ << ": " << q.lastError();
        }
        else
        {
            if(q.next())
                m_activeId = q.value(0).toInt();
        }
    }

    m_rowCount = m_idTasks.count();
    beginResetModel();
    endResetModel();
}

int TrashFilterModel::indexOf(Task *task)
{
    if(m_activeId == -1)
        return 0;
    return m_idTasks.indexOf(m_activeId);

}

Task *TrashFilterModel::getTask(int index, Task *source)
{
    Task *res = 0;
    int id = 0;
    int row = index - m_cursor;
    if(row >= 0 && row < m_idTasks.count() )
    {

        if(!source)
        {
            id = m_idTasks[row];
            delete m_activeTask;
            m_activeTask = new Task();
            m_activeTask->id = id;
            res = m_activeTask;
        }
        else res = source;
        res->getTask();

//        QMapIterator<QDateTime, int> iter(m_bufferTasks);
//        for(int i = 0; i < index+1; i++)
//        {
//            if (iter.hasNext()) {
//                iter.next();
//            }
//        }
//        res->dt_current = res->dt_end = res->dt_prev = iter.key();
//        QList<QDateTime> ldateTimes = m_bufferTasks.keys(id);
//        row = ldateTimes.indexOf(res->dt_current);
//        if(row > 0)
//            res->dt_prev = ldateTimes[row - 1];
//        if(ldateTimes.count() > row + 1)
//            res->dt_next = ldateTimes[row + 1];



    }
    return res;

}

QHash<int, QByteArray> TrashFilterModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[TimeRole] = "time";
    roles[StateRole] = "stateNote";
    roles[FlagRole] = "flag";
    roles[TypeRole] = "type";
    return roles;

}



bool TransferTime::operator ==(const TransferTime &other) const
{
    bool res = true;
    if(m_status != other.m_status || dt_start != other.dt_start || dt_end != other.dt_end
            || dt_start_new != other.dt_start_new || dt_end_new != other.dt_end_new)
        res = false;
    return res;
}

//PDocument::PDocument(QObject *parent) : QObject(parent)
//{
//    k = 10;
//    lBlocks = new QList<Block*>;
//}


EJDBTasksFilterModel::EJDBTasksFilterModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_activeTask = 0;
    m_activeModelIndex = -1;
    m_viewNewTask = false;
    isFiltering = false;
//    m_templateFilter.append(8);
//    m_templateFilter.append(1);
//    m_selectedFilter = &m_templateFilter;
    m_filter = nullptr;
    m_rowCount = 0;
    m_changedSearch = true;
    m_connectionMode = OFFLINE_MODE;
    m_isAbsentData = false;
}

EJDBTasksFilterModel::~EJDBTasksFilterModel()
{
    delete m_activeTask;
    m_activeTask = NULL;
}

int EJDBTasksFilterModel::rowCount(const QModelIndex &parent) const
{
    int res = 0;
//    if(m_connectionMode == ONLINE_MODE)
//    {
//        res = m_lTasksParams.count();
//    }
//    else
//    {
//        res = m_listId.count();
//    }
    res = m_lTasksOnline.count() + m_rowCount;
    return res;
}

QVariant EJDBTasksFilterModel::data(const QModelIndex &index, int role) const
{
//    QLocale locale = QLocale::system();
    int id = -1;
    int n;
    int propIndex = index.row();
//    quint32 time;
//    QString resStr;
//    char *chr;
//    int len;
    static Task task;
    Task *curTask;
    bool checketEnabled, activeCategory;
    bool isIndexOnline;

//    if(m_viewNewTask) index.row() > 0 ? n = index.row() -1 : n = -1;
//    else n = index.row();
//    if(m_viewNewTask) index.row() > 0 ? id = getId(index.row() -1) : id = -1;
//    else id = getId(index.row());
////    if(m_viewNewTask) index.row() > 0 ? id = index.row() : id = -1;
////    else id = index.row() + 1;

    if(index.row() < m_rowCount)
    {
        if(m_viewNewTask) index.row() > 0 ? n = index.row() - 1 : n = - 1;
        else n = index.row();
        if(m_viewNewTask) index.row() > 0 ? id = getId(index.row() - 1) : id = -1;
        else id = getId(index.row());
        curTask = &task;
        isIndexOnline = false;
    }
    else
    {
        if(m_viewNewTask) n = index.row() - m_rowCount - 1;
        else n = index.row() - m_rowCount;
        id = -1;
        if(n < 0)
            return QByteArray();
        curTask = m_lTasksOnline[n];
        isIndexOnline = true;
    }

    if(role == TaskId)
        return id;
    if(role == IsInSearch)
    {
            return true;
    }
//    if(id != -1 && id != 0)
    if((isIndexOnline && n > -1) || (!isIndexOnline && id > 0))
    {
        if(isIndexOnline)
        {

        }
        else if(task.id != id) {
            task.id = id;
//            EjDb::Instance()->loadTask(&task);
        }

        if(role == TitleRole)
        {
//            chr =  EjDb::Instance()->documents()->find_info(id,len,false);
//            resStr = QString::fromUtf8(chr,len);
//            return resStr;
            return curTask->title();
        }
        else if(role == RegNumRole)
        {
            return curTask->regNum();
        }
        else if (role == WorkState)
        {
            return curTask->m_workState;
        }
        else if (role == WorkStateColor)
        {
            return curTask->getWorkStateColor();
        }
        else if (role == WorkStateBorderColor)
        {
            return curTask->getWorkStateBorderColor();
        }
        else if (role == TimeRole)
        {
//            return curTask->fromTime();   //startTime();
            QDateTime resDt;
            if(curTask->isDocument())
            {
                resDt = curTask->registryTime();
            }
            else
            {
                resDt = curTask->endTime();
            }
            return resDt;
        }
        else if (role == TimeTitleRole)
        {
            QString res;
            if(curTask->isDocument())
            {
                if(curTask->m_regNum.isEmpty())
                    res = tr("Create date");
                else
                    res = tr("Registration date");
            }
            else
            {
//                if(curTask->m_workState == Task::WS_FINISH)
//                    res = tr("Date finish");
//                else
//                    res = tr("Date close");
                res = tr("Finish date");
            }
            return res;
        }
        else if (role == TimeDateRole)
			return QDateTime(curTask->startTime().date(), QTime());
        else if (role == TimeStringRole)
        {
            QString str = "";
            if(curTask->getRepeatModels()->activeCategory() > 0)
            {
                QDateTime dt_start = curTask->startTime();
                QDateTime dt_end = curTask->endTime();
                QString str2 = "dd MMM hh:mm";
                if(dt_start.date().year() != QDate::currentDate().year())
                    str2 = "dd MMM yyyy hh:mm";
                if(dt_start.date() == dt_end.date())
                {
                    if(dt_start == dt_end)
                    {
                        str = dt_start.toString(str2);
                    }
                    else
                        str = dt_start.toString(str2) +  " → " + dt_end.toString("hh:mm");
                }
                else
                   str = dt_start.toString(str2) +  " → " + dt_end.toString(str2);
            }
            return str;
        }
        else if(role == IsOverdue)
        {

//            if(n > -1 && m_viewMode != ALL_DOCS && m_viewMode != DOCS_AND_TASKS)
//                return  m_lTasksParams.at(n).dt_overdue !=  m_lTasksParams.at(n).dt_current;
//            else if(n > -1)
////                return false;
//                return  m_lTasksParams.at(n).work_state < 2 && m_lTasksParams.at(n).dt_overdue <  QDateTime::currentDateTime();
//            else return false;
            return curTask->isOverdue();
        }
        else if(role == IsCloseVisible)
        {
            return curTask->getRepeatModels()->checkedEnabled() &&
                    (curTask->getRepeatModels()->activeCategory() == RepeatModels::NO_REPEAT ||
                     curTask->getRepeatModels()->activeCategory() == RepeatModels::NO_TIME);
        }
        else if (role == IsDeletingRole)
        {
            return curTask->flag;
        }
        else if (role == IsDocument)
        {
            return curTask->isDocument();
        }
        else if (role == IsHaveAgreeding)
        {
            return curTask->isHaveAgreeding();
        }
        else if (role == SecurityRole)
        {
            return curTask->security;
        }
        else if (role == WorkState)
        {
            return curTask->workState();
        }
        else if (role == WorkStateName)
        {
            return curTask->getWorkStateName();
        }
        else if (role == IsReading)
        {
            return curTask->isReading();
        }
        else if (role == IsHaveReply)
        {
            return curTask->isHaveReply();
        }
        else if (role == AuthorName)
        {
            return curTask->nameAuthor();
        }
        else if (role == ShortAuthorName)
        {
            return curTask->shortNameAuthor();
        }
        else if (role == ResponsibleName)
        {
            return curTask->nameResponsible();
        }
        else if (role == ShortResponsibleName)
        {
            return curTask->shortNameResponsible();
        }
    }
    return QVariant();
}

void EJDBTasksFilterModel::refresh()
{
//    //m_connectionMode = ONLINE_MODE;
//    if(m_connectionMode == ONLINE_MODE)
//    {
//        qDeleteAll(m_lTasksOnline);
//    //    while(m_lTasksOnline.count() > 0)
//    //    {
//    //        Task *task = m_lTasksOnline.takeLast();
//    //        task->deleteLater();
//    //    }
//        m_lTasksOnline.clear();
//        m_listId.clear();
//        m_rowCount = 0;
//        m_offset = 0;
//        m_lastNumber = m_rowCount;
//        m_changedSearch = true;
//        emit getTasksModelSignal(m_lastNumber, m_offset);

//        beginResetModel();
//        endResetModel();
//        return;
//    }

////    MBlockDoc *blockdoc = EjDb::Instance()->blockDoc();

////    if(!blockdoc)
////        return;

//    int len = EjDb::Instance()->documents()->get_maxKey() + 1;

//    MComplexMask *p_cmask = new MComplexMask(len,len);
//    LogicOperator mask_logic = LogicOperator::NONE;
//    QList<LogicOperator> list_logic;

//    m_listId.clear();
//    m_rowCount = 0;

//    if(m_filter && m_filter->activeNavigation() == Filter::AN_CALENDAR)
//    {
//        return;
//    }

////    p_cmask->nullify(nullptr);
//    p_cmask->setCount(len);

//    if(m_filter && !m_filter->searchString().isEmpty())
//    {
//        p_cmask->nullify(nullptr);
//        QList<int> lRes;
//        p_cmask->toArray(lRes);

//        mask_logic = LogicOperator::NONE;
//        QList<QString> strlist_result;
//        QList<QString> strlist_words = m_filter->searchString().split(" ");
//        strlist_words.removeAll("");

////        QList<QString> strlist_words = m_searchString.split(QRegExp("\\W+"), QString::SkipEmptyParts);
//        if(!strlist_words.isEmpty())
//            blockdoc->find_by3gramms(Definition::Word, strlist_words[0], strlist_result);
//        for(int i = 0;i<strlist_result.length();i++)
//        {
//            //        if(i==0)
//            //            list_logic.append(LogicOperator::NEGATION);
//            //        else
//            list_logic.append(LogicOperator::NONE);
//            if(i < strlist_result.length()-1)
//                list_logic.append(LogicOperator::OR);
////            list_logic.append(LogicOperator::AND);

//        }
//        if(!strlist_result.isEmpty())
//            blockdoc->find_by_words(strlist_result,list_logic,p_cmask, mask_logic);
//        p_cmask->setwrittendecompressedbitcount(p_cmask->count());
//         p_cmask->toArray(lRes);

//        mask_logic = LogicOperator::OR;
//        strlist_result.clear();
//        if(!strlist_words.isEmpty())
//            blockdoc->find_by3gramms(Definition::Attribute, strlist_words[0], strlist_result);
//       for(int i = 0;i<strlist_result.length();i++)
//       {
//           //        if(i==0)
//           //            list_logic.append(LogicOperator::NEGATION);
//           //        else
//           list_logic.append(LogicOperator::NONE);
//           if(i < strlist_result.length()-1)
//               list_logic.append(LogicOperator::OR);
////            list_logic.append(LogicOperator::AND);

//       }
//       if(!strlist_result.isEmpty())
//           blockdoc->find_by_attrs(strlist_result,list_logic,p_cmask, mask_logic);

//       p_cmask->setwrittendecompressedbitcount(p_cmask->count());
//        p_cmask->toArray(lRes);
////        QStringList strlist_attr;
////        list_logic.clear();
////        list_logic.append(LogicOperator::NONE);
////        strlist_attr << m_searchString;
////        blockdoc->find_by_attrs(strlist_attr,list_logic,p_cmask, mask_logic);

//    }
//    else
//    {
//        p_cmask->unnullify();
//    }

////    if(m_filter && m_filter->viewMode() != Filter::DOCS_AND_TASKS)
//    if(m_filter)
//    {
//        mask_logic = LogicOperator::AND;
//        list_logic.clear();
//        QStringList strlist_tags;

//        if(m_filter->enabled())
//        {
//            QList<int> lSyncTags;

//            m_filter->getFromList(lSyncTags);
//            for(int i = 0; i < lSyncTags.count(); i++)
//            {
//                if(!strlist_tags.isEmpty())
//                    list_logic.append(LogicOperator::OR);
//                list_logic.append(LogicOperator::NONE);
//                strlist_tags << "t" + QString::number(lSyncTags[i]);
//            }

//            m_filter->getExactlyFilter(lSyncTags);
//            for(int i = 0; i < lSyncTags.count(); i++)
//            {
//                if(!strlist_tags.isEmpty())
//                    list_logic.append(LogicOperator::AND);
//                list_logic.append(LogicOperator::NONE);
//                strlist_tags << "t" + QString::number(lSyncTags[i]);
//            }

//            m_filter->getNotExactlyFilter(lSyncTags);
//            for(int i = 0; i < lSyncTags.count(); i++)
//            {
//                if(!strlist_tags.isEmpty())
//                    list_logic.append(LogicOperator::AND);
//                list_logic.append(LogicOperator::NEGATION);
//                strlist_tags << "t" + QString::number(lSyncTags[i]);
//            }

//        }

//        if(m_filter && m_filter->viewMode() == Filter::ALL_DOCS) {
//            if(!strlist_tags.isEmpty())
//                list_logic.append(LogicOperator::AND);
//            list_logic.append(LogicOperator::NONE);
//            strlist_tags<<"sysIsDoc";// << "Open";
//            //    list_logic.append(LogicOperator::OR);
//            //    list_logic.append(LogicOperator::NEGATION);
//        }
//        else if(m_filter && m_filter->viewMode() == Filter::ALL_TASKS) {
//            if(!strlist_tags.isEmpty())
//                list_logic.append(LogicOperator::AND);
//            list_logic.append(LogicOperator::NEGATION);
//            strlist_tags<<"sysIsDoc";// << "Open";
//        }
//        else if(m_filter && m_filter->viewMode() == Filter::TASKS_FOR_MY) {
//            if(!strlist_tags.isEmpty())
//                list_logic.append(LogicOperator::AND);
//            list_logic.append(LogicOperator::NONE);
//            strlist_tags<<"sysIsForMy";
//        }
//        else if(m_filter && m_filter->viewMode() == Filter::TASKS_FROM_MY) {
//            if(!strlist_tags.isEmpty())
//                list_logic.append(LogicOperator::AND);
//            list_logic.append(LogicOperator::NONE);
//            strlist_tags<<"sysIsFromMy";
//        }
//        //    listint = p_mask->toArray();
////        list_logic.clear();
////        strlist_tags.clear();


//        if(!strlist_tags.isEmpty())
//            blockdoc->find_by_tags(strlist_tags,list_logic,p_cmask,mask_logic);
//        //    listint = p_mask->toArray();
//    }


////    QList<int> lRes = p_cmask->toArray();
//    QList<int> lRes;
//    p_cmask->setwrittendecompressedbitcount(p_cmask->count());
//    p_cmask->toArray(lRes);

////    M_BytesInt_Base *changes = EjDb::Instance()->changes();
////    QList<int> pages = changes->get_sortedIntPagesPositions();
//    M_BytesInt_Base *fromSorting = EjDb::Instance()->fromSorting();
//    QList<int> pages = fromSorting->get_sortedIntPagesPositions();
//    QList<int>::iterator it;
//    std::sort(lRes.begin(),lRes.end());
//    for(int i = 0; i < pages.count(); i++)
//    {
//       QList<MMemoryChunkPointer> lKeys;
////       changes->get_intPageByteKeys(pages.at(pages.count() - i - 1),lKeys);
//       lKeys = fromSorting->get_intPageByteKeys(pages.at(pages.count() - i - 1));

//       for(int j = 0; j < lKeys.count(); j++)
//       {
//          char *p_chr = lKeys[lKeys.count() - j - 1].get_pointer();
//          int id = *((int*)(p_chr)) - 1;

////          int arr[] = {1,7,9,23,34,47,67,89,123,234,345,567};
////              int len = sizeof(arr)/sizeof(arr[0]);
////              std::vector<int> v(arr,arr+len);

////              std::vector<int>::iterator upper;
////              upper = std::upper_bound(v.begin(), v.end(), 123);

//////          std::vector<int>::iterator it = std::lower_bound(lRes.begin(), lRes.end(), id);
//////          int * p = std::lower_bound(lRes.begin(), lRes.end(), id);
//////          if(std::binary_search(lRes.begin(), lRes.end(), id))
//          QList<int>::iterator it = std::lower_bound(lRes.begin(), lRes.end(), id);
//          if(it != lRes.end() && id == *it)
// //         if(it != lRes.end())
//       //   if(lRes.contains(id))
//          {
//              qDebug() << *it;
//              m_listId.append(id);
////              int index = lRes.indexOf(id);
//              int index = it - lRes.begin();
//              if(index > -1)
//                  lRes.takeAt(index);
//          }
//       }

//    }
////    m_listId.append(lRes);

////    m_listId = p_cmask->toArray();
////    m_listId.clear(); // for test
////    while(lRes.count() > 0)
////    {
////        m_listId.append(lRes.takeLast());
////    }
////    m_listId.clear();
//    m_rowCount = m_listId.count();
//    if(m_viewNewTask)
//        m_rowCount++;

//    delete p_cmask;

//    qDeleteAll(m_lTasksOnline);
////    while(m_lTasksOnline.count() > 0)
////    {
////        Task *task = m_lTasksOnline.takeLast();
////        task->deleteLater();
////    }
//    m_lTasksOnline.clear();
//    m_offset = 0;
//    m_lastNumber = m_rowCount;
//    m_changedSearch = true;
////    emit getTasksModelSignal(m_lastNumber, m_offset);

//    beginResetModel();
//    endResetModel();
}

QVariantMap EJDBTasksFilterModel::getData(int row)
{
    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;
    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
        //cout << i.key() << ": " << i.value() << endl;
    }
    return res;
}

int EJDBTasksFilterModel::getId(int row) const
{
    int res = -1;
    if(row > -1 && row < m_listId.count())
        res = m_listId.at(row) + 1;
    return res;
}

int EJDBTasksFilterModel::getRow(int id) const
{
    int res = - 1;
//    if(m_connectionMode == ONLINE_MODE)
//    {
//        QUuid key = EjDb::Instance()->getTaskKey(id);
//        for(int i = 0; i < m_lTasksOnline.count(); i++)
//        {
//            if(m_lTasksOnline[i]->key == key)
//            {
//                res = i;
//                break;
//            }
//        }
//    }
//    else
//        res = m_listId.indexOf(id - 1);
    return res;
}

void EJDBTasksFilterModel::prependTask(Task *activeTask)
{
//    if(m_connectionMode == ONLINE_MODE)
//    {
//        Task *task = new Task();
//        task->id  = activeTask->id;
//        EjDb::Instance()->loadTask(task);
//        task->copyBaseData(activeTask);
//        m_lTasksOnline.prepend(task);
//    }
//    else
//        m_listId.prepend(activeTask->id - 1);
}

Task *EJDBTasksFilterModel::getTask(int index, Task *source)
{
    if(m_viewNewTask && !source)
        return m_activeTask;
    if(m_viewNewTask || index < 0)
        return source;

    Task *res = nullptr;
    if(!source)
    {
        res = m_activeTask;
        m_activeTask = new Task();
        m_activeTask->key = QUuid::createUuid();
//        delete res;
		res->deleteLater();
        res = m_activeTask;
    }
    else res = source;
    res->m_index = index;
    res->id = getId(index);
    if(index < m_rowCount)
    {
//        EjDb::Instance()->loadTask(res);
    }
    else
    {
        int n = 0;
        if(m_viewNewTask) n = index - m_rowCount - 1;
        else n = index - m_rowCount;
        if(n < m_lTasksOnline.count())
        {
//            int id = res->id  = EjDb::Instance()->getTaskId(m_lTasksOnline[n]->key);
//            EjDb::Instance()->loadTask(res);
            quint32 offset = res->offset_history;
            res->copyBaseData(m_lTasksOnline[n]);
            res->offset_history = offset;
//            res->id = id;
        }
//        res->m_repeatModels = m_lTasksOnline[n]->m_repeatModels;
    }

    //    if(index < m_lTasksOnline.count())
//        res->key = m_lTasksOnline[index]->key;

//    res->dt_current = res->dt_end = res->dt_end_interval = res->dt_prev = m_lTasksParams.at(index - m_cursor).dt_current;
//    //        QList<QDateTime> ldateTimes = m_bufferTasks.keys(id);
//    QList<QDateTime> ldateTimes;

//    for(int i = 0; i < m_lTasksParams.count(); i++)
//    {

//        if(m_lTasksParams.at(i).id == res->id)
//            ldateTimes.append(m_lTasksParams.at(i).dt_current);
//        //                cout << i.key() << ": " << i.value() << endl;
//    }


//    int row = ldateTimes.indexOf(res->dt_current);
//    if(row > 0)
//        res->dt_prev = ldateTimes[row - 1];
//    if(ldateTimes.count() > row + 1)
//        res->dt_next = ldateTimes[row + 1];

	res->m_interval = res->endTime().toSecsSinceEpoch() - res->startTime().toSecsSinceEpoch();
    res->dt_end_interval = res->dt_end;

    //res->getTask();
    return res;
}

void EJDBTasksFilterModel::copyBaseData(int index, Task *source)
{
    if(m_filter && m_filter->activeNavigation() == Filter::AN_CALENDAR)
    {
        QList<int> keys = m_dateTasks.keys();
        for(int i = 0; i < keys.count(); i++)
        {
            QList<QObject*> listObj = m_dateTasks.value(keys[i]);
            for(int j = 0; j < listObj.count(); j++)
            {
                Task *task = qobject_cast<Task*>(listObj[j]);
                if(task && task->id == source->id)
                {
                    task->copyBaseData(source);
                    task->workStateChanged();
                    task->titleChanged();
                }
            }
        }
    }
    else
    {
    if(index < m_rowCount)
    {
//        EjDb::Instance()->loadTask(res);
    }
    else
    {
        int n = 0;
        if(m_viewNewTask) n = index - m_rowCount - 1;
        else n = index - m_rowCount;
        if(n < m_lTasksOnline.count())
        {
            m_lTasksOnline[n]->copyBaseData(source);
        }
//        res->m_repeatModels = m_lTasksOnline[n]->m_repeatModels;
    }
    }
}

Task *EJDBTasksFilterModel::getFullTask(int index)
{
    quint32 offset;
    qint16 last_key;
    quint16 last_ver;
    getTask(index);
    if(m_activeTask && m_activeTask->m_doc->lBlocks->size() == 0)
        ext_storage->loadTasksBody(m_activeTask, offset, last_key, last_ver);
//    if(m_activeTask)
//    {
//        foreach (SubTask *curSubTask, m_activeTask->m_lSubTasks) {
//            ext_storage->loadTasksBody(curSubTask,offset,last_key, last_ver);
//        }
//    }
    return m_activeTask;
}

Task *EJDBTasksFilterModel::addTask(QDate date)
{
    if(!m_activeTask || m_activeTask->id > -1)
    {
        delete m_activeTask;
        m_activeTask = new Task();
        m_activeTask->key = QUuid::createUuid();
    }
    m_activeTask->dt_modify = m_activeTask->dt_start = QDateTime::currentDateTime();
    int h = m_activeTask->dt_start.time().hour();
    m_activeTask->dt_start.setTime(QTime(h,0));
    m_activeTask->dt_start.setDate(date);
    qint64 mSec = m_activeTask->dt_start.toMSecsSinceEpoch();
    m_activeTask->dt_start.setMSecsSinceEpoch(mSec + 3600*1000);
    m_activeTask->dt_end = m_activeTask->dt_end_interval =  m_activeTask->dt_current = m_activeTask->dt_start;
    //    m_activeTask->dt_end.addSecs(3600);
    m_viewNewTask = true;
    m_activeId = -1;
    return m_activeTask;
}

int EJDBTasksFilterModel::indexOf(Task *task)
{
    int res = -1;
    if(task)
        res = getRow(task->id);
//        res = task->m_index;
    return res;
}

int EJDBTasksFilterModel::indexOfSection(QDate section)
{
    return 0;
}

int EJDBTasksFilterModel::selectDate(QDate date)
{
    return -1;
}

int EJDBTasksFilterModel::countTasks(QDate date)
{
    return 0;
}


QVariant EJDBTasksFilterModel::getTasksModel(QDate date, int modelIndex)
{
//    if(!EjDb::Instance()->blockDoc())
//        return QVariant();
//    QDateTime datetime_1 = QDateTime(date);
//    QDateTime datetime_2 = QDateTime(date);
//    datetime_2.setTime(QTime(23,59,59));
//    qDebug() << __FILE__ << __LINE__  << datetime_1 << datetime_2;

//    if(m_connectionMode == ONLINE_MODE)
//    {
//        QList<QObject*> dataobj;
//        if(m_dateTasks.contains(modelIndex))
//        {
//          dataobj =m_dateTasks.take(modelIndex);
//          qDeleteAll(dataobj);
//          dataobj.clear();
//        }
//        m_dateTasks.insert(modelIndex,dataobj);

//        emit getTasksModelSignal(modelIndex, 0);
//        return QVariant();
//    }


//    QVariant res;
//    qint64 isf_mt;
////    int int_delta,
////            i_intervallen = 8000000,
////            i_intervalsqua = 3;
//    QList<QDateTime> list_intervals;
//    LogicOperator mask_logic = LogicOperator::NONE;
//    QList<LogicOperator> list_logic;
//    QList<int> listint;
//    QList<Task *> lTasks;
//    int fileIndex;
//    QList<MDateTimeInterval> lIntervals;
//    MDateTimeInterval interval;
//    Task *curTask;
//    bool checkEnabled;
//    qint8 activeCategory;
//    QList<QObject*> dataobj;
//    QList<QObject*> dataobj_tmp;



////    QDateTime datetime_1 = QDateTime(QDate(1970,1,1));

//    MComplexMask*p_mask = new MComplexMask(100,100);
//    p_mask->nullify(nullptr);


//    mask_logic = LogicOperator::NONE;
//    list_logic.append(LogicOperator::NONE);
////    list_logic.append(LogicOperator::AND);
//    list_intervals.append(datetime_1);
//    list_intervals.append(datetime_2);

//    EjDb::Instance()->blockDoc()->find_by_intervals(list_intervals,list_logic,p_mask,mask_logic);
//    p_mask->setwrittendecompressedbitcount(p_mask->count());
//    p_mask->toArray(listint);

//    if(m_filter && m_filter->viewMode() != Filter::DOCS_AND_TASKS)
//    {
//        QStringList strlist_tags;
//        MBlockDoc *blockdoc = EjDb::Instance()->blockDoc();

//        mask_logic = LogicOperator::AND;
//        list_logic.clear();

//        if(m_filter->viewMode() == Filter::ALL_DOCS) {
//            strlist_tags<<"sysIsDoc";// << "Open";
//            list_logic.append(LogicOperator::NONE);
//            //    list_logic.append(LogicOperator::OR);
//            //    list_logic.append(LogicOperator::NEGATION);
//        }
//        else if(m_filter->viewMode() == Filter::ALL_TASKS) {
//            strlist_tags<<"sysIsDoc";// << "Open";
//            list_logic.append(LogicOperator::NEGATION);
//        }
//        else if(m_filter->viewMode() == Filter::TASKS_FOR_MY) {
//            strlist_tags<<"sysIsForMy";
//            list_logic.append(LogicOperator::NONE);
//        }
//        else if(m_filter->viewMode() == Filter::TASKS_FROM_MY) {
//            strlist_tags<<"sysIsFromMy";
//            list_logic.append(LogicOperator::NONE);
//        }
//        if(!strlist_tags.isEmpty())
//            blockdoc->find_by_tags(strlist_tags,list_logic,p_mask,mask_logic);
//    }


//    p_mask->setwrittendecompressedbitcount(p_mask->count());
//    p_mask->toArray(listint);

//    for(int i = 0; i < listint.count(); i++)
//    {
//        fileIndex = listint.at(i);

////        curTask = new Task(this);
////        curTask->id = fileIndex + 1;
////        EjDb::Instance()->loadTask(curTask);
////        curTask->dt_current =  curTask->dt_start;
////        curTask->dt_end_interval = curTask->dt_end;
////        curTask->m_interval = curTask->dt_end_interval.toSecsSinceEpoch() - curTask->dt_current.toSecsSinceEpoch();
////        dataobj.append(curTask);

//        lIntervals = EjDb::Instance()->blockDoc()->findMDateTimeIntervals(fileIndex);
//        for(int j = 0; j < lIntervals.count(); j++)
//        {
//            curTask = new Task(this);
//            curTask->id = fileIndex + 1;
//            EjDb::Instance()->loadTask(curTask);
//            interval = lIntervals.at(j);
//            curTask->dt_current =  QDateTime::fromSecsSinceEpoch(interval.get_beg());    //curTask->dt_start;
//            curTask->dt_end_interval = QDateTime::fromSecsSinceEpoch(interval.get_end()); //curTask->dt_end;
//			curTask->m_interval = curTask->dt_end_interval.toSecsSinceEpoch() - curTask->dt_current.toSecsSinceEpoch();
//            qDebug() << __FILE__ << __LINE__ << curTask->dt_current << curTask->dt_end_interval;
////            curTask->m_isClosed = interval.get_info() >> 7;
////            curTask->m_isOverdue = (curTask->m_isClosed < 2  && curTask->dt_end < QDateTime::currentDateTime());
//            if(m_activeTask && curTask->id == m_activeTask->id && curTask->dt_current == m_activeTask->dt_current)
//                m_activeTask->m_index = curTask->m_index;
//    //        ext_storage->loadNotesBody(cur_task, offset, last_key, last_ver);
//            dataobj.append(curTask);
//        }
//    }

//    delete p_mask;


//    res.setValue(dataobj);
//    if(m_dateTasks.contains(modelIndex))
//    {
//      dataobj_tmp =m_dateTasks.take(modelIndex);
//      qDeleteAll(dataobj_tmp);
//      dataobj_tmp.clear();
//      qDebug() << "clearDateObj";
//    }
//    m_dateTasks.insert(modelIndex,dataobj);
////    QVariant res2 = res;
////    QList<QObject*> dataobj2 = res2.value<QList<QObject*> >();
//    qDebug() << "getTasksModel";


//    return res;

}

QVariant EJDBTasksFilterModel::getTasksCalendarModel(int modelIndex)
{
    QVariant retVal;
    if(m_dateTasks.contains(modelIndex))
    {
        retVal.setValue(m_dateTasks.value(modelIndex));
    }
    return retVal;
}

QVariant EJDBTasksFilterModel::getFirstTask(int modelIndex)
{
    QVariant res;
    if(m_dateTasks.contains(modelIndex))
    {
        QList<QObject*> dataobj = m_dateTasks.value(modelIndex);
        if(dataobj.count() > 0)
            res.setValue(dataobj.at(0));
    }
    if(res.isNull())
        res.setValue(nullptr);
    return res;
}

void EJDBTasksFilterModel::setActiveModelIndex(int modelIndex)
{

}

void EJDBTasksFilterModel::clearTasksModel(int modelIndex)
{

}

void EJDBTasksFilterModel::setSearchString(QString search)
{
    if(m_filter)
        m_filter->setSearchString(search);
    if(search.isEmpty())
        refresh();
     else
    {
        m_changedSearch = true;
        m_lastNumber = QUuid::createUuid().data1;
        emit getTasksModelSignal(m_lastNumber, 0);
    }
}

void EJDBTasksFilterModel::setViewNewTask(bool source)
{
     m_viewNewTask = source;
}

void EJDBTasksFilterModel::setIsFiltering(bool source)
{
    isFiltering = source;
    refresh();
}


void EJDBTasksFilterModel::addTasksParams(quint32 number, quint32 offset, QList<Task*> &lTasksOnline)
{
//    if(m_filter && m_filter->activeNavigation() == Filter::AN_CALENDAR)
//    {
//        if(lTasksOnline.isEmpty()
//                && (!m_dateTasks.contains(number) || m_dateTasks[number].isEmpty()))
//        {
//              setIsAbsentData(true);
//        }
//        else
//        {
//            setIsAbsentData(false);
//        }
//        QList<QObject*> dataobj;
//        for(int i = 0; i < lTasksOnline.count(); i++)
//        {
//            Task *task = lTasksOnline[i];
//            task->setParent(this);
//            task->id = EjDb::Instance()->getTaskId(task->key);
//            if(task->id < 0)
//            {
//                EjDb::Instance()->setTaskSecKey(task, QByteArray());
//                task->id = EjDb::Instance()->getTaskId(task->key);
//            }
//            if(task->offset_history == 0)
//            {
//                Task taskTmp;
//                taskTmp.id = task->id;
//                EjDb::Instance()->loadTask(&taskTmp);
//                task->offset_history = taskTmp.offset_history;
//            }
//            dataobj.append(task);
//        }
//        lTasksOnline.clear();
//        if(m_dateTasks.contains(number))
//        {
//          m_dateTasks[number].append(dataobj);
//        }
//        else
//            m_dateTasks.insert(number,dataobj);
//        emit reseivedTasks(number);
//    }
//    else
//    {

//        if(m_changedSearch)
//        {
//            qDeleteAll(m_lTasksOnline);
//            m_lTasksOnline.clear();
//            if(number == m_lastNumber)
//                m_changedSearch = false;
//        }
//        if(lTasksOnline.isEmpty() && m_lTasksOnline.isEmpty())
//        {
//              setIsAbsentData(true);
//        }
//        else
//        {
//            setIsAbsentData(false);
//        }

//        m_lTasksOnline.append(lTasksOnline);
//        lTasksOnline.clear();
//        m_offset = offset;
//        beginResetModel();
//        endResetModel();
//        emit updateSignal();
//    }
}

bool EJDBTasksFilterModel::isAbsentData() const
{
    return m_isAbsentData;
}

void EJDBTasksFilterModel::setIsAbsentData(bool isAbsentData)
{
    if (m_isAbsentData == isAbsentData)
        return;

    m_isAbsentData = isAbsentData;
    emit isAbsentDataChanged(m_isAbsentData);
}

void EJDBTasksFilterModel::setFilter(Filter *filter)
{
    m_filter = filter;
}

Filter *EJDBTasksFilterModel::filter()
{
    return m_filter;
}

quint32 EJDBTasksFilterModel::getOffset(quint32 number)
{
    quint32 retVal = 0;
    if(m_filter && m_filter->activeNavigation() == Filter::AN_CALENDAR)
    {
    }
    else
        retVal = m_offset;
    return retVal;
}

void EJDBTasksFilterModel::setConnectionMode(bool mode)
{
    if(mode)
        m_connectionMode = ONLINE_MODE;
    else
        m_connectionMode = OFFLINE_MODE;
}

QHash<int, QByteArray> EJDBTasksFilterModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[TimeRole] = "time";
    roles[TimeDateRole] = "date";
    roles[TimeStringRole] = "timeString";
    roles[IsDeletingRole] = "status";
    roles[SectionRole] = "section";
    roles[IsRepeate] = "isrepeate";
    roles[WorkState] = "workState";
    roles[WorkStateColor] = "workStateColor";
    roles[WorkStateBorderColor] = "workStateBorderColor";
    roles[TransferStatus] = "transferStatus";
    roles[SecurityRole] = "security";
    roles[IsOverdue] = "isOverdue";
    roles[TaskId] = "taskid";
    roles[IsInSearch] = "isinsearch";
    roles[IsCloseVisible] = "isCloseVisible";
    roles[IsReading] = "isReading";
    roles[IsHaveReply] = "isHaveReply";
    roles[RegNumRole] = "regNum";
    roles[IsDocument] = "isDocument";
    roles[IsHaveAgreeding] = "isHaveAgreeding";
    roles[WorkStateName] = "workStateName";
    roles[AuthorName] = "authorName";
    roles[ShortAuthorName] = "shortAuthorName";
    roles[ResponsibleName] = "responsibleName";
    roles[ShortResponsibleName] = "shortResponsibleName";
    roles[TimeTitleRole] = "timeTitle";
    return roles;
}

int Task::workState() const
{
    return  m_workState;
}
