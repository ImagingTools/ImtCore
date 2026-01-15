#ifndef LARGETEXTPLUG_H
#define LARGETEXTPLUG_H
//#include "imageplug_global.h"
#include <QObject>
#include "interfaces.h"

class LargeTextBlock : public GroupBlock
{
public:
    enum NumProp {
        LARGE_KEY = 0,
        LARGE_NAME,
        LARGE_TEXT_HIDDEN,
    };

    LargeTextBlock(quint32 userKey);
    virtual ~LargeTextBlock(){}


    void createDefault(QList<Block*> *lBlocks, int index) override;
//    virtual void beforeCalc(QList<Block*> *lBlocks) override;
    virtual void childCalc(Block *child) override;
    virtual void afterCalc(CalcParams *calcParams) override;
    void calcBlock(int &index, CalcParams *calcParams) override;
    void calcString(JString *string, Page *page, CalcParams *calcParams);
    virtual bool isGlassy() override { return true; }


    QQuickItem *getItem(int &index, CalcParams *calcParams, QQuickItem *parentItem) override;


//    QList<MapLabelBlock*> *getLabels() { return &lLabels; }

//protected:
    QUuid key;
    QByteArray name;
    QImage small_image;
    quint8 vid;
//    int m_descent;
    quint16 m_width_image;
    quint16 m_height_image;
    bool m_is_interactive;
    bool m_show_border;

//    QList<Block*> *m_lBlocks;


//    Block* makeCopy();
//    bool compare(const Block &other) const;
//    QDataStream& write(QDataStream &os) const;

//    QDataStream& read(QDataStream &is);
};

//class IMAGEPLUGSHARED_EXPORT ImagePlug : public JotInterface
class LargeTextPlug : public JotInterface
{
    Q_OBJECT
public:
    LargeTextPlug();
    quint8 type() const override { return EXT_LARGETEXT_BAK; }
    QString name() const override { return QString("Text block"); }
    Block *newBlock(int tid) override;
    ItemBlock* newItem(Block *block, CalcParams *calcParams, QQuickItem *parent) override;
    QQuickItem* createDefault(int index, Document *doc, QQuickItem *parent) override;
    Q_INVOKABLE QQuickItem* getActiveViewItem(int vid, QQuickItem *parent) override;
    Q_INVOKABLE QQuickItem* getActivePropItem(int vid, QQuickItem *parent, QString command, QString data) override;
    int menuActivate(QString command, QString data, PopupMenuModel &popupModel, e_statusMode statusMode) override; // 0 - not active, 1 - item active, 2 -  menu active

public slots:
    void addImage(QString command, QString data);
    void exitProp();

};

#endif // LARGETEXTPLUG_H
