pragma Singleton
import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Item {
    id: styleContainer;

    property int fontSizeExtraLarge: 64
	property int fontSizeHuge: 32
	property int fontSizeLarge: 24
	property int fontSizeMedium: 12
	property int fontSizeSmall: 8
	property int fontSizeTiny: 4
	property int buttonWidthExtraSmall: 4
	property int buttonWidthLarge: 32
	property int buttonWidthMedium: 24
	property int buttonWidthSmall: 16
	property int buttonWidthTiny: 8
    property int buttonRadius: 2
    property int buttonBorderWidth: 1
	property int horizontalPageMargin: 24
	property int iconSizeExtraSmall: 8
	property int iconSizeSmall: 16
	property int iconSizeMedium: 32
	property int iconSizeLarge: 64
	property int iconSizeExtraLarge: 128
	property int iconSizeLauncher: 100
	property int itemSizeExtraSmall: 8
	property int itemSizeSmall: 16
	property int itemSizeMedium: 32
	property int itemSizeLarge: 48
	property int itemSizeExtraLarge: 64
	property int itemSizeHuge: 128
	property real maximumFlickVelocity: 75
	property real opacityFaint: 0.05
	property real opacityHigh: 0.75
	property real opacityLow: 0.25
	property real opacityOverlay: 0.15
	property int paddingSmall: 8
	property int paddingMedium: 16
	property int paddingLarge: 32
	property int paddingHuge: 64
	property real highlightBackgroundOpacity: 0.15
	property color placeHolderTextColor : '#AAA'
	property color darkPrimaryColor : '#000'
	property color darkSecondaryColor: '#FFF'
	property color errorColor: errorTextColor
	property color highlightBackgroundColor: '#000'
	property color highlightColor: selectedColor
	property color highlightDimmerColor: '#DED'
	property color lightPrimaryColor: '#000'
	property color lightSecondaryColor: '#000'
	property color overlayBackgroundColor: '#000'
	property color primaryColor: mainColor
	property color secondaryColor: secondColor
	property color secondaryHighlightColor: linkColor

	//------------------------------------------for basic components

	property real selectedOpacity: 0.75
	property real hoverOpacity: selectedOpacity/2

    property bool enableAlternating: true;
	property color alternatingColor: '#000'
	property real alternatingOpacity: 0.05
	property bool enableHoverEffect: true

    property int size_indicatorHeight: 50;
    property int size_leftPanelWidth: 250;
    property string color_text_titles: "#7700FF";
    property string color_text_notActive: "#C2CEDB";

    property alias mainFontSource: mainFont.source;
    property alias boldFontSource: boldFont.source;

    property bool mobile;
    property real mainWidth;
    property real mainHeight;
    property real mainSize;

    property string theme: "Light";

    property string fontFamily: mainFont.name;
    property string fontFamilyBold: boldFont.name;

    property string mainColor: "#e6e6e8";

    property string greenColor: "#00ff00";
    property string secondColor: "#F18C16";
    property string titleColor: "#055A85";
    property string subtitleColor: "#6A6E6F";
    property string baseColor: "white";
    property string alternateBaseColor: "#f5f5f5";
    property string backgroundColor: "#e6e6e8";
    property string linkColor: "#0078D7";
    property string selectedColor: "#e3eefa";

    property string color_scrollBackground: styleContainer.baseColor;
    property string color_scrollIndicator: Style.imagingToolsGradient3;
    property string color_scrollHighlight: styleContainer.baseColor;

    property string lightBlueColor: "#69b8e3";

    property string tabSelectedColor: "#FF4500";

    property string errorTextColor: "#FF4500";
    property string shadowColor: "#aaaaaa";

//    property string buttonHoverColor: "#e6e6e8";
//    property string buttonColor: "#dedede";
    property string buttonColor: "#e0e0e0";
    property string buttonBorderColor: "#b0b0b0";
    property string buttonHoverColor: "#d2d2d2";
    property string buttonPressedColor: "#c6c6c6";
    property color inactive_buttonColor: "#C0C0C0";
    property color buttonText: "#335777";
    property color inactive_buttonText: "#335777";

    property string borderColor: "#d0d0d2";
    property string borderFocusColor: "#b0b0b0";
    property string borderColor2: "#a4a4a6";

    property string dialogBackgroundColor: "#808080";

    property string imagingToolsGradient0: "#f6f6f6";
    property string imagingToolsGradient1: "#e3e3e5";
    property string imagingToolsGradient2: "#d4d4d6";
    property string imagingToolsGradient3: "#bebec0";
    property string imagingToolsGradient4: "#b4b4b6";

    property string iconColorOnSelected: "#1a76e7";

    property int fontSize_title: 20;
    property int fontSize_subtitle: 18; // mobile ? 16 :18;
    property int fontSize_common: 13; // mobile ? 14 :16;
    property int fontSize_small: 11;

    property string hover: "#e6e6e8";

    property string textColor: "#335777";
    property string textSelected: "#0078d7";
    property color inactive_textColor: "#aaaaaa";

    property string disabledInActiveTextColor: "#c0c0c0";

    property int margin: 13;

    property string menuPanelBackgroundColor: "#e2e2e8";

    function getImageSource (name, styleTheme, buttonState, buttonMode)
    {
        var imageSource = "Icons/" + styleTheme + "/" + name + "_" + buttonState + "_" + buttonMode + ".svg";
        return imageSource;
    }

    FontLoader{
        id: mainFont;
        source: "../../Fonts/Ubuntu-Light.ttf";
    }

    FontLoader{
        id: boldFont;
        source: "../../Fonts/Ubuntu-Medium.ttf";
    }

    function getDesignScheme(theme){
        console.log("PreferenceDialog callGetStyleQuery", theme);
        styleQuery.getStyle(theme);
    }

    function getThemeColor(colorType, colorKey, themeType){
        var colorPalette = themeType.GetData("Style").GetData(colorType).GetData(colorKey);
        return themeType.GetData("ColorPalette").GetData(colorPalette);
    }

    property string menuButtonDecoratorPath: "MenuPanelButtonDecorator.qml";
    property string subMenuButtonDecoratorPath;
    property string tabPanelDecoratorPath: "TabPanelDecorator.qml";
    property string topPanelDecoratorPath: "TopPanelDecorator.qml";

    property string filterPanelDecoratorPath: "FilterPanelDecorator.qml";

    property string topLeftPanelDecoratorPath;
    property string topCenterPanelDecoratorPath: "TopCenterPanelDecorator.qml";
    property string topRightPanelDecoratorPath: "TopRightPanelDecorator.qml";
    property string customPanelDecoratorPath;

    function getDecorator(decorators, section, type){
        let dataModelLocal = decorators.GetData("Decorators");
        if (dataModelLocal.ContainsKey(section)){
            dataModelLocal = dataModelLocal.GetData(section);
            if (dataModelLocal.ContainsKey(type)){
                return dataModelLocal.GetData(type);
            }
        }
    }

    property Component topButtonDecorator: topButtonDecoratorComp

    Component {
        id: topButtonDecoratorComp
        TopButtonDecoratorNew{

        }
    }

    function parseStyleTheme(themeType){
        let dataSource = themeType.GetData("source");
        Style.borderColor = styleContainer.getThemeColor("ActiveColors", "BorderColor", dataSource);
        Style.baseColor = styleContainer.getThemeColor("ActiveColors", "Base", dataSource);
        Style.alternateBaseColor = styleContainer.getThemeColor("ActiveColors", "AlternateBase", dataSource);
        Style.backgroundColor = styleContainer.getThemeColor("ActiveColors", "Background", dataSource);

        Style.textColor = styleContainer.getThemeColor("ActiveColors", "Text", dataSource);
        Style.textSelected = styleContainer.getThemeColor("ActiveColors", "TextSelectedBackground", dataSource);
        Style.inactive_textColor = styleContainer.getThemeColor("InactiveColors", "Text", dataSource);

        Style.selectedColor = styleContainer.getThemeColor("ActiveColors", "ItemSelected", dataSource);

//        Style.buttonColor = styleContainer.getThemeColor("ActiveColors", "Button", dataSource);
//        Style.buttonBorderColor = styleContainer.getThemeColor("ActiveColors", "ButtonBorder", dataSource);
        Style.inactive_buttonColor = styleContainer.getThemeColor("InactiveColors", "Button", dataSource);
        Style.buttonText = styleContainer.getThemeColor("ActiveColors", "ButtonText", dataSource);
        Style.inactive_buttonText = styleContainer.getThemeColor("InactiveColors", "ButtonText", dataSource);

        Style.disabledInActiveTextColor = styleContainer.getThemeColor("DisabledInActiveColors", "Text", dataSource);

        Style.hover = styleContainer.getThemeColor("ActiveColors", "Hover", dataSource);
//        Style.buttonHoverColor = styleContainer.getThemeColor("ActiveColors", "Hover", dataSource);
//        Style.buttonPressedColor = styleContainer.getThemeColor("ActiveColors", "BackgroundSelected", dataSource);

//        Style.color_scrollBackground = styleContainer.getThemeColor("ActiveColors", "ScrollBackground", dataSource);
//        Style.color_scrollIndicator = styleContainer.getThemeColor("ActiveColors", "ScrollIndicator", dataSource);
//        Style.color_scrollHighlight = styleContainer.getThemeColor("ActiveColors", "ScrollHighlight", dataSource);

        Style.imagingToolsGradient0 = dataSource.GetData("ColorPalette").GetData("ImagingToolsGradient0");
        Style.imagingToolsGradient1 = dataSource.GetData("ColorPalette").GetData("ImagingToolsGradient1");
        Style.imagingToolsGradient2 = dataSource.GetData("ColorPalette").GetData("ImagingToolsGradient2");
        Style.imagingToolsGradient3 = dataSource.GetData("ColorPalette").GetData("ImagingToolsGradient3");
        Style.imagingToolsGradient4 = dataSource.GetData("ColorPalette").GetData("ImagingToolsGradient4");
        Style.greenColor = dataSource.GetData("ColorPalette").GetData("Green");

        Style.iconColorOnSelected = styleContainer.getThemeColor("IconColor", "OnSelected", dataSource);
        Style.tabSelectedColor = styleContainer.getThemeColor("ActiveColors", "TabSelected", dataSource);
        Style.errorTextColor = styleContainer.getThemeColor("ActiveColors", "ErrorText", dataSource);

        Style.shadowColor = styleContainer.getThemeColor("ActiveColors", "Shadow", dataSource);

        let dataDecorators = themeType.GetData("decorators");

        console.log('DEBUG::dataDecorators', dataDecorators)

        Style.menuButtonDecoratorPath = styleContainer.getDecorator(dataDecorators, "MenuPanel", "MainButton");
        Style.subMenuButtonDecoratorPath = styleContainer.getDecorator(dataDecorators, "MenuPanel", "SubButton");
        Style.tabPanelDecoratorPath = styleContainer.getDecorator(dataDecorators, "TabPanel", "Base");
        Style.topPanelDecoratorPath = styleContainer.getDecorator(dataDecorators, "TopPanel", "Base");
        Style.filterPanelDecoratorPath = styleContainer.getDecorator(dataDecorators,"FilterPanel", "Base");
        Style.topCenterPanelDecoratorPath = styleContainer.getDecorator(dataDecorators, "TopPanel", "CenterPanel");
        Style.topRightPanelDecoratorPath = styleContainer.getDecorator(dataDecorators, "TopPanel", "RightPanel");
        Style.topLeftPanelDecoratorPath = styleContainer.getDecorator(dataDecorators, "TopPanel", "LeftPanel");
        Style.customPanelDecoratorPath = styleContainer.getDecorator(dataDecorators, "CustomPanel", "Base");
    }

    GqlModel {
        id: styleQuery;

        function getStyle(theme){
            var query = Gql.GqlRequest("query", "GetStyle");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("theme", theme);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("style");
            queryFields.InsertField("theme", theme);
            queryFields.InsertField("source", "source");
            queryFields.InsertField("test", "test");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("styleQuery gqlData", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, styleQuery);
            console.log("AAAAA");
            if (this.state === "Ready") {
                console.log("BBBBB");
                var dataModelLocal;

                if (styleQuery.ContainsKey("errors")){
                    return;
                }

                console.log("styleQuery");
                if (styleQuery.ContainsKey("data")){
                    dataModelLocal = styleQuery.GetData("data");

                    console.log("data");

                    if(dataModelLocal.ContainsKey("GetStyle")) {
                        console.log("GetStyle");
                        dataModelLocal = dataModelLocal.GetData("GetStyle");
                    }

                    if (dataModelLocal.ContainsKey("theme")){
                        console.log("theme");
                        styleContainer.theme = dataModelLocal.GetData("theme");
                    }

                    if(dataModelLocal.ContainsKey("source")){
                        console.log("source");
                        styleContainer.parseStyleTheme(dataModelLocal);
                    }
                }
            }
        }
    }
}
