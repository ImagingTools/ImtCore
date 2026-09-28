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
	property string fontFamily: "Segoe UI";
	property string fontFamilyBold: "Segoe UI";
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
	property int fontSizeXXXXLarge: 40		//fontSizeBXL
	property int fontSizeXXXLarge: 32		//fontSizeXXXL
	property int fontSizeXXLarge: 24		//fontSizeXXL
	property int fontSizeXLarge: 20			//fontSizeXL
	property int fontSizeLarge: 16			//fontSizeL
	property int fontSizeNormal: 14			//fontSizeM
	property int fontSizeSmall: 12			//fontSizeS
	property int fontSizeXSmall: 11			//fontSizeXS
	property int fontSizeXXSmall: 10			//fontSizeXXS
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
	property int fontSizeBXL: 40
	property int fontSizeXXXL: 32
	property int fontSizeXXL: 24
	property int fontSizeXL: 20
	property int fontSizeL: 16
	property int fontSizeM: 14
	property int fontSizeS: 12
	property int fontSizeXS: 11
	property int fontSizeXXS: 10
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
	property int controlHeightL: 40
	property int controlHeightM: 32
	property int controlHeightS: 24

	//radius - Primer keeps a three step scale: small, medium (the default) and
	//large. Anything outside it reads as a different design language, so the
	//wider names are kept as aliases only, for screens and product styles that
	//already reference them.
	property int radiusL: 12
	property int radiusM: 6
	property int radiusS: 3
	//deprecated
	property int radiusXXL: radiusL
	property int radiusXL: radiusL
	property int radiusXS: radiusS

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
	property int buttonHeightS: 28
	property int buttonHeightXS: 24
	property int buttonHeightXXS: 16
	property int buttonHeightXXXS: 10

	property int buttonRadius: 6
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
	property int menuRowHeight: 32
	property int menuMinWidth: 200

	//table sizes
	property int tableRowHeight: controlHeightL + marginXS
	property int tableHeaderHeight: 40

	//grid cell size
	property int gridCellSize: 110

	//treeView
	property int treeBranchOffset: 35;
	//NEW


	property int mainWindowMargin: 0;
	property int pageMargin: 0;

	//Radius
	property int textFieldRadius: 6;
	property int comboBoxRadius: 6;
	property int menuPanelRadius: radiusL;

	//Other sizes
	property int sizePanelsHeight: 50;
	property int headerHeight: 50;
	//Octicons are drawn on a 16/32/64 grid and the chrome uses the smallest one.
	property int menuPanelIconSize: iconSizeS;

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
	property string imagingToolsGradient1: "#f6f8fa";
	property string imagingToolsGradient2: "#eaeef2";
	property string imagingToolsGradient3: "#d1d9e0";
	property string imagingToolsGradient4: "#afb8c1";


	//Colors
		//main
	property string baseColor: "#ffffff";
	// A true inversion of baseColor: near-black on the light theme, near-white
	// on the dark one. Do not use it as a surface color.
	property string baseColorInverted: styleContainer.textColor;
	property string mainColor: styleContainer.neutralSubtleColor;
	property string firstColor: styleContainer.linkColor;
	property string secondColor: styleContainer.attentionColor;
	property string firstColorHighlight: styleContainer.selectedColor;
	// Kept as an alias so styles that still assign it keep working; it used to
	// hold its own value and drift away from menuPanelBackgroundColor.
	property string colorMenuPanel: menuPanelBackgroundColor;

		//error
	property string errorColor: errorTextColor
	property string errorTextColor: "#d1242f";

		//accent
	property string imaginToolsAccentColor: "#0969da"
	property string positiveAccentColor: styleContainer.successColor;
	property string negativeAccentColor: styleContainer.dangerColor;
	property string middleAccentColor: styleContainer.attentionColor;

		//semantic status colors
	property string successColor: "#1a7f37";
	property string successSubtleColor: "#dafbe1";
	property string attentionColor: "#9a6700";
	property string attentionSubtleColor: "#fff8c5";
	property string dangerColor: "#d1242f";
	property string dangerSubtleColor: "#ffebe9";
	property string doneColor: "#8250df";
	property string doneSubtleColor: "#fbefff";
	property string severeColor: "#bc4c00";
	property string neutralEmphasisColor: "#6e7781";
	property string neutralSubtleColor: "#eaeef2";

		//text
	property string textColor: "#1f2328";
	property string titleColor: "#1f2328";
	property string subtitleColor: "#59636e";
	property string textSelectedColor: "#0969da";
	property string inactiveTextColor: "#818b98";
	property string placeHolderTextColor : "#6e7781"
	property string linkColor: "#0969da";
	property string selectedLinkFromColor: styleContainer.linkColor;
	property string selectedLinkToColor: styleContainer.successColor;

		//popup / toast (PopupManager → PopupContainer)
	property string popupErrorBackgroundColor: "#ffebe9";
	property string popupWarningBackgroundColor: "#fff8c5";
	property string popupSuccessBackgroundColor: "#dafbe1";
	property string popupInfoBackgroundColor: "#ddf4ff";
	property string popupErrorAccentColor: errorTextColor;
	property string popupWarningAccentColor: secondColor;
	property string popupSuccessAccentColor: selectedLinkToColor;
	property string popupInfoAccentColor: linkColor;
	property string popupCloseHoverColor: styleContainer.menuPanelItemHoverColor;
	property string popupCloseIdleColor: "#00000000";
	property real popupIconChipOpacity: 0.14;

		//border
	property string borderColor: "#d1d9e0";
	property string borderColor2: "#afb8c1";

		//background
	property string backgroundColor: "#f6f8fa";
	property string backgroundColor2: "#ffffff";

		//chrome: left menu panel (GitHub sidebar / NavList). Inset as a card on
		//the page canvas, so it takes the elevated surface rather than the muted
		//one; selected and hovered rows are neutral washes and the accent is
		//carried by the bar alone.
	property string menuPanelBackgroundColor: "#ffffff";
	property string menuPanelItemSelectedColor: "#26818b98";
	property string menuPanelItemHoverColor: "#1a818b98";

		//chrome: top panel. GitHub's own header is a dark bar, but ours carries
		//theme-colored icons that would vanish on one, so the panel stays on the
		//page surface and is separated by a divider instead.
	property string topPanelBackgroundColor: "#ffffff";
	property string topPanelBorderColor: "#d1d9e0";

	property string dialogBackgroundColor: "#ffffff";
		//chrome: modal backdrop. The base color only; callers apply their own
		//opacity on top of it.
	property string overlayBackgroundColor: "#c8d1da";
	property string tabPanelBackgroundColor: backgroundColor;

		//highlight
	property string highlightBackgroundColor: '#000'
	property string highlightColor: selectedColor
	property string highlightDimmerColor: styleContainer.neutralSubtleColor
	property string secondaryHighlightColor: linkColor

		//selected
	property string selectedColor: "#ddf4ff";
	property string tabSelectedColor: "#fd8c73";
	property string iconColorOnSelected: "#0969da";

		//button
	property string buttonColor: "#f6f8fa";
	property string buttonBorderColor: "#d1d9e0";
	property string buttonHoverColor: "#eff2f5";
	property string buttonPressedColor: "#e6eaef";
	property string buttonAccentColor: imaginToolsAccentColor;
	property string buttonInactiveColor: "#eff2f5";
	property string buttonBorderFocusColor: imaginToolsAccentColor;
	property string buttonBorderInactiveColor: "#1a818b98";
	property string buttonTextColor: "#1f2328";
	property string buttonInactiveTextColor: "#818b98";
	// Text drawn on top of an accent-colored surface (e.g. a selected/active chip
	// or badge) - not simply "the opposite of textColor", since an accent fill's
	// own brightness doesn't track baseColor/backgroundColor at all.
	property string highlightedTextColor: "#ffffff";

		//button variants ("primary" / "danger"), see ButtonDecorator.variant
	property string buttonPrimaryColor: "#1f883d";
	property string buttonPrimaryHoverColor: "#1a7f37";
	property string buttonPrimaryPressedColor: "#197935";
	property string buttonPrimaryBorderColor: "#1f883d";
	property string buttonPrimaryTextColor: "#ffffff";
	property string buttonDangerColor: "#cf222e";
	property string buttonDangerHoverColor: "#a40e26";
	property string buttonDangerPressedColor: "#8b0820";
	property string buttonDangerBorderColor: "#cf222e";
	property string buttonDangerTextColor: "#ffffff";

		//focus ring drawn outside the control border
	property string focusRingColor: imaginToolsAccentColor;
	property int focusRingWidth: 2;

		//Table
	property string tableHeaderColor: "#f6f8fa";
	property string tableCellColor: "#ffffff";
	property string tableHeaderBorederColor: "#d1d9e0";
	property string tableCellBorderColor: "#d1d9e0";
	property string tableHeaderFontColor: "#59636e";
	property string tableCellFontColor: "#1f2328";

		//scroll
	property string scrollBackgroundColor: styleContainer.backgroundColor2;
	property string scrollIndicatorColor: styleContainer.borderColor2;
	property string scrollHighlightColor: styleContainer.backgroundColor;

		//shadow, hover
	property string shadowColor: "#d1d9e0";
	property string hover: "#eff2f5";

		//Alternating
	property color alternatingColor: '#000'
	property string alternateBaseColor: "#f6f8fa";

		//simple colors
	property string grayColor: styleContainer.borderColor2;
	property string greenColor: "#1a7f37";
	property string lightBlueColor: "#54aeff";

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

			// Icons are a resource directory named after the style ("/Light/Icons",
			// "/Dark/Icons"), so getIconPath() only finds the right set when the
			// theme name follows the scheme that was just loaded.
			if (dataSource.containsKey("Style")){
				let styleModel = dataSource.getData("Style");
				if (styleModel.containsKey("Name")){
					styleContainer.theme = styleModel.getData("Name");
				}
			}

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

			// Disabled state: InactiveColors holds the muted text color, not the disabled one
			let textDisabledColor = styleContainer.getThemeColor("ActiveColors", "TextDisabled", dataSource);
			if (textDisabledColor !== ""){
				styleContainer.inactiveTextColor = textDisabledColor;
				styleContainer.buttonInactiveTextColor = textDisabledColor;
			}

			let buttonDisabledColor = styleContainer.getThemeColor("ActiveColors", "ButtonDisabled", dataSource);
			if (buttonDisabledColor !== ""){
				styleContainer.buttonInactiveColor = buttonDisabledColor;
			}

			let buttonBorderDisabledColor = styleContainer.getThemeColor("ActiveColors", "ButtonBorderDisabled", dataSource);
			if (buttonBorderDisabledColor !== ""){
				styleContainer.buttonBorderInactiveColor = buttonBorderDisabledColor;
			}

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
			styleContainer.lightBlueColor = styleContainer.getThemeColor("ActiveColors", "BlueLabel", dataSource);
			styleContainer.titleColor = styleContainer.getThemeColor("ActiveColors", "Text", dataSource);
			styleContainer.subtitleColor = styleContainer.getThemeColor("InactiveColors", "Text", dataSource);
			styleContainer.placeHolderTextColor = styleContainer.getThemeColor("InactiveColors", "Text", dataSource);
			styleContainer.menuPanelBackgroundColor = styleContainer.getThemeColor("ActiveColors", "MenuPanelBackground", dataSource);
			styleContainer.menuPanelItemSelectedColor = styleContainer.getThemeColor("ActiveColors", "MenuPanelItemSelected", dataSource);
			styleContainer.menuPanelItemHoverColor = styleContainer.getThemeColor("ActiveColors", "MenuPanelItemHover", dataSource);
			styleContainer.topPanelBackgroundColor = styleContainer.getThemeColor("ActiveColors", "TopPanelBackground", dataSource);
			styleContainer.topPanelBorderColor = styleContainer.getThemeColor("ActiveColors", "TopPanelBorder", dataSource);
			styleContainer.overlayBackgroundColor = styleContainer.getThemeColor("ActiveColors", "OverlayBackground", dataSource);
			styleContainer.dialogBackgroundColor = styleContainer.getThemeColor("ActiveColors", "DialogBackground", dataSource);
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

			let focusRingThemeColor = styleContainer.getThemeColor("ActiveColors", "FocusRing", dataSource);
			if (focusRingThemeColor !== ""){
				styleContainer.focusRingColor = focusRingThemeColor;
			}

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
