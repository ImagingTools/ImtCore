pragma Singleton
import QtQuick 2.0

Item {

    property string mainText: "STYLE!!!";

    property bool mobile: mainWidth < 800;
    property real mainWidth: window.width;
    property real mainHeight: window.height;
    property real mainSize: window.width;

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
    property string selectedColor: "#c0c0c0";

    property string buttonColor: "#C0C0C0";
    property string buttonBorderColor: "#CCCCCC";

    property string imagingToolsGradient1: "#e3e3e5";
    property string imagingToolsGradient2: "#d4d4d6";
    property string imagingToolsGradient3: "#bebec0";
    property string imagingToolsGradient4: "#b4b4b6";


    property int fontSize_title: 20; //mobile ? 20 : mainSize/40 > 20 ? mainSize/40 : 20;
    property int fontSize_subtitle: mobile ? 16 :20; //width/96
    property int fontSize_common: mobile ? 14 :18; //width/107
    property int fontSize_small: 16;
    property string textColor: "#335777";

    FontLoader{
        id: mainFont;
//        source: "../../Fonts/Ubuntu-Light.ttf";
    }

    FontLoader{
        id: boldFont;
//        source: "../../Fonts/Ubuntu-Medium.ttf";
    }

}
