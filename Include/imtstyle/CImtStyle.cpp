#include <imtstyle/CImtStyle.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QStyleFactory>
#include <QtGui/QPixmapCache>

// ACF includes
#include <istd/TDelPtr.h>
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


namespace imtstyle
{


// public methods


CImtStyle* CImtStyle::m_instancePtr = nullptr;


CImtStyle* CImtStyle::GetInstance()
{
	if (m_instancePtr == nullptr){
		m_instancePtr = new CImtStyle;
	}

	return m_instancePtr;
}


int CImtStyle::GetDesignSchemaCount() const
{
	return DS_LAST;
}


CImtStyle::DesignSchema CImtStyle::GetDesignSchemaFromIndex(int index) const
{
	if (index >= 0 && index < CImtStyle::DS_LAST){
		return (DesignSchema)index;
	}

	return DS_INVALID;
}


CImtStyle::DesignSchema CImtStyle::GetDesignSchema() const
{
	return m_designSchema;
}


void CImtStyle::SetDesignSchema(DesignSchema designSchema)
{
	if (m_blockerCount == 0){
		if (m_designSchema != designSchema){
			if (qApp != nullptr){
				BaseClass::unpolish(qApp);
			}

			m_designSchema = designSchema;

			EnsureStyleSheetApplied(true);

			if (qApp != nullptr){
				BaseClass::polish(qApp);
			}
		}
	}
}


CImtStyle::StyleType CImtStyle::GetStyleType() const
{
	return m_styleType;
}


void CImtStyle::SetStyleType(StyleType styleType)
{
	m_styleType = styleType;

	EnsureStyleSheetApplied(true);

	if (qApp != nullptr){
		BaseClass::polish(qApp);
	}
}


QByteArray CImtStyle::GetDesignSchemaId(DesignSchema designSchema) const
{
	switch (designSchema){
	case DS_LIGHT:
		return "Light";
	case DS_DARK:
		return "Dark";
	default:
		return QByteArray();
	}
}


QString CImtStyle::GetDesignSchemaName(DesignSchema designSchema) const
{
	switch (designSchema){
	case DS_LIGHT:
		return tr("Light");
	case DS_DARK:
		return tr("Dark");
	default:
		return QString();
	}
}


void CImtStyle::SetPalette(DesignSchema designSchema, const QPalette& palette)
{
	m_colorSchemaMap[designSchema].palette = palette;
}


// reimplemented (QStyle)

void CImtStyle::polish(QWidget* widgetPtr)
{
	EnsureStyleSheetApplied();

	BaseClass::polish(widgetPtr);
}


void CImtStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
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

	if ((optionPtr->toolButtonStyle == Qt::ToolButtonTextUnderIcon) || (optionPtr->toolButtonStyle == Qt::ToolButtonIconOnly)){
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

					if (m_designSchema == DS_LIGHT){
						painter->fillPath(path, Qt::white);
					}
					else{
						painter->fillPath(path, QColor(88, 88, 88));
					}
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

		if (optionPtr->toolButtonStyle == Qt::ToolButtonTextUnderIcon){
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
}


void CImtStyle::EnsureStyleSheetApplied(bool force) const
{
	if (force){
		m_wasStyleSheetInitialized = false;
	}

	if (qApp != nullptr){
		if (!m_wasStyleSheetInitialized){
			m_wasStyleSheetInitialized = true;

			QPixmapCache::clear();

			const ColorSchema& colorSchema = m_colorSchemaMap[m_designSchema];

			qApp->setPalette(colorSchema.palette);

			QWidget widget;

			iqtgui::SetStyleSheetFromFile(widget, ":/Styles/ImtStyle");

			QString imtNoColorStyle = widget.styleSheet();
			iqtgui::SetStyleSheetFromFile(widget, colorSchema.stylePath);

			QString imtStyle = widget.styleSheet();
			qApp->setStyleSheet(imtNoColorStyle + imtStyle);
		}
	}
	else{
		m_wasStyleSheetInitialized = false;
	}
}


// private methods

CImtStyle::CImtStyle()
	:m_designSchema(DS_LIGHT),
	m_styleType(ST_IMAGINGTOOLS),
	m_wasStyleSheetInitialized(false),
	m_blockerCount(0)
{
	ColorSchema light;
	light.toolButtonGradientColors.startColor = QColor(248, 248, 251);
	light.toolButtonGradientColors.endColor = QColor(235, 235, 238);
	light.pressedToolButtonGradientColors.startColor = QColor(245, 245, 245);
	light.pressedToolButtonGradientColors.endColor = QColor(245, 245, 245);
	istd::TDelPtr<QStyle> baseStylePtr(QStyleFactory::create("fusion"));
	light.palette = baseStylePtr->standardPalette();
	light.palette.setColor(QPalette::Highlight, QColor("#1a76e7"));
	light.palette.setColor(QPalette::Text, QColor("#335777"));
	light.stylePath = ":/Styles/ImtColorStyle";

	m_colorSchemaMap[DS_LIGHT] = light;

	ColorSchema dark;
	dark.toolButtonGradientColors.startColor = QColor(83, 83, 83);
	dark.toolButtonGradientColors.endColor = QColor(58, 58, 58);
	dark.pressedToolButtonGradientColors.startColor = QColor(77, 77, 77);
	dark.pressedToolButtonGradientColors.endColor = QColor(77, 77, 77);
	dark.palette.setColor(QPalette::Window, QColor(53, 53, 53));
	dark.palette.setColor(QPalette::WindowText, Qt::white);
	dark.palette.setColor(QPalette::Base, QColor(33, 33, 33));
	dark.palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	dark.palette.setColor(QPalette::ToolTipBase, Qt::lightGray);
	dark.palette.setColor(QPalette::ToolTipText, Qt::lightGray);
	dark.palette.setColor(QPalette::Text, Qt::lightGray);
	dark.palette.setColor(QPalette::ButtonText, Qt::lightGray);
	dark.palette.setColor(QPalette::BrightText, Qt::white);
	dark.palette.setColor(QPalette::Link, QColor(42, 130, 218));
	dark.palette.setColor(QPalette::Highlight, QColor("#1a76e7"));
	dark.palette.setColor(QPalette::HighlightedText, Qt::white);
	dark.palette.setColor(QPalette::Light, QColor(63, 63, 63));
	dark.palette.setColor(QPalette::Midlight, QColor(58, 58, 58));
	dark.palette.setColor(QPalette::Button, QColor(53, 53, 53));
	dark.palette.setColor(QPalette::Mid, QColor(48, 48, 48));
	dark.palette.setColor(QPalette::Dark, QColor(43, 43, 43));
	dark.palette.setColor(QPalette::Shadow, QColor(30, 30, 30));
	dark.stylePath = ":/Styles/ImtColorStyle";

	m_colorSchemaMap[DS_DARK] = dark;
}


// public methods of the embedded class DesignSchemaSetterBlocker

CImtStyle::DesignSchemaSetterBlocker::DesignSchemaSetterBlocker(CImtStyle& parent)
	:m_parent(parent)
{
	m_parent.m_blockerCount++;
}


CImtStyle::DesignSchemaSetterBlocker::~DesignSchemaSetterBlocker()
{
	m_parent.m_blockerCount--;
	Q_ASSERT(m_parent.m_blockerCount >= 0);
}


} // namespace imtstyle


