#include "docprops.h"
#include "ejdocument.h"
#include "ejtextcontrol.h"



QString EjLinkProp::key()
{
    QString str;
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, LINK_KEY);
    if(propKey)
        str = QString(propKey->key.toRfc4122().toHex());
    return str;
}

EjLinkProp::EjLinkProp(QObject *parent) : QObject(parent), EjPropDoc()
{
    m_vid = DOC_LINK;
    m_extDoc = nullptr;

}

EjLinkProp::~EjLinkProp()
{
    if(m_extDoc)
        delete m_extDoc;
    m_extDoc =  nullptr;
}

int EjLinkProp::num()
{
    int res = -1;
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, LINK_NUM);
    if(propInt)
    {
        res = propInt->value;
    }
    return res;
}

EjLinkProp::Status EjLinkProp::status()
{
    int res = 0;
	EjPropInt8Block *propInt = dynamic_cast<EjPropInt8Block*>(findProp(m_doc->lPropBlocks, PROP_INT8, LINK_STATUS));
    if(propInt)
    {
        res = propInt->value;
    }
    return static_cast<Status>(res);
}

QUuid EjLinkProp::keyUuid()
{
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, LINK_KEY);
    if(propKey)
        return propKey->key;
    return QUuid();
}


void EjLinkProp::addLinkFromClipboard()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString str = clipboard->text();
    if(str.length() > 15)
    {
        setKey(str);
    }

}

void EjLinkProp::copyLinkToClipboard()
{
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, LINK_KEY);
    if(!propKey)
        return;
    QString str = QString(propKey->key.toRfc4122().toHex());
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(str);
}

void EjLinkProp::addLink64FromClipboard()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString str = clipboard->text();
    str = str.prepend("0000000000000000");
    if(str.length() > 15)
    {
        setKey(str);
    }

}

void EjLinkProp::copyLink64ToClipboard()
{
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, LINK_KEY);
    if(!propKey)
        return;
    QString str = QString(propKey->key.toRfc4122().toHex());
    str = str.right(16);
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(str);
}


void EjLinkProp::setNum(int num)
{
    if (!m_doc)
        return;
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, LINK_NUM);
    if(!propInt)
    {
		propInt = new EjPropIntBlock(LINK_NUM);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = num;

    emit numChanged(num);
}

void EjLinkProp::setStatus(EjLinkProp::Status status)
{
    if (!m_doc)
        return;
	EjPropInt8Block *propInt = dynamic_cast<EjPropInt8Block*>(findProp(m_doc->lPropBlocks, PROP_INT8, LINK_STATUS));
    if(!propInt)
    {
		propInt = new EjPropInt8Block(LINK_STATUS);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = status;

    emit statusChanged(status);
}

void EjLinkProp::setKey(QString key)
{
    QByteArray ba = QByteArray::fromHex(key.toLatin1());
    QUuid uuid = QUuid::fromRfc4122(ba);
    if (keyUuid() == uuid || !m_doc)
        return;
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_INT, LINK_KEY);
    if(!propKey)
    {
		propKey = new EjPropKeyBlock(LINK_KEY);
        addProp(m_doc->lPropBlocks,propKey);
    }
    propKey->key = uuid;

    emit keyChanged(key);
}

void EjLinkProp::setKey(QUuid key)
{
    if (keyUuid() == key || !m_doc)
        return;
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_INT, LINK_KEY);
    if(!propKey)
    {
		propKey = new EjPropKeyBlock(LINK_KEY);
        addProp(m_doc->lPropBlocks,propKey);
    }
    propKey->key = key;

}

QString EjLinkProp::key64()
{
    QString str = key();
    str = str.right(16);
    return str;
}

void EjLinkProp::setKey64(QString key)
{
    key = key.prepend("0000000000000000");
    setKey(key);
}

EjLinkProp::TypeLink EjLinkProp::typeLink()
{
    EjLinkProp::TypeLink res = LINK_INPUT;
	EjPropInt8Block *propInt = dynamic_cast<EjPropInt8Block*>(findProp(m_doc->lPropBlocks, PROP_INT8, LINK_TYPE));
    if(propInt)
    {
        res = static_cast<EjLinkProp::TypeLink>(propInt->value);
    }
    return res;
}

