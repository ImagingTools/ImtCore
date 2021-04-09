#include <imtwidgets/CImtStyle.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QToolButton>

// ACF includes
#include <iqtgui/iqtgui.h>


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


// public methods

CImtStyle::CImtStyle()
	:m_designSchema(DS_LIGHT),
	m_styleType(ST_IMAGINGTOOLS),
	m_wasStyleSheetInitialized(false)
{
	ColorSchema light;
	light.toolButtonGradientColors.startColor = QColor(248, 248, 251);
	light.toolButtonGradientColors.endColor = QColor(235, 235, 238);
	light.pressedToolButtonGradientColors.startColor = QColor(245, 245, 245);
	light.pressedToolButtonGradientColors.endColor = QColor(245, 245, 245);
	light.styleSheetPath = ":/Styles/ImtLightStyle";

	m_colorSchemaMap[DS_LIGHT] = light;

	ColorSchema dark;
	dark.toolButtonGradientColors.startColor = QColor(160, 160, 160);
	dark.toolButtonGradientColors.endColor = QColor(135, 135, 135);
	dark.pressedToolButtonGradientColors.startColor = QColor(155, 155, 155);
	dark.pressedToolButtonGradientColors.endColor = QColor(155, 155, 155);
	dark.styleSheetPath = ":/Styles/ImtDarkStyle";

	m_colorSchemaMap[DS_DARK] = dark;
}


CImtStyle::DesignSchema CImtStyle::GetDesignSchema() const
{
	return m_designSchema;
}


void CImtStyle::SetDesignSchema(DesignSchema designSchema)
{
	m_designSchema = designSchema;

	EnsureStyleSheetApplied(true);

	BaseClass::polish(qApp);
}


CImtStyle::StyleType CImtStyle::GetStyleType() const
{
	return m_styleType;
}


void CImtStyle::SetStyleType(StyleType styleType)
{
	m_styleType = styleType;

	EnsureStyleSheetApplied(true);

	BaseClass::polish(qApp);
}


// reimplemented (QStyle)

void CImtStyle::polish(QWidget* widgetPtr)
{
	EnsureStyleSheetApplied();

	BaseClass::polish(widgetPtr);
}


void CImtStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption * option, QPainter * painter, const QWidget * widget) const
{
	if (m_styleType == ST_IMAGINGTOOLS){
		if (pe == QStyle::PE_PanelButtonCommand){
			if (widget != nullptr){
				QWidget* parentWidget = widget->parentWidget();
				if ((parentWidget != nullptr) && parentWidget->property("ImtToolBar").toBool()){
					return;
				}
			}
		}
	}

	BaseClass::drawPrimitive(pe, option, painter, widget);
}


void CImtStyle::drawControl(
			ControlElement element,
			const QStyleOption* option,
			QPainter* painter,
			const QWidget* widget) const
{
	if (element == CE_ToolButtonLabel){
		if ((m_styleType == ST_IMAGINGTOOLS) && (widget != nullptr)){
			QWidget* parentWidget = widget->parentWidget();
			if ((parentWidget != nullptr) && parentWidget->property("ImtToolBar").toBool()){
				DrawImagingToolsToolButton(qstyleoption_cast<const QStyleOptionToolButton *>(option), painter, widget);

				return;
			}
		}
	}

	BaseClass::drawControl(element, option, painter, widget);
}


QRect CImtStyle::subElementRect(SubElement subElement, const QStyleOption* option, const QWidget* widget) const
{
	if (m_styleType == ST_IMAGINGTOOLS){
		if (subElement == QStyle::SE_ToolButtonLayoutItem){
			return QRect();
		}
	}

	return BaseClass::subElementRect(subElement, option, widget);
}


QSize CImtStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget) const
{
	QSize size = BaseClass::sizeFromContents(type, option, contentsSize, widget);

	if (m_styleType == ST_IMAGINGTOOLS){
		const QStyleOptionToolButton* toolButtonStyleOptionPtr = qstyleoption_cast<const QStyleOptionToolButton *>(option);
		if (toolButtonStyleOptionPtr != nullptr){
			if (toolButtonStyleOptionPtr->toolButtonStyle == Qt::ToolButtonTextUnderIcon){
				int iconWidth = toolButtonStyleOptionPtr->iconSize.width();

				int elementWitdh = 3 * iconWidth;
				size.setWidth(qMax(size.width(), elementWitdh));
			}
		}
	}

	return size;
}


// protected methods

