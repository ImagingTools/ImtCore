#ifndef COMMON_H
#define COMMON_H

#include "common_global.h"
#include <QString>
#include <QDataStream>
#include <QColor>
#include <QImage>
#include <QDateTime>
#include <QVariant>
#include <QUuid>
#include <QFont>


COMMONSHARED_EXPORT int writeSmallString(QDataStream &stream,const QString &source);
COMMONSHARED_EXPORT void readSmallString(QDataStream &stream, QString &source);
COMMONSHARED_EXPORT int writeBA(QDataStream &stream, const QByteArray &source);
COMMONSHARED_EXPORT void readBA(QDataStream &stream, QByteArray &source);
COMMONSHARED_EXPORT double getDValue(QString s_value, bool *bOk);
COMMONSHARED_EXPORT QString getDText(double value, int accuracy);
COMMONSHARED_EXPORT void initCommonResources();



enum e_typeBlocks
{
    BASE = 0,
    ENTER,         // 1
    SPACE,         // 2
    TEXT,          // 3
    CONTACT,       // 4
    COLUM_D,       // 5
    ROW_D,         // 6
    BASECELL,      // 7
    PROP_DOC_D,    // 8
    ERROR_BLOCK,   // 9
    FRAGMENT,      // 10
    END_FRAGMENT,  // 11
    TABLEFRAGMENT, // 12
    END_GROUP,     // 13
    NUM_STYLE,     // 14

    PROP_KEY,      // 15
    PROP_INT,      // 16
    PROP_DOUBLE,   // 17
    PROP_TXT,      // 18
    PROP_PNT,      // 19
    PROP_INT8,     // 20
    PROP_INT64,    // 21
    PROP_BA,       // 22
    PROP_COLOR,    // 23
    PROP_ACCESS,   // 24

    ACCESS_RULES = 49,

    GROUP_BLOCK = 50,   // ext blocks
    PROP_DOC,      // 51
    PROP_BIG_TEXT, // 52


    GROUP_PARAM,   // 53
    GROUP_COMPLEX, // 54
    VECTOR_POINT,  // 55
    VECTOR_TEXT,   // 56
    VECTOR_IMAGE,  // 57
    MAP_LABEL,     // 58
    FIGURE,        // 59


    EXT_MAP = 101, //
    EXT_DIAGRAM,   // 102
    EXT_IMAGE,     // 103
    EXT_TABLE,     // 104
    EXT_HADDEN_BAK,    // 105
    EXT_LABEL,  // 106
    EXT_LARGETEXT_BAK, // 107
    EXT_TASKS_GROUP,   // 108
};

enum e_viewMode
{
    RICH_TEXT = 0,
    FIXPAGE
};

enum e_statusMode
{
    READ_ONLY = 0,
    EDIT_TEXT,
    EDIT_CELL,
};

enum e_selectMode
{
    NO_SELECTED = 0,
    SELECTED,
};

enum e_securityMode
{
    SECURITY_FULL_EDIT = 0,
    SECURITY_EDIT = 32,
    SECURITY_INPUT = 64,
    SECURITY_READ_AND_SELECT = 128,
    SECURITY_READ = 255,
};

class ItemBlock;
class QQuickItem;
class JString;
class EjTableBlock;
class EjCellBlock;
class EjDocument;
class LargeTextBlock;
class LabelBlock;


struct EjCalcParams;

class COMMONSHARED_EXPORT EjBlock
{
public:
	EjBlock(EjBlock *parent_ = nullptr){
        type = width = x = y = ascent = descent = interval_top = interval_bottom = 0; flag_redraw = true; parent = parent_; }
	EjBlock(int f_type, EjBlock *parent_ = nullptr):EjBlock(parent_) {  type = static_cast<quint8>(f_type); }
	EjBlock(const EjBlock&) = default;
	EjBlock(EjBlock&&) = default;
	EjBlock& operator=(const EjBlock&) = default;
	EjBlock& operator=(EjBlock&&) = default;
	virtual ~EjBlock() {}
    quint8 type; // 0-BASE 1-ENTER 2-SPACE 3-TEXT 4-CONTACT 5 - IMAGE ...
    qint32 width;
    qint32 interval_top;
    qint32 ascent;
    quint16 descent;
    quint16 interval_bottom;
    qint32 x;
    qint32 y;
    bool flag_redraw;
	EjBlock *parent;


