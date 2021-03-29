#include <imtwidgets/CImtStyle.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QToolButton>


extern void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);


static QWindow* GetWindowHandle(const QWidget* widgetPtr)
{
	if (widgetPtr != nullptr){
		QWidget* windowPtr = widgetPtr->window();
		if (windowPtr != nullptr){
			return windowPtr->windowHandle();
		}
	}

	return nullptr;
}


static void DrawArrow(
			const QStyle* style,
			const QStyleOptionToolButton* toolButtonStyleOptionPtr,
			const QRect& rect,
			QPainter* painter,
			const QWidget* widget = nullptr)
{
	QStyle::PrimitiveElement pe;

	switch (toolButtonStyleOptionPtr->arrowType){
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

	QStyleOption arrowOpt = *toolButtonStyleOptionPtr;
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
//						painter->fillRect(option->rect, Qt::transparent);
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
	const QStyleOptionToolButton* toolButtonStyleOptionPtr = qstyleoption_cast<const QStyleOptionToolButton *>(option);

	switch (element){
		case CE_ToolButtonLabel:
			if (widget != nullptr){
				QWidget* parentWidget = widget->parentWidget();
				if (!parentWidget->property("ImtToolBar").toBool()){
					BaseClass::drawControl(element, option, painter, widget);
					return;
				}
			}
			else {
				BaseClass::drawControl(element, option, painter, widget);
				return;
			}

			if (toolButtonStyleOptionPtr != nullptr){
				QRect rect = toolButtonStyleOptionPtr->rect;
				int shiftX = 0;
				int shiftY = 0;
				if (toolButtonStyleOptionPtr->state & (State_Sunken | State_On)){
					shiftX = proxy()->pixelMetric(PM_ButtonShiftHorizontal, toolButtonStyleOptionPtr, widget);
					shiftY = proxy()->pixelMetric(PM_ButtonShiftVertical, toolButtonStyleOptionPtr, widget);
				}

				// Arrow type always overrules and is always shown
				bool hasArrow = toolButtonStyleOptionPtr->features & QStyleOptionToolButton::Arrow;

				QPixmap pm;
				QRect pixmapRect = rect.adjusted(1, 1, -1, -1);
				QSize pmSize = toolButtonStyleOptionPtr->iconSize;
				if (!toolButtonStyleOptionPtr->icon.isNull()){
					QIcon::State state = toolButtonStyleOptionPtr->state & State_On ? QIcon::On : QIcon::Off;
					QIcon::Mode mode;
					if (!(toolButtonStyleOptionPtr->state & State_Enabled)){
						mode = QIcon::Disabled;
					}
					else if ((option->state & State_MouseOver) && (option->state & State_AutoRaise)){
						mode = QIcon::Active;
					}
					else{
						mode = QIcon::Normal;
					}

					QSize iconSize = pixmapRect.size().boundedTo(toolButtonStyleOptionPtr->iconSize);

					pm = toolButtonStyleOptionPtr->icon.pixmap(GetWindowHandle(widget), iconSize, mode, state);

					double deviceRatio = pm.devicePixelRatio();

					pmSize = pm.size() / deviceRatio;
				}

				painter->setFont(toolButtonStyleOptionPtr->font);
				QRect textRect = rect;

				if (toolButtonStyleOptionPtr->toolButtonStyle == Qt::ToolButtonTextUnderIcon){
					pixmapRect.setHeight(pmSize.height() + 6);
					textRect.adjust(0, pixmapRect.height() - 1, 0, -1);
					pixmapRect.translate(shiftX, shiftY);

					if (!hasArrow){
						if (!pmSize.isEmpty()){
							painter->setRenderHint(QPainter::Antialiasing, true);
							QRectF borderRect(QPoint(0, 0), QPoint(2.5 * pmSize.width(), pmSize.height()));
							borderRect.moveCenter(pixmapRect.center());

#if 0 // Draw layout boxes
							painter->setPen(Qt::black);
							painter->drawRect(rect);

							painter->setPen(Qt::darkGreen);
							painter->drawRect(borderRect);

							painter->setPen(Qt::red);
							painter->drawRect(pixmapRect);
#endif
							painter->save();

							borderRect = borderRect.adjusted(-2, -2, 2, 2);
							int blurredRadius = 5;
							QImage _temp(borderRect.toRect().adjusted(-blurredRadius, -blurredRadius, blurredRadius, blurredRadius).size(), QImage::Format_ARGB32_Premultiplied);
							_temp.fill(0);
							QPainter _tempPainter(&_temp);
							QPainterPath path;

							path.addRoundedRect(blurredRadius + 2, blurredRadius + 2, borderRect.width() - 4, borderRect.height() - 4, 5, 5);

							_tempPainter.setRenderHint(QPainter::Antialiasing, true);
							_tempPainter.fillPath(path, Qt::black);
							_tempPainter.end();

							QImage blurred(_temp.size(), QImage::Format_ARGB32_Premultiplied);
							blurred.fill(0);
							QPainter blurPainter(&blurred);
							qt_blurImage(&blurPainter, _temp, blurredRadius, false, true);
							blurPainter.end();

							QPoint topLeft = borderRect.toRect().topLeft();

							if (toolButtonStyleOptionPtr->state & (State_Sunken | State_On)){
								topLeft -= QPoint(blurredRadius, blurredRadius);
							}
							else{
								topLeft -= QPoint(blurredRadius, blurredRadius - 1);
							}

							painter->drawImage(QRect(topLeft, blurred.size()), blurred);

							path = QPainterPath();
							
							if (toolButtonStyleOptionPtr->state & (State_Sunken | State_On)){
								path.addRect(borderRect.adjusted(15, 0, -15, 3));
								painter->fillPath(path, QColor(25, 200, 60));
							}
							else{
								path.addRoundedRect(borderRect.adjusted(0, 0, 0, 0), 6, 6);
								painter->fillPath(path, Qt::white);
							}

							path = QPainterPath();
							path.addRoundedRect(borderRect.adjusted(0, 0, 0, 0), 6, 6);

							QLinearGradient gradient(0, 0, 0, borderRect.height());
							gradient.setSpread(QGradient::PadSpread);

							if (toolButtonStyleOptionPtr->state & (State_Sunken | State_On)){
								gradient.setColorAt(1, QColor(245, 245, 245));
								gradient.setColorAt(0, QColor(245, 245, 245));
							}
							else {
								gradient.setColorAt(0, QColor(251, 251, 251));
								gradient.setColorAt(1, QColor(235, 235, 235));
							}

							painter->fillPath(path, gradient);

							painter->restore();

							proxy()->drawItemPixmap(painter, pixmapRect, Qt::AlignCenter, pm);
						}
					}
					else{
						DrawArrow(proxy(), toolButtonStyleOptionPtr, pixmapRect, painter, widget);
					}

					int alignment = Qt::TextShowMnemonic;
					if (!proxy()->styleHint(SH_UnderlineShortcut, option, widget)){
						alignment |= Qt::TextHideMnemonic;
					}

					alignment |= Qt::AlignCenter;

					textRect.translate(shiftX, shiftY);

					proxy()->drawItemText(painter, QStyle::visualRect(option->direction, rect, textRect), alignment, toolButtonStyleOptionPtr->palette,
						toolButtonStyleOptionPtr->state & State_Enabled, toolButtonStyleOptionPtr->text,
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


QSize CImtStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const
{
	QSize size = BaseClass::sizeFromContents(type, option, contentsSize, widget);

	const QStyleOptionToolButton* toolButtonStyleOptionPtr = qstyleoption_cast<const QStyleOptionToolButton *>(option);
	if (toolButtonStyleOptionPtr != nullptr) {
		if (toolButtonStyleOptionPtr->toolButtonStyle == Qt::ToolButtonTextUnderIcon){
			int iconWidth = toolButtonStyleOptionPtr->iconSize.width();

			size.setWidth(qMax(size.width(), 3 * iconWidth));
		}
	}

	return size;
}


} // namespace imtwidgets


