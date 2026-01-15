#ifndef TEXTPLUGIN_H
#define TEXTLUGIN_H
#include <QObject>
#include "interfaces.h"
//#include "jotplugin_global.h"


class TextBlock : public GroupBlock
{
public:
    DiagramBlock();
    virtual ~DiagramBlock() {}
//    void calcParams(QList<Block *> *plBlocks, bool force = false);
//    void createDefault(QList<Block*> *lBlocks, int index) override;
    virtual void beforeCalc(QList<Block*> *lBlocks_) override;
    virtual void childCalc(Block *child) override;
//    virtual void afterCalc() override {}

    QUuid key;
//    QList<LabelBlock*> *getLabels() { return &lLabels; }

    enum NumProp {
        MAP_COORDS = 0,
        MAP_SCALE
    };

protected:
    QList<Block*> *lBlocks;
//    QList<LabelBlock*> lLabels;
//    QMap<qint8,Block*> mProperties;
//    QSGTexture *m_texture;


//    QList<QPoint> lTrace;
};

class TextPlug : public JotInterface
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "org.easyjotter.JotInterface" FILE "jotplugin.json")
//    Q_INTERFACES(JotInterface)

public:
    TextPlug();
    quint8 type() const override { return EXT_DIAGRAM; }
    QString name() const override { return QString("Figures plugin"); }
    GroupBlock* newBlock() override;
    ItemBlock* newItem(qreal viewScale, Block* block, QQuickItem *parent) override;
//    void createDefault(QList<Block*> *lBlocks, int index) override;
    Q_INVOKABLE QQuickItem* getActiveViewItem(int vid, QQuickItem *parent) override;
};

#endif // TEXTPLUGIN_H
