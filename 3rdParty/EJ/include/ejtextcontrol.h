#ifndef TEXTCONTROL_H
#define TEXTCONTROL_H

#include <QObject>
#include <QFont>
#include <QRectF>
#include <QVariant>
#include <QPainter>
#include "ejcommon.h"
#include "ejstyles.h"
#include "ejdocument.h"

class EjTextControl;
class PatchUndoRedo;

struct EjCalcParams
{
  EjCalcParams();
  EjTextStyle *textStyle;
  LineStyle *lineStyle;
  EjParagraphStyle *paragraphStyle;
  EjTextControl *control;
  int maxY;
  qreal viewScale;
  int contentX;
  int contentY;
  e_statusMode statusMode;
  bool force;
  bool isViewDoc;
  int rightPosition;
  int leftColontitul;
  int index_string;
  int index_page;
  int baseY;
  int baseX;
  int interval;
};


class PatchUndoRedo
{
public:
	PatchUndoRedo(){
		m_position = m_activeIndex = 0;
	}
	qint16 m_position;
	quint32 m_activeIndex;
	int m_startSelectBlock;
	int m_endSelectBlock;
	int m_startSelectPos;
	int m_endSelectPos;
	int m_startSelectWidth;
	int m_endSelectWidth;
	int m_selectMode;
	QByteArray m_body;
	QByteArray m_attributes;

};

class COMMONSHARED_EXPORT EjTextControl : public QObject
{
    Q_OBJECT
public:
	explicit EjTextControl(QObject *parent = 0);
	~EjTextControl();
    QVariant inputMethodQuery(Qt::InputMethodQuery property, QVariant argument) const;

    QString SurroundingText;
    QString TextBeforeCursor;
    QString TextAfterCursor;
    QString CurrentSelection;
    int anchor;
    int position;
    int position_w;
    int leftColontitul;
    int rightColontitul;
    int topColontitul;
    int bottomColontitul;
    int m_defaultPageWidth;
    int m_defaultPageHeight;
    QRectF cursorRect;
    int activeIndex;
    int id_inputSelBlock;
    int m_startSelectBlock;
    int m_endSelectBlock;
    int m_startSelectPos;
    int m_endSelectPos;
    int m_startSelectWidth;
    int m_endSelectWidth;

    float scaleSize;
    bool m_startCursor;

    void inputText(QString text);
    void addImage(QString path);
	void addExtBlock(EjBlock *block);
    void addShopList();
    void addClearTable();
    bool splitText(int &block, int &pos);
    quint32 inputEnter(bool force = false);
    void inputSpace();
    void inputBackSpace();
    void setWidth(int width);
    void setHeight(int height);
    int width() { return m_width; }
    int height() { return m_height; } // < m_minHeight ? m_minHeight : m_height; }
    void calcCursor(bool force = false);
    void calcData(bool force = false);
    void calcNext(bool force = false);
    void setCursor(int x, int y);
    void startSelected(int x,int y);
    void updateSelected();
    void clearSelected();
    int wichBlock(int x, int y);
	EjBlock * getBlock(int x, int y);
	EjBlock* currentBlock();
    void selectBlock(int x, int y);
    void cursorLeft();
    void cursorRight();
    void worldLeft();
    void worldRight();
    void cursorUp();
    void cursorDown();
    void cursorFirst();
    void cursorLast();
    int setInputMode(bool mode, QString &text, int preeditCursor);
    void setStatusMode(e_statusMode mode); // { m_statusMode = mode; m_startSelectBlock = m_endSelectBlock = -1; }
    void setSelectMode(e_selectMode mode);
    void setContent(int x, int y) { m_contentX = x; m_contentY = y; }
    QString getText() const;
    void setText(const QString &source);
	void groupBlocksIndexes();
	void setFirstTextStyle();
	void setTextStyle(EjTextStyle *textStyle, bool force = true, bool autoClose = false);
    bool textStyleOptimize();
    void setParagraphTextStyle(EjTextStyle *textStyle);
    void setParagraphStyle(EjParagraphStyle *paragraphStyle);
    void clear();
    void inputTable(int rows,int colums);
    void addTableString();
    void addTableColum();
	void delTableString(QList<EjBlock *> *l_blocks, int &active_block);
	void delTableColum(QList<EjBlock*> *l_blocks, int &active_block);
    void moveTableString(bool isUp);
    void moveTableColum(bool isLeft);
    void updateFormulas(int fromVal, int toVal, bool isRow, bool isAdd,bool isMove, EjTableBlock *curTable);
    bool tableLinkParams(QString txt, EjTableBlock *curTable, EjTableBlock *curTable2, int &numTable, int &numRow, int &numColum);
    void updateShopList(EjTableBlock *curTableBlock);
	void cellParams(EjTableBlock *TableBlock, int index,int &row,int &colum, QList<EjBlock *> *l_blocks = 0);
	void cellParams(EjTableBlock *TableBlock,EjBlock *block,int &row,int &colum, QList<EjBlock *> *l_blocks = 0);
    void resetUpDown();
	static int tableCellIndex(EjTableBlock *tableBlock, int row, int colum, QList<EjBlock *> *l_blocks);
    void updateTables(EjDocument *fdoc);
    EjTableBlock *isTable(int index);
    void checkFormula();
    bool isNumber();
    bool isActiveText();
    bool isCellEditFormula();
    bool isCellSelected(int index);
    bool getBaseCellParams(int index, int &row, int &colum);
    int getIndexString(int index);
    int tableVid();
    void loadLinks();
    void signLinks(qint32 sign_id, qint32 group_id, int status, QString comment);
    void updateLinks();
	void undo();
	void redo();
    e_statusMode m_statusMode;
    e_selectMode m_selectMode;
    bool m_inputSelectMode;
    bool m_inputAddMode; // deprecate
    bool m_showCell;
    int m_interval;
    bool is_startInputMode;
    int m_calcIndex;
    EjDocument m_doc;
    EjDocument *doc;
    EjDocument *docPrev;
    QFontMetrics metric;