    int height() { return ascent + descent; }
    int fullHeight() { return interval_top + ascent + descent + interval_bottom; }
	EjBlock *rootBlock();


	virtual EjBlock* makeCopy()
    {
		EjBlock *res = new EjBlock();
        copyData(res);
        return res;
    }

	void copyData(EjBlock *result)
    {
        result->ascent = ascent;
        result->descent = descent;
        result->interval_top = interval_top;
        result->interval_bottom = interval_bottom;
        result->type = type;
        result->width = width;
        result->x = x;
        result->y = y;
    }


	bool operator ==(const EjBlock &other) const
    {
        return compare(other);
    }


	virtual bool compare(const EjBlock &other) const {
        bool res = (this->type == other.type) ? true : false;
        return res;
    }
    virtual ItemBlock* newItem(qreal viewScale, QQuickItem *parent) { Q_UNUSED(viewScale); Q_UNUSED(parent); return NULL; }
    virtual QQuickItem* newViewItem(int vid, QQuickItem *parent) { Q_UNUSED(vid); Q_UNUSED(parent); return NULL; }
    virtual void calcBlock(int &index, EjCalcParams *calcParams) { Q_UNUSED(index); Q_UNUSED(calcParams); }
    virtual bool isSelected(int &index, int &startSelect, int &endSelect) { return index >= startSelect && index <= endSelect; }
    virtual bool isGlassy() { return false; }
    bool isProperty();

};


class JString
{
public:
    void calcCursor();
    JString() { startBlock = endBlock = 0; height = width = x = y = ascent = 0; }
    qint32 startBlock;
    qint32 endBlock;
    qint32 width;
    qint32 height;
    qint16 x;
    qint32 y;
    qint32 ascent;
    qint32 interval;

};


class Page
{
public:
    Page();
    qint32 startBlock;
    qint32 endBlock;
    qint32 width;
    qint32 height;
    quint16 leftMarging;
    quint16 rightMarging;
    quint16 topMarging;
    quint16 bottomMarging;
    qint16 x;
    qint32 y;
    bool bOrientation;
    quint16 num;
    bool flag_redraw;
};



struct Param
{
    Param() {iValue = 0;}
    bool operator ==(const Param &other) const
    {
        return iValue == other.iValue && sValue == other.sValue;
    }

    quint32 iValue;
    QString sValue;
};

class MAlign
{
public:
    MAlign() { align = 0; }
    quint8 align; // 0- noAlign(auto) 1-alignLeft 2-alignRight 3-alignHCenter
                  // 4-alignTop 8-alignBottom 12-alignVCenter
                  // 16-(90) 32-(-90) 48-(180)
                  // 64-absolute 128-baseline
    bool isLeft() { return (align & 1) == 1; }
    bool isRigth() { return (align & 2) == 2; }
    bool isHCenter() { return (align & 3) == 3; }
    bool isTop() { return (align & 4) == 4; }
    bool isBottom() { return (align & 8) == 8; }
    bool isVCenter() { return (align & 12) == 12; }
};

class Fragment
{
public:
    enum Type {
        Clear,
        Bullets,
        Numbering,
        Bold,
        Italic,
        Underline,
        DBold,
        DItalic,
        DUnderline,
        AlignHAuto,
        AlignLeft,
        AlignRight,
        AlignHCenter,
        AlignVAuto,
        AlignTop,
        AlignBottom,
        AlignVCenter,
        Angle,
        Absolute,
        BaseLine,
        DAbsolute,
        DBaseLine,
        FontSize,
        FontFamily,
        PenColor,
        BrushColor,
        FontColor,
        ParagraphColor,
        Select
    };

    Fragment() { startBlock = endBlock = 0;  }
    virtual ~Fragment() {}
    qint32 startBlock;
    qint32 endBlock;
    quint8 type; // 0-normal text 1-bullets 2-numbering ......
    Param param;
    bool operator ==(const Fragment &other) const
    {
        return startBlock == other.startBlock && endBlock == other.endBlock && type == other.type; // && mParams == other.mParams;
    }

