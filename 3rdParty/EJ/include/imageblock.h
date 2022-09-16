#ifndef IMAGEPLUG_H
#define IMAGEPLUG_H
//#include "imageplug_global.h"
#include <QObject>
#include <QtCore/qglobal.h>
#include "EjDocument.h"


#if defined(IMAGE_LIBRARY)
#  define IMAGESHARED_EXPORT Q_DECL_EXPORT
#else
#  define IMAGESHARED_EXPORT Q_DECL_IMPORT
#endif

class IMAGESHARED_EXPORT ImageBlock : public EjGroupBlock
{
public:
    enum NumProp {
        IMG_KEY = 0,
        IMG_NAME,
        IMG_VID,
        IMG_SIZE,
        IMG_SHOW_BORDER,
        IMG_INTERACTIVE
    };

    ImageBlock();
    ImageBlock(QImage &image, EjDocument *doc, int index);
    ImageBlock(quint32 userKey);  // deprecate
    virtual ~ImageBlock(){}


    void createDefault(QList<EjBlock*> *lBlocks, int index) override;
//    virtual void beforeCalc(CalcParams *calcParams) override;
	virtual void childCalc(EjBlock *child, EjCalcParams *calcParams) override;
	virtual void afterCalc(EjCalcParams *calcParams) override;
    void setName(QList<EjBlock *> *lBlocks, QByteArray source);

//    QQuickItem *getItem(int &index, CalcParams *calcParams, QQuickItem *parentItem) override;
    void setVid(int vid,QList<EjBlock *> *lBlocks);
    void setSize(int width, int height, QList<EjBlock *> *lBlocks);
    void setIsInteractive(bool isInteractive, QList<EjBlock *> *lBlocks);
    void setIsShowBorders(bool isShowBorders, QList<EjBlock *> *lBlocks);
    virtual EjBlock* makeCopy() override;

//    QList<MapLabelBlock*> *getLabels() { return &lLabels; }

//protected:
    int key;
    QByteArray m_Name;
    QImage m_smallImage;
    quint8 m_vid; // 0 - auto, 1 - fix params
//    int m_descent;
    quint16 m_widthImage;
    quint16 m_heightImage;
    bool m_isInteractive;
    bool m_showBorders;

//    QList<Block*> *m_lBlocks;


//    Block* makeCopy();
//    bool compare(const Block &other) const;
//    QDataStream& write(QDataStream &os) const;

//    QDataStream& read(QDataStream &is);
};

//class IMAGEPLUGSHARED_EXPORT ImagePlug : public JotInterface
#endif
