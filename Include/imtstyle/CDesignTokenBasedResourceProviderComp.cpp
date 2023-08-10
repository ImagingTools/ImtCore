#include <imtstyle/CDesignTokenBasedResourceProviderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QMap>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QStyle>

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtstyle
{


// public methods

bool CDesignTokenBasedResourceProviderComp::GetColorPalette(
			const QByteArray& designSchemaId,
			ColorSchema& palette) const
{
	if(m_designTokenFileParserCompPtr.IsValid()){
		return m_designTokenFileParserCompPtr->GetColorPalette(designSchemaId, palette);
	}

	if (m_paletteMap.contains(designSchemaId)){
		palette = m_paletteMap[designSchemaId];

		return true;
	}

	return false;
}


// reimplemented (IFontProvider)

const imtbase::ICollectionInfo& CDesignTokenBasedResourceProviderComp::GetFontList(const QByteArray& designSchemaId) const
{
	if(m_designTokenFileParserCompPtr.IsValid()){
		return m_designTokenFileParserCompPtr->GetFontList(designSchemaId);
	}

	if (m_fonts.contains(designSchemaId) && m_fontsCollectionInfos[designSchemaId].GetPtr() != nullptr){
		const imtbase::ICollectionInfo& retval = *m_fontsCollectionInfos[designSchemaId];
		return retval;
	}
	return m_emptyCollectionInfo;
}


bool CDesignTokenBasedResourceProviderComp::GetFont(const QByteArray& designSchemaId, const QByteArray& fontId, QFont& font) const
{	
	if(m_designTokenFileParserCompPtr.IsValid()){
		return m_designTokenFileParserCompPtr->GetFont(designSchemaId, fontId, font);
	}

	if (m_fonts.contains(designSchemaId) && m_fonts[designSchemaId].contains(fontId)){
		font = m_fonts[designSchemaId][fontId];
		return true;
	}

	return false;
}


// reimplemented (IColorPaletteProvider)

const imtbase::ICollectionInfo& CDesignTokenBasedResourceProviderComp::GetDesignSchemaList() const
{
	if(m_designTokenFileParserCompPtr.IsValid()){
		return m_designTokenFileParserCompPtr->GetDesignSchemaList();
	}

	return m_designSchemaList;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CDesignTokenBasedResourceProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	CreateDefaultPalettes();

	int count = qMin(m_designShemaIdAttrPtr.GetCount(), m_resourceFileNameAttrPtr.GetCount());

	for (int i = 0; i < count; i++){
		if (!m_resourceFileNameAttrPtr.IsValid()){
			Q_ASSERT(false);

			continue;
		}

		if (m_designTokenFileParserCompPtr.IsValid()){
			m_designTokenFileParserCompPtr->SetFile(m_resourceFileNameAttrPtr[i]);
			m_designTokenFileParserCompPtr->ParseFile();
		}
		else{
			QFile paletteFile(m_resourceFileNameAttrPtr[i]);
			if (paletteFile.open(QIODevice::ReadOnly)){
				QByteArray contents = paletteFile.readAll();
				QJsonDocument document = QJsonDocument::fromJson(contents);
				if (document.isObject()){
					QJsonObject rootObject = document.object();
					QJsonObject paletteObject = rootObject["palette"].toObject();
					if (paletteObject.isEmpty()){
						SendErrorMessage(0, QObject::tr("Palette file parsing error"));

						return;
					}

					QMap<QByteArray, QPalette> paletteMap;

					if (!m_designShemaIdAttrPtr[i].isEmpty() && !m_paletteModeAttrPtr[i].isEmpty()){
						QJsonObject modeObject;
						if (!GetObjectValue(paletteObject, m_paletteModeAttrPtr[i], modeObject)){
							SendErrorMessage(0, QObject::tr("Palette file parsing error"));

							return;
						}

						QJsonObject textObject = modeObject["text"].toObject();
						QJsonObject backgroundObject = modeObject["background"].toObject();
						if (textObject.isEmpty() || backgroundObject.isEmpty()){
							SendErrorMessage(0, QObject::tr("Palette file parsing error"));

							return;
						}

						if (!textObject.contains("primary") || !backgroundObject.contains("default")){
							SendErrorMessage(0, QObject::tr("Palette file parsing error"));

							return;
						}

						if (!textObject["primary"].isString() || !backgroundObject["default"].isString()){
							SendErrorMessage(0, QObject::tr("Palette file parsing error"));

							return;
						}

						QString textColorString = textObject["primary"].toString();
						QString backgroundColorString = backgroundObject["default"].toString();

						QColor textColor;
						QColor backgroundColor;

						if (!StringToColor(textColorString, textColor) || !StringToColor(backgroundColorString, backgroundColor)){
							SendErrorMessage(0, QObject::tr("Palette file parsing error"));

							return;
						}

						QByteArray designShemaId = m_designShemaIdAttrPtr[i];
						if (m_designSchemaList.GetElementIds().contains(designShemaId)){
							ColorSchema colorSchema = m_paletteMap[designShemaId];
							colorSchema.id = designShemaId;
							colorSchema.palette.setColor(QPalette::WindowText, textColor);
							colorSchema.palette.setColor(QPalette::Text, textColor);
							colorSchema.palette.setColor(QPalette::Window, backgroundColor);
							colorSchema.palette.setColor(QPalette::Base, backgroundColor);

							m_paletteMap[designShemaId] = colorSchema;
						}
					}

					QJsonObject typographyObject;
					if (!GetObjectValue(rootObject, "typography", typographyObject)){
						SendErrorMessage(0, QObject::tr("Palette file parsing error"));

						return;
					}

					QJsonArray fontFamilyArray;
					double fontSize = 0.0;
					if (		!GetArrayValue(typographyObject, "fontFamily", fontFamilyArray) ||
								!GetDoubleValue(typographyObject, "fontSize", fontSize)){
						SendErrorMessage(0, QObject::tr("Palette file parsing error"));

						return;
					}

					for (int fontFamilyIndex = 0; fontFamilyIndex < fontFamilyArray.count(); fontFamilyIndex++){
						QString fontName;
						if (!GetStringValue(fontFamilyArray, fontFamilyIndex, fontName)){
							SendErrorMessage(0, QObject::tr("Palette file parsing error"));

							return;
						}

						QFont font(fontName, fontSize);

						QByteArray fontId = fontName.toLatin1();
						fontId.replace(" ", "");
					}
				}
			}

			paletteFile.close();
		}
	}
}


// private methods

int CDesignTokenBasedResourceProviderComp::GetCount() const
{
	return qMin(m_designShemaIdAttrPtr.GetCount(), m_paletteModeAttrPtr.GetCount());
}


void CDesignTokenBasedResourceProviderComp::CreateDefaultPalettes()
{
	ColorSchema light;
	light.id = "Light";
	istd::TDelPtr<QStyle> baseStylePtr(QStyleFactory::create("fusion"));
	QPalette lightPalette = baseStylePtr->standardPalette();
	light.palette.setColor(QPalette::Highlight, QColor("#1a76e7"));
	light.palette.setColor(QPalette::Text, QColor("#335777"));
	light.toolButtonGradientColors.startColor = QColor(248, 248, 251);
	light.toolButtonGradientColors.endColor = QColor(235, 235, 238);
	light.pressedToolButtonGradientColors.startColor = QColor(245, 245, 245);
	light.pressedToolButtonGradientColors.endColor = QColor(245, 245, 245);
	light.stylePath = ":/Styles/Light/ImtColorStyle";

	m_paletteMap["Light"] = light;
	m_designSchemaList.InsertItem("Light", QObject::tr("Light"),"");

	ColorSchema dark;
	dark.id = "Dark";
	dark.palette.setColor(QPalette::Window, QColor(53, 53, 53));
	dark.palette.setColor(QPalette::WindowText, Qt::white);
	dark.palette.setColor(QPalette::Base, QColor(33, 33, 33));
	dark.palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	dark.palette.setColor(QPalette::ToolTipBase, Qt::lightGray);
	dark.palette.setColor(QPalette::ToolTipText, Qt::lightGray);
	dark.palette.setColor(QPalette::Text, Qt::lightGray);
	dark.palette.setColor(QPalette::Button, QColor(53, 53, 53));
	dark.palette.setColor(QPalette::ButtonText, Qt::lightGray);
	dark.palette.setColor(QPalette::BrightText, Qt::white);
	dark.palette.setColor(QPalette::Link, QColor(42, 130, 218));
	dark.palette.setColor(QPalette::Highlight, QColor("#1a76e7"));
	dark.palette.setColor(QPalette::HighlightedText, Qt::white);
	dark.stylePath = ":/Styles/Dark/ImtColorStyle";

	m_paletteMap["Dark"] = dark;
	m_designSchemaList.InsertItem("Dark", QObject::tr("Dark"), "");
}


bool CDesignTokenBasedResourceProviderComp::StringToColor(const QString& colorString, QColor& color) const
{
	QString tempColorString = colorString;
	tempColorString.replace(" ", "");
	tempColorString.replace("\t", "");

#if QT_VERSION < 0x060000
	QRegExp stdFormat("^#[0-9A-Fa-f]{6,6}$");
	if (stdFormat.exactMatch(tempColorString)){
		color = QColor(tempColorString);

		return true;
	}
#else
	QRegularExpression stdFormat("^#[0-9A-Fa-f]{6,6}$");
	if (stdFormat.match(tempColorString).hasMatch()){
		color = QColor(tempColorString);

		return true;
	}
#endif

	if (tempColorString.startsWith("rgba(") && tempColorString.endsWith(")")){
		QStringList values = tempColorString.mid(5, tempColorString.count() - 6).split(QChar(','));
		if (values.count() == 4){
			bool isOkR, isOkG, isOkB, isOkA;

			double r = values[0].toDouble(&isOkR) * 255;
			double g = values[1].toDouble(&isOkG) * 255;
			double b = values[2].toDouble(&isOkB) * 255;
			double a = values[3].toDouble(&isOkA) * 255;

			if (isOkR && isOkG && isOkB && isOkA){
				color.setRgb(r, g, b, a);

				return true;
			}
		}
	}

	return false;
}


} // namespace imtstyle


