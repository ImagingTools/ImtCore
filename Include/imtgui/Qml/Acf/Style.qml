pragma Singleton
import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: styleContainer;

    property string language: "en_US";

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
    property string borderColor: "#d0d0d2";
    property string borderColor2: "#a4a4a6";

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

    property string camera: "/Icons/Light/Camera.svg";

    property string disabledInActiveTextColor: "#c0c0c0";

    property var iconNames: ["Camera", "CameraFlash", "CameraLens2"];

    Component.onCompleted: {
        console.log("Style onCompleted");
        styleContainer.changeSchemeDesign("");
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

    function changeSchemeDesign(theme){
        console.log("PreferenceDialog callGetStyleQuery", theme);
        styleQuery.getStyle(theme);
    }

    function getThemeColor(colorType, colorKey, themeType){
        var colorPalette = themeType.GetData("Style").GetData(colorType).GetData(colorKey);
        return themeType.GetData("ColorPalette").GetData(colorPalette);
    }

    function parseStyleTheme(themeType){
        console.log("PreferenceDialog parseStyleTheme");

        Style.borderColor = styleContainer.getThemeColor("ActiveColors", "BorderColor", themeType);
        Style.baseColor = styleContainer.getThemeColor("ActiveColors", "Base", themeType);
        Style.alternateBaseColor = styleContainer.getThemeColor("ActiveColors", "AlternateBase", themeType);
        Style.backgroundColor = styleContainer.getThemeColor("ActiveColors", "Background", themeType);
        Style.textColor = styleContainer.getThemeColor("ActiveColors", "Text", themeType);
        Style.textSelected = styleContainer.getThemeColor("ActiveColors", "TextSelectedBackground", themeType);
        Style.selectedColor = styleContainer.getThemeColor("ActiveColors", "ItemSelected", themeType);
        Style.buttonColor = styleContainer.getThemeColor("ActiveColors", "HeaderBorder", themeType);
        Style.buttonBorderColor = styleContainer.getThemeColor("ActiveColors", "ButtonBorder", themeType);

        Style.disabledInActiveTextColor = styleContainer.getThemeColor("DisabledInActiveColors", "Text", themeType);

        Style.hover = styleContainer.getThemeColor("ActiveColors", "Hover", themeType);

        Style.imagingToolsGradient0 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient0");
        Style.imagingToolsGradient1 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient1");
        Style.imagingToolsGradient2 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient2");
        Style.imagingToolsGradient3 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient3");
        Style.imagingToolsGradient4 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient4");
        Style.greenColor = themeType.GetData("ColorPalette").GetData("Green");

        Style.iconColorOnSelected = styleContainer.getThemeColor("IconColor", "OnSelected", themeType);
        Style.tabSelectedColor = styleContainer.getThemeColor("ActiveColors", "TabSelected", themeType);
        Style.errorTextColor = styleContainer.getThemeColor("ActiveColors", "ErrorText", themeType);

        Style.shadowColor = styleContainer.getThemeColor("ActiveColors", "Shadow", themeType);
    }

    GqlModel {
        id: styleQuery;

        function getStyle(theme){
            var query = Gql.GqlRequest("query", "GetStyle");
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("theme");
            inputParams.InsertFieldArgument("theme", theme);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("style");
            queryFields.InsertField("theme");
            queryFields.InsertField("source");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("Preference GqlModel getStyle query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, styleQuery);
            if (this.state === "Ready") {
                var dataModelLocal;

                if (styleQuery === ""){
                    thumbnailDecorator.setNoConnection(true);
                }

                if (styleQuery.ContainsKey("errors")){
                    return;
                }

                if (styleQuery.ContainsKey("data")){
                    dataModelLocal = styleQuery.GetData("data");

                    if(dataModelLocal.ContainsKey("GetStyle")) {
                        dataModelLocal = dataModelLocal.GetData("GetStyle");
                    }

                    if (dataModelLocal.ContainsKey("theme")){
                        styleContainer.theme = dataModelLocal.GetData("theme");
                    }

                    if(dataModelLocal.ContainsKey("source")){
                        dataModelLocal = dataModelLocal.GetData("source");
                        styleContainer.parseStyleTheme(dataModelLocal);
                    }
                }
            }
        }
    }
}
