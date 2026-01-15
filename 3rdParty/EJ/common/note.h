#ifndef NOTE_H
#define NOTE_H
#include <QUuid>
#include <QList>
#include <QSet>
#include <QDataStream>
#include <QDateTime>
#include <QAbstractListModel>
#include <QStringList>
#include <QObject>
#include <QFont>
#include <QColor>
#include <QImage>
#include <QBuffer>
#include "repeatmodels.h"
#include "notifymodel.h"
#include "attachmentsmodel.h"
#include "docattributes.h"
#include "common.h"
#include "ej_interfaces.h"

//#include <QJsonObject>


quint64 getKey(quint64 key,char* data);
quint64 getKey_min(quint64 key,char* data);


class COMMONSHARED_EXPORT Group : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Name READ Name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString shortName READ shortName WRITE setShortName NOTIFY shortNameChanged)
    Q_PROPERTY(qint32 Id READ Id NOTIFY idChanged)
    Q_PROPERTY(qint32 Level READ Level NOTIFY levelChanged)
    Q_PROPERTY(bool isOpen READ isOpen WRITE setIsOpen NOTIFY isOpenChanged)
    Q_PROPERTY(bool isOpenChilds READ isOpenChilds WRITE setIsOpenChilds NOTIFY isOpenChildsChanged)
    Q_PROPERTY(bool childsEnabled READ childsEnabled WRITE setChildsEnabled NOTIFY childsEnabledChanged)
    Q_PROPERTY(int access READ access() WRITE setAccess NOTIFY accessChanged)

    bool m_isOpen;
    bool m_childsEnabled;
    bool m_isOpenChilds;    
    int m_access;    
    QString m_shortName;

public:
    Group(QObject *parent = nullptr);
    QString Name() { return name; }
    void setName(QString source) { name = source; emit nameChanged(); }
    qint32 Id() { return id; }
    qint32 Level() { return level; }
//    void setId(QString source) { key = source; emit keyChanged(); }
    QUuid key;
    qint32 id;
    quint32 offset;
    quint16 ver;
    QString name;
    quint8 is_modify;
    quint32 time_modify;
    QUuid parentKey;
    QUuid authorKey;
    QUuid rootKey;
    Group *parentGroup;
    qint8 level;

    bool isOpen() const;
    bool childsEnabled() const;
    bool isOpenChilds() const;   
    int access() const;    
    QString shortName() const;
    Group *rootGroup();

public slots:
    void setIsOpen(bool isOpen);
    void setChildsEnabled(bool childsEnabled);
    void setIsOpenChilds(bool isOpenChilds);
    void setAccess(int access);    
    void setShortName(QString shortName);

signals:
    void nameChanged();
    void idChanged();
    void levelChanged();
    void isOpenChanged(bool isOpen);
    void childsEnabledChanged(bool childsEnabled);
    void isOpenChildsChanged(bool isOpenChilds);
    void accessChanged(int access);
    void shortNameChanged(QString shortName);
};



class COMMONSHARED_EXPORT Tag : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Name READ Name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString shortName READ shortName WRITE setShortName NOTIFY shortNameChanged)
//    Q_PROPERTY(qint32 Key READ Key WRITE setKey NOTIFY keyChanged)
    Q_PROPERTY(quint32 Id READ Id NOTIFY idChanged)
    Q_PROPERTY(VidTag Vid READ Vid WRITE setVid NOTIFY vidChanged)
    QString m_shortName;

public:
//    Q_ENUM(VidTag)
    enum VidTag {
        WHAT = 0,
        WHERE,
        WHOM
    };
    Q_ENUM(VidTag)

    Tag(QObject *parent = nullptr);// : offset(0) {}
    QString Name() { return name; }
    void setName(QString source) { name = source; emit nameChanged(); }
    quint32 Id() { return id; }
    VidTag Vid() { return vid; }
    void setVid(VidTag source);
//    void setKey(QString source) { key = source; emit keyChanged(); }
//    void setKey(QUuid source) { key = source; emit keyChanged(); }
    QString name;
//    qint32 key; // deprecated
    QUuid key;
    QUuid authorKey;
    qint32 id;
    QList<qint32> lIDsGroups;  //key,offset in groups_tags.dat
//    QList<qint32> lKeysGroups;  //key,offset in groups_tags.dat  deprecated
    quint32 offset;
    quint16 ver;
    quint8 is_modify;
    quint32 time_modify;
    quint32 time_checked;
    VidTag vid;    
    QString shortName() const;

public slots:
    void setShortName(QString shortName);

signals:
    void nameChanged();
    void vidChanged();
    void idChanged();
    void shortNameChanged(QString shortName);
};


class COMMONSHARED_EXPORT EJUser : public Tag
{
    Q_OBJECT
    Q_PROPERTY(quint32 Flag READ Flag)
    Q_PROPERTY(QString Login READ Login)
    Q_PROPERTY(QDateTime TimeModify READ TimeModify)
public:
    enum Flag {
        ACTIVE,
        NEW,
        DELETED,
        INBOX,
        AUTHORIZED
    };

    EJUser(QObject *parent=nullptr) : Tag(parent) { security = 0; }
    QString Login() { return login; }
    quint32 Flag() { return flag; }
    QString login;
    quint8 flag; // 0-active 1 - new (unauthorized) 2-deleted 3 - inbox 4 - authorized
    quint8 security;  // 0 - clean 2 - read 3 - edit 4 - full
	QDateTime TimeModify() { return QDateTime::fromSecsSinceEpoch(time_modify); }
    QByteArray pubKey;
//    quint32 id;
//    QUuid uuid;
};