void EjLinkProp::setTypeLink(EjLinkProp::TypeLink typeLink)
{
    if (!m_doc)
        return;
	EjPropInt8Block *propInt = dynamic_cast<EjPropInt8Block*>(findProp(m_doc->lPropBlocks, PROP_INT8, LINK_TYPE));
    if(!propInt)
    {
		propInt = new EjPropInt8Block(LINK_TYPE);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = typeLink;

    emit typeLinkChanged(typeLink);
}


EjAttrProp::EjAttrProp(QObject *parent) : QObject(parent), EjPropDoc()
{
    m_doc = nullptr;
    m_vid = DOC_ATTR;
    m_patchKey = m_patchVer = m_patchTime = 0;
    m_docMargings = nullptr;
    m_docLayout = nullptr;
}

EjAttrProp::~EjAttrProp()
{
//    qDebug() << __FILE__ << __LINE__ << "destructor AttrProp";
}

QString EjAttrProp::key()
{
    if(!m_doc)
        return QString();
    QString str;
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, AP_DOC_KEY);
    if(propKey)
        str = QString(propKey->key.toRfc4122().toHex());
    return str;
}

QString EjAttrProp::key64()
{
    QString str = key();
    str = str.right(16);
    return str;
}

QUuid EjAttrProp::keyUuid()
{
    if(!m_doc)
        return QUuid();
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, AP_DOC_KEY);
    if(propKey)
        return propKey->key;
    return QUuid();
}

qint64 EjAttrProp::keyInt64()
{
    QUuid uuid = keyUuid();
    return  *((qint64*)uuid.data4);
}

QUuid EjAttrProp::keyTemplate()
{
    if(!m_doc)
        return QUuid();
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, AP_DOC_TEMPLATE_KEY);
    if(propKey)
        return propKey->key;
    return QUuid();
}

qint64 EjAttrProp::keyTemplate64()
{
    QUuid uuid = keyTemplate();
    return  *((qint64*)uuid.data4);
}

QString EjAttrProp::name()
{
    if(!m_doc)
        return QString();
    QString txt;
	EjBlock* curBlock = findProp(m_doc->lPropBlocks, PROP_BIG_TEXT, AP_DOC_NAME);
    if(curBlock)
    {
		EjPropBigTextBlock *propText = (EjPropBigTextBlock*)curBlock;
        if(propText)
            txt = propText->text(m_doc->lPropBlocks);
    }
    return txt;
}

int EjAttrProp::editLevel()
{
	int res = EjPropAccessBlock::READ_AND_WRITE;
    if(!m_doc)
        return res;
    QString txt;
	EjPropAccessBlock *propAccess = dynamic_cast<EjPropAccessBlock*>(findProp(m_doc->lPropBlocks, PROP_ACCESS, AP_DOC_ACCESS));
    if(propAccess)
        res = propAccess->editLevel();

    return res;
}

bool EjAttrProp::isEditAsParent()
{
    bool res = true;
    if(!m_doc)
        return res;
    QString txt;
	EjPropAccessBlock *propAccess = dynamic_cast<EjPropAccessBlock*>(findProp(m_doc->lPropBlocks, PROP_ACCESS, AP_DOC_ACCESS));
    if(propAccess)
        res = propAccess->isEditAsParent();

    return res;
}

char EjAttrProp::type()
{
    char res = 0;
    if(!m_doc)
        return res;
    QString txt;
	EjPropInt8Block *propInt = (EjPropInt8Block*)findProp(m_doc->lPropBlocks, PROP_INT8, AP_DOC_TYPE);
    if(propInt)
        res = propInt->value;

    return res;
}

void EjAttrProp::addLink(QString keyStr, quint8 typeLink)
{
    if(!m_doc)
        return;

    if(keyStr.length() > 15)
    {
        QByteArray ba = QByteArray::fromHex(keyStr.toLatin1());
        QUuid uuid = QUuid::fromRfc4122(ba);
        addLink(uuid,static_cast<EjLinkProp::TypeLink>(typeLink));
    }
}

