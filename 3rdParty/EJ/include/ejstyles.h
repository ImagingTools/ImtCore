#ifndef STYLES_H
#define STYLES_H
#include <QDataStream>
#include <QRgb>
#include <QFont>
#include <QColor>
#include <QFontMetrics>
#include "docprops.h"



class COMMONSHARED_EXPORT EjBaseStyle : public QObject, public EjPropDoc
{
public:
	EjBaseStyle(QObject *parent = 0) : QObject(parent), EjPropDoc() { m_vid = BASE_STYLE; num = 0; m_parentStyle = NULL; }
	EjBaseStyle(int num_) : EjBaseStyle(nullptr) { num = num_; }
	EjBaseStyle *getStyleByNum(int num, QList<EjBaseStyle *> *lStyles);
	virtual void createDefault(QList<EjBlock*> *lBlocks, int index) override;

    enum NumProp {
        STYLE_NUM = 0,
        STYLE_PARENT_NUM
    };
	EjBaseStyle *m_parentStyle;

};

class COMMONSHARED_EXPORT EjTextStyle : public EjBaseStyle
{
    Q_OBJECT
    Q_PROPERTY(e_fontStyle fontStyle READ fontStyle WRITE setFontStyle NOTIFY fontStyleChanged)
    Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily NOTIFY fontFamilyChanged)
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(bool fontBold READ fontBold WRITE setFontBold NOTIFY fontBoldChanged)
    Q_PROPERTY(bool fontItalic READ fontItalic WRITE setFontItalic NOTIFY fontItalicChanged)
    Q_PROPERTY(bool fontStrikeOut READ fontStrikeOut WRITE setFontStrikeOut NOTIFY fontStrikeOutChanged)
    Q_PROPERTY(bool fontUnderline READ fontUnderline WRITE setFontUnderline NOTIFY fontUnderlineChanged)
    Q_PROPERTY(QColor fontColor READ fontColor WRITE setFontColor NOTIFY fontColorChanged)
    Q_PROPERTY(QColor brushColor READ brushColor WRITE setBrushColor NOTIFY brushColorChanged)

    Q_PROPERTY(bool underlineChange READ underlineChange WRITE setUnderlineChange NOTIFY underlineChangeChanged)
    Q_PROPERTY(bool boldChange READ boldChange WRITE setBoldChange NOTIFY boldChangeChanged)
    Q_PROPERTY(bool italicChange READ italicChange WRITE setItalicChange NOTIFY italicChangeChanged)
    Q_PROPERTY(bool strikeOutChange READ strikeOutChange WRITE setStrikeOutChange NOTIFY strikeOutChangeChanged)
    Q_PROPERTY(bool sizeChange READ sizeChange WRITE setSizeChange NOTIFY sizeChangeChanged)
    Q_PROPERTY(bool styleChange READ styleChange WRITE setStyleChange NOTIFY styleChangeChanged)
    Q_PROPERTY(bool familyChange READ familyChange WRITE setFamilyChange NOTIFY familyChangeChanged)
    Q_PROPERTY(bool colorChange READ colorChange WRITE setColorChange NOTIFY colorChangeChanged)
    Q_PROPERTY(bool brushChange READ brushChange WRITE setBrushChange NOTIFY brushChangeChanged)
    bool m_underlineChange;
    bool m_sizeChange;
    bool m_boldChange;
    bool m_italicChange;
    bool m_strikeOutChange;    
    bool m_styleChange;
    bool m_familyChange;
    bool m_colorChange;
    bool m_brushChange;

