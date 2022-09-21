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
	/*---------------Сальдо---------------------*/

	EjDocument* ejDoc = new EjDocument();
	ejDoc->attributes()->setTemplateKey64(1234);
	EjTextControl control;
	control.setDocument(ejDoc);

		// ------------- Clear document ---------
	control.clear();

	// ------------- Add title --------------

	EjParagraphStyle curParagraph;
	EjTextStyle curTextStyle;
	EjCellBlock *curCell;

	control.setStatusMode(EDIT_TEXT);

	curParagraph.m_align = EjParagraphStyle::AlignHCenter;
	control.setParagraphStyle(&curParagraph);
	curTextStyle.m_font.setPixelSize(18);
	curTextStyle.m_font.setBold(true);
	control.setTextStyle(&curTextStyle);
	control.setText("Сальдо общее на ");
	control.inputEnter();
	control.inputEnter();

	curTextStyle.setDefault();
	control.setTextStyle(&curTextStyle);

	//номинальная услуга
	EjTableBlock *table = new EjTableBlock(3 ,6, ejDoc, control.activeIndex);

	curTextStyle.m_font.setBold(true);
	control.setTextStyle(&curTextStyle);
	curParagraph.m_align = EjParagraphStyle::AlignHCenter;
	control.setParagraphStyle(&curParagraph);

	 control.activeIndex = table->cellIndex(0,0,ejDoc->lBlocks);
	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	 curCell->merge(2,6);
	 curTextStyle.m_font.setBold(true);
	 curParagraph.m_align = EjParagraphStyle::AlignHCenter;
	 curParagraph.m_align = EjParagraphStyle::AlignVCenter;
	 curTextStyle.m_font.setPixelSize(12);
	 curCell->setText("Нотариальная услуга",&control);
	 control.setTextStyle(&curTextStyle);
	 control.setParagraphStyle(&curParagraph);
	 curTextStyle.setDefault();


	 control.activeIndex = table->cellIndex(2,0,ejDoc->lBlocks);
	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
	 curCell->merge(1,5);
	 curCell->setText("Общий итог",&control);

	 control.inputEnter();
	 control.inputEnter();
	 control.inputEnter();

	 //опломбировка


	 //поверка пу



	 //почтовые расходы


//	 control.activeIndex = table->cellIndex(0,1,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("Сальдо_начальное_тг",&control);

//	 control.activeIndex = table->cellIndex(0,2,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("Начисление_тг",&control);

//	 control.activeIndex = table->cellIndex(0,3,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("Оплата_тг",&control);

//	 control.activeIndex = table->cellIndex(0,4,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("Кор-ка_тг",&control);

//	 control.activeIndex = table->cellIndex(0,5,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("Сальдо_конечное_тг",&control);


//	 control.activeIndex = table->cellIndex(1,0,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("(пусто)",&control);

//	 for(int i = 1; i <= 9; i++)
//	 {
//		 control.activeIndex = table->cellIndex(i+1,0,ejDoc->lBlocks);
//		 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//		 curCell->setText(QString::number(i,10),&control);
//	 }

//	 control.activeIndex = table->cellIndex(11,0,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("нет",&control);

//	 control.activeIndex = table->cellIndex(12,0,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("Итого БС",&control);

//	 control.activeIndex = table->cellIndex(13,0,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("ЧС 1",&control);

//	 control.activeIndex = table->cellIndex(14,0,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("ЧС 2",&control);

//	 control.activeIndex = table->cellIndex(15,0,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("ЧС 3",&control);

//	 control.activeIndex = table->cellIndex(16,0,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curCell->setText("Итого ЧС",&control);





//	 control.activeIndex = table->cellIndex(0,2,ejDoc->lBlocks);
//	 curCell = (EjCellBlock*)ejDoc->lBlocks->at(control.activeIndex);
//	 curTextStyle.m_font.setItalic(true);
//	 control.setTextStyle(&curTextStyle);
//	 control.setText("data 2,2");
//	 curCell->setTextStyle(&curTextStyle,&control);
//	 curCell->setText("data 2,2\ndata 3,3",&control);
	control.calcData();
	return ejDoc;
}

QString CReportProvider::getReportId(){
	return "reportTest";
}

} // namespace imtej


