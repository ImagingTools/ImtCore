#pragma once


#include <QtCore/QObject>

// custom types includes
#include <modsdlsdl/SDL/1.0/CPP/PrinterBase_fwd.h>


namespace sdl::V1_0::modsdl
{

Q_NAMESPACE

enum class PrintingTechnology {
	Inkjet,
	Toner,
};

Q_ENUM_NS(PrintingTechnology)


class EnumPrintingTechnology: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString Inkjet READ GetInkjet NOTIFY InkjetChanged)
	Q_PROPERTY(QString Toner READ GetToner NOTIFY TonerChanged)

protected:
	static QString GetInkjet() { return "Inkjet"; }
	static QString GetToner() { return "Toner"; }

signals:
	void InkjetChanged();
	void TonerChanged();
};



// type forward declarations
class CDigitalPrinterSpecification;
class CDigitalPrinter;

} // namespace sdl::V1_0::modsdl