void EjAttrProp::removeLink(QString keyStr)
{
    if(!m_doc)
        return;

    if(keyStr.length() > 15)
    {
//        setKey(str);
        QByteArray ba = QByteArray::fromHex(keyStr.toLatin1());
        QUuid uuid = QUuid::fromRfc4122(ba);
        removeLink(uuid);
    }
}

void EjAttrProp::addLinkFromClipboard()
{
    if(!m_doc)
        return;
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString str = clipboard->text();
    if(str.length() > 15)
    {
        QByteArray ba = QByteArray::fromHex(str.toLatin1());
        QUuid uuid = QUuid::fromRfc4122(ba);
        addLink(uuid,EjLinkProp::LINK_INPUT);
    }

}

void EjAttrProp::copyLinkToClipboard()
{
    if(!m_doc)
        return;
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, AP_DOC_KEY);
    if(!propKey)
        return;
    QString str = QString(propKey->key.toRfc4122().toHex());
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(str);
}

void EjAttrProp::addLink64FromClipboard()
{
    if(!m_doc)
        return;
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString str = clipboard->text();
    str = str.prepend("0000000000000000");

    if(str.length() > 15)
    {
        QByteArray ba = QByteArray::fromHex(str.toLatin1());
        QUuid uuid = QUuid::fromRfc4122(ba);
        addLink(uuid,EjLinkProp::LINK_INPUT);
    }

}

void EjAttrProp::copyLink64ToClipboard()
{
    if(!m_doc)
        return;
	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, AP_DOC_KEY);
    if(!propKey)
        return;
    QString str = QString(propKey->key.toRfc4122().toHex());
    str = str.right(16);
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(str);

}

EjDocLayout *EjAttrProp::getDocLayout()
{
    if(!m_doc)
        return nullptr;
    EjDocLayout *docLayout = (EjDocLayout*)findProp(m_doc->lPropBlocks, PROP_DOC, AP_DOC_LAYOUT);
    if(!docLayout)
    {
        docLayout = new EjDocLayout();
        docLayout->num = AP_DOC_LAYOUT;
        docLayout->createDefaultWithNum(m_doc->lPropBlocks,m_index + m_counts, AP_DOC_LAYOUT);
        this->m_counts += docLayout->m_counts + 1;
    }
    return docLayout;
}

void EjAttrProp::setDocLayout(EjDocLayout *docLayout)
{
    if(!m_doc || !docLayout)
        return;
    EjDocLayout *docLayoutLocal = getDocLayout();
    docLayoutLocal->setDocWidth(docLayout->docWidth());
    docLayoutLocal->setDocHeight(docLayout->docHeight());
    docLayoutLocal->setDocOrientation(docLayout->docOrientation());
    m_doc->calcProps();
}

void EjAttrProp::copyDataLayout(EjDocLayout *result)
{
    if(!result)
        return;
    if(!m_doc)
    {
        result->setDocWidth(21000);
        result->setDocHeight(29700);
        result->setDocOrientation(EjDocLayout::ORN_PORTRAIT);
        return;
    }
    EjDocLayout *docLayout = getDocLayout();
    result->setDocWidth(docLayout->docWidth());
    result->setDocHeight(docLayout->docHeight());
    result->setDocOrientation(docLayout->docOrientation());
}

EjDocMargings *EjAttrProp::getDocMargings()
{
    if(!m_doc)
        return nullptr;
    EjDocMargings *docMargings = (EjDocMargings*)findProp(m_doc->lPropBlocks, PROP_DOC, AP_DOC_MARGINGS);
    if(!docMargings)
    {
        docMargings = new EjDocMargings();
        docMargings->num = AP_DOC_MARGINGS;
        docMargings->createDefaultWithNum(m_doc->lPropBlocks,m_index + m_counts, AP_DOC_MARGINGS);
        this->m_counts += docMargings->m_counts + 1;
    }
    return docMargings;
}

