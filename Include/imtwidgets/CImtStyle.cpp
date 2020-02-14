#include <imtwidgets/CImtStyle.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QToolButton>


static QWindow *qt_getWindow(const QWidget *widget)
{
	return widget ? widget->window()->windowHandle() : 0;
}

static void drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton,
	const QRect &rect, QPainter *painter, const QWidget *widget = 0)
{
	QStyle::PrimitiveElement pe;
	switch (toolbutton->arrowType){
		case Qt::LeftArrow:
			pe = QStyle::PE_IndicatorArrowLeft;
			break;
		case Qt::RightArrow:
			pe = QStyle::PE_IndicatorArrowRight;
			break;
		case Qt::UpArrow:
			pe = QStyle::PE_IndicatorArrowUp;
			break;
		case Qt::DownArrow:
			pe = QStyle::PE_IndicatorArrowDown;
			break;
		default:
			return;
	}
	QStyleOption arrowOpt = *toolbutton;
	arrowOpt.rect = rect;
	style->drawPrimitive(pe, &arrowOpt, painter, widget);
}


namespace imtwidgets
{


void CImtStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption * option, QPainter * painter, const QWidget * widget) const
{
	switch (pe){
		case QStyle::PE_PanelButtonCommand:
			{
				if (widget != nullptr){
					QWidget* parentWidget = widget->parentWidget();
					if ((parentWidget != nullptr) && parentWidget->property("ImtToolBar").toBool()){
						painter->fillRect(option->rect, Qt::transparent);
					}
					else{
						BaseClass::drawPrimitive(pe, option, painter, widget);
						return;
					}
				}
				else{
					BaseClass::drawPrimitive(pe, option, painter, widget);
					return;
				}
			}
			break;

		default:
			BaseClass::drawPrimitive(pe, option, painter, widget);
			break;
	}
}


void CImtStyle::drawControl(ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget) const
{
	switch (element){
		case CE_ToolButtonLabel:
			if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(option)){
				QRect rect = toolbutton->rect;

				if (widget != nullptr){
					QWidget* parentWidget = widget->parentWidget();
					if (!parentWidget->property("ImtToolBar").toBool()){
						BaseClass::drawControl(element, option, painter, widget);
						return;
					}
				}
				else{
					BaseClass::drawControl(element, option, painter, widget);
					return;
				}

				int shiftX = 0;
				int shiftY = 0;
				if (toolbutton->state & (State_Sunken | State_On)){
					shiftX = proxy()->pixelMetric(PM_ButtonShiftHorizontal, toolbutton, widget);
					shiftY = proxy()->pixelMetric(PM_ButtonShiftVertical, toolbutton, widget);
				}

				// Arrow type always overrules and is always shown
				bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;

				QPixmap pm;
				QSize pmSize = toolbutton->iconSize;
				if (!toolbutton->icon.isNull()){
					QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
					QIcon::Mode mode;
					if (!(toolbutton->state & State_Enabled))
						mode = QIcon::Disabled;
					else if ((option->state & State_MouseOver) && (option->state & State_AutoRaise))
						mode = QIcon::Active;
					else
						mode = QIcon::Normal;
					pm = toolbutton->icon.pixmap(qt_getWindow(widget), toolbutton->rect.size().boundedTo(toolbutton->iconSize),
						mode, state);
					pmSize = pm.size() / pm.devicePixelRatio();
				}

				painter->setFont(toolbutton->font);
				QRect pixmapRect = rect;
				QRect textRect = rect;
				int alignment = Qt::TextShowMnemonic;
				if (!proxy()->styleHint(SH_UnderlineShortcut, option, widget))
					alignment |= Qt::TextHideMnemonic;

				if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon){
					pixmapRect.setHeight(pmSize.height() + 6);
					textRect.adjust(0, pixmapRect.height() - 1, 0, -1);
					pixmapRect.translate(shiftX, shiftY);
					if (!hasArrow){
						painter->setRenderHint(QPainter::Antialiasing, true);
						QRect borderRect(QPoint(0, 0), QPoint(2 * pmSize.width(), pmSize.height()));
						borderRect.moveCenter(pixmapRect.center());

						painter->save();

						QPen pen(QColor("#9d9d9d"), 0);
						painter->setPen(pen);

						QPainterPath path;
						path.addRoundedRect(borderRect.adjusted(-1, -1, 2, 2), 3, 3);
						painter->drawPath(path);

						path.clear();
						path.addRoundedRect(borderRect.adjusted(-2, -2, 2, 2), 3, 3);

						painter->fillPath(path, QColor(245, 245, 245));

						if (toolbutton->state & (State_Sunken | State_On)){
							QPainterPath path;
							path.addRoundedRect(borderRect.adjusted(-2, 0, 2, 2), 3, 3);
							painter->fillPath(path, QColor("#dddddd"));
						}

//						painter->drawPath(path);
						painter->restore();

						proxy()->drawItemPixmap(painter, borderRect, Qt::AlignCenter, pm);
					}
					else {
						drawArrow(proxy(), toolbutton, pixmapRect, painter, widget);
					}
					alignment |= Qt::AlignCenter;
					textRect.translate(shiftX, shiftY);
					proxy()->drawItemText(painter, QStyle::visualRect(option->direction, rect, textRect), alignment, toolbutton->palette,
						toolbutton->state & State_Enabled, toolbutton->text,
						QPalette::ButtonText);
				}
				else{
					BaseClass::drawControl(element, option, painter, widget);
				}
			}
			break;

		default:
			BaseClass::drawControl(element, option, painter, widget);
			break;
	}
}


QRect CImtStyle::subElementRect(SubElement subElement, const QStyleOption * option, const QWidget * widget) const
{
	switch (subElement){
		case QStyle::SE_ToolButtonLayoutItem:
			break;

		default:
			return BaseClass::subElementRect(subElement, option, widget);
	}

	return QRect();
}


} // namespace imtwidgets


