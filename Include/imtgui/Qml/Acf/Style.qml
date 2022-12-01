pragma Singleton
import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: styleContainer;

    property int size_indicatorHeight: 50;
    property int size_leftPanelWidth: 250;
    property string color_text_titles: "#7700FF";
    property string color_text_notActive: "#C2CEDB";

    property string language;

    property string mainText: "STYLE!!!";

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
    property string alternateBaseColor: "#c0c0c0";
    property string backgroundColor: "#e6e6e8";
    property string linkColor: "#0078D7";
    property string selectedColor: "#e3eefa";

    property string lightBlueColor: "#69b8e3";

    property string tabSelectedColor: "#FF4500";

    property string errorTextColor: "#FF4500";
    property string shadowColor: "#aaaaaa";

    property string buttonColor: "#C0C0C0";
    property string buttonBorderColor: "#CCCCCC";
    property color inactive_buttonColor: "#C0C0C0";
    property color buttonText: "#335777";
    property color inactive_buttonText: "#335777";

    property string borderColor: "#d0d0d2";
    property string borderColor2: "#a4a4a6";

    property string dialogBackgroundColor: "#80808080";

    property string imagingToolsGradient0: "#f6f6f6";
    property string imagingToolsGradient1: "#e3e3e5";
    property string imagingToolsGradient2: "#d4d4d6";
    property string imagingToolsGradient3: "#bebec0";
    property string imagingToolsGradient4: "#b4b4b6";

    property string iconColorOnSelected: "#1a76e7";

    property int fontSize_title: 20;
    property int fontSize_subtitle: 18; // mobile ? 16 :18;
    property int fontSize_common: 13; // mobile ? 14 :16;
    property int fontSize_small: 10;

    property string hover: "#e6e6e8";

    property string textColor: "#335777";
    property string textSelected: "#0078d7";
    property color inactive_textColor: "#dedede";

    property string camera: "/Icons/Light/Camera.svg";

    property string disabledInActiveTextColor: "#c0c0c0";

    property var iconNames: ["Camera", "CameraFlash", "CameraLens2"];

    Component.onCompleted: {
        console.log("Style onCompleted");
       //styleContainer.getDesignScheme("");
    }

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

    property string menuButtonDecoratorPath;
    property string subMenuButtonDecoratorPath;
    property string tabPanelDecoratorPath: "TabPanelDecorator.qml";
    property string topPanelDecoratorPath: "TopPanelDecorator.qml";

    property string filterPanelDecoratorPath: "FilterPanelDecorator.qml";

    property string topLeftPanelDecoratorPath;
    property string topCenterPanelDecoratorPath: "TopCenterPanelDecorator.qml";
    property string topRightPanelDecoratorPath;
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

    function parseStyleTheme(themeType){
        console.log("parseStyleTheme", themeType);
        let dataSource = themeType.GetData("source");
        console.log("dataSource", dataSource.toJSON());
        Style.borderColor = styleContainer.getThemeColor("ActiveColors", "BorderColor", dataSource);
        Style.baseColor = styleContainer.getThemeColor("ActiveColors", "Base", dataSource);
        Style.alternateBaseColor = styleContainer.getThemeColor("ActiveColors", "AlternateBase", dataSource);
        Style.backgroundColor = styleContainer.getThemeColor("ActiveColors", "Background", dataSource);

        Style.textColor = styleContainer.getThemeColor("ActiveColors", "Text", dataSource);
        Style.textSelected = styleContainer.getThemeColor("ActiveColors", "TextSelectedBackground", dataSource);
        Style.inactive_textColor = styleContainer.getThemeColor("InactiveColors", "Text", dataSource);

        Style.selectedColor = styleContainer.getThemeColor("ActiveColors", "ItemSelected", dataSource);

        Style.buttonColor = styleContainer.getThemeColor("ActiveColors", "Button", dataSource);
        Style.buttonBorderColor = styleContainer.getThemeColor("ActiveColors", "ButtonBorder", dataSource);
        Style.inactive_buttonColor = styleContainer.getThemeColor("InactiveColors", "Button", dataSource);
        Style.buttonText = styleContainer.getThemeColor("ActiveColors", "ButtonText", dataSource);
        Style.inactive_buttonText = styleContainer.getThemeColor("InactiveColors", "ButtonText", dataSource);

        Style.disabledInActiveTextColor = styleContainer.getThemeColor("DisabledInActiveColors", "Text", dataSource);

        Style.hover = styleContainer.getThemeColor("ActiveColors", "Hover", dataSource);

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