void CImtStyle::DrawImagingToolsToolButton(
			const QStyleOptionToolButton* optionPtr,
			QPainter* painter,
			const QWidget* widget) const
{
	if (optionPtr == nullptr){
		return;
	}

	ColorSchema colorSchema = m_colorSchemaMap[m_designSchema];

	QRect rect = optionPtr->rect;
	int shiftX = 0;
	int shiftY = 0;
	if (optionPtr->state & (State_Sunken | State_On)){
		shiftX = proxy()->pixelMetric(PM_ButtonShiftHorizontal, optionPtr, widget);
		shiftY = proxy()->pixelMetric(PM_ButtonShiftVertical, optionPtr, widget);
	}

	// Arrow type always overrules and is always shown
	bool hasArrow = optionPtr->features & QStyleOptionToolButton::Arrow;

	QPixmap toolButtonPixmap;

	QRect pixmapRect = rect.adjusted(1, 1, -1, -1);
	QSize pmSize = optionPtr->iconSize;

	if (!optionPtr->icon.isNull()){
		QIcon::State state = optionPtr->state & State_On ? QIcon::On : QIcon::Off;
		QIcon::Mode mode;
		if (!(optionPtr->state & State_Enabled)){
			mode = QIcon::Disabled;
		}
		else if ((optionPtr->state & State_MouseOver) && (optionPtr->state & State_AutoRaise)){
			mode = QIcon::Active;
		}
		else{
			mode = QIcon::Normal;
		}

		QSize iconSize = pixmapRect.size().boundedTo(optionPtr->iconSize);

		toolButtonPixmap = optionPtr->icon.pixmap(GetWindowHandle(widget), iconSize, mode, state);

		double deviceRatio = toolButtonPixmap.devicePixelRatio();

		pmSize = toolButtonPixmap.size() / deviceRatio;
	}

	painter->setFont(optionPtr->font);
	QRect textRect = rect;

	if (optionPtr->toolButtonStyle == Qt::ToolButtonTextUnderIcon){
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
				QImage shadowTempImage(borderRect.toRect().adjusted(-blurredRadius, -blurredRadius, blurredRadius, blurredRadius).size(), QImage::Format_ARGB32_Premultiplied);
				shadowTempImage.fill(0);
				QPainter shadowPainter(&shadowTempImage);
				QPainterPath path;

				path.addRoundedRect(blurredRadius + 2, blurredRadius + 2, borderRect.width() - 4, borderRect.height() - 4, 5, 5);

				shadowPainter.setRenderHint(QPainter::Antialiasing, true);
				shadowPainter.fillPath(path, Qt::black);
				shadowPainter.end();

				QImage shadowImage(shadowTempImage.size(), QImage::Format_ARGB32_Premultiplied);
				shadowImage.fill(0);
				QPainter blurPainter(&shadowImage);
				qt_blurImage(&blurPainter, shadowTempImage, blurredRadius, false, true);
				blurPainter.end();

				QPoint topLeft = borderRect.toRect().topLeft();

				if (optionPtr->state & (State_Sunken | State_On)){
					topLeft -= QPoint(blurredRadius, blurredRadius);
				}
				else{
					topLeft -= QPoint(blurredRadius, blurredRadius - 1);
				}

				painter->drawImage(QRect(topLeft, shadowImage.size()), shadowImage);

				path = QPainterPath();

				if (optionPtr->state & (State_Sunken | State_On)){
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

				if (optionPtr->state & (State_Sunken | State_On)){
					gradient.setColorAt(1, colorSchema.pressedToolButtonGradientColors.startColor);
					gradient.setColorAt(0, colorSchema.pressedToolButtonGradientColors.endColor);
				}
				else{
					gradient.setColorAt(0, colorSchema.toolButtonGradientColors.startColor);
					gradient.setColorAt(1, colorSchema.toolButtonGradientColors.endColor);
				}

				painter->fillPath(path, gradient);

				painter->restore();

				proxy()->drawItemPixmap(painter, pixmapRect, Qt::AlignCenter, toolButtonPixmap);
			}
		}
		else{
			DrawArrow(proxy(), optionPtr, pixmapRect, painter, widget);
		}

		int alignment = Qt::TextShowMnemonic;
		if (!proxy()->styleHint(SH_UnderlineShortcut, optionPtr, widget)){
			alignment |= Qt::TextHideMnemonic;
		}

		alignment |= Qt::AlignCenter;

		textRect.translate(shiftX, shiftY);

		proxy()->drawItemText(
					painter,
					QStyle::visualRect(optionPtr->direction, rect, textRect),
					alignment,
					optionPtr->palette,
					optionPtr->state & State_Enabled,
					optionPtr->text,
					QPalette::ButtonText);
	}
}


void CImtStyle::EnsureStyleSheetApplied(bool force) const
{
	if (force){
		m_wasStyleSheetInitialized = false;
	}

	if (qApp != nullptr){
		if (!m_wasStyleSheetInitialized){
			m_wasStyleSheetInitialized = true;

			iqtgui::SetStyleSheetFromFile(*qApp, m_colorSchemaMap[m_designSchema].styleSheetPath);
		}
	}
	else{
		m_wasStyleSheetInitialized = false;
	}
}


} // namespace imtwidgets