class COMMONSHARED_EXPORT EJUserAdd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint32 groupId READ groupId)
    Q_PROPERTY(qint32 userId READ userId)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString role READ role)
public:
    EJUserAdd(QObject *parent=nullptr) : QObject(parent) { m_groupId = -1; is_modify = false; ver = 0; }
    QString name() { return m_name; }
    QString role() { return m_role; }
    qint32 groupId() { return m_groupId; }
    qint32 userId() { return m_userId; }
    QString m_name;
    QString m_role;
    qint32 m_groupId;
    qint32 m_userId;
    quint32 time_modify;
    bool is_modify;
    quint16 ver;
};

class COMMONSHARED_EXPORT TaskProperty : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 data READ data NOTIFY dataChanged)
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
public:
    enum e_type
    {
        WORK_STATE = 0,
        TAG,
        SYNC_TAG,
        START_DATE,
        END_DATE,
        REPEATES,
        TRANSFER_TIMES,
        FLAG,
        RESPONSIBLE,
        SIGNED,
        CHAT,
        SYNC_CHAT,
        ATTACHMENT,
        DOC_ATTRIBUTES,
        DOC_STYLE,
        DOC_LINK,
        SYNC_SIGNED,
        SYNC_RESPONSIBLE,
        SYNC_SIGNED2,
        SYNC_RESPONSIBLE2,
        SIGNED2,
        RESPONSIBLE2,
        USER,
        SYNC_USER,
        CHANGE_NAME,
        REPLY,
        SYNC_REPLY,
        AUTHOR,
        SYNC_AUTHOR,
        SYNC_ATTACHMENT,
        SYNC_DOC_LINK,
        CABINET,
        SYNC_CABINET,
        GROUP_ACCESS,
        SYNC_GROUP_ACCESS
    };

    enum e_status
    {
        OFF = 0,
        ON,
        CANSEL,
        REMOVED
    };

    TaskProperty(e_type typeProp = WORK_STATE, QObject *parent = 0) : QObject(parent)
    { uiData = 0; m_status = ON; type = typeProp; userId = groupId = -1; }

    qint32 data() {return uiData;}
    QString text() {return strData;}
    int status() {return m_status;}

    qint32 uiData;
    qint32 uiData2;
    QString strData;
    qint32 userId;
    qint32 groupId;
    QByteArray baData;
    QByteArray baData2;
//    e_status m_status;
    quint8 m_status;
    QUuid creatorKey;
    e_type type;
signals:
    void dataChanged();
    void textChanged();
    void statusChanged();
};

class COMMONSHARED_EXPORT TransferTime : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged)
    Q_PROPERTY(QDateTime endTime READ endTime WRITE setEndTime NOTIFY endTimeChanged)
    Q_PROPERTY(QDateTime startTimeNew READ startTimeNew WRITE setStartTimeNew NOTIFY startTimeNewChanged)
    Q_PROPERTY(QDateTime endTimeNew READ endTimeNew WRITE setEndTimeNew NOTIFY endTimeNewChanged)
    Q_PROPERTY(int status READ status WRITE setStatus NOTIFY statusChanged )

public:
    enum e_status
    {
        NO_TRANSFER = 0,
        MOVE_TIME,
        REMOVE_TIME
    };
    explicit TransferTime(QObject *parent = 0) : QObject(parent) { m_status = NO_TRANSFER; is_used = false; }
    ~TransferTime() {}
    QDateTime startTime() {return dt_start;}
    void setStartTime(QDateTime source) {dt_start = source; emit startTimeChanged(); }
    QDateTime endTime() {return dt_end;}
    void setEndTime(QDateTime source) {dt_end = source; emit endTimeChanged(); }

    QDateTime startTimeNew() {return dt_start_new;}
    void setStartTimeNew(QDateTime source) {dt_start_new = source; emit startTimeNewChanged(); }
    QDateTime endTimeNew() {return dt_end_new;}
    void setEndTimeNew(QDateTime source) {dt_end_new = source; emit endTimeNewChanged(); }
    int status() {return m_status;}
    void setStatus(int source) {m_status = (e_status)source; emit statusChanged(); }

    bool operator ==(const TransferTime &other) const;
    bool operator !=(const TransferTime &other) const
    {
        return !(*this == other);
    }


    e_status m_status;
    QDateTime dt_start;
    QDateTime dt_end;
    QDateTime dt_start_new;
    QDateTime dt_end_new;
    bool is_used;
signals:
    void startTimeChanged();
    void endTimeChanged();
    void startTimeNewChanged();
    void endTimeNewChanged();
    void statusChanged();
};

//class PDocument : public QObject
//{
//    Q_OBJECT
//  public:
//    PDocument(QObject *parent=0);
////    ~PDocument();
//    int k;
////    void clear();

//    QList<Block*> *lBlocks;
////    QList<JString*> *lStrings;
////    QList<Page*> *lPages;
////    QList<FragmentBlock*> *lFragments;
////    QList<TableBlock*> *lTables;
////    QList<BaseStyle *> *lStyles;
//////    QString title;
//////    QStringList lTags;
////    QDateTime dt_modify;
////    quint32 user_key;
////    Q_INVOKABLE QDateTime getTime() { return dt_modify; }
////    Q_INVOKABLE quint32 getUserKey() { return user_key; }
////    BaseStyle *getStyle(int num);
////    void copy(Document *doc);
////    void move(Document *doc);
//};



class SubTask;
class Reply;