public:
	EjTextStyle(QObject *parent=0);
    QFont m_font;
    QColor m_fontColor;
    QColor m_brushColor;
    QFontMetrics m_fontMetrics;

    enum NumProp {
        FONT_STYLE = 2,
        FONT_FAMILY,
        FONT_SIZE,
        FONT_BOLD,
        FONT_ITALIC,
        FONT_STRIKE_OUT,
        FONT_UNDERLINE,
        FONT_COLOR,
        FONT_BRUSH_COLOR,
    };

    enum e_fontStyle {
        NORMAL = 0,
        NORMAL_SMALL1,
        NORMAL_SMALL2,
        NORMAL_BIG1,
        NORMAL_BIG2,

        HEADING1 = 10,
        HEADING2,
        HEADING3,
        HEADING4,
        HEADING5,


    } m_fontStyle;
    Q_ENUM(e_fontStyle)

    void setDefault();

	EjBlock* makeCopy() override;
	Q_INVOKABLE EjTextStyle* makeTextStyle();
	Q_INVOKABLE void copy(EjTextStyle *style);
	Q_INVOKABLE void copyChanges(EjTextStyle *style);
	bool fullCompare(const EjBlock *other) const;

    e_fontStyle fontStyle() const;

    QString fontFamily() const;

    bool fontBold() const;
    bool fontItalic() const;
    bool fontStrikeOut() const;
    QColor fontColor() const;
    QColor brushColor() const;    
    int fontSize() const;
    bool fontUnderline() const;

    void beforeCalc(EjCalcParams *calcParams) override;
	void childCalc(EjBlock *child, EjCalcParams *calcParams) override;

    bool underlineChange() const;
    bool sizeChange() const;
    bool boldChange() const;
    bool italicChange() const;
    bool strikeOutChange() const;
    bool styleChange() const;
    bool familyChange() const;
    bool colorChange() const;
    bool brushChange() const;

public slots:
	void setParentStyle(EjTextStyle *parentStyle);
	void normalizeStyle(EjTextStyle *referenceStyle, bool force);
    void setFontStyle(e_fontStyle fontStyle);
    void setFontFamily(QString fontFamily);
    void setFontBold(bool fontBold);
    void setFontItalic(bool fontItalic);
    void setFontStrikeOut(bool fontStrikeOut);
    void setFontColor(QColor fontColor);
    void setBrushColor(QColor brushColor);    
    void setFontSize(int fontSize);

    void setFontUnderline(bool fontUnderline);
    void setUnderlineChange(bool underlineChange);
    void setSizeChange(bool sizeChange);
    void setBoldChange(bool boldChange);
    void setItalicChange(bool italicChange);
    void setStrikeOutChange(bool strikeOutChange);
    void setStyleChange(bool styleChange);
    void setFamilyChange(bool familyChange);
    void setColorChange(bool colorChange);
    void setBrushChange(bool brushChange);

signals:
    void fontStyleChanged(e_fontStyle fontStyle);
    void fontFamilyChanged(QString fontFamily);
    void fontBoldChanged(bool fontBold);
    void fontItalicChanged(bool fontItalic);
    void fontStrikeOutChanged(bool fontStrikeOut);
    void fontColorChanged(QColor fontColor);
    void brushColorChanged(QColor brushColor);

    void fontSizeChanged(int fontSize);
    void fontUnderlineChanged(bool fontUnderline);
    void underlineChangeChanged(bool underlineChange);
    void sizeChangeChanged(bool sizeChange);
    void boldChangeChanged(bool boldChange);
    void italicChangeChanged(bool italicChange);
    void strikeOutChangeChanged(bool strikeOutChange);
    void styleChangeChanged(bool styleChange);
    void familyChangeChanged(bool familyChange);
    void colorChangeChanged(bool colorChange);
    void brushChangeChanged(bool brushChange);

protected:
};


class COMMONSHARED_EXPORT EjBorderStyle : public EjBaseStyle
{
    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor NOTIFY penColorChanged)

public:
    enum NumProp {
        BORDER_WIDTH = 2,
        BORDER_COLOR,
    };

	EjBorderStyle(QObject *parent=0);
	EjBorderStyle(int num_) : EjBorderStyle(nullptr) { num = num_; }
	bool fullCompare(const EjBlock *other) const;

	void operator =(const EjBorderStyle &other);
	void childCalc(EjBlock *child, EjCalcParams *calcParams) override;
	EjBlock* makeCopy() override;

    enum e_penType {
        SOLID = 0
    } m_penType;
    Q_ENUM(e_penType)

    qint16 m_width;
    QColor m_penColor;

    int width() const;

    QColor penColor() const;

public slots:
    void setWidth(int width);
    void setPenColor(QColor penColor);

signals:
void widthChanged(int width);
void penTypeChanged(e_penType penType);
void penColorChanged(QColor penColor);
};

