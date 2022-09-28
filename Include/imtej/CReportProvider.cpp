#include <imtej/CReportProvider.h>


//EjDoc includes
#include <ejtextcontrol.h>
#include <QDateTime>


namespace imtej
{


// public methods

EjDocument* CReportProvider::getReport(iprm::IParamsSet* reportParams)
{
//	EjDocument* ejDoc = new EjDocument();
//	ejDoc->attributes()->setTemplateKey64(1234);
//	EjTextControl control;
//	control.setDocument(ejDoc);

//		// ------------- Clear document ---------
//	control.clear();

//	// ------------- Add title --------------

//	EjParagraphStyle curParagraph;
//	EjTextStyle curTextStyle;
//	EjCellBlock *curCell;

//	control.setStatusMode(EDIT_TEXT);

//	curParagraph.m_align = EjParagraphStyle::AlignHCenter;
//	control.setParagraphStyle(&curParagraph);

//	curTextStyle.m_font =  QFont("Helvetica [Cronyx]");
//	curTextStyle.m_font.setPixelSize(25);
//	curTextStyle.m_font.setBold(true);
//	curTextStyle.m_fontStyle  = EjTextStyle::HEADING1;
//	control.setTextStyle(&curTextStyle);

//	control.setText("123456");
//	control.inputSpace();
//	control.setText("789");

//	control.cursorLast();
//	 control.inputEnter();
//	 curParagraph.m_align = EjParagraphStyle::AlignLeft;
//	 control.setParagraphStyle(&curParagraph);
//	 curTextStyle.m_font.setPixelSize(12);
//	 curTextStyle.m_fontStyle  = EjTextStyle::NORMAL;
//	 curTextStyle.m_font.setBold(false);
//	 control.setTextStyle(&curTextStyle);
//	 control.cursorLast();
//	 control.setText("text left");

//	 control.inputEnter();
	/*------------------invoice_notification------------------*/
	QDateTime date = QDateTime::currentDateTime();
	EjDocument* ejDoc = new EjDocument();
	ejDoc->attributes()->setTemplateKey64(1234);
	EjTextControl control;
	control.setDocument(ejDoc);

		// ------------- Clear document ---------
	control.clear();
	Page *pagePtr = new Page;
	control.getPages()->append(pagePtr);
	pagePtr->width = 29700;
	pagePtr->height = 21000;



	// ------------- Add title --------------

	EjParagraphStyle curParagraph;
	EjTextStyle curTextStyle;
	EjCellBlock *curCell;

	control.setStatusMode(EDIT_TEXT);
	curParagraph.m_align = EjParagraphStyle::AlignHCenter;
	control.setParagraphStyle(&curParagraph);
	curTextStyle.m_font.setPixelSize(9);
//	curTextStyle.m_font.setBold(true);
	control.setTextStyle(&curTextStyle);
	control.setText("ШОТ ХАБАРЛАМА (Счет - извещение)");
	control.inputEnter();
	curParagraph.m_align = EjParagraphStyle::AlignHCenter;
	control.setParagraphStyle(&curParagraph);
	curTextStyle.m_font.setPixelSize(7);
	control.setTextStyle(&curTextStyle);
	control.setText(
				"«Батыс су арнасы» – ЖШС " +
				QString::number(date.date().year()) +
				" ж. қыркүйек айында ұсынылған қызметтері үшін/за услуги оказанные в " +
				date.toString("MMMM") +
				" " +
				QString::number(date.date().year()) + " г. ТОО «Батыс су арнасы» ");
	control.inputEnter();

	curTextStyle.setDefault();
	control.setTextStyle(&curTextStyle);

	EjTableBlock *table = new EjTableBlock(6 ,12, ejDoc, control.activeIndex);

	control.activeIndex = table->cellIndex(0,0,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curTextStyle.m_font.setBold(true);
	curTextStyle.m_font.setPixelSize(8);
	control.setTextStyle(&curTextStyle);
	curParagraph.m_align = EjParagraphStyle::AlignHCenter;
	control.setParagraphStyle(&curParagraph);
	curCell->setText("Қызметтердің аталуы/Наименование услуг",&control);


	control.activeIndex = table->cellIndex(0,1,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Ай басындағы сальдо/ Сальдо на 01.09.2022",&control);

	control.activeIndex = table->cellIndex(0,2,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Алдыңғы көрсеткіш/ Пред. Показ гор/хол",&control);

	control.activeIndex = table->cellIndex(0,3,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Ағымдық көрсеткіш/Тек. Показ гор/хол",&control);

	control.activeIndex = table->cellIndex(0,4,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Саны(кол-во), м3/ шт",&control);

	control.activeIndex = table->cellIndex(0,5,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Құны/Стоимость тенге",&control);

	control.activeIndex = table->cellIndex(0,6,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText(date.toString("MM.yyyy")+" ж. есептеледі/начислено за " + date.toString("MM.yyyy") + " г.",&control);

	control.activeIndex = table->cellIndex(0,7,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Төлем (Оплата за месяц)",&control);

	control.activeIndex = table->cellIndex(0,8,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Өсімақы/Пеня",&control);

	control.activeIndex = table->cellIndex(0,9,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Түзетулер/Корректировки",&control);

	control.activeIndex = table->cellIndex(0,10,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Ай аяғындағы сальдо/Сальдо на 30.09.2022",&control);

	control.activeIndex = table->cellIndex(0,11,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Төленетін сома/К оплате",&control);

	curTextStyle.setDefault();
	control.setTextStyle(&curTextStyle);

	curTextStyle.m_font.setPixelSize(7);
	control.setTextStyle(&curTextStyle);
	curParagraph.m_align = EjParagraphStyle::AlignHCenter;
	control.setParagraphStyle(&curParagraph);

	control.activeIndex = table->cellIndex(1,0,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Водоснабжение по  ИПУ",&control);

	control.activeIndex = table->cellIndex(2,0,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Водоотведение по ИПУ",&control);

	control.activeIndex = table->cellIndex(3,0,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Водоснабжение по норме",&control);

	control.activeIndex = table->cellIndex(4,0,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("Водоотведение по норме",&control);

	curTextStyle.m_font.setBold(true);
	control.setTextStyle(&curTextStyle);

	control.activeIndex = table->cellIndex(5,0,ejDoc->lBlocks);
	curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	curCell->setText("ЖиыныИтого",&control);

	control.inputEnter();

	curTextStyle.m_font.setBold(false);
	curTextStyle.m_font.setPixelSize(9);
	control.setTextStyle(&curTextStyle);
	control.setText("«Батыс су арнасы» ЖШС,  БСН 040840000451,КБе 16, КНП 856, ЖСК KZ788562203113052265  (KZT),  БСК KCJBKZKX, «Банк Центр Кредит» АҚ");


	control.calcData();
	return ejDoc;
}

QString CReportProvider::getReportId(){
	return "reportTest";
}

} // namespace imtej