class COMMONSHARED_EXPORT Task : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString regNum READ regNum WRITE setRegNum NOTIFY regNumChanged)
    Q_PROPERTY(QVariant signatures READ signatures NOTIFY signaturesChanged)
    Q_PROPERTY(QVariant replies READ replies NOTIFY repliesChanged)
    Q_PROPERTY(QVariant chats READ chats NOTIFY chatsChanged)
    Q_PROPERTY(QVariant attachments READ attachments NOTIFY attachmentsChanged)
    Q_PROPERTY(int newChats READ newChats NOTIFY newChatsChanged)
    Q_PROPERTY(int countAgreed READ countAgreed NOTIFY countAgreedChanged)
//    Q_PROPERTY(QStringList signatures READ signatures NOTIFY signaturesChanged)
    Q_PROPERTY(qint32 responsible READ responsible NOTIFY responsibleChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(QString repeatTitle READ repeatTitle NOTIFY repeatTitleChanged)
    Q_PROPERTY(int notify READ notify WRITE setNotify NOTIFY notifyChanged)
    Q_PROPERTY(int workState READ workState WRITE setWorkState NOTIFY workStateChanged)
//    Q_PROPERTY(quint8 workState READ workState WRITE setWorkState NOTIFY workStateChanged)
//    Q_PROPERTY(QString workStateName READ workStateName WRITE setWorkStateNem NOTIFY workStateNameChanged)
//    Q_PROPERTY(QString workStateColor READ workStateColor WRITE setWorkStateColor NOTIFY workStateColorChanged)
//    Q_PROPERTY(QString workStateBorderColor READ workStateBorderColor WRITE setWorkStateBorderColor NOTIFY workStateBorderColorChanged)
    Q_PROPERTY(bool isReading READ isReading WRITE setIsReading NOTIFY isReadingChanged)
    Q_PROPERTY(bool isAllDay READ isAllDay WRITE setIsAllDay NOTIFY isAllDayChanged)
    Q_PROPERTY(bool isOverdue READ isOverdue NOTIFY isOverdueChanged)
    Q_PROPERTY(bool isHaveReply READ isHaveReply WRITE setIsHaveReply NOTIFY isHaveReplyChanged)
    Q_PROPERTY(QDateTime startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged)
    Q_PROPERTY(QDateTime endTime READ endTime WRITE setEndTime NOTIFY endTimeChanged)
    Q_PROPERTY(QDateTime endIntervalTime READ endIntervalTime WRITE setEndIntervalTime NOTIFY endIntervalTimeChanged)
//    Q_PROPERTY(QVariant notes READ notes NOTIFY notesChanged)
    Q_PROPERTY(QDateTime date READ date)
    Q_PROPERTY(QDateTime currentTime READ currentTime WRITE setCurrentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(QVariant inputDocs READ inputDocs NOTIFY inputDocsChanged)
    Q_PROPERTY(QVariant outputDocs READ outputDocs NOTIFY outputDocsChanged)
    Q_PROPERTY(QVariant subtasks READ subtasks NOTIFY subtasksChanged)

public:

    Task(QObject *parent = 0);
    virtual ~Task();

    enum Type {
        NOTE,
        TASK,
        SUBTASK
    };

    enum IsDeleteFlag {
        ACTIVE,
        IN_WORK, // deprecated
        CLOSED,  // deprecated
        DELETED
    };

    Q_ENUM(IsDeleteFlag);

    enum WorkState {
        WS_NONE = -1,
        WS_OPEN = 0,
        WS_ACCEPTED,
        WS_IN_WORK,
        WS_FINISH,
        WS_CLOSED,
        WS_REJECTED,
        WS_DELETED, // deprecated
        WS_AGREEDING = 50,
        WS_AGREED
    };
    Q_ENUM(WorkState);

    enum Security {
        CLEAN,
        READ_NOT_COPY,
        READ,
        EDIT,
        FULL
    };

    Q_ENUM(Security);

    void clear();

    int interval();
    void setInterval(int source);
    int notify() { return m_notify; }
    void setNotify(int source){ m_notify = source; emit notifyChanged(); }
    int workState() const;
    void setWorkState(int source, bool saved = true);
    bool isReading() { return m_isReading; }
    void setIsReading(bool source);
    bool isAllDay();
    void setIsAllDay(bool source);
    bool isOverdue();
    bool isHaveReply() {return  m_isHaveReply;}
    void setIsHaveReply(bool source);

	QDateTime date() {return dt_start;}
    QDateTime startTime() const {return dt_start;}
    void setStartTime(QDateTime source); //{dt_start = source; emit startTimeChanged(); }
    QDateTime currentTime() {return dt_current;}
    void setCurrentTime(QDateTime source) {dt_current = source; emit currentTimeChanged(); }

    QDateTime endTime() const {return dt_end;}
    void setEndTime(QDateTime source); //{dt_end = source; emit endTimeChanged(); }

    QDateTime endIntervalTime() {return dt_end_interval;}
    void setEndIntervalTime(QDateTime source) {dt_end_interval = source; emit endIntervalTimeChanged(); }

    bool isContainsTags(QSet<qint32> &filter_tags);
    void loadLinks(bool force = false);
    void reloadLink(QUuid key);
//    friend QDataStream &operator<<(QDataStream &dataStream, const Note* src);
//    friend QDataStream &operator>>(QDataStream &dataStream, Note* src);

//    void getTask(bool isFromId);
    QVariant signatures();
    QVariant replies();
    QVariant chats();
    QVariant attachments(int authorId = 0);
    Q_INVOKABLE QList<QObject *> getAttachAndLinks(int authorId = 0);
    QVariant inputDocs();
    QVariant outputDocs();
    QVariant subtasks();
    Q_INVOKABLE void loadSubtasks();

    int newChats() { return m_newChats; }

    void copyBaseData(Task *source);

//    QVariant notes();

    Q_INVOKABLE QString title() {return m_title;}
//    Q_INVOKABLE QDateTime time() {return dt_modify;}
    Q_INVOKABLE QString getKey() {return key.toString(); }
    Q_INVOKABLE void setTitle(QString source);
    Q_INVOKABLE int getId() { return id; }
    Q_INVOKABLE int getSecurity() { return security; }
    Q_INVOKABLE int getCountUsers() { return m_users.count(); }//{ return count_users; }
    Q_INVOKABLE quint32 getAuthorId() { return creator_id; }
    Q_INVOKABLE void sign(int status, QString comment);
    Q_INVOKABLE void addReply(int status, QString comment);
    Q_INVOKABLE void setDocDefaults(DocLayout *docLayout, DocMargings *docMargings);
    Q_INVOKABLE Reply* myReply();
    Q_INVOKABLE void setChat(Document *doc);
    Q_INVOKABLE void setResponsible(qint32 id);
    Q_INVOKABLE qint32 responsible();
    Q_INVOKABLE QString nameResponsible();
    Q_INVOKABLE QString shortNameResponsible();
    Q_INVOKABLE QString nameAuthor();
    Q_INVOKABLE QString shortNameAuthor();
    Q_INVOKABLE QString fromWhom();
//    Q_INVOKABLE QDateTime time();
//    Q_INVOKABLE QDateTime timeTitle();
    Q_INVOKABLE QDateTime fromTime();
    Q_INVOKABLE QDateTime registryTime();
    Q_INVOKABLE QVariant getSubTasks(bool isAll);
    Q_INVOKABLE QVariant getRepeatDates();
//    Q_INVOKABLE int countNotes() {return m_lNotes.count(); }
    Q_INVOKABLE int countSubTasks() {return m_lSubTasks.count(); }
    Q_INVOKABLE int countAgreed();
    Q_INVOKABLE int getIndex() { return m_index; }
    Q_INVOKABLE SubTask* addSubTask();
    Q_INVOKABLE RepeatModels* getRepeatModels() { return &m_repeatModels; }
    Q_INVOKABLE NotifyModel* getNotifyModel() { return &m_notifyModel; }
    Q_INVOKABLE DocAttributes* getDocAttributes() { return &m_docAttributes; }
    Q_INVOKABLE QVariant getTransferAllTimes();
    Q_INVOKABLE QVariant getTransferTime();
    Q_INVOKABLE int countTransfers() { return m_lTransferTimes.count(); }
    Q_INVOKABLE int transferStatus() { return m_transfer_status; }
    Q_INVOKABLE bool isOneDay();
    Q_INVOKABLE bool isAllIntervalDay();
    Q_INVOKABLE void reload();
    Q_INVOKABLE Document *document();
    Q_INVOKABLE bool attachFile(QString path);
    Q_INVOKABLE bool openAttach(int authorId, int index);
    Q_INVOKABLE bool remAttach(int index);
    Q_INVOKABLE QList<qint32> getTags() { return m_tags; }
    Q_INVOKABLE bool isContainsReplies();
    static Q_INVOKABLE QString getWorkStateName(quint8 workState);
    Q_INVOKABLE QString getWorkStateName();
    Q_INVOKABLE QColor getWorkStateColor();
    Q_INVOKABLE QColor getWorkStateColor(int workState);
    Q_INVOKABLE QColor getWorkStateBorderColor();
    Q_INVOKABLE bool isDocument();
    Q_INVOKABLE bool isHaveAgreeding();
    Q_INVOKABLE bool isSignResolution(int authorId);
    Q_INVOKABLE bool relpyEnabled();
    QString regNum() const;
    void setRegNum(QString regNum);
//    Q_INVOKABLE void changeTimeSubTask(int index, QDateTime new_time);
    QString repeatTitle();

    void getTask();

    quint16 m_notify;
    int m_interval;
    int m_index;
    QString m_notifyParams;
    Task *baseTask;


    QDateTime dt_start;
    QDateTime dt_end;
    QDateTime dt_end_interval;
    QDateTime dt_current;
    QDateTime dt_prev;
    QDateTime dt_next;

//    quint8 m_repeat; // 0-NotRepead, 1-ForDay, 2-ForWeek, 3-ForMonth, 4-ForYear
    RepeatModels m_repeatModels;
    NotifyModel m_notifyModel;
    AttachmentsModel m_attachmentModel;
    DocAttributes m_docAttributes;

//    QList<Note*> m_lNotes;
    QList<SubTask*> m_lSubTasks;
//    QList<TransferTime*> *m_lTransferTimes;
    QList<TransferTime*> m_lTransferTimes;
    TransferTime *m_transferTime;
    bool m_isLocalTransfer;
    bool m_isOverdue;
    bool m_isHaveReply;
    bool m_isReading;
//    bool m_isAgreeding;
    quint8 m_workState;

    TransferTime::e_status m_transfer_status;
    static void parseTransfer(QByteArray &buffer, QList<TransferTime*> &m_lTransferTimes, QObject *parent = NULL);
    bool getRepeatProperties();

    QString m_title;
    QString m_regNum;
//    QList<Tag> lTags;
    QList<qint32> m_tags;
    QMap<qint32, QPair<qint32,qint8>> m_users; // id, group, state
//    QList<quint32>lKeyUsers;
//    QList<Block*> lBlocks;
//    QList<Fragment*> lFragments;
    QList<TaskProperty*> m_newProperties;
    QList<TaskProperty*> m_signatures;
//    QList<TaskProperty*> m_replies;
    QList<Reply*> m_replies;
    QList<TaskProperty*> m_chats;
    QList<Task*> m_inputLinks;
    QList<Task*> m_outputLinks;
    //QList<TaskProperty*> m_usersProps;
    Document *m_doc;
//    QList<BaseStyle*> lStyles;

    QUuid key;
    qint16 patch_key;
    QString author;
    qint16 prev_key;
    quint32 offset_history;
    quint32 offset_model;
    quint32 offset_tags;
    quint32 offset_title;
    qint32 id;
    quint32 creator_id;  // author_id
    quint16 count_users;
    QPair<qint32,qint32> m_responsible;
    QPair<qint32,qint32> m_author;
    int m_newChats;
    int m_from;
    quint32 m_fromTime;
    quint32 m_registryTime;

    quint8 flag;  // 0 - active 1 - new 2 - closed 3 - deleted
    quint8 security;  // 0 - clean 2 - read 3 - edit 4 - full

    QList<Task*> lChilds;
    Task *prevTask;
    bool isEditable;
//    Message mess;
    QDateTime dt_modify;
    QDateTime dt_history;
    quint64 data;
//    QDateTime dt_title_modify;
    quint16 ver;
    quint16 ver_patch;
    quint16 prev_ver;
    quint16 last_ver;
    quint8 is_modify;
    quint8 type;  // 0 - note 1 - task 2 - subtask



signals:
    void titleChanged();
    void signaturesChanged();
    void repliesChanged();
    void chatsChanged();
    void newChatsChanged();
    void countAgreedChanged();
    void responsibleChanged();
    void keyChanged();
    void workStateChanged();
    void isAllDayChanged();
    void isOverdueChanged();
    void isHaveReplyChanged();
    void startTimeChanged();
    void currentTimeChanged();
    void endTimeChanged();
    void endIntervalTimeChanged();
//    void notesChanged();
    void intervalChanged();
    void taskChanged();
//    void repeatIntervalChanged();
//    void repeatChanged();
    void notifyChanged();
//    void repeatParamsChanged();
    void repeatTitleChanged();
    void subTasksChanged();
    void attachmentsChanged();
    void inputDocsChanged();
    void outputDocsChanged();
    void subtasksChanged();
    void isReadingChanged();
    void regNumChanged();
    void getFullTask(Task *task);

protected:
    void findLink(const QUuid &key, QList<LinkProp *> &lLinks);
    void findTaskLink(const QUuid &key, QList<Task *> &lLinks, QList<Reply *> &lReplies);
};

class COMMONSHARED_EXPORT SubTask  : public Task
{
    Q_OBJECT
    Q_PROPERTY(bool workState READ workState WRITE setWorkState NOTIFY workStateChanged)
    Q_PROPERTY(QDateTime taskTime READ taskTime NOTIFY taskTimeChanged)

public:
    SubTask(QObject *parent=0) : Task(parent) { type = SUBTASK;  } //dt_close = QDateTime::fromTime_t(0);
//    quint8 flag;  // 0-active  3 - deleted 2 - closed
//    bool isEditable;
//    QDateTime dt_modify;
//    QDateTime dt_date;
    QDateTime dt_task;
//    QDateTime dt_close;
//    qint32 m_assigned;

    Q_INVOKABLE void setTaskTime(QDateTime new_time) { dt_task = new_time; isEditable = true; emit taskTimeChanged(); }
    QDateTime taskTime() { return dt_task; }
    bool isClosed() { return  m_workState == CLOSED; }  // dt_close.toTime_t() > 0;
    Q_INVOKABLE bool isOverdue();
    Q_INVOKABLE void setDeleted();
    void setWorkState(bool source);
signals:
    void workStateChanged();
    void taskTimeChanged();
    void isReadingChanged();
};




//class Task : public Note
//{
//    Q_OBJECT
//    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
//    Q_PROPERTY(QString repeatTitle READ repeatTitle NOTIFY repeatTitleChanged)
//    Q_PROPERTY(int notify READ notify WRITE setNotify NOTIFY notifyChanged)
//    Q_PROPERTY(bool isClosed READ isClosed WRITE setIsClosed NOTIFY isClosedChanged)
//    Q_PROPERTY(bool isAllDay READ isAllDay WRITE setIsAllDay NOTIFY isAllDayChanged)
//    Q_PROPERTY(bool isOverdue READ isOverdue NOTIFY isOverdueChanged)
//    Q_PROPERTY(QDateTime startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged)
//    Q_PROPERTY(QDateTime endTime READ endTime WRITE setEndTime NOTIFY endTimeChanged)
//    Q_PROPERTY(QDateTime endIntervalTime READ endIntervalTime WRITE setEndIntervalTime NOTIFY endIntervalTimeChanged)
//    Q_PROPERTY(QVariant notes READ notes NOTIFY notesChanged)
//    Q_PROPERTY(QDateTime date READ date)
//    Q_PROPERTY(QDateTime currentTime READ currentTime WRITE setCurrentTime NOTIFY currentTimeChanged)
//public:
//    Task(QObject *parent = 0);
//    virtual ~Task();
//    int interval();
//    void setInterval(int source);
//    int notify() {return m_notify;}
//    void setNotify(int source){m_notify = source; emit notifyChanged(); }
//    bool isClosed() {return  m_isClosed;}
//    void setIsClosed(bool source);
//    bool isAllDay();
//    void setIsAllDay(bool source);
//    bool isOverdue() {return  m_isOverdue;}

//    QDateTime date() {return QDateTime(dt_start.date());}
//    QDateTime startTime() {return dt_start;}
//    void setStartTime(QDateTime source); //{dt_start = source; emit startTimeChanged(); }
//    QDateTime currentTime() {return dt_current;}
//    void setCurrentTime(QDateTime source) {dt_current = source; emit currentTimeChanged(); }

//    QDateTime endTime() {return dt_end;}
//    void setEndTime(QDateTime source); //{dt_end = source; emit endTimeChanged(); }

//    QDateTime endIntervalTime() {return dt_end_interval;}
//    void setEndIntervalTime(QDateTime source) {dt_end_interval = source; emit endIntervalTimeChanged(); }

//    QVariant notes();
//    Q_INVOKABLE QVariant getSubTasks(bool isAll);
//    Q_INVOKABLE QVariant getRepeatDates();
//    Q_INVOKABLE int countNotes() {return m_lNotes.count(); }
//    Q_INVOKABLE int countSubTasks() {return m_lSubTasks.count(); }
//    Q_INVOKABLE int getIndex() { return m_index; }
//    Q_INVOKABLE SubTask* addSubTask();
//    Q_INVOKABLE RepeatModels* getRepeatModels() { return &m_repeatModels; }
//    Q_INVOKABLE NotifyModel* getNotifyModel() { return &m_notifyModel; }
//    Q_INVOKABLE QVariant getTransferAllTimes();
//    Q_INVOKABLE QVariant getTransferTime();
//    Q_INVOKABLE int countTransfers() { return m_lTransferTimes.count(); }
//    Q_INVOKABLE int transferStatus() { return m_transfer_status; }
//    Q_INVOKABLE bool isOneDay();
//    Q_INVOKABLE bool isAllIntervalDay();
//    Q_INVOKABLE void reload();

////    Q_INVOKABLE void changeTimeSubTask(int index, QDateTime new_time);
//    QString repeatTitle();

//    void getTask();

//    quint16 m_notify;
//    int m_interval;
//    int m_index;
//    QString m_notifyParams;
//    Task *baseTask;


//    QDateTime dt_start;
//    QDateTime dt_end;
//    QDateTime dt_end_interval;
//    QDateTime dt_current;
//    QDateTime dt_prev;
//    QDateTime dt_next;

////    quint8 m_repeat; // 0-NotRepead, 1-ForDay, 2-ForWeek, 3-ForMonth, 4-ForYear
//    RepeatModels m_repeatModels;
//    NotifyModel m_notifyModel;

//    QList<Note*> m_lNotes;
//    QList<SubTask*> m_lSubTasks;
////    QList<TransferTime*> *m_lTransferTimes;
//    QList<TransferTime*> m_lTransferTimes;
//    TransferTime *m_transferTime;
//    bool m_isLocalTransfer;
//    bool m_isOverdue;
//    bool m_isClosed;

//    TransferTime::e_status m_transfer_status;
//    static void parseTransfer(QByteArray &buffer, QList<TransferTime*> &m_lTransferTimes, QObject *parent = NULL);
//    bool getRepeatProperties();

//signals:
//    void titleChanged();
//    void keyChanged();
//    void isClosedChanged();
//    void isAllDayChanged();
//    void isOverdueChanged();
//    void startTimeChanged();
//    void currentTimeChanged();
//    void endTimeChanged();
//    void endIntervalTimeChanged();
//    void notesChanged();
//    void intervalChanged();
//    void taskChanged();
////    void repeatIntervalChanged();
////    void repeatChanged();
//    void notifyChanged();
////    void repeatParamsChanged();
//    void repeatTitleChanged();
//    void subTasksChanged();

//};




//class NotesFilterModel : public QAbstractListModel
//{
//    Q_OBJECT
//public:
//    enum NoteRoles {
//        TitleRole = Qt::UserRole + 1,
//        TimeRole,
//        StateRole,
//        FlagRole
//    };

//    NotesFilterModel(QObject *parent = 0);
////![1]

////    void addAnimal(const Animal &animal);
////    QList<Note*> & notes() { return m_filterNotes; }

//    int rowCount(const QModelIndex & parent = QModelIndex()) const;

//    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
//    int getId(int row) const;
//    Note* getNote(int index, Note *source = NULL);
//    Note* getNoteById(int idNote);
//    Note* addNote();
//    void refresh();
//    int indexOf(Note* note);
//    void setSelectedFilter(QList<qint32> *source) {m_selectedFilter = source;}
//    bool isFiltering;
//    bool m_viewNewNote;


////    Q_INVOKABLE Note* getNote(int index);

//protected:
//    QHash<int, QByteArray> roleNames() const;
//private:
////    QList<Note*> m_filterNotes;
//    mutable QList<int> m_idNotes;
//    QList<qint32> *m_selectedFilter;
//    int m_rowCount;
//    Note *m_activeTask;
//    int m_activeId;
//    mutable int m_cursor;
////![2]
//};

class COMMONSHARED_EXPORT TrashFilterModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum TrashRoles {
        TitleRole = Qt::UserRole + 1,
        TimeRole,
        StateRole,
        FlagRole,
        TypeRole
    };

    TrashFilterModel(QObject *parent = 0);
//![1]

//    void addAnimal(const Animal &animal);
//    QList<Note*> & notes() { return m_filterNotes; }

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int getId(int row) const;
//    Task* getTask(int index);
    Q_INVOKABLE int getType(int index);
    Task* getTaskById(int idTask);
//    Note* addNote();
    void refresh();
    int indexOf(Task* task);

    Task* getTask(int index, Task *source = NULL);
//    bool m_viewNewNote;
    bool isFiltering;

    void setSelectedFilter(QList<qint32> *source) {m_selectedFilter = source;}


//    Q_INVOKABLE Note* getNote(int index);

protected:
    QHash<int, QByteArray> roleNames() const;
private:
//    QList<Note*> m_filterNotes;
    mutable QList<int> m_idTasks;
    mutable QList<int> m_typeTasks;
    QList<qint32> *m_selectedFilter;
    int m_rowCount;
//    Note *m_activeTask;
    Task *m_activeTask;
    int m_activeId;
    int m_activeType;
    mutable int m_cursor;
//![2]
};