    friend QDataStream& operator <<(QDataStream &os, const Fragment &value)
    {
        return value.write(os);
    }

    friend QDataStream& operator >>(QDataStream &is, Fragment &value)
    {
        return value.read(is);
    }


    virtual QDataStream& write(QDataStream &os) const;

    virtual QDataStream& read(QDataStream &is);

};





class COMMONSHARED_EXPORT EjGroupBlock : public EjBlock
{
public:
	EjGroupBlock():EjBlock() { type = GROUP_BLOCK; m_index = -1; m_counts = 0; }
	EjBlock* findProp(QList<EjBlock *> *lBlocks, int type, int num, bool check = false);
	int findPropIndex(QList<EjBlock *> *lBlocks, int type, int num, int endIndex, bool check = false);
	int findPropIndex(QList<EjBlock *> *lBlocks, int type, int num, int startIndex, int endIndex, bool check = false);
	void addProp(QList<EjBlock*> *lBlocks, EjBlock *block);
	void remBlock(QList<EjBlock*> *lBlocks, EjBlock *block);
	virtual void createDefault(QList<EjBlock*> *lBlocks, int index);
	virtual void createDefaultWithNum(QList<EjBlock*> *lBlocks, int index, int num);
	virtual void remFromBlocks(QList<EjBlock*> *lBlocks);
    virtual void beforeCalc(EjCalcParams *calcParams) { Q_UNUSED(calcParams) }
	virtual void childCalc(EjBlock *child, EjCalcParams *calcParams) { Q_UNUSED(child) Q_UNUSED(calcParams) }
    virtual void afterCalc(EjCalcParams *calcParams) { Q_UNUSED(calcParams) }
	virtual QString getTextData(QList<EjBlock *> *lBlocks) { Q_UNUSED(lBlocks) return QString(); }
    void calcBlock(int &index, EjCalcParams *calcParams) override;
	void calcLenght(int &index, QList<EjBlock *> *lBlocks);
	void clear(QList<EjBlock*> *lBlocks);

	virtual EjBlock* makeCopy() override {
			EjGroupBlock *res = new EjGroupBlock();
            copyData(res);
            return res;
    }
	bool compare(const EjBlock &other) const override{
		return EjBlock::compare(other);
    }

    int m_index;
    int m_counts;
};


class EjDocument;
// INTERFACESHARED_EXPORT
class COMMONSHARED_EXPORT EjPropDoc : public EjGroupBlock
{

public:
	EjPropDoc():EjGroupBlock() { type = PROP_DOC; m_vid = 0; num = 0; m_doc = nullptr; }
	virtual ~EjPropDoc() override{}

	EjBlock* makeCopy() override;
	bool compare(const EjBlock &other) const override;
    quint8 m_vid;
    int num;

    EjDocument *m_doc;

};


class EjVectorText : public EjGroupBlock
{
	EjVectorText():EjGroupBlock() {type = VECTOR_TEXT;}
    ItemBlock* newItem(qreal viewScale, QQuickItem *parent) override;
    QQuickItem* newViewItem(int vid, QQuickItem *parent) override;

};


class EjPointBlock : public EjBlock
{
public:
	EjPointBlock():EjBlock(VECTOR_POINT) { }
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
};



class EjPropBase : public EjBlock
{
public:
	EjPropBase(int type):EjBlock(type) { num = 0; }
	virtual ~EjPropBase() override {}
    quint8 num;
	EjBlock* makeCopy() override = 0;
	bool compare(const EjBlock &other) const override = 0;
};

class COMMONSHARED_EXPORT EjPropKeyBlock : public EjPropBase
{
public:
	EjPropKeyBlock():EjPropBase(PROP_KEY) { key = QUuid::createUuid(); }
	EjPropKeyBlock(quint8 num_):EjPropBase(PROP_KEY) { num = num_; key = QUuid::createUuid(); }
    QUuid key;
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
};