void EjAttrProp::setDocMargings(EjDocMargings *docMargings)
{
    if(!m_doc || !docMargings)
        return;
    EjDocMargings *docMargingsLocal = getDocMargings();
    docMargingsLocal->setTop(docMargings->top());
    docMargingsLocal->setBottom(docMargings->bottom());
    docMargingsLocal->setLeft(docMargings->left());
    docMargingsLocal->setRight(docMargings->right());
    docMargingsLocal->setMultiplePages(docMargings->multiplePages());
    m_doc->calcProps();
}

void EjAttrProp::copyDataMargings(EjDocMargings *result)
{
    if(!result)
        return;
    if(!m_doc)
    {
        result->setTop(2000);
        result->setBottom(2000);
        result->setLeft(3000);
        result->setRight(1500);
        result->setMultiplePages(EjDocMargings::MP_NORMAL);
        return;
    }
    EjDocMargings *docMargings = getDocMargings();
    result->setTop(docMargings->top());
    result->setBottom(docMargings->bottom());
    result->setLeft(docMargings->left());
    result->setRight(docMargings->right());
    result->setMultiplePages(docMargings->multiplePages());
}

void EjAttrProp::setKey(QString key)
{
    if (!m_doc || key.length() < 16)
        return;
    QByteArray ba = QByteArray::fromHex(key.toLatin1());
    QUuid uuid = QUuid::fromRfc4122(ba);

    if(uuid.isNull())
        return;

	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, AP_DOC_KEY);
    if(!propKey)
    {
		propKey = new EjPropKeyBlock(AP_DOC_KEY);
        m_doc->lPropBlocks->insert(m_index + 1,propKey);
        propKey->parent = this;
        m_counts++;
    }
    propKey->key = uuid;
    emit keyChanged(key);
}

void EjAttrProp::setKey64(QString key)
{
    key = key.prepend("0000000000000000");
    setKey(key);
}

void EjAttrProp::setKey(QUuid key)
{
    QByteArray ba = key.toRfc4122();
    setKey(QString(ba.toHex()));
}

void EjAttrProp::setKeyInt64(qint64 key)
{
    QUuid uuid;
    *((qint64*)uuid.data4) = key;
    setKey(uuid);
}

void EjAttrProp::setTemplateKey(QString key)
{
    if (!m_doc || key.length() < 16)
        return;
    QByteArray ba = QByteArray::fromHex(key.toLatin1());
    QUuid uuid = QUuid::fromRfc4122(ba);

    if(uuid.isNull())
        return;

	EjPropKeyBlock *propKey = (EjPropKeyBlock*)findProp(m_doc->lPropBlocks, PROP_KEY, AP_DOC_TEMPLATE_KEY);
    if(!propKey)
    {
		propKey = new EjPropKeyBlock(AP_DOC_TEMPLATE_KEY);
        m_doc->lPropBlocks->insert(m_index + 1,propKey);
        propKey->parent = this;
        m_counts++;
    }
    propKey->key = uuid;
}

void EjAttrProp::setTemplateKey(QUuid key)
{
    QByteArray ba = key.toRfc4122();
    setTemplateKey(QString(ba.toHex()));
}

void EjAttrProp::setTemplateKey64(qint64 key)
{
    QUuid uuid;
    *((qint64*)uuid.data4) = key;
    setTemplateKey(uuid);
}

void EjAttrProp::setName(QString name)
{
    if (!m_doc)
        return;
//    PropTextBlock *propText = (PropTextBlock*)findProp(m_doc->lPropBlocks, PROP_TXT, DOC_NAME);
	EjPropBigTextBlock *propText = (EjPropBigTextBlock*)findProp(m_doc->lPropBlocks, PROP_BIG_TEXT, AP_DOC_NAME);
    if(!propText)
    {
		propText = new EjPropBigTextBlock(AP_DOC_NAME);
        propText->createDefault( m_doc->lPropBlocks, m_index + 1);
//        m_doc->lPropBlocks->insert(m_index + 1,(GroupBlock*)propText);
        ((EjGroupBlock*)propText)->parent = this;
        m_counts += propText->m_counts + 1;
//        addProp(m_document->lBlocks,propKey);
    }
    int counts_text_bak = propText->m_counts;
    propText->setText(name,m_doc->lPropBlocks);
    m_counts += (propText->m_counts - counts_text_bak);
//    propText->text = name;
    emit nameChanged(name);

}