class Tags;
struct TaskParams;

class COMMONSHARED_EXPORT TasksFilterModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum TaskRoles {
        TitleRole = Qt::UserRole + 1,
        TimeRole,
        TimeDateRole,
        TimeStringRole,
        StatusRole,         // flag
        SectionRole,
        IsRepeate,
        WorkState,
        TransferStatus,
        SecurityRole,          // state Security
        IsOverdue,
        TaskId,
        IsInSearch,
        IsCloseVisible,
        RegNumRole
    };

    enum e_vew_mode {
        TO_DO,
        ALL_TASKS,
        LAST_MODIFY,
        NOTES_AND_TASKS,
        ONLY_NOTES
    };

//    struct TaskParams {
//        TaskParams() { repeatModels = NULL; }
////        ~TaskParams() { qDeleteAll(lTransferTimes); lTransferTimes.clear(); }
//        int id;
//        int interval; //sec
////        int is_overdue;
//        TransferTime::e_status transfer_status;
//        quint8 is_closed;
//        QDateTime dt_start;
//        QDateTime dt_current;
//        QDateTime dt_overdue;
//        QList<TransferTime *> lTransferTimes;
//        RepeatModels *repeatModels;
//    };

    TasksFilterModel(QObject *parent = 0);
    ~TasksFilterModel();
