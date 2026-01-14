#ifndef WORD_DOCUMENT_H
#define WORD_DOCUMENT_H

#include <QDomDocument>
#include <QList>
#include <QColor>

#include "ejtableblocks.h"

class EjWordDocument {

    enum TypeElement {
        type_body,
        type_p,
        type_tbl,
        type_tr,
        type_tc,
        type_pict,
    };

    int width, height, left, right, header, top, footer, bottom, gutter;

    QDomDocument document;
    QDomDocument rels;

    QDomElement rels_relationships;

    QList<TypeElement> types;
    QList<QDomElement> blocks;

    static const int number_system_color;
    static const QColor system_color[];
    static const QString system_color_name[];

    QSet<TypeElement> paragraph_parent;
    QSet<TypeElement> groups;

    QMap<QString, int> relations;
    static const int shift_relation;

    QMap<int, QString> aligns;
    QString align;

    QMap<int, QString> vAligns;
    QString vAlign;

    int text_size;
    QColor color_text;
    QString highlight_color;
    bool highlight_color_exist;
    bool is_bold, is_italic, is_under_line, is_strike_out;
    bool landscape = false;

    QList<QList<int>> skiped_cell_i, skiped_cell_j, continue_cell_i, continue_cell_j, value_continue;
    QList<int> widths_tables, heights_tables, i, j;

    void removeElementList();
    void addElementList(TypeElement const& type, QDomElement const& element);

    void createSectPr();
    bool createParagraph();
    bool createTableLine();
    QDomElement createBorrier(QString const& name, int width);
    void initializationDocument();
    void initializationRels();
public:
	EjWordDocument();
    void addOrientation(bool value);
    void reset();
    void setPageSettings(int new_width = 11905, int new_height= 16837, int new_left=1133, int new_right=850,
                         int new_top=850,int new_bottom=850, int new_header=0, int new_footer=0, int new_gutter=0);
    bool setTextSize(int value);
    bool setAilgn(int style = EjParagraphStyle::AlignLeft);
    bool setColorText(QColor const& color);
    bool setBold(bool value);
    bool setItalic(bool value);
    bool setUnderLine(bool value);
    bool setStrikeOut(bool value);
    bool setHighlightColor(QColor const& color);
    bool addText(QString const& text);
    bool addSpace();
    bool endParagraph();
    bool createTable(int width, int tbl_ind, QList<int> const& widths, int width_table, int height_table);
    bool addCell(int width, int merge_rows, int merge_colums, QColor const& background,
                 int top_barrier_width, int left_barrier_width, int right_barrier_width, int bottom_barrier_width, bool);
    bool addPict(int width, int height);
    bool addImage(QString const& name);
    bool endGroup();
    void finish();
    const QDomDocument& getDocument() const;
    const QDomDocument& getRels() const;
    QList<QString> getRelations() const;
};

#endif // WORD_DOCUMENT_H
