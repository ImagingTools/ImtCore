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
	property string logoIconName;
	property string language;//deprecated?

	//Font properties
	property string mainFontSource: "../../../Fonts/Ubuntu-Light.ttf";
	property string boldFontSource: "../../../Fonts/Ubuntu-Medium.ttf";
	property string fontFamily: "Arial";
	property string fontFamilyBold: "Arial";
	property string fontFamilyMono: "Consolas";

	property alias mainFontLoader: mainFont;
	property alias boldFontLoader: boldFont;

	//Start window sizes
	property int startWindowWidth: 800;
	property int startWindowHeight: 600;


	//deprecated, will be romoved after refactoring
	//Font size - deprecated,change to --------------->
	property int fontSizeXHuge: 64			//fontSizeBXXXL
	property int fontSizeHuge: 48			//fontSizeBXXL
	property int fontSizeXXXXLarge: 34		//fontSizeBXL
	property int fontSizeXXXLarge: 28		//fontSizeXXXL
	property int fontSizeXXLarge: 22		//fontSizeXXL
	property int fontSizeXLarge: 20			//fontSizeXL
	property int fontSizeLarge: 16			//fontSizeL
	property int fontSizeNormal: 13			//fontSizeM
	property int fontSizeSmall: 11			//fontSizeS
	property int fontSizeXSmall: 10			//fontSizeXS
	property int fontSizeXXSmall: 9			//fontSizeXXS
	property int fontSizeTiny: 6			//fontSizeXXXS


	//Button sizes - deprecated, change to ---------->
	property int buttonWidthLarge: 32		//buttonWidthL
	property int buttonWidthMedium: 24		//buttonWidthM
	property int buttonWidthSmall: 16		//buttonWidthXS
	property int buttonWidthXSmall: 8		//buttonWidthXXS
	property int buttonWidthTiny: 4			//buttonWidthXXXS

	property int buttonHeight: 32 //  deprecated, change to ----->buttonHeightM

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
	property int fontSizeBXL: 34
	property int fontSizeXXXL: 28
	property int fontSizeXXL: 22
	property int fontSizeXL: 20
	property int fontSizeL: 16
	property int fontSizeM: 13
	property int fontSizeS: 11
	property int fontSizeXS: 10
	property int fontSizeXXS: 9
	property int fontSizeXXXS: 6

	//Margins
	property int marginXXXL: 64;
	property int marginXXL: 32;
	property int marginXL: 24;
	property int marginL: 16;
	property int marginM: 10;
	property int marginS: 8;
	property int marginXS: 6;
	property int marginXXS: 4;
	property int marginXXXS: 2;

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
	property int controlHeightL: 44
	property int controlHeightM: 32
	property int controlHeightS: 28

	//radius
	property int radiusXXL: 30
	property int radiusXL: 22
	property int radiusL: 16
	property int radiusM: 10
	property int radiusS: 6
	property int radiusXS: 4

	//spacing
	property int spacingXXXL: 50
	property int spacingXXL: 25
	property int spacingXL: 20
	property int spacingL: 16
	property int spacingM: 10
	property int spacingS: 8
	property int spacingXS: 4
	property int spacingXXS: 2

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
	property int buttonHeightM: 32
	property int buttonHeightS: 30
	property int buttonHeightXS: 28
	property int buttonHeightXXS: 16
	property int buttonHeightXXXS: 10

	property int buttonRadius: 10
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
	property int menuRowHeight: 34
	property int menuMinWidth: 200

	//table sizes
	property int tableRowHeight: controlHeightL + marginXS
	property int tableHeaderHeight: 36

	//grid cell size
	property int gridCellSize: 110

	//treeView
	property int treeBranchOffset: 35;
	//NEW


	property int mainWindowMargin: 0;
	property int pageMargin: 0;

	//Radius
	property int textFieldRadius: 10;
	property int comboBoxRadius: 10;
	property int menuPanelRadius: radiusL;

	//Other sizes
	property int sizePanelsHeight: 50;
	property int headerHeight: 50;
	property int menuPanelIconSize: iconSizeM;

	property int menuPanelMinWidth: 2 * marginL + menuPanelIconSize;
	property int menuPanelWidth: sizeHintXXS;
	property int menuPanelAutoCollapseWidth: sizeHintXXL;
	//Upper bound for centered, readability-limited page content (lists, forms).
	property int contentWidthMax: 1000;

	//Opacity
	property real opacityFaint: 0.05
	property real opacityHigh: 0.75
	property real opacityLow: 0.25
	property real opacityOverlay: 0.15
	property real selectedOpacity: 0.75
	property real hoverOpacity: selectedOpacity/2
	property real highlightBackgroundOpacity: 0.15

	//Alternating
	// Off by default: rows are separated by borders, not by zebra striping.
	property bool enableAlternating: false;
	property real alternatingOpacity: 0.05

	//Gradient
	property string imagingToolsGradient0: "#ffffff";
	property string imagingToolsGradient1: "#f2f2f7";
	property string imagingToolsGradient2: "#e5e5ea";
	property string imagingToolsGradient3: "#d1d1d6";
	property string imagingToolsGradient4: "#aeaeb2";


	//Colors
		//main
	property string baseColor: "#ffffff";
	property string baseColorInverted: "#000000";
	property string mainColor: "#e5e5ea";
	property string firstColor: "#007aff";
	property string secondColor: "#ff9500";
	property string firstColorHighlight: "#d9ebff";
	property string colorMenuPanel: baseColor;

		//error
	property string errorColor: errorTextColor
	property string errorTextColor: "#ff3b30";

		//accent
	property string imaginToolsAccentColor: "#007aff"
	property string positiveAccentColor: "#34c759";
	property string negativeAccentColor: "#ff3b30";
	property string middleAccentColor: "#ff9500";

		//semantic status colors
	property string successColor: "#34c759";
	property string successSubtleColor: "#e0f7e6";
	property string attentionColor: "#ff9500";
	property string attentionSubtleColor: "#ffefd9";
	property string dangerColor: "#ff3b30";
	property string dangerSubtleColor: "#ffe2e0";
	property string doneColor: "#af52de";
	property string doneSubtleColor: "#f3e5fa";
	property string severeColor: "#ff2d55";
	property string neutralEmphasisColor: "#8e8e93";
	property string neutralSubtleColor: "#e5e5ea";

		//text
	property string textColor: "#000000";
	property string titleColor: "#000000";
	property string subtitleColor: "#8a8a8e";
	property string textSelectedColor: "#007aff";
	property string inactiveTextColor: "#aeaeb2";
	property string placeHolderTextColor : "#8a8a8e"
	property string linkColor: "#007aff";
	property string selectedLinkFromColor: "#007aff";
	property string selectedLinkToColor: "#34c759";

		//popup / toast (PopupManager → PopupContainer)
	property string popupErrorBackgroundColor: "#ffe2e0";
	property string popupWarningBackgroundColor: "#ffefd9";
	property string popupSuccessBackgroundColor: "#e0f7e6";
	property string popupInfoBackgroundColor: "#d9ebff";
	property string popupErrorAccentColor: errorTextColor;
	property string popupWarningAccentColor: secondColor;
	property string popupSuccessAccentColor: selectedLinkToColor;
	property string popupInfoAccentColor: linkColor;
	property string popupCloseHoverColor: "#1A000000";
	property string popupCloseIdleColor: "#00000000";
	property real popupIconChipOpacity: 0.14;

		//border
	property string borderColor: "#c6c6c8";
	property string borderColor2: "#aeaeb2";

		//background
	property string backgroundColor: "#f2f2f7";
	property string backgroundColor2: "#ffffff";
	property string menuPanelBackgroundColor: "#f2f2f7";

	property string dialogBackgroundColor: "#ffffff";
	property string overlayBackgroundColor: '#000'
	property string tabPanelBackgroundColor: backgroundColor;

		//highlight
	property string highlightBackgroundColor: '#000'
	property string highlightColor: selectedColor
	property string highlightDimmerColor: "#e5e5ea"
	property string secondaryHighlightColor: linkColor

		//selected
	property string selectedColor: "#d9ebff";
	property string tabSelectedColor: "#007aff";
	property string iconColorOnSelected: "#007aff";

		//button
	property string buttonColor: "#ffffff";
	property string buttonBorderColor: "#d1d1d6";
	property string buttonHoverColor: "#f2f2f7";
	property string buttonPressedColor: "#e5e5ea";
	property string buttonAccentColor: imaginToolsAccentColor;
	property string buttonInactiveColor: "#f2f2f7";
	property string buttonBorderFocusColor: imaginToolsAccentColor;
	property string buttonBorderInactiveColor: "#d1d1d6";
	property string buttonTextColor: "#000000";
	property string buttonInactiveTextColor: "#aeaeb2";
	// Text drawn on top of an accent-colored surface (e.g. a selected/active chip
	// or badge) - not simply "the opposite of textColor", since an accent fill's
	// own brightness doesn't track baseColor/backgroundColor at all.
	property string highlightedTextColor: "#ffffff";

		//button variants ("primary" / "danger"), see ButtonDecorator.variant
	property string buttonPrimaryColor: "#007aff";
	property string buttonPrimaryHoverColor: "#0071eb";
	property string buttonPrimaryPressedColor: "#0062cc";
	property string buttonPrimaryBorderColor: "#007aff";
	property string buttonPrimaryTextColor: "#ffffff";
	property string buttonDangerColor: "#ff3b30";
	property string buttonDangerHoverColor: "#eb3429";
	property string buttonDangerPressedColor: "#d32e24";
	property string buttonDangerBorderColor: "#ff3b30";
	property string buttonDangerTextColor: "#ffffff";

		//focus ring drawn outside the control border
	property string focusRingColor: imaginToolsAccentColor;
	property int focusRingWidth: 3;

		//Table
	property string tableHeaderColor: "#f2f2f7";
	property string tableCellColor: "#ffffff";
	property string tableHeaderBorederColor: "#c6c6c8";
	property string tableCellBorderColor: "#c6c6c8";
	property string tableHeaderFontColor: "#8a8a8e";
	property string tableCellFontColor: "#000000";

		//scroll
	property string scrollBackgroundColor: styleContainer.backgroundColor2;
	property string scrollIndicatorColor: styleContainer.borderColor2;
	property string scrollHighlightColor: styleContainer.backgroundColor;

		//shadow, hover
	property string shadowColor: "#c6c6c8";
	property string hover: "#f2f2f7";

		//Alternating
	property color alternatingColor: '#000'
	property string alternateBaseColor: "#f2f2f7";

		//simple colors
	property string grayColor: "#aeaeb2";
	property string greenColor: "#34c759";
	property string lightBlueColor: "#5ac8fa";

	//other parameters
	property real maximumFlickVelocity: 75
	property bool enableHoverEffect: true
	property bool enableMenuPanelCollapse: true


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

	function getLogoIconPath(state, mode, extention){
		let iconName = styleContainer.logoIconName !== "" ? styleContainer.logoIconName : "Icons/AppIcon";
		return styleContainer.getIconPath(iconName, state, mode, extention);
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
		if (sourceDecorators.menuPanelDecorator){
			styleContainer.menuPanelDecorator = sourceDecorators.menuPanelDecorator
		}
		if (sourceDecorators.filterPanelDecorator){
			styleContainer.filterPanelDecorator = sourceDecorators.filterPanelDecorator
		}
		if (sourceDecorators.metaInfoPanelDecorator){
			styleContainer.metaInfoPanelDecorator = sourceDecorators.metaInfoPanelDecorator
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
			styleContainer.buttonBorderInactiveColor = styleContainer.getThemeColor("InactiveColors", "ButtonBorder", dataSource);
			styleContainer.buttonTextColor = styleContainer.getThemeColor("ActiveColors", "ButtonText", dataSource);
			styleContainer.buttonInactiveTextColor = styleContainer.getThemeColor("InactiveColors", "ButtonText", dataSource);
			styleContainer.highlightedTextColor = styleContainer.getThemeColor("ActiveColors", "HighlightedText", dataSource);

			styleContainer.inactiveTextColor = styleContainer.getThemeColor("InactiveColors", "Text", dataSource);

			styleContainer.buttonHoverColor = styleContainer.getThemeColor("ActiveColors", "Midlight", dataSource);
			styleContainer.buttonPressedColor = styleContainer.getThemeColor("ActiveColors", "Mid", dataSource);
			styleContainer.hover = styleContainer.getThemeColor("ActiveColors", "Midlight", dataSource);

			styleContainer.imagingToolsGradient0 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient0");
			styleContainer.imagingToolsGradient1 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient1");
			styleContainer.imagingToolsGradient2 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient2");
			styleContainer.imagingToolsGradient3 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient3");
			styleContainer.imagingToolsGradient4 = dataSource.getData("ColorPalette").getData("ImagingToolsGradient4");
			styleContainer.greenColor = dataSource.getData("ColorPalette").getData("Green");

			styleContainer.iconColorOnSelected = styleContainer.getThemeColor("ActiveColors", "Highlight", dataSource);

			styleContainer.tabSelectedColor = styleContainer.getThemeColor("ActiveColors", "TabSelected", dataSource);
			styleContainer.errorTextColor = styleContainer.getThemeColor("ActiveColors", "ErrorText", dataSource);

			styleContainer.popupErrorBackgroundColor = styleContainer.getThemeColor("ActiveColors", "PopupErrorBackground", dataSource);
			styleContainer.popupWarningBackgroundColor = styleContainer.getThemeColor("ActiveColors", "PopupWarningBackground", dataSource);
			styleContainer.popupSuccessBackgroundColor = styleContainer.getThemeColor("ActiveColors", "PopupSuccessBackground", dataSource);
			styleContainer.popupInfoBackgroundColor = styleContainer.getThemeColor("ActiveColors", "PopupInfoBackground", dataSource);

			styleContainer.successColor = styleContainer.getThemeColor("ActiveColors", "Success", dataSource);
			styleContainer.successSubtleColor = styleContainer.getThemeColor("ActiveColors", "SuccessSubtle", dataSource);
			styleContainer.attentionColor = styleContainer.getThemeColor("ActiveColors", "Attention", dataSource);
			styleContainer.attentionSubtleColor = styleContainer.getThemeColor("ActiveColors", "AttentionSubtle", dataSource);
			styleContainer.dangerColor = styleContainer.getThemeColor("ActiveColors", "Danger", dataSource);
			styleContainer.dangerSubtleColor = styleContainer.getThemeColor("ActiveColors", "DangerSubtle", dataSource);
			styleContainer.doneColor = styleContainer.getThemeColor("ActiveColors", "Done", dataSource);
			styleContainer.doneSubtleColor = styleContainer.getThemeColor("ActiveColors", "DoneSubtle", dataSource);
			styleContainer.severeColor = styleContainer.getThemeColor("ActiveColors", "Severe", dataSource);
			styleContainer.neutralEmphasisColor = styleContainer.getThemeColor("ActiveColors", "NeutralEmphasis", dataSource);
			styleContainer.neutralSubtleColor = styleContainer.getThemeColor("ActiveColors", "NeutralSubtle", dataSource);

			styleContainer.linkColor = styleContainer.getThemeColor("ActiveColors", "Link", dataSource);
			styleContainer.titleColor = styleContainer.getThemeColor("ActiveColors", "Text", dataSource);
			styleContainer.subtitleColor = styleContainer.getThemeColor("InactiveColors", "Text", dataSource);
			styleContainer.placeHolderTextColor = styleContainer.getThemeColor("InactiveColors", "Text", dataSource);
			styleContainer.menuPanelBackgroundColor = styleContainer.getThemeColor("ActiveColors", "Background", dataSource);
			styleContainer.borderColor2 = styleContainer.getThemeColor("ActiveColors", "ScrollIndicator", dataSource);

			styleContainer.tableHeaderColor = styleContainer.getThemeColor("ActiveColors", "HeaderSection", dataSource);
			styleContainer.tableHeaderBorederColor = styleContainer.getThemeColor("ActiveColors", "HeaderBorder", dataSource);
			styleContainer.tableHeaderFontColor = styleContainer.getThemeColor("InactiveColors", "Text", dataSource);
			styleContainer.tableCellColor = styleContainer.getThemeColor("ActiveColors", "Base", dataSource);
			styleContainer.tableCellBorderColor = styleContainer.getThemeColor("ActiveColors", "BorderColor", dataSource);
			styleContainer.tableCellFontColor = styleContainer.getThemeColor("ActiveColors", "Text", dataSource);

			styleContainer.buttonPrimaryColor = styleContainer.getThemeColor("ActiveColors", "ButtonPrimary", dataSource);
			styleContainer.buttonPrimaryHoverColor = styleContainer.getThemeColor("ActiveColors", "ButtonPrimaryHover", dataSource);
			styleContainer.buttonPrimaryPressedColor = styleContainer.getThemeColor("ActiveColors", "ButtonPrimaryPressed", dataSource);
			styleContainer.buttonPrimaryBorderColor = styleContainer.getThemeColor("ActiveColors", "ButtonPrimary", dataSource);
			styleContainer.buttonPrimaryTextColor = styleContainer.getThemeColor("ActiveColors", "HighlightedText", dataSource);
			styleContainer.buttonDangerColor = styleContainer.getThemeColor("ActiveColors", "ButtonDanger", dataSource);
			styleContainer.buttonDangerHoverColor = styleContainer.getThemeColor("ActiveColors", "ButtonDangerHover", dataSource);
			styleContainer.buttonDangerPressedColor = styleContainer.getThemeColor("ActiveColors", "ButtonDangerPressed", dataSource);
			styleContainer.buttonDangerBorderColor = styleContainer.getThemeColor("ActiveColors", "ButtonDanger", dataSource);
			styleContainer.buttonDangerTextColor = styleContainer.getThemeColor("ActiveColors", "HighlightedText", dataSource);
			styleContainer.focusRingColor = styleContainer.getThemeColor("ActiveColors", "Highlight", dataSource);

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