//![1]

//    void addAnimal(const Animal &animal);
 //   QList<Task*> & tasks() { return m_filterTasks; }

    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE void refresh();
    Q_INVOKABLE QVariantMap getData(int row);
    int getId(int row) const;
    Task* getTask(int index, Task *source = NULL);
    Q_INVOKABLE Task* getFullTask(int index);
    Task* addTask(QDate date);
    int indexOf(Task* task);
    Q_INVOKABLE int indexOfSection(QDate section);
    Q_INVOKABLE void goToDay(QDate date) { emit goToDaySignal(date); }
    Q_INVOKABLE int getIndexOfDay(QDate date) { return -date.daysTo(QDate::currentDate()); }
    int selectDate(QDate date);
    Q_INVOKABLE int countTasks(QDate date);
    Q_INVOKABLE void setViewMode(int source);
    Q_INVOKABLE QVariant getTasksModel(QDate date, int modelIndex);
    Q_INVOKABLE void setActiveModelIndex(int modelIndex);
    Q_INVOKABLE void clearTasksModel(int modelIndex);
    Q_INVOKABLE void setSearchString(QString search);
    Q_INVOKABLE void setViewNewTask(bool source);
    Q_INVOKABLE void setIsFiltering(bool source) { isFiltering = source; }
    Q_INVOKABLE void setTags(Tags *tags);

    void setSelectedFilter(QList<qint32> *source) { m_selectedFilter = source; }
    bool isFiltering;
    e_vew_mode m_viewMode;
    bool m_viewNewTask;
    mutable QMap<QDateTime,int> m_bufferTasks;
    mutable QMap<int,QList<QObject*> > m_dateTasks;
