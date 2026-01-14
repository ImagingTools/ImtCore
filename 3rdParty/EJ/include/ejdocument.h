#ifndef EJDOCUMENT_H
#define EJDOCUMENT_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "docprops.h"
#include "ejcommon.h"
#include "ejstyles.h"
//INTERFACESHARED_EXPORT COMMONSHARED_EXPORT
class PatchDoc;

class COMMONSHARED_EXPORT EjMapProps : public QObject
{
    Q_OBJECT
public:
	EjMapProps(QObject *parent=nullptr) : QObject(parent){ m_map = new QVariantMap(); }
	~EjMapProps() { delete m_map; }
    Q_INVOKABLE void setValue(QString key, QVariant value) { m_map->insert(key,value); }
    Q_INVOKABLE QVariant value(QString key) {
        QVariant res;
        if(m_map->contains(key))
            res = m_map->value(key);
        return res;
    }
private:
    QVariantMap *m_map;
};

class COMMONSHARED_EXPORT EjDocument : public QObject
{
    Q_OBJECT
	Q_PROPERTY(EjAttrProp* attributes READ attributes NOTIFY attributesChanged)

public:
	EjDocument(QUuid key = QUuid(), QObject *parent=0);
	~EjDocument();
    void clear();

	QList<EjBlock*> *lBlocks;
	QList<EjBlock*> *lPropBlocks;
    QList<JString*> *lStrings;
    QList<Page*> *lPages;
	QList<EjTableBlock*> *lTables;
    QList<LargeTextBlock*>  *lLabels_bak;
    QList<LabelBlock*>  *lLabels;
	QList<EjBaseStyle*>  *lStyles;
	EjAttrProp *m_attrProp;
    QList<PatchDoc*> *m_lPatches;
    QDateTime dt_modify;
    quint32 user_key;
    Q_INVOKABLE QDateTime getTime() { return dt_modify; }
    Q_INVOKABLE quint32 getUserKey() { return user_key; }
	Q_INVOKABLE EjAttrProp *attributes();
	void copy(EjDocument *doc);
	void copyData(EjDocument *doc);
	void move(EjDocument *doc);
	EjBaseStyle *getUndefinedStyle(int num);
	EjTextStyle *getTextStyle(int num);
	EjTextStyle *createDefaultTextStyle();
	EjTextStyle *fromTextStyles(EjTextStyle *inputStyle);
	EjParagraphStyle *createDefaultPrgStyle();
	EjParagraphStyle *getParagraphStyle(int num);
	EjParagraphStyle *fromParagraphStyles(EjParagraphStyle *inputStyle);
	EjCellStyle *createDefaultCellStyle();
	EjCellStyle *getCellStyle(int num);

	EjTextStyle *currentTextStyle(int block);
	EjParagraphStyle *currentParagraphStyle(int block);
	EjCellStyle *currentCellStyle(int block);

//    Q_INVOKABLE DocProps *getDocProps();
    int nextTableKey();
    int nextLabelKey();
	void appendStyle(EjBaseStyle *style);
    void calcProps();
    bool testProps();

signals:
    void attributesChanged();
	void tableStringsInserted(EjTableBlock *table, int first, int last);
	void tableColumsInserted(EjTableBlock *table, int first, int last);
	void tableStringsRemoved(EjTableBlock *table, int first, int last);
	void tableColumsRemoved(EjTableBlock *table, int first, int last);
	void tableStringsMoved(EjTableBlock *table, int start, int end, int dest);
	void tableColumsMoved(EjTableBlock *table, int start, int end, int dest);
	void cellDataChanged(EjCellBlock *cell);

protected:
};

#endif // DOCUMENT_H