class COMMONSHARED_EXPORT EjPropPntBlock : public EjPropBase
{
public:
	EjPropPntBlock():EjPropBase(PROP_PNT) { }
	EjPropPntBlock(quint8 num_):EjPropBase(PROP_PNT) { num = num_; }
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
    qint32 x_value;
    qint32 y_value;

};

class COMMONSHARED_EXPORT EjPropColorBlock : public EjPropBase
{
public:
	EjPropColorBlock():EjPropBase(PROP_COLOR) { }
	EjPropColorBlock(quint8 num_):EjPropBase(PROP_COLOR) { num = num_; }
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
    QColor color;
};

class COMMONSHARED_EXPORT EjPropIntBlock : public EjPropBase
{
public:
	EjPropIntBlock():EjPropBase(PROP_INT) { }
	EjPropIntBlock(quint8 num_):EjPropBase(PROP_INT) { num = num_; }
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
    qint32 value;
};


class COMMONSHARED_EXPORT EjPropInt8Block : public EjPropBase
{
public:
	EjPropInt8Block():EjPropBase(PROP_INT8) {  }
	EjPropInt8Block(quint8 num_):EjPropBase(PROP_INT8) { num = num_; }
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
    qint8 value;
};


class COMMONSHARED_EXPORT EjPropInt64Block : public EjPropBase
{
public:
	EjPropInt64Block():EjPropBase(PROP_INT64) {  }
	EjPropInt64Block(quint8 num_):EjPropBase(PROP_INT64) { num = num_; }
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
    qint64 value;
};


class COMMONSHARED_EXPORT EjPropTextBlock : public EjPropBase
{
public:
	EjPropTextBlock():EjPropBase(PROP_TXT) { }
	EjPropTextBlock(quint8 num_):EjPropBase(PROP_TXT) { num = num_; }
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
    QString text;
};

class COMMONSHARED_EXPORT EjPropByteArrayBlock : public EjPropBase
{
public:
	EjPropByteArrayBlock():EjPropBase(PROP_BA) {  }
	EjPropByteArrayBlock(quint8 num_):EjPropBase(PROP_BA) { num = num_; }
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
    QByteArray data;
};


class COMMONSHARED_EXPORT EjPropAccessBlock : public EjPropBase
{
public:
    enum e_editLevel{
        ONLY_READ,
        READ_AND_SELECT,
        READ_AND_WRITE,
        FULL_ACCESS
    };

	EjPropAccessBlock():EjPropBase(PROP_ACCESS) {  }
	EjPropAccessBlock(quint8 num_):EjPropBase(PROP_ACCESS) { num = num_; }

	EjBlock* makeCopy() override;
	bool compare(const EjBlock &other) const override;

    bool isEditAsParent() { return value & 0x01; }
    bool isSecAsParent() { return (value >> 5) & 0x01; }
    int editLevel() { return (value >> 1) & 0x03; }
    int secLevel() { return (value >> 6) & 0x03; }
    void setIsEditAsParent(bool source);
    void setIsSecAsParent(bool source);
    void setEditLevel(int source);
    void setSecLevel(int source);
    quint8 value;
};

class EjAccessBlock : public EjBlock
{
public:
    enum e_editLevel{
        ONLY_READ,
        READ_AND_SELECT,
        READ_AND_WRITE,
        FULL_ACCESS
    };

	EjAccessBlock():EjBlock() { type = ACCESS_RULES; }

	EjBlock* makeCopy() override;
	bool compare(const EjBlock &other) const override;
    bool isEditAsParent() { return value & 0x01; }
    bool isSecAsParent() { return (value >> 5) & 0x01; }
    int editLevel() { return (value >> 1) & 0x03; }
    int secLevel() { return (value >> 6) & 0x03; }
    void setIsEditAsParent(bool source);
    void setIsSecAsParent(bool source);
    void setEditLevel(int source);
    void setSecLevel(int source);
    quint8 value;
};

class COMMONSHARED_EXPORT EjPropBigTextBlock : public EjGroupBlock
{
public:
	EjPropBigTextBlock(): EjGroupBlock() { type = PROP_BIG_TEXT; num = 0; }
	EjPropBigTextBlock(quint8 num_):EjGroupBlock() { type = PROP_BIG_TEXT; num = num_; }
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
	void setText(QString source, QList<EjBlock*> *lBlocks);
	QString text(QList<EjBlock*> *lBlocks);
    quint8 num;
};