//    mutable QList<int> m_idTasks;
    mutable QList<TaskParams> m_lTasksParams;
    QList<TransferTime *> m_lTransferTimes;
    QList<RepeatModels *> m_lRepeatModels;


//    QList<QObject*> m_dateObj;
//    int indexOf(Note* note);
signals:
   void goToDaySignal(QDate date);

protected:
    QHash<int, QByteArray> roleNames() const;
private:
 //   QList<Task*> m_filterTasks;
    QList<qint32> *m_selectedFilter;
    int m_rowCount;
    Task *m_activeTask;
    int m_activeId;
    int m_activeModelIndex;
    QString m_searchString;
    mutable int m_cursor;
    QList<qint32> m_templateFilter;

//![2]
};

struct TaskParams {
    TaskParams() { repeatModels = NULL; }
//        ~TaskParams() { qDeleteAll(lTransferTimes); lTransferTimes.clear(); }
    QUuid key;
    int id;
    int interval; //sec
//        int is_overdue;
    QString title;
    TransferTime::e_status transfer_status;
    quint8 work_state;
    quint8 is_overdue;
    quint8 is_close_visible;
    quint8 is_delete;
    quint8 security;
//    quint8 is_losed;
    quint8 is_have_reply;
    quint8 is_reading;
    QDateTime dt_start;
    QDateTime dt_end;
    QDateTime dt_current;
    QDateTime dt_overdue;
    QList<TransferTime *> lTransferTimes;
    RepeatModels *repeatModels;
};

