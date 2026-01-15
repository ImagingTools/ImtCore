#ifndef EJCALCULATOR_H
#define EJCALCULATOR_H
#include "calculatorhelper.h"
#include "ejcommon.h"

#define is_operator(c) (c == '+' || c == '-' || c == '/' || c == '*' || c == '!' || c == '%' || c == '=')
#define is_value(c) (c >= 'A' && c <= 'Z')
#define is_ident(c) ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z'))
#define is_num(c) ((c >= '0' && c <= '9'))
#define is_extend(c) (c == 'r' || c == 't' || c == 'b' || c == 'f')

struct CalcItem{
    CalcItem() { type = NONE; func = NOFUNC; value = 0;  }
    enum Type {
        NONE = 0,
        NUMERIC,
        FUNC,
        OPERATOR
    } type;
    double value;
    QString text;

    enum TypeFunc{
        NOFUNC,
        SUMM,
        SIN,
        COS,
        MIN,
        MAX
    } func;

};



class EjCalculator
{
public:
	EjCalculator(EjDocument *document);
    int calcTables(bool isReplace = false);
    void updateFormulas(bool isReplace);
	void setLinkData(QString str_link, EjBlock *source);
	int calcCell(EjCellBlock *cell, EjBlock *source);
	double calcFormula(QString formula,  EjBlock *source, bool *bOk);


    EjDocument *m_doc;
    static bool is_split(const char c);
    static bool is_ext_link(QString &txt);
    static bool is_contains_operator(QString &txt);
    static void split(QString &txt,QStringList &lStr);
    static void getLinkParams(QString txt, LinkParams &params);
protected:
    int op_preced(const char c);
    bool op_left_assoc(const char c);
    unsigned int op_arg_count(const char c);
    bool shunting_yard(QList<CalcItem>&input, QList<CalcItem> &output);
    double execution_order(QList<CalcItem> &input);
	double getValue(QString txt, EjBlock *source);
	QString getStringValue(QString txt, EjBlock *source);
	bool getValues(QString txt_from, QString txt_to, QList<CalcItem> &output, EjBlock *source);
};

#endif // EJCALCULATOR_H
