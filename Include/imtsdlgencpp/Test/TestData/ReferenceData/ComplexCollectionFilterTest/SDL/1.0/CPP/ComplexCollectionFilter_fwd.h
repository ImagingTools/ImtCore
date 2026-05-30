#pragma once


#include <QtCore/QObject>

// custom types includes
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes_fwd.h>


namespace sdl::V1_0::imtbase
{

Q_NAMESPACE

enum class ValueType {
	Integer,
	Number,
	String,
	Bool,
};

Q_ENUM_NS(ValueType)


class EnumValueType: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString Integer READ GetInteger NOTIFY IntegerChanged)
	Q_PROPERTY(QString Number READ GetNumber NOTIFY NumberChanged)
	Q_PROPERTY(QString String READ GetString NOTIFY StringChanged)
	Q_PROPERTY(QString Bool READ GetBool NOTIFY BoolChanged)

protected:
	static QString GetInteger() { return "Integer"; }
	static QString GetNumber() { return "Number"; }
	static QString GetString() { return "String"; }
	static QString GetBool() { return "Bool"; }

signals:
	void IntegerChanged();
	void NumberChanged();
	void StringChanged();
	void BoolChanged();
};


enum class FilterOperation {
	Not,
	Equal,
	Less,
	Greater,
	Contains,
};

Q_ENUM_NS(FilterOperation)


class EnumFilterOperation: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString Not READ GetNot NOTIFY NotChanged)
	Q_PROPERTY(QString Equal READ GetEqual NOTIFY EqualChanged)
	Q_PROPERTY(QString Less READ GetLess NOTIFY LessChanged)
	Q_PROPERTY(QString Greater READ GetGreater NOTIFY GreaterChanged)
	Q_PROPERTY(QString Contains READ GetContains NOTIFY ContainsChanged)

protected:
	static QString GetNot() { return "Not"; }
	static QString GetEqual() { return "Equal"; }
	static QString GetLess() { return "Less"; }
	static QString GetGreater() { return "Greater"; }
	static QString GetContains() { return "Contains"; }

signals:
	void NotChanged();
	void EqualChanged();
	void LessChanged();
	void GreaterChanged();
	void ContainsChanged();
};


enum class LogicalOperation {
	And,
	Or,
};

Q_ENUM_NS(LogicalOperation)


class EnumLogicalOperation: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString And READ GetAnd NOTIFY AndChanged)
	Q_PROPERTY(QString Or READ GetOr NOTIFY OrChanged)

protected:
	static QString GetAnd() { return "And"; }
	static QString GetOr() { return "Or"; }

signals:
	void AndChanged();
	void OrChanged();
};



// type forward declarations
class CTimeFilter;
class CFieldSortingInfo;
class CFieldFilter;
class CGroupFilter;
class CComplexCollectionFilter;

} // namespace sdl::V1_0::imtbase