class COMMONSHARED_EXPORT EjCellStyle : public EjBaseStyle
{
    Q_OBJECT
	Q_PROPERTY(EjBorderStyle *topBorder READ topBorder NOTIFY topBorderChanged)
	Q_PROPERTY(EjBorderStyle *bottomBorder READ bottomBorder NOTIFY bottomBorderChanged)
	Q_PROPERTY(EjBorderStyle *leftBorder READ leftBorder NOTIFY leftBorderChanged)
	Q_PROPERTY(EjBorderStyle *rightBorder READ rightBorder NOTIFY rightBorderChanged)
	Q_PROPERTY(EjBorderStyle *horisontalLine READ horisontalLine NOTIFY horisontalLineChanged)
	Q_PROPERTY(EjBorderStyle *verticalLine READ verticalLine NOTIFY verticalLineChanged)
    Q_PROPERTY(QColor brushColor READ brushColor WRITE setBrushColor NOTIFY brushColorChanged)

	EjBorderStyle *m_topBorder;
	EjBorderStyle *m_bottomBorder;
	EjBorderStyle *m_leftBorder;
	EjBorderStyle *m_rightBorder;
	EjBorderStyle *m_horisontalLine;
	EjBorderStyle *m_verticalLine;

public:
	EjCellStyle(QObject *parent=0);
	~EjCellStyle();
    enum NumProp {
        CELL_TOP = 2,
        CELL_BOTTOM,
        CELL_LEFT,
        CELL_RIGHT,
        CELL_HORISONTAL,
        CELL_VERTICAL,
        CELL_BRUSH_COLOR
    };

	bool fullCompare(const EjBlock *other) const;
	void operator =(const EjCellStyle &other);
    void beforeCalc(EjCalcParams *calcParams) override;
	void childCalc(EjBlock *child, EjCalcParams *calcParams) override;
	EjBlock* makeCopy() override;

	EjBorderStyle * topBorder();
	EjBorderStyle * bottomBorder();
	EjBorderStyle * leftBorder();
	EjBorderStyle * rightBorder();
	EjBorderStyle * horisontalLine();
	EjBorderStyle * verticalLine();

	void setTopBorder(EjBorderStyle *border);
	void setBottomBorder(EjBorderStyle *border);
	void setLeftBorder(EjBorderStyle *border);
	void setRightBorder(EjBorderStyle *border);


    QColor m_brushColor;

    QColor brushColor() const
    {
        return m_brushColor;
    }
	void setBorderStyle(EjBorderStyle *borderStyle);

public slots:
    void setBrushColor(QColor brushColor);
	void setParentStyle(EjCellStyle *parentStyle);

signals:
    void brushColorChanged();
    void topBorderChanged();
    void bottomBorderChanged();
    void leftBorderChanged();
    void rightBorderChanged();
    void horisontalLineChanged();
    void verticalLineChanged();
protected:
	EjBorderStyle m_topBorder_st;
	EjBorderStyle m_bottomBorder_st;
	EjBorderStyle m_leftBorder_st;
	EjBorderStyle m_rightBorder_st;
	EjBorderStyle m_horisontalLine_st;
	EjBorderStyle m_verticalLine_st;

};



class COMMONSHARED_EXPORT EjParagraphStyle : public EjBaseStyle
{
    Q_OBJECT
    Q_PROPERTY(int align READ align WRITE setAlign NOTIFY alignChanged)

public:
	EjParagraphStyle(QObject *parent=0);

    enum NumProp {
        PRG_ALIGN = 2,
    };

    enum AlignFlag {
        AlignLeft = 0x0001,
        AlignRight = 0x0002,
        AlignHCenter = 0x0004,
        AlignJustify = 0x0008,
        AlignTop = 0x0020,
        AlignBottom = 0x0040,
        AlignVCenter = 0x0080
    };
    Q_ENUM(AlignFlag)
    Q_DECLARE_FLAGS(Align, AlignFlag)

	EjBlock* makeCopy() override;
	Q_INVOKABLE EjParagraphStyle* makePrgStyle();
	Q_INVOKABLE void copy(EjParagraphStyle *style);
	bool fullCompare(const EjBlock *other) const;
	void childCalc(EjBlock *child, EjCalcParams *calcParams) override;
    void beforeCalc(EjCalcParams *calcParams) override;

    int m_align;
    int align() const;

public slots:
    void setAlign(int align);

signals:

    void alignChanged(int align);

protected:
};

#endif // STYLES_H
