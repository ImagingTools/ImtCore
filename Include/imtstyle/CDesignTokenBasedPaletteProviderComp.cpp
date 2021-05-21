#include <imtstyle/CDesignTokenBasedPaletteProviderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QFile>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QStyle>

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtstyle
{


// public methods

const imtbase::ICollectionInfo& CDesignTokenBasedPaletteProviderComp::GetDesignSchemaList() const
{
	return m_designSchemaList;
}


bool CDesignTokenBasedPaletteProviderComp::GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const
{
	if (m_paletteMap.contains(designSchemaId)){
		palette = m_paletteMap[designSchemaId];
		return true;
	}

	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CDesignTokenBasedPaletteProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	CreateDefaultPalettes();

	if (m_resourceNameAttrPtr.IsValid()){
		QFile paletteFile(*m_resourceNameAttrPtr);

		int count = qMin(m_designShemaIdAttrPtr.GetCount(), m_paletteModeAttrPtr.GetCount());
		if (count > 0 && paletteFile.open(QIODevice::ReadOnly)){
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

				for (int i = 0; i < count; i++){
					if (!m_designShemaIdAttrPtr[i].isEmpty() && !m_paletteModeAttrPtr[i].isEmpty()){
						QJsonObject modeObject = paletteObject[m_paletteModeAttrPtr[i]].toObject();
						if (modeObject.isEmpty()){
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
							QPalette palette = m_paletteMap[m_designShemaIdAttrPtr[i]];
							palette.setColor(QPalette::WindowText, textColor);
							palette.setColor(QPalette::Text, textColor);
							palette.setColor(QPalette::Window, backgroundColor);
							palette.setColor(QPalette::Base, backgroundColor);
							paletteMap[designShemaId] = palette;
						}
					}
				}

				m_paletteMap = paletteMap;
			}

			paletteFile.close();
		}
	}
}


// private methods

void CDesignTokenBasedPaletteProviderComp::CreateDefaultPalettes()
{
	istd::TDelPtr<QStyle> baseStylePtr(QStyleFactory::create("fusion"));
	QPalette lightPalette = baseStylePtr->standardPalette();
	lightPalette.setColor(QPalette::Highlight, QColor(130, 210, 255));
	m_paletteMap["Light"] = lightPalette;
	m_designSchemaList.InsertItem("Light", QObject::tr("Light"),"");

	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::WindowText, Qt::white);
	darkPalette.setColor(QPalette::Base, QColor(33, 33, 33));
	darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ToolTipBase, Qt::lightGray);
	darkPalette.setColor(QPalette::ToolTipText, Qt::lightGray);
	darkPalette.setColor(QPalette::Text, Qt::lightGray);
	darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ButtonText, Qt::lightGray);
	darkPalette.setColor(QPalette::BrightText, Qt::white);
	darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::HighlightedText, Qt::white);
	m_paletteMap["Dark"] = darkPalette;
	m_designSchemaList.InsertItem("Dark", QObject::tr("Dark"), "");
}


bool CDesignTokenBasedPaletteProviderComp::StringToColor(const QString& colorString, QColor& color) const
{
	QString tempColorString = colorString;
	tempColorString.replace(" ", "");
	tempColorString.replace("\t", "");

	QRegExp stdFormat("^#[0-9A-Fa-f]{6,6}$");
	if (stdFormat.exactMatch(tempColorString)){
		color = QColor(tempColorString);
		return true;
	}

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