void EjAttrProp::setType(char type)
{
    if (!m_doc)
        return;
	EjPropInt8Block *propInt = (EjPropInt8Block*)findProp(m_doc->lPropBlocks, PROP_INT8, AP_DOC_TYPE);
    if(!propInt)
    {
		propInt = new EjPropInt8Block(AP_DOC_TYPE);
        addProp(m_doc->lPropBlocks,propInt);
        propInt->parent = this;
    }
    propInt->value = type;
    emit typeChanged(type);

}

void EjAttrProp::setEditLevel(int source)
{
    if (!m_doc)
        return;
	EjPropAccessBlock *propAccess = dynamic_cast<EjPropAccessBlock*>(findProp(m_doc->lPropBlocks, PROP_ACCESS, AP_DOC_ACCESS));
    if(!propAccess)
    {
		propAccess = new EjPropAccessBlock();
        addProp(m_doc->lPropBlocks,propAccess);
        propAccess->parent = this;
    }
    propAccess->setEditLevel(source);
    emit editLevelChanged();
}

void EjAttrProp::setIsEditAsParent(bool source)
{
    if (!m_doc)
        return;
	EjPropAccessBlock *propAccess = dynamic_cast<EjPropAccessBlock*>(findProp(m_doc->lPropBlocks, PROP_ACCESS, AP_DOC_ACCESS));
    if(!propAccess)
    {
		propAccess = new EjPropAccessBlock();
        addProp(m_doc->lPropBlocks,propAccess);
        propAccess->parent = this;
    }
    propAccess->setIsEditAsParent(source);
    emit isEditAsParentChanged();
}

void EjAttrProp::addLink(QUuid uuidKey, EjLinkProp::TypeLink typeLink)
{
    if(uuidKey == keyUuid())
        return;
    if(!uuidKey.isNull())
    {
        int max = 0;
        foreach (EjLinkProp *prop, m_lLinks) {
            if(max < prop->num())
                max = prop->num();
            if(prop->keyUuid() == uuidKey) {
                prop->setStatus(EjLinkProp::ENABLED);
                prop->setTypeLink(typeLink);
                return;
            }
        }
        max++;
        EjLinkProp *linkProp = new EjLinkProp();
        linkProp->m_doc = m_doc;
        linkProp->createDefault(m_doc->lPropBlocks,m_index + m_counts);
        linkProp->setKey(uuidKey);
        linkProp->setNum(max);
        linkProp->setTypeLink(typeLink);
        this->m_counts += linkProp->m_counts + 1;
        m_lLinks.append(linkProp);
    }
}

void EjAttrProp::removeLink(QUuid uuidKey)
{
    if(uuidKey == keyUuid())
        return;
    if(!uuidKey.isNull())
    {
        int max = 0;
        foreach (EjLinkProp *prop, m_lLinks) {
            if(max < prop->num())
                max = prop->num();
            if(prop->keyUuid() == uuidKey) {
                prop->setStatus(EjLinkProp::REMOVED);
                return;
            }
        }
    }
}

QVariant EjAttrProp::getLinks()
{
    QList<QObject*> data;

    foreach (EjLinkProp *linkProp, m_lLinks) {
        if(linkProp->status() == EjLinkProp::ENABLED)
            data.append(linkProp);
//        listLinks << QString(linkProp->key().toRfc4122().toHex());
    }
    return QVariant::fromValue(data);
}


void EjAttrProp::beforeCalc(EjCalcParams *calcParams)
{
    if(calcParams->control->doc) {
        m_doc = calcParams->control->doc;
        m_doc->m_attrProp = this;
    }
    m_lLinks.clear();
}

