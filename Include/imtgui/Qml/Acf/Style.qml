pragma Singleton
import QtQuick 2.0

Item {

    property string mainText: "STYLE!!!";

    property bool mobile; //: mainWidth < 800;
    property real mainWidth; //: window.width;
    property real mainHeight; //: window.height;
    property real mainSize; //: window.width;

    property string theme: "Light";

    property string fontFamily: mainFont.name;
    property string fontFamilyBold: boldFont.name;
    //    property string fontFamily: "MS Shell Dlg 2"

    property string mainColor: "#e6e6e8";

    property string secondColor: "#F18C16";
    property string titleColor: "#055A85";
    property string subtitleColor: "#6A6E6F";
    property string baseColor: "white";
    property string backgroundColor: "#e6e6e8";
    property string linkColor: "#0078D7";
    property string selectedColor: "#e3eefa";

    property string buttonColor: "#C0C0C0";
    property string buttonBorderColor: "#CCCCCC";

    property string imagingToolsGradient1: "#e3e3e5";
    property string imagingToolsGradient2: "#d4d4d6";
    property string imagingToolsGradient3: "#bebec0";
    property string imagingToolsGradient4: "#b4b4b6";

    property string iconColorOnSelected: "#1a76e7";

    property int fontSize_title: 20;
    property int fontSize_subtitle: 18; // mobile ? 16 :18;
    property int fontSize_common: 14; // mobile ? 14 :16;
    property int fontSize_small: 12;
    property string textColor: "#335777";

    property string camera: "/Icons/Light/Camera.svg";

    property var iconNames: ["Camera", "CameraFlash", "CameraLens2"];

    function getImageSource (name, styleTheme, buttonState, buttonMode)
    {
        var imageSource = "Icons/" + styleTheme + "/" + name + "_" + buttonState + "_" + buttonMode + ".svg";
        return imageSource;
    }

    FontLoader{
        id: mainFont;
        source: "../../Fonts/Ubuntu-Light.ttf";
//        source: "../Fonts/Times New Roman.ttf";
    }

    FontLoader{
        id: boldFont;
//        source: "../../Fonts/Ubuntu-Medium.ttf";
        source: "../../Fonts/Ubuntu-Bold.ttf";
    }

}