    bool isEmpty() { return doc->lBlocks->isEmpty(); }
    void setIsViewDoc(bool source);



    void setDocument(EjDocument *document);
    EjDocument *document() const { return doc; }

    bool pasteCells();
    bool copyCells();

    bool menuActivate(QString command, QString data);
    QString isTell();
    QList<EjFragmentBlock *> getActualFragments(int block, EjTableBlock *cur_Table = 0, int row = 0, int colum = 0);
    QList<EjFragmentBlock *> getSelectFragments();
    void removeFragments(QList<EjFragmentBlock *> *l_fragments, int vid);
    QFontMetrics getDrawMetrics(int block);
    EjTextStyle *getTextStyle(int block) const;
    EjTextStyle *getSelectedTextStyle(int block) const;
    EjParagraphStyle *getParagraphStyle(int block);
    QRect selectArea();
    void updateFragments(int index, bool is_add, bool posNotNul = false);
    void blockOptimize();
    void fragmentOptimize(int vid);
    void fragmentDOptimize(int vid, int startBlock, int endBlock);
    QPoint getCursor() {return QPoint(m_posCursorX,m_posCursorY); }
    void calcTables();
    void moveTable(int dX);
	QList<EjBlock*> *getBlocks() { return doc->lBlocks; }
    QList<JString*> *getStrings() { return doc->lStrings; }
    QList<Page*> *getPages() { return doc->lPages; }
    QList<EjBaseStyle*> *getStyles() { return doc->lStyles; }
    QList<EjTableBlock*> *getTables() { return doc->lTables; }
    void calcInputMethodParams();
    int delText(int delta, int count);
	void calcString(JString *string, Page *page, EjCalcParams *calcParams);
	void addString(EjCalcParams *calcParams, int indexBlock);
    int startText(int index) const;
    int endText(int index) const;
    bool isEndText(int index) const;

signals:
    void cursorPositionChanged(int x, int y);
    void cursorHeightChanged(int cur_height);
    void controlHeightChanged();
    void ring(QString tel);
    void update();
    void patchChanged();

public slots:

public:
    EjDocument *m_clipboardDoc;
    QPainter *m_painter;
    int m_deltapos;
    int m_defPageWidth;
    int m_defPageHeight;
    int m_height;
    int m_width;
    e_viewMode m_viewMode;
    int m_posCursorX;
    int m_posCursorY;
    int m_heightCursor;
    int m_contentX;
    int m_contentY;
    int m_viewH;
    int m_viewW;
    int m_upDown_x;
    int m_upDown_y;
    bool m_isViewDoc;
    int m_timerId;
    int m_currentPatch;
    bool m_createPatchEnabled;
    QList<PatchUndoRedo *> m_lPatches;

    bool m_selectStart;
	EjBlock *m_inputSelBlock;
    int m_inputSelPos;
    QString m_htmlBuffer;
    int getBaseWidth(int index, QFontMetrics &drawMetric);

    void calcBlocks(int index = 0);
    void calcPages(int index = 0);
    JString *wichString(int index);

    void calc(int index = 0, bool force = false);
	void calcBlock(EjBlock *cur_Block, EjCalcParams *calcParams);
    void timerEvent(QTimerEvent *event);

};

#endif // TEXTCONTROL_H