void EjAttrProp::childCalc(EjBlock *child, EjCalcParams *calcParams)
{
    Q_UNUSED(calcParams)
	EjPropDoc *propDoc;
    switch (child->type) {
    case PROP_DOC:
		propDoc = dynamic_cast<EjPropDoc *>(child);
        if(propDoc->m_vid == DOC_LINK)
        {
            EjLinkProp *curLink = dynamic_cast<EjLinkProp*>(child);
            m_lLinks.append(curLink);
            curLink->m_doc = this->m_doc;
        }
        else if(propDoc->m_vid == DOC_LAYOUT)
        {
            EjDocLayout *docLayout = dynamic_cast<EjDocLayout*>(child);
            docLayout->m_doc = this->m_doc;
        }
        else if(propDoc->m_vid == DOC_MARGINGS)
        {
            EjDocMargings *docMargings= dynamic_cast<EjDocMargings*>(child);
            docMargings->m_doc = this->m_doc;
        }
        break;
    default:
        break;
    }
}



EjDocLayout::EjDocLayout(QObject *parent): QObject(parent), EjPropDoc()
{
    m_vid = DOC_LAYOUT;
    m_docWidth = 21000;
    m_docHeight = 29700;
    m_docOrientation = ORN_PORTRAIT;
}

EjDocLayout::~EjDocLayout()
{

}

int EjDocLayout::docWidth()
{
    return m_docWidth;
}

int EjDocLayout::docHeight()
{
    return m_docHeight;
}

EjDocLayout::Orientation EjDocLayout::docOrientation()
{
    return m_docOrientation;
}

void EjDocLayout::childCalc(EjBlock *child, EjCalcParams *calcParams)
{
    Q_UNUSED(calcParams)
	EjPropIntBlock *curInt;
	EjPropInt8Block *curInt8;
    switch (child->type) {
    case PROP_INT:
		curInt = (EjPropIntBlock *)child;
        if(curInt->num == DL_NUM)
        {
            num = curInt->value;
        }
        if(curInt->num == DL_WIDTH)
        {
            m_docWidth = curInt->value;
        }
        if(curInt->num == DL_HEIGHT)
        {
            m_docHeight = curInt->value;
        }
        break;
    case PROP_INT8:
		curInt8 = (EjPropInt8Block *)child;
        if(curInt8->num == DL_ORIENTATION)
        {
            m_docOrientation = static_cast<Orientation>(curInt8->value);
        }
        break;
    default:
        break;
    }
}

void EjDocLayout::setDocWidth(int width)
{
    if(m_docWidth == width)
        return;
    m_docWidth = width;
    if (!m_doc) {
        emit docWidthChanged(width);
        return;
    }
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, DL_WIDTH);
    if(!propInt && width != 21000)
    {
		propInt = new EjPropIntBlock(DL_WIDTH);
        addProp(m_doc->lPropBlocks,propInt);
    }
    if(propInt)
    {
        propInt->value = width;
        emit docWidthChanged(width);
    }
}

void EjDocLayout::setDocHeight(int height)
{
    if(m_docHeight == height)
        return;
    m_docHeight = height;
    if (!m_doc) {
        emit docHeightChanged(height);
        return;
    }
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, DL_HEIGHT);
    if(!propInt && height != 29700)
    {
		propInt = new EjPropIntBlock(DL_HEIGHT);
        addProp(m_doc->lPropBlocks,propInt);
    }
    if(propInt)
        propInt->value = height;

    emit docHeightChanged(height);
}

void EjDocLayout::setDocOrientation(EjDocLayout::Orientation orientation)
{
    if(m_docOrientation == orientation)
        return;
    m_docOrientation = orientation;
    if (!m_doc) {
        emit docOrientationChanged(orientation);
        return;
    }
	EjPropInt8Block *propInt = dynamic_cast<EjPropInt8Block*>(findProp(m_doc->lPropBlocks, PROP_INT8, DL_ORIENTATION));
    if(!propInt && orientation != ORN_PORTRAIT)
    {
		propInt = new EjPropInt8Block(DL_ORIENTATION);
        addProp(m_doc->lPropBlocks,propInt);
    }
    if(propInt)
        propInt->value = orientation;

    emit docOrientationChanged(orientation);
}


EjDocMargings::EjDocMargings(QObject *parent): QObject(parent), EjPropDoc()
{
    m_vid = DOC_MARGINGS;
    m_top = 2000;
    m_bottom = 2000;
    m_left = 3000;
    m_right = 1500;
    m_multiplePages = MP_NORMAL;
}

EjDocMargings::~EjDocMargings()
{

}

int EjDocMargings::top()
{
    return m_top;
}