class EjBaseStyle;

class COMMONSHARED_EXPORT EjNumStyleBlock : public EjBlock
{
public:
	EjNumStyleBlock():EjBlock() { type = NUM_STYLE; num = 0; style = NULL; }
	~EjNumStyleBlock() override {}
    EjBaseStyle *style;

	EjBlock* makeCopy() override;
	bool compare(const EjBlock &other) const override;
    void calcBlock(int &index, EjCalcParams *calcParams) override;

    quint16 num;
};



class PointStyle;
class COMMONSHARED_EXPORT EjMapLabelBlock : public EjGroupBlock
{
public:
	EjMapLabelBlock():EjGroupBlock() { type = MAP_LABEL; m_style = NULL; }
	bool compare(const EjBlock &other) const override;
    void setCoords(qint32 x_souce, qint32 y_source);
protected:
    PointStyle *m_style;
};

class LineStyle;

class EjPolylineBlock : public EjGroupBlock
{
public:
	EjPolylineBlock():EjGroupBlock() { m_style = NULL; index = 0; }
	~EjPolylineBlock() override {}
	void setPoints(QList<EjBlock*> lPoints);
	QList<EjPointBlock*> *getPoints();
protected:
    LineStyle *m_style;
    int index;
};

class EjFragmentBlock : public EjBlock
{
public:
	EjFragmentBlock() { EjBlock(); type = FRAGMENT; vid = Fragment::Clear; countBlocks = iValue = index = startBlock = endBlock = 0; sValue = ""; }
    quint8 vid; // Fragment::Type
    quint16 countBlocks;
    quint16 index;
    quint32 iValue;
    QString sValue;

    qint32 startBlock;
    qint32 endBlock;

	virtual EjBlock* makeCopy() override;
	virtual bool compare(const EjBlock &other) const override;
};

class EjEndFragmentBlock : public EjBlock
{
public:
	EjEndFragmentBlock() { EjBlock(); type = END_FRAGMENT; vid = Fragment::Clear;  }
    quint8 vid; // Fragment::Type

	virtual EjBlock* makeCopy();
	virtual bool compare(const EjBlock &other) const;
};

class EjTableFragment : public EjFragmentBlock
{
public:
	EjTableFragment() { EjFragmentBlock(); type = TABLEFRAGMENT; }
    quint16 startRow() const { return quint16(startBlock >> 16);  }
    quint16 startColum() const { return quint16(startBlock & 0xff); }
    quint16 endRow() const { return quint16(endBlock >> 16); }
    quint16 endColum() const { return quint16(endBlock & 0xff); }

    void setStartRow(quint16 row);
    void setStartColum(quint16 colum);
    void setEndRow(quint16 row);
    void setEndColum(quint16 colum);
	EjTableFragment *makeCopy() override;
	virtual bool compare(const EjBlock &other) const override;
};



class EjSpaceBlock : public EjBlock
{
public:
	EjSpaceBlock():EjBlock() {type = SPACE;}
	virtual ~EjSpaceBlock() override {}

	EjBlock* makeCopy() override;
    void calcBlock(int &index, EjCalcParams *calcParams) override;
};

class COMMONSHARED_EXPORT EjTextBlock : public EjBlock
{
public:
	EjTextBlock():EjBlock() {type = TEXT;}
	EjTextBlock(QString txt) : EjTextBlock() { text = txt; }
	virtual ~EjTextBlock() override;
    QString text;

	EjBlock* makeCopy() override;
	bool compare(const EjBlock &other) const override;
    void calcBlock(int &index, EjCalcParams *calcParams) override;

};

struct EjSizeProp : public EjBlock {
	EjSizeProp() { type = 9; min = 0; max = 0xffff; }
    quint16 num;
    quint16 min;
    quint16 max;
    quint16 current;
    bool isMax;
	EjBlock* makeCopy();
	bool compare(const EjBlock &other) const;
};



#endif // COMMON_H
