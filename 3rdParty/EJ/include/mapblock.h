#ifndef MAPPLUG_H
#define MAPPLUG_H
#include <QObject>
#include <QSGTexture>
#include "ej_interfaces.h"

#if defined(MAP_LIBRARY)
#  define MAPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MAPSHARED_EXPORT Q_DECL_IMPORT
#endif

class MAPSHARED_EXPORT MapBlock : public GroupBlock
{
public:
    MapBlock(quint32 userKey);
    virtual ~MapBlock() {}
//    void calcParams(QList<Block *> *plBlocks, bool force = false);
    void createDefault(QList<Block*> *lBlocks, int index);
    void addLabel(int num, qint32 lon, qint32 lat, QString name);
    void moveLabel(int num, qint32 lon, qint32 lat);
    void delLabels();
    void setMapCoords(QPoint coords);
    QPoint getMapCoords();
    void setMapScale(int scale);
    int getMapScale();
    virtual void beforeCalc(CalcParams *calcParams) override;
    virtual void childCalc(Block *child, CalcParams *calcParams) override;
    virtual void afterCalc(CalcParams *calcParams) override;

    QUuid key;
    QList<MapLabelBlock*> *getLabels() { return &lLabels; }

    enum NumProp {
        MAP_COORDS = 0,
        MAP_SCALE
    };

protected:
    QList<Block*> *lBlocks;
    QList<MapLabelBlock*> lLabels;
    QMap<qint8,Block*> mProperties;
    QSGTexture *m_texture;


//    QList<QPoint> lTrace;
};

class MAPSHARED_EXPORT MapPlug : public JotInterface
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "org.easyjotter.JotInterface" FILE "jotplugin.json")
//    Q_INTERFACES(JotInterface)

public:
    MapPlug();
    quint8 type() const override { return EXT_MAP; }
    QString name() const override { return QString("Map"); }
    Q_INVOKABLE bool addEnabled(quint8 type) override { Q_UNUSED(type) return true; }//
    Q_INVOKABLE QQuickItem* getActiveViewItem(int vid, QQuickItem *parent) override;
    Q_INVOKABLE QQuickItem* getActivePropItem(int vid, QQuickItem *parent, QString command, QString data) override;
    Q_INVOKABLE void addLabel(int lon, int lat);
    Q_INVOKABLE void moveLabel(int num, int lon, int lat);
    Q_INVOKABLE void delLabels();
    Q_INVOKABLE int getIndex();
    Q_INVOKABLE int setMapCoords(QPoint coords);
    Q_INVOKABLE void closeActiveView();
    Q_INVOKABLE bool isViewMode();
    Block *newBlock(int tid) override;
    ItemBlock* newItem(Block *block, CalcParams *calcParams, QQuickItem *parent) override;
    QQuickItem* createDefault(int index, Document *doc, QQuickItem *parent) override;
    void registerQMLTypes(QQmlContext *ctxt) override;
//    void setActiveBlock(Block* block) override { m_activeBlock = block; }
private:
    bool m_isActiveView;
    bool m_isViewMode;
    QQuickItem *m_childItem;

};

#endif // MAPPLUG_H