int EjDocMargings::bottom()
{
    return m_bottom;
}

int EjDocMargings::left()
{
    return m_left;
}

int EjDocMargings::right()
{
    return m_right;
}

EjDocMargings::MultiplePages EjDocMargings::multiplePages()
{
    return m_multiplePages;
}

void EjDocMargings::childCalc(EjBlock *child, EjCalcParams *calcParams)
{
    Q_UNUSED(calcParams)
	EjPropIntBlock *curInt;
	EjPropInt8Block *curInt8;
    switch (child->type) {
    case PROP_INT:
		curInt = (EjPropIntBlock *)child;
        if(curInt->num == MARG_NUM)
        {
            num = curInt->value;
        }
        if(curInt->num == MARG_TOP)
        {
            m_top = curInt->value;
        }
        if(curInt->num == MARG_BOTTOM)
        {
            m_bottom = curInt->value;
        }
        if(curInt->num == MARG_LEFT)
        {
            m_left = curInt->value;
        }
        if(curInt->num == MARG_RIGHT)
        {
            m_right = curInt->value;
        }
        break;
    case PROP_INT8:
		curInt8 = (EjPropInt8Block *)child;
        if(curInt8->num == MARG_MULTIPLE_PAGES)
        {
            m_multiplePages = static_cast<MultiplePages>(curInt8->value);
        }
        break;
    default:
        break;
    }
}

void EjDocMargings::setTop(int top)
{
    if(m_top == top)
        return;
    m_top = top;
    if (!m_doc) {
        emit topChanged(top);
        return;
    }
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, MARG_TOP);
    if(!propInt && top != 2000)
    {
		propInt = new EjPropIntBlock(MARG_TOP);
        addProp(m_doc->lPropBlocks,propInt);
    }
    if(propInt)
        propInt->value = top;

    emit topChanged(top);

}

void EjDocMargings::setBottom(int bottom)
{
    if(m_bottom == bottom)
        return;
    m_bottom = bottom;
    if (!m_doc) {
        emit bottomChanged(bottom);
        return;
    }
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, MARG_BOTTOM);
    if(!propInt && bottom != 2000)
    {
		propInt = new EjPropIntBlock(MARG_BOTTOM);
        addProp(m_doc->lPropBlocks,propInt);
    }
    if(propInt)
        propInt->value = bottom;

    emit bottomChanged(bottom);
}

void EjDocMargings::setLeft(int left)
{
    if(m_left == left)
        return;
    m_left = left;
    if (!m_doc) {
        emit leftChanged(left);
        return;
    }
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, MARG_LEFT);
    if(!propInt && left != 3000)
    {
		propInt = new EjPropIntBlock(MARG_LEFT);
        addProp(m_doc->lPropBlocks,propInt);
    }
    if(propInt)
        propInt->value = left;

    emit leftChanged(left);

}

void EjDocMargings::setRight(int right)
{
    if(m_right == right)
        return;
    m_right = right;
    if (!m_doc) {
        emit rightChanged(right);
        return;
    }
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, MARG_RIGHT);
    if(!propInt && right != 1500)
    {
		propInt = new EjPropIntBlock(MARG_RIGHT);
        addProp(m_doc->lPropBlocks,propInt);
    }
    if(propInt)
        propInt->value = right;

    emit rightChanged(right);

}

void EjDocMargings::setMultiplePages(EjDocMargings::MultiplePages multiplePages)
{
    if(m_multiplePages == multiplePages)
        return;
    m_multiplePages = multiplePages;
    if (!m_doc) {
        emit multiplePagesChanged(multiplePages);
        return;
    }
	EjPropInt8Block *propInt = dynamic_cast<EjPropInt8Block*>(findProp(m_doc->lPropBlocks, PROP_INT8, MARG_MULTIPLE_PAGES));
    if(!propInt && multiplePages != MP_NORMAL)
    {
		propInt = new EjPropInt8Block(MARG_MULTIPLE_PAGES);
        addProp(m_doc->lPropBlocks,propInt);
    }
    if(propInt)
        propInt->value = multiplePages;

    emit multiplePagesChanged(multiplePages);

}
