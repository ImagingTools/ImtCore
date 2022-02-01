pragma Singleton
import QtQuick 2.0

Item {

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

    property string secondColor: "#F18C16";
    property string titleColor: "#055A85";
    property string subtitleColor: "#6A6E6F";
    property string baseColor: "white";
    property string alternateBaseColor: "#c0c0c0";
    property string backgroundColor: "#e6e6e8";
    property string linkColor: "#0078D7";
    property string selectedColor: "#e3eefa";

    property string tabSelectedColor: "#FF4500";

    property string errorTextColor: "#FF4500";

    property string buttonColor: "#C0C0C0";
    property string buttonBorderColor: "#CCCCCC";
    property string borderColor2: "#a4a4a6";

    property string imagingToolsGradient1: "#e3e3e5";
    property string imagingToolsGradient2: "#d4d4d6";
    property string imagingToolsGradient3: "#bebec0";
    property string imagingToolsGradient4: "#b4b4b6";

    property string iconColorOnSelected: "#1a76e7";

    property int fontSize_title: 20;
    property int fontSize_subtitle: 18; // mobile ? 16 :18;
    property int fontSize_common: 13; // mobile ? 14 :16;
    property int fontSize_small: 12;

    property string hover: "#e6e6e8";

    property string textColor: "#335777";
    property string textSelected: "#0078d7";

    property string camera: "/Icons/Light/Camera.svg";

    property string disabledInActiveTextColor: "#c0c0c0";

    property var iconNames: ["Camera", "CameraFlash", "CameraLens2"];

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

}
