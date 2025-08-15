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

	//Start window sizes
	property int startWindowWidth: 800;
	property int startWindowHeight: 600;


	//deprecated, will be romoved after refactoring
	//Font size - deprecated,change to --------------->
	property int fontSizeXHuge: 64			//fontSizeBXXXL
	property int fontSizeHuge: 48			//fontSizeBXXL
	property int fontSizeXXXXLarge: 32		//fontSizeBXL
	property int fontSizeXXXLarge: 24		//fontSizeXXXL
	property int fontSizeXXLarge: 20		//fontSizeXXL
	property int fontSizeXLarge: 18			//fontSizeXL
	property int fontSizeLarge: 16			//fontSizeL
	property int fontSizeNormal: 13			//fontSizeM
	property int fontSizeSmall: 11			//fontSizeS
	property int fontSizeXSmall: 10			//fontSizeXS
	property int fontSizeXXSmall: 8			//fontSizeXXS
	property int fontSizeTiny: 6			//fontSizeXXXS


	//Button sizes - deprecated, change to ---------->
	property int buttonWidthLarge: 32		//buttonWidthL
	property int buttonWidthMedium: 24		//buttonWidthM
	property int buttonWidthSmall: 16		//buttonWidthXS
	property int buttonWidthXSmall: 8		//buttonWidthXXS
	property int buttonWidthTiny: 4			//buttonWidthXXXS

	property int buttonHeight: 30 //  deprecated, change to ----->buttonHeightM

	//Icon sizes - - deprecated,change to ---------->
	property int iconSizeXLarge: 128	//iconSizeXXXL
	property int iconSizeLarge: 64		//iconSizeXXL
	property int iconSizeMedium: 32		//iconSizeL
	property int iconSizeSmall: 16		//iconSizeS
	property int iconSizeXSmall: 8		//iconSizeXXXS

	//Item sizes - deprecated,change to ---------->
	property int itemSizeHuge: 128		//itemSizeXXL
	property int itemSizeXLarge: 64		//itemSizeXL
	property int itemSizeLarge: 48		//itemSizeL
	property int itemSizeMedium: 32		//itemSizeM
	property int itemSizeSmall: 16		//itemSizeS
	property int itemSizeXSmall: 8		//itemSizeXS

	//Paddings - deprecated,change to ---------->
	property int paddingHuge: 64		//paddingXL
	property int paddingLarge: 32		//paddingL
	property int paddingMedium: 16		//paddingM
	property int paddingSmall: 8		//paddingS

	//Margins - deprecated, change to ---------->
	property int sizeLargeMargin: 23;	//marginXL
	property int sizeMainMargin: 10;	//marginM
	property int sizeSmallMargin: 6;	//marginXS

	//NEW
	//Font size
	property int fontSizeBXXXL: 64
	property int fontSizeBXXL: 48
	property int fontSizeBXL: 32
	property int fontSizeXXXL: 24
	property int fontSizeXXL: 20
	property int fontSizeXL: 18
	property int fontSizeL: 16
	property int fontSizeM: 13
	property int fontSizeS: 11
	property int fontSizeXS: 10
	property int fontSizeXXS: 8
	property int fontSizeXXXS: 6

	//Margins
	property int marginXXXL: 64;
	property int marginXXL: 32;
	property int marginXL: 24;
	property int marginL: 16;
	property int marginM: 10;
	property int marginS: 8;
	property int marginXS: 6;

	//SizeHint
	property int sizeHintXXXL: 1000
	property int sizeHintXXL: 700
	property int sizeHintXL: 600
	property int sizeHintL: 500
	property int sizeHintM: 400
	property int sizeHintS: 350
	property int sizeHintXS: 300
	property int sizeHintXXS: 200
	property int sizeHintXXXS: 150
	property int sizeHintBXS: 100


	//Control height
	property int controlHeightL: 40
	property int controlHeightM: 30
	property int controlHeightS: 20

	//radius
	property int radiusXXL: 30
	property int radiusXL: 16
	property int radiusL: 8
	property int radiusM: 4
	property int radiusS: 3
	property int radiusXS: 2

	//spacing
	property int spacingXXXL: 50
	property int spacingXXL: 25
	property int spacingXL: 20
	property int spacingL: 16
	property int spacingM: 10
	property int spacingS: 8
	property int spacingXS: 4

	//Icon sizes
	property int iconSizeXXXL: 128
	property int iconSizeXXL: 64
	property int iconSizeXL: 40
	property int iconSizeL: 32
	property int iconSizeM: 20
	property int iconSizeS: 16
	property int iconSizeXS: 12
	property int iconSizeXXS: 10
	property int iconSizeXXXS: 8

	//Button width
	property int buttonWidthXXXL: 150
	property int buttonWidthXXL: 100
	property int buttonWidthXL: 70
	property int buttonWidthL: 32
	property int buttonWidthM: 24
	property int buttonWidthS: 20
	property int buttonWidthXS: 16
	property int buttonWidthXXS: 8
	property int buttonWidthXXXS: 4

	//Button height
	property int buttonHeightM: 30
	property int buttonHeightS: 22
	property int buttonHeightXS: 20
	property int buttonHeightXXS: 16
	property int buttonHeightXXXS: 10

	property int buttonRadius: 4
	property int buttonBorderWidth: 1

	//Item sizes
	property int itemSizeXXL: 128
	property int itemSizeXL: 64
	property int itemSizeL: 48
	property int itemSizeM: 32
	property int itemSizeS: 16
	property int itemSizeXS: 8

	//Paddings
	property int paddingXL: 64
	property int paddingL: 32
	property int paddingM: 16
	property int paddingS: 8

	//menu row
	property int menuRowHeight: 30

	//table sizes
	property int tableRowHeight: 35
	property int tableHeaderHeight: 35

	//grid cell size
	property int gridCellSize: 110

	//treeView
	property int treeBranchOffset: 35;
	//NEW


	property int mainWindowMargin: 0;
	property int pageMargin: 0;

	//Radius
	property int textFieldRadius: 3;
	property int comboBoxRadius: 3;

	//Other sizes
	property int sizePanelsHeight: 50;
	property int headerHeight: 50;

	//Opacity
	property real opacityFaint: 0.05
	property real opacityHigh: 0.75
	property real opacityLow: 0.25
	property real opacityOverlay: 0.15
	property real selectedOpacity: 0.75
	property real hoverOpacity: selectedOpacity/2
	property real highlightBackgroundOpacity: 0.15

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
	property string firstColor: "#4682B4";
	property string secondColor: "#F18C16";
	property string firstColorHighlight: "#B0C4DE";
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

		//Table
	property string tableHeaderColor: "#E5E5E5";
	property string tableCellColor: "#ffffff";
	property string tableHeaderBorederColor: "#686671";
	property string tableCellBorderColor: "#E5E5E5";
	property string tableHeaderFontColor: "#686671";
	property string tableCellFontColor: "#686671";

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

		if (sourceDecorators.popupDecoratorGql){
			styleContainer.popupDecoratorGql = sourceDecorators.popupDecoratorGql
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
