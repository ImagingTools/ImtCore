#ifndef DOCPROPS_H
#define DOCPROPS_H

#include <QObject>
#include <QList>
#include <QGuiApplication>
#include <QClipboard>
#include "ejcommon.h"

//class Task;

enum e_PropDoc
{
    DOC_LINK = 0,
    DOC_ATTR,
    BASE_STYLE,
    PAGE_STYLE,
    PARAGRAPH_STYLE,
    TEXT_STYLE,
    TABLE_STYLE,
    CELL_STYLE,
    LINE_STYLE,
    BRUSH_STYLE,
    POINT_STYLE,
    BORDER_STYLE,
    DOC_LAYOUT,
    DOC_MARGINGS,
};

class COMMONSHARED_EXPORT EjDocMargings : public QObject, public EjPropDoc
{
    Q_OBJECT
    Q_PROPERTY(int top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(int bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(int left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(int right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(MultiplePages multiplePages READ multiplePages WRITE setMultiplePages NOTIFY multiplePagesChanged)

public:
	EjDocMargings(QObject *parent = nullptr);
	~EjDocMargings();
    enum NumProp {
        MARG_NUM = 0,
        MARG_TOP,
        MARG_BOTTOM,
        MARG_LEFT,
        MARG_RIGHT,
        MARG_MULTIPLE_PAGES,
    };
    enum MultiplePages {
        MP_NORMAL,
        MP_MIRROR_FIELDS,
    };
    Q_ENUMS(MultiplePages)

    int top();
    int bottom();
    int left();
    int right();
    MultiplePages multiplePages();
	void childCalc(EjBlock *child, EjCalcParams *calcParams) override;

public slots:
    void setTop(int top);
    void setBottom(int bottom);
    void setLeft(int left);
    void setRight(int right);
    void setMultiplePages(MultiplePages multiplePages);

signals:
    void topChanged(int top);
    void bottomChanged(int bottom);
    void leftChanged(int left);
    void rightChanged(int right);
    void multiplePagesChanged(MultiplePages multiplePages);

protected:
    int m_top;
    int m_bottom;
    int m_left;
    int m_right;
    MultiplePages m_multiplePages;

};

class COMMONSHARED_EXPORT EjDocLayout : public QObject, public EjPropDoc
{
    Q_OBJECT
    Q_PROPERTY(int docWidth READ docWidth WRITE setDocWidth NOTIFY docWidthChanged)
    Q_PROPERTY(int docHeight READ docHeight WRITE setDocHeight NOTIFY docHeightChanged)
    Q_PROPERTY(Orientation docOrientation READ docOrientation WRITE setDocOrientation NOTIFY docOrientationChanged)

public:
	EjDocLayout(QObject *parent = nullptr);
	~EjDocLayout();
    enum NumProp {
        DL_NUM = 0,
        DL_WIDTH,
        DL_HEIGHT,
        DL_ORIENTATION,
    };
    enum Orientation {
        ORN_PORTRAIT,
        ORN_LANDSCAPE,
    };
    Q_ENUMS(Orientation)

    int docWidth();
    int docHeight();
    Orientation docOrientation();
	void childCalc(EjBlock *child, EjCalcParams *calcParams) override;

public slots:
    void setDocWidth(int width);
    void setDocHeight(int height);
    void setDocOrientation(Orientation orientation);

signals:
    void docWidthChanged(int width);
    void docHeightChanged(int height);
    void docOrientationChanged(Orientation orientation);

protected:
    int m_docWidth;
    int m_docHeight;
    Orientation m_docOrientation;
};


class COMMONSHARED_EXPORT EjLinkProp : public QObject, public EjPropDoc
{
    Q_OBJECT
    Q_PROPERTY(int num READ num WRITE setNum NOTIFY numChanged)
    Q_PROPERTY(QString key READ key WRITE setKey NOTIFY keyChanged)
    Q_PROPERTY(QString key64 READ key64 WRITE setKey64 NOTIFY keyChanged)
    Q_PROPERTY(TypeLink typeLink READ typeLink WRITE setTypeLink NOTIFY typeLinkChanged)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)

public:
	EjLinkProp(QObject *parent = nullptr);
	~EjLinkProp();
    enum NumProp {
        LINK_NUM      = 0,
        LINK_KEY,
        LINK_TYPE,
        LINK_STATUS
    };

    enum TypeLink {
        LINK_INPUT = 0,
        LINK_OUTPUT,
        LINK_OUTPUT_TASK
    };

    Q_ENUMS(TypeLink)

    enum Status
    {
        ENABLED = 0,
        REMOVED
    };
    Q_ENUMS(Status)

    int num();
    Status status();
    QUuid keyUuid();
    Q_INVOKABLE void addLinkFromClipboard();
    Q_INVOKABLE void copyLinkToClipboard();
    Q_INVOKABLE void addLink64FromClipboard();
    Q_INVOKABLE void copyLink64ToClipboard();

    EjDocument *m_extDoc;

public slots:
    void setNum(int num);
    void setStatus(Status status);
    QString key();
    void setKey(QString key);
    void setKey(QUuid key);
    QString key64();
    void setKey64(QString key);    
    TypeLink typeLink();
    void setTypeLink(TypeLink typeLink);

signals:
    void numChanged(int num);
    void statusChanged(Status status);
    void keyChanged(QString key);
    void typeLinkChanged(TypeLink typeLink);

protected:
};


class COMMONSHARED_EXPORT EjAttrProp : public QObject, public EjPropDoc
{
    Q_OBJECT
    Q_PROPERTY(QString key READ key WRITE setKey NOTIFY keyChanged)
    Q_PROPERTY(QString key64 READ key64 WRITE setKey64 NOTIFY key64Changed)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(char type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int editLevel READ editLevel WRITE setEditLevel NOTIFY editLevelChanged)
    Q_PROPERTY(bool isEditAsParent READ isEditAsParent WRITE setIsEditAsParent NOTIFY isEditAsParentChanged)

public:
	EjAttrProp(QObject *parent=0);
    enum NumProp {
        AP_DOC_KEY      = 0,
        AP_DOC_NAME,
        AP_DOC_TYPE,
        AP_DOC_TEMPLATE_KEY,
        AP_DOC_ACCESS,
        AP_DOC_LAYOUT,
        AP_DOC_MARGINGS
    };



	~EjAttrProp() override;
    QString key();
    QString key64();
    QUuid keyUuid();
    qint64 keyInt64();
    QUuid keyTemplate();
    qint64 keyTemplate64();
    QString name();
    int editLevel();
    bool isEditAsParent();
    char type();
    Q_INVOKABLE void addLink(QString keyStr, quint8 typeLink);
    Q_INVOKABLE void removeLink(QString keyStr);
    Q_INVOKABLE void addLinkFromClipboard();
    Q_INVOKABLE void copyLinkToClipboard();
    Q_INVOKABLE void addLink64FromClipboard();
    Q_INVOKABLE void copyLink64ToClipboard();
	Q_INVOKABLE EjDocLayout* getDocLayout();
	Q_INVOKABLE void setDocLayout(EjDocLayout* docLayout);
	Q_INVOKABLE void copyDataLayout(EjDocLayout *result);
	Q_INVOKABLE EjDocMargings* getDocMargings();
	Q_INVOKABLE void setDocMargings(EjDocMargings* docMargings);
	Q_INVOKABLE void copyDataMargings(EjDocMargings *result);

    Q_INVOKABLE QVariant getLinks();
//	void changeLink();

    void beforeCalc(EjCalcParams *calcParams) override;
	void childCalc(EjBlock *child, EjCalcParams *calcParams) override;

	void addLink(QUuid uuidKey, EjLinkProp::TypeLink typeLink);
    void removeLink(QUuid uuidKey);

	QList<EjLinkProp*> m_lLinks;
    qint16 m_patchKey;
    quint16 m_patchVer;
    quint32 m_patchTime;

public slots:
    void setKey(QString key);
    void setKey64(QString key);
    void setKey(QUuid key);
    void setKeyInt64(qint64 key);
    void setTemplateKey(QString key);
    void setTemplateKey(QUuid key);
    void setTemplateKey64(qint64 key);
    void setName(QString name);
    void setType(char type);
    void setEditLevel(int source);
    void setIsEditAsParent(bool source);
signals:
    void keyChanged(QString key);
    void key64Changed(QString key);
    void nameChanged(QString name);
    void typeChanged(char type);
    void editLevelChanged();
    void isEditAsParentChanged();
protected:
	EjDocLayout *m_docLayout;
	EjDocMargings *m_docMargings;
};

#endif // DOCPROPS_H