class Filter;

class COMMONSHARED_EXPORT EJDBTasksFilterModel : public QAbstractListModel
{
    Q_OBJECT
//    Q_PROPERTY(bool  READ  WRITE set NOTIFY Changed)
    Q_PROPERTY(bool isAbsentData READ isAbsentData WRITE setIsAbsentData NOTIFY isAbsentDataChanged)
public:
    enum TaskRoles {
        TitleRole = Qt::UserRole + 1,
        TimeRole,
        TimeDateRole,
        TimeStringRole,
        IsDeletingRole,
        SectionRole,
        IsRepeate,
        WorkState,
        TransferStatus,
        SecurityRole,
        IsOverdue,
        TaskId,
        IsInSearch,
        IsCloseVisible,
        IsReading,
        WorkStateColor,
        WorkStateBorderColor,
        IsHaveReply,
        RegNumRole,
        IsDocument,
        IsHaveAgreeding,
        WorkStateName,
        AuthorName,
        ShortAuthorName,
        ResponsibleName,
        ShortResponsibleName,
        TimeTitleRole
    };

    enum ConnectionMode {
        ONLINE_MODE,
        OFFLINE_MODE
    };
    Q_ENUM(ConnectionMode)

    EJDBTasksFilterModel(QObject *parent = 0);
    ~EJDBTasksFilterModel();
//![1]

//    void addAnimal(const Animal &animal);
 //   QList<Task*> & tasks() { return m_filterTasks; }

    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE void refresh();
    Q_INVOKABLE QVariantMap getData(int row);
    int getId(int row) const;
    int getRow(int id) const;
    void prependTask(Task *activeTask);
    Task* getTask(int index, Task *source = NULL);
    void copyBaseData(int index, Task *source);
    Q_INVOKABLE Task* getFullTask(int index);
    Task* addTask(QDate date);
    int indexOf(Task* task);
    Q_INVOKABLE int indexOfSection(QDate section);
    Q_INVOKABLE void goToDay(QDate date) { emit goToDaySignal(date); }
    Q_INVOKABLE int getIndexOfDay(QDate date) { return -date.daysTo(QDate::currentDate()); }
    int selectDate(QDate date);
    Q_INVOKABLE int countTasks(QDate date);
    Q_INVOKABLE QVariant getTasksModel(QDate date, int modelIndex);
    Q_INVOKABLE QVariant getTasksCalendarModel(int modelIndex);
    Q_INVOKABLE QVariant getFirstTask(int modelIndex);
    Q_INVOKABLE void setActiveModelIndex(int modelIndex);
    Q_INVOKABLE void clearTasksModel(int modelIndex);
    Q_INVOKABLE void setSearchString(QString search);
    Q_INVOKABLE void setViewNewTask(bool source);
    Q_INVOKABLE void setIsFiltering(bool source);
    Q_INVOKABLE void setFilter(Filter *filter);
    Q_INVOKABLE Filter* filter();
    Q_INVOKABLE quint32 getOffset(quint32 number = 0);
    Q_INVOKABLE void setConnectionMode(bool mode);
//    Q_INVOKABLE void setTags(Tags *tags);
//    Q_INVOKABLE void setTaskReading(int id);

//    void setSelectedFilter(QList<qint32> *source) { m_selectedFilter = source; }
    void addTasksParams(quint32 number, quint32 offset, QList<Task *> &lTasksOnline);
    bool isFiltering;
    ConnectionMode m_connectionMode;
    bool m_viewNewTask;
    mutable QMap<QDateTime,int> m_bufferTasks;
    mutable QMap<int,QList<QObject*> > m_dateTasks;
//    mutable QList<int> m_idTasks;
    mutable QList<Task*> m_lTasksOnline;
    QList<TransferTime *> m_lTransferTimes;
    QList<RepeatModels *> m_lRepeatModels;


//    QList<QObject*> m_dateObj;
    //    int indexOf(Note* note);
    bool isAbsentData() const;

public slots:
    void setIsAbsentData(bool isAbsentData);

signals:
   void goToDaySignal(QDate date);
   void getTasksModelSignal(quint32 number, quint32 offset);
   void updateSignal();
   void reseivedTasks(quint32 number);
   void isAbsentDataChanged(bool isAbsentData);

protected:
    QHash<int, QByteArray> roleNames() const;
private:
 //   QList<Task*> m_filterTasks;
//    QList<qint32> *m_selectedFilter;
    int m_rowCount;
    Task *m_activeTask;
    int m_activeId;
    int m_activeModelIndex;
//    QString m_searchString;
    mutable int m_cursor;
    quint32 m_offset;
//    QList<qint32> m_templateFilter;
    QList<int> m_listId;
    Filter *m_filter;
    bool m_changedSearch;
    uint m_lastNumber;

