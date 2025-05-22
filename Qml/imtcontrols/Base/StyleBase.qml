import QtQuick 2.0


/*!
	\brief
		The basic component for creating a style.
		Inherit it to create your own style for the app.
		To create your own style, you need to create your own module with Style.qml file.

	\warning
		The module must have a name 'Acf' and a version 1.0.

	\example
		\file qmldir
			\code
				module Acf
				singleton Style 1.0 Style.qml
*/
StyleComponents {
	id: styleContainer;

	//Main
	property string styleId: "StyleAcf"

	property bool mobile;
	property real mainWidth;
	property real mainHeight;
	property real mainSize;

	property string theme: "Light";

	property string language;//deprecated?

	//Font properties
	property string mainFontSource: "../../../Fonts/Ubuntu-Light.ttf";
	property string boldFontSource: "../../../Fonts/Ubuntu-Medium.ttf";
	property string fontFamily: "Verdana";
	property string fontFamilyBold: "Verdana";

	property alias mainFontLoader: mainFont;
	property alias boldFontLoader: boldFont;

	//Font size
	property int fontSizeXHuge: 64
	property int fontSizeHuge: 48
	property int fontSizeXXXXLarge: 32
	property int fontSizeXXXLarge: 24
	property int fontSizeXXLarge: 20
	property int fontSizeXLarge: 18
	property int fontSizeLarge: 16
	property int fontSizeNormal: 13
	property int fontSizeSmall: 11
	property int fontSizeXSmall: 10
	property int fontSizeXXSmall: 8
	property int fontSizeTiny: 6

	//Button sizes
	property int buttonWidthLarge: 32
	property int buttonWidthMedium: 24
	property int buttonWidthSmall: 16
	property int buttonWidthXSmall: 8
	property int buttonWidthTiny: 4

	property int buttonRadius: 4
	property int buttonBorderWidth: 1
	property int buttonHeight: 30

	//Icon sizes
	property int iconSizeXLarge: 128
	property int iconSizeLarge: 64
	property int iconSizeMedium: 32
	property int iconSizeSmall: 16
	property int iconSizeXSmall: 8

	//Item sizes
	property int itemSizeHuge: 128
	property int itemSizeXLarge: 64
	property int itemSizeLarge: 48
	property int itemSizeMedium: 32
	property int itemSizeSmall: 16
	property int itemSizeXSmall: 8

	//Opacity
	property real opacityFaint: 0.05
	property real opacityHigh: 0.75
	property real opacityLow: 0.25
	property real opacityOverlay: 0.15
	property real selectedOpacity: 0.75
	property real hoverOpacity: selectedOpacity/2
	property real highlightBackgroundOpacity: 0.15

	//Paddings
	property int paddingHuge: 64
	property int paddingLarge: 32
	property int paddingMedium: 16
	property int paddingSmall: 8

	//Margins
	property int sizeSmallMargin: 6;
	property int sizeMainMargin: 11;
	property int sizeLargeMargin: 23;

	property int mainWindowMargin: 0;
	property int pageMargin: 0;

	//Radius
	property int textFieldRadius: 3;
	property int comboBoxRadius: 3;

	//Other sizes
	property int sizePanelsHeight: 50;


	//Alternating
	property bool enableAlternating: true;
	property real alternatingOpacity: 0.05

	//Gradient
	property string imagingToolsGradient0: "#f6f6f6";
	property string imagingToolsGradient1: "#e3e3e5";
	property string imagingToolsGradient2: "#d4d4d6";
	property string imagingToolsGradient3: "#bebec0";
	property string imagingToolsGradient4: "#b4b4b6";


	//Colors
		//main
	property string baseColor: "white";
	property string baseColorInverted: "black";
	property string mainColor: "#e6e6e8";
	property string firstColor: mainColor
	property string secondColor: "#F18C16";
	property string colorMenuPanel: imagingToolsGradient1;

		//error
	property string errorColor: errorTextColor
	property string errorTextColor: "#FF4500";

		//accent
	property string imaginToolsAccentColor: "#2a82da"
	property string positiveAccentColor: "#00FFD1";
	property string negativeAccentColor: "#FF5449";
	property string middleAccentColor: "yellow";

		//text
	property string textColor: "#335777";
	property string titleColor: "#055A85";
	property string subtitleColor: "#6A6E6F";
	property string textSelectedColor: "#0078d7";
	property string inactiveTextColor: "#aaaaaa";
	property string placeHolderTextColor : '#AAA'
	property string linkColor: "#0078D7";
	property string selectedLinkFromColor: "#1974d2";
	property string selectedLinkToColor: "#51AC2A";

		//border
	property string borderColor: "#d0d0d2";
	property string borderColor2: "#a4a4a6";

		//background
	property string backgroundColor: "#e6e6e8";
	property string backgroundColor2: "#f7f7f7";
	property string menuPanelBackgroundColor: "#e2e2e8";

	property string dialogBackgroundColor: "#f6f6f6";
	property string overlayBackgroundColor: '#000'
	property string tabPanelBackgroundColor: backgroundColor;

		//highlight
	property string highlightBackgroundColor: '#000'
	property string highlightColor: selectedColor
	property string highlightDimmerColor: '#DED'
	property string secondaryHighlightColor: linkColor

		//selected
	property string selectedColor: "#e3eefa";
	property string tabSelectedColor: "#FF4500";
	property string iconColorOnSelected: "#1a76e7";

		//button
	property string buttonColor: "#dddddd";
	property string buttonBorderColor: "#b0b0b0";
	property string buttonHoverColor: "#d2d2d2";
	property string buttonPressedColor: "#c6c6c6";
	property string buttonAccentColor: imaginToolsAccentColor;
	property string buttonInactiveColor: "#C0C0C0";
	property string buttonBorderFocusColor: borderColor;
	property string buttonTextColor: "#335777";
	property string buttonInactiveTextColor: "#335777";

		//scroll
	property string scrollBackgroundColor: styleContainer.imagingToolsGradient0;
	property string scrollIndicatorColor: styleContainer.imagingToolsGradient3;
	property string scrollHighlightColor: styleContainer.imagingToolsGradient0;

		//shadow, hover
	property string shadowColor: "#ececec";
	property string hover: "#e6e6e8";

		//Alternating
	property color alternatingColor: '#000'
	property string alternateBaseColor: "#f5f5f5";

		//simple colors
	property string grayColor: "#C2CEDB";
	property string greenColor: "#00ff00";
	property string lightBlueColor: "#69b8e3";

	//other parameters
	property real maximumFlickVelocity: 75
	property bool enableHoverEffect: true


	function getIconPath(iconName, state, mode, extention /* = '.svg'*/){
		if(iconName == undefined){
			return;
		}
		if (iconName.length > 0){
			if (iconName.charAt(0) !== '/'){
				iconName = '/' + iconName;
			}
		}
		else {
			return ""
		}

		let iconPath = styleContainer.theme + iconName;

		if (state && state !== ""){
			iconPath += "_" + state;
		}

		if (mode && mode !== ""){
			iconPath += "_" + mode;
		}

		if (typeof extention !== 'string'){
			iconPath += ".svg";
		}
		else {
			iconPath += extention
		}

		return iconPath;
	}

	function getThemeColor(colorType, colorKey, themeType){
		if (themeType.containsKey("Style")){
			let styleModel = themeType.getData("Style");
			if (styleModel.containsKey(colorType)){
				let typeModel = styleModel.getData(colorType);
				if (typeModel.containsKey(colorKey)){
					let colorPalette = typeModel.getData(colorKey);

					if (themeType.containsKey("ColorPalette")){
						let themeModel = themeType.getData("ColorPalette")
						if (themeModel.containsKey(colorPalette)){
							return themeModel.getData(colorPalette)
						}
					}
				}
			}
		}

		return "";
	}

	function setDecorators(sourceDecorators){
		if (sourceDecorators.styleId){
			styleContainer.styleId = sourceDecorators.styleId
		}

		if (sourceDecorators.menuPanelButtonDecorator){
			styleContainer.menuPanelButtonDecorator = sourceDecorators.menuPanelButtonDecorator
		}
		if (sourceDecorators.subMenuPanelButtonDecorator){
			styleContainer.subMenuPanelButtonDecorator = sourceDecorators.subMenuPanelButtonDecorator
		}
		if (sourceDecorators.filterPanelDecorator){
			styleContainer.filterPanelDecorator = sourceDecorators.filterPanelDecorator
		}
		if (sourceDecorators.bottomPanelDecorator){
			styleContainer.bottomPanelDecorator = sourceDecorators.bottomPanelDecorator
		}
		if (sourceDecorators.topButtonDecorator){
			styleContainer.topButtonDecorator = sourceDecorators.topButtonDecorator
		}
		if (sourceDecorators.topPanelDecorator){
			styleContainer.topPanelDecorator = sourceDecorators.topPanelDecorator
		}
		if (sourceDecorators.topCenterPanelDecorator){
			styleContainer.topCenterPanelDecorator = sourceDecorators.topCenterPanelDecorator
		}
		if (sourceDecorators.topLeftPanelDecorator){
			styleContainer.topLeftPanelDecorator = sourceDecorators.topLeftPanelDecorator
		}
		if (sourceDecorators.topRightPanelDecorator){
			styleContainer.topRightPanelDecorator = sourceDecorators.topRightPanelDecorator
		}
		if (sourceDecorators.tabPanelDecorator){
			styleContainer.tabPanelDecorator = sourceDecorators.tabPanelDecorator
		}
		if (sourceDecorators.buttonDecorator){
			styleContainer.buttonDecorator = sourceDecorators.buttonDecorator
		}
		if (sourceDecorators.firstSegmentDecorator){
			styleContainer.firstSegmentDecorator = sourceDecorators.firstSegmentDecorator
		}
		if (sourceDecorators.middleSegmentDecorator){
			styleContainer.middleSegmentDecorator = sourceDecorators.middleSegmentDecorator
		}
		if (sourceDecorators.lastSegmentDecorator){
			styleContainer.lastSegmentDecorator = sourceDecorators.lastSegmentDecorator
		}
		if (sourceDecorators.roundButtonDecorator){
			styleContainer.roundButtonDecorator = sourceDecorators.roundButtonDecorator
		}
		if (sourceDecorators.textFieldDecorator){
			styleContainer.textFieldDecorator = sourceDecorators.textFieldDecorator
		}
		if (sourceDecorators.textEditDecorator){
			styleContainer.textEditDecorator = sourceDecorators.textEditDecorator
		}
		if (sourceDecorators.comboBoxDecorator){
			styleContainer.comboBoxDecorator = sourceDecorators.comboBoxDecorator
		}
		if (sourceDecorators.checkBoxDecorator){
			styleContainer.checkBoxDecorator = sourceDecorators.checkBoxDecorator
		}
		if (sourceDecorators.switchDecorator){
			styleContainer.switchDecorator = sourceDecorators.switchDecorator
		}
		if (sourceDecorators.sliderDecorator){
			styleContainer.sliderDecorator = sourceDecorators.sliderDecorator
		}
		if (sourceDecorators.rangeSliderDecorator){
			styleContainer.rangeSliderDecorator = sourceDecorators.rangeSliderDecorator
		}
		if (sourceDecorators.radioButtonDecorator){
			styleContainer.radioButtonDecorator = sourceDecorators.radioButtonDecorator
		}
		if (sourceDecorators.textAreaDecorator){
			styleContainer.textAreaDecorator = sourceDecorators.textAreaDecorator
		}
		if (sourceDecorators.spinBoxDecorator){
			styleContainer.spinBoxDecorator = sourceDecorators.spinBoxDecorator
		}
		if (sourceDecorators.dialogDecorator){
			styleContainer.dialogDecorator = sourceDecorators.dialogDecorator
		}
		if (sourceDecorators.progressBarDecorator){
			styleContainer.progressBarDecorator = sourceDecorators.progressBarDecorator
		}
		if (sourceDecorators.busyIndicatorDecorator){
			styleContainer.busyIndicatorDecorator = sourceDecorators.busyIndicatorDecorator
		}
		if (sourceDecorators.tooltipDecorator){
			styleContainer.tooltipDecorator = sourceDecorators.tooltipDecorator
		}
		if (sourceDecorators.scrollBarDecorator !== undefined){
			styleContainer.scrollBarDecorator = sourceDecorators.scrollBarDecorator
		}

		if (sourceDecorators.backGroundDecorator){
			styleContainer.backGroundDecorator = sourceDecorators.backGroundDecorator
		}

		if (sourceDecorators.messageDecorator){
			styleContainer.messageDecorator = sourceDecorators.messageDecorator
		}

		if (sourceDecorators.titleDecorator){
			styleContainer.titleDecorator = sourceDecorators.titleDecorator
		}

		if (sourceDecorators.inputTitleDecorator){
			styleContainer.inputTitleDecorator = sourceDecorators.inputTitleDecorator
		}

		if (sourceDecorators.frame){
			styleContainer.frame = sourceDecorators.frame
		}

		if (sourceDecorators.commonButtonDecorator){
			styleContainer.commonButtonDecorator = sourceDecorators.commonButtonDecorator
		}

		if (sourceDecorators.topPanelDialogDecorator){
			styleContainer.topPanelDialogDecorator = sourceDecorators.topPanelDialogDecorator
		}

		if (sourceDecorators.itemDelegateDecorator){
			styleContainer.itemDelegateDecorator = sourceDecorators.itemDelegateDecorator
		}

		if (sourceDecorators.toolButtonDecorator){
			styleContainer.toolButtonDecorator = sourceDecorators.toolButtonDecorator
		}

		if (sourceDecorators.popupDecorator){
			styleContainer.popupDecorator = sourceDecorators.popupDecorator
		}

		if (sourceDecorators.popupItemDelegateDecorator){
			styleContainer.popupItemDelegateDecorator = sourceDecorators.popupItemDelegateDecorator
		}

		if (sourceDecorators.detailedPopupItemDelegateDecorator){
			styleContainer.detailedPopupItemDelegateDecorator = sourceDecorators.detailedPopupItemDelegateDecorator
		}

		if (sourceDecorators.collectionTableDecorator){
			styleContainer.collectionTableDecorator = sourceDecorators.collectionTableDecorator
		}

		if (sourceDecorators.drawingContainerDecorator){
			styleContainer.drawingContainerDecorator = sourceDecorators.drawingContainerDecorator
		}

	}

	function parseStyleTheme(themeType){
		if (themeType.containsKey("source")){
			let dataSource = themeType.getData("source");
			styleContainer.borderColor = styleContainer.getThemeColor("ActiveColors", "BorderColor", dataSource);
			styleContainer.baseColor = styleContainer.getThemeColor("ActiveColors", "Base", dataSource);
			styleContainer.alternateBaseColor = styleContainer.getThemeColor("ActiveColors", "AlternateBase", dataSource);
			styleContainer.backgroundColor = styleContainer.getThemeColor("ActiveColors", "Background", dataSource);
			styleContainer.backgroundColor2 = styleContainer.getThemeColor("ActiveColors", "Background2", dataSource);

			styleContainer.textColor = styleContainer.getThemeColor("ActiveColors", "Text", dataSource);
			styleContainer.textSelectedColor = styleContainer.getThemeColor("ActiveColors", "TextSelectedBackground", dataSource);
			styleContainer.shadowColor = styleContainer.getThemeColor("ActiveColors", "Shadow", dataSource);

			styleContainer.selectedColor = styleContainer.getThemeColor("ActiveColors", "ItemSelected", dataSource);

			styleContainer.buttonColor = styleContainer.getThemeColor("ActiveColors", "Button", dataSource);
			styleContainer.buttonBorderColor = styleContainer.getThemeColor("ActiveColors", "ButtonBorder", dataSource);
			styleContainer.buttonInactiveColor = styleContainer.getThemeColor("InactiveColors", "Button", dataSource);
			styleContainer.buttonTextColor = styleContainer.getThemeColor("ActiveColors", "ButtonText", dataSource);
			styleContainer.buttonInactiveTextColor = styleContainer.getThemeColor("InactiveColors", "ButtonText", dataSource);

			styleContainer.inactiveTextColor = styleContainer.getThemeColor("InactiveColors", "Text", dataSource);

			styleContainer.buttonHoverColor = styleContainer.alternateBaseColor;
			styleContainer.buttonPressedColor = styleContainer.getThemeColor("ActiveColors", "BackgroundSelected", dataSource);

			styleContainer.imagingToolsGradient0 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient0");
			styleContainer.imagingToolsGradient1 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient1");
			styleContainer.imagingToolsGradient2 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient2");
			styleContainer.imagingToolsGradient3 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient3");
			styleContainer.imagingToolsGradient4 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient4");
			styleContainer.greenColor = dataSource.getData("ColorPalette").getData("Green");

			styleContainer.iconColorOnSelected = styleContainer.getThemeColor("ActiveColors", "Highlight", dataSource);

			styleContainer.tabSelectedColor = styleContainer.getThemeColor("ActiveColors", "TabSelected", dataSource);
			styleContainer.errorTextColor = styleContainer.getThemeColor("ActiveColors", "ErrorText", dataSource);

			styleContainer.imaginToolsAccentColor = dataSource.getData("ColorPalette").getData("ImaginTools Accent");
		}
	}

	FontLoader{
		id: mainFont;
		source: styleContainer.mainFontSource;
	}

	FontLoader{
		id: boldFont;
		source: styleContainer.boldFontSource;
	}
}