    //![2]
    bool m_isAbsentData;
};


class COMMONSHARED_EXPORT DateTasksModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DateTasksRoles {
        TimeRole = Qt::UserRole + 1,
        TasksRole
    };
    DateTasksModel(QObject *parent = 0);

    Q_INVOKABLE int getIndexOfDay(QDate date);
    Q_INVOKABLE QVariant getTasksModel(QDate date, int modelIndex);
    Q_INVOKABLE void clearTasksModel(int modelIndex);
//    void setViewMode(int source) { m_viewMode = (TasksFilterModel::e_vew_mode)source; refresh(); }


    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QDate getDate(int row) const;
    void refresh();
    void setSelectedFilter(QList<qint32> *source) {m_selectedFilter = source;}

    bool isFiltering;
    mutable QMap<QDateTime,int> *m_curBufferTasks;
    mutable QList<QDate> m_lDates;
    mutable int m_cursor;
    mutable QMap<int,QList<QObject*> > m_dateTasks;
    TasksFilterModel::e_vew_mode m_viewMode;


    int m_rowCount;
    QList<qint32> *m_selectedFilter;

};




COMMONSHARED_EXPORT QUuid GenerateGUID();
COMMONSHARED_EXPORT bool caseNoteMoreThan(const Task *task1, const Task *task2);
COMMONSHARED_EXPORT bool caseGroupMoreThan(const QObject *obj1, const QObject *obj2);
COMMONSHARED_EXPORT bool caseTagMoreThan(const QObject *obj1, const QObject *obj2);
COMMONSHARED_EXPORT bool caseCheckTagMoreThan(const QObject *obj1, const QObject *obj2);
COMMONSHARED_EXPORT bool caseUserMoreThan(const QObject *obj1, const QObject *obj2);
COMMONSHARED_EXPORT bool caseUserMoreThanUnreg(const QObject *obj1, const QObject *obj2);
extern COMMONSHARED_EXPORT QByteArray autorData;
extern COMMONSHARED_EXPORT quint64 keyData;

#endif // NOTE_H
