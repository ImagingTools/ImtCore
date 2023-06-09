import QtQuick 2.12

import Acf 1.0
import imtqml 1.0
import imtgui 1.0


Item {
    id: diagram;

    width: 1000;
    height: 500;
    property string title: "Количество пройденых объектов";
    property string legendX: "Контроллеры";
    property string legendY: "Кол-во";
    property string colorPositive: "yellow";
    property string colorNegative: "blue";
    property int barWidth: 30;
    property alias model: barsList.model;
    property bool visibleAxeX: false;
    property bool visibleAxeY: true;
    property int fontSize: 18;
    property int maxValue: 100;
    property int maxAxeYValue: 100;
    property real minWidth: Math.max((axeYValuesContainer.width + axeY.width + barsList.contentWidth + barsList.anchors.leftMargin),
                                     titleText.width, legendXText.width, legendYText.width);


    //TO STYLE
    property int size_panelsHeight: Style.size_panelsHeight;
    property int size_mainCornerRadius: Style.size_mainCornerRadius;
    property int size_mainMargin: Style.size_mainMargin;
    property int size_tableDelegateHeight: Style.size_tableDelegateHeight;

    property int fontSize_title: Style.fontSize_title;
    property int fontSize_subtitle: Style.fontSize_subtitle;
    property int fontSize_common: Style.fontSize_common;
    property int fontSize_small: Style.fontSize_small;

    property string color_first: Style.color_first;
    property string color_second: Style.color_second;
    property string color_third: Style.color_third;
    property string color_table_delegate: Style.color_table_delegate;
    property string color_table_header: Style.color_table_header;
    property string color_text_common: Style.color_text_common;
    property string color_text_important: Style.color_text_important;
    property string color_text_titles: Style.color_text_titles;
    property string color_text_notActive: Style.color_text_notActive;
    property string color_text_subtitles: Style.color_text_subtitles;
    property string color_background: Style.color_background;
    //TO STYLE



    onModelChanged: {
        diagram.setMaxValue();
    }


    function setMaxValue(){
        if(diagram.model !== 0){
            var maxVal = 0;
            //for(var i = 0; i < diagram.model.count; i++)
            for(var i = 0; i < diagram.model.GetItemsCount(); i++)
            {
                //console.log(model.get(i).positive, model.get(i).negative);
                //var currVal = diagram.model.get(i).positive + diagram.model.get(i).negative;
                var currVal = diagram.model.GetData("positive",i) + diagram.model.GetData("negative",i);
                if(currVal > maxVal){
                    maxVal = currVal
                }

            }
        }

        //console.log(maxVal);
        diagram.maxValue = maxVal;
        sizeText.text = maxVal;
        diagram.fillAxeYModel();

    }

    function fillAxeYModel(){
        axeYValueModel.clear();
        for(var i = 4; i >= 0 ; i--){
            var val;
            if(i == 4){
                val = diagram.roundDigit((diagram.maxValue/4 * i),true);
                diagram.maxAxeYValue = val;
                axeYValueModel.append({"text" : val})
            }
            else {
                val = diagram.roundDigit((diagram.maxValue/4 * i),false);
                axeYValueModel.append({"text" : val})
            }
        }

    }

    function roundDigit(digit, ceil){

        var retval;
        var count = String(Math.floor(digit)).length;
        var coeff = Math.pow(10, count -1);
        var beforeDot = Math.floor((digit/coeff));
        var afterDot = digit/coeff - beforeDot;

        if(ceil){
            if(afterDot == 0){
                afterDot = 0;
            }
            else if(afterDot > 0 && afterDot < 0.5){
                afterDot = 0.5;
            }
            else{
                afterDot = 1;
            }

        }

        else if(digit > 1){
            if(afterDot >= 0 && afterDot < 0.25){
                afterDot = 0;
            }
            else if(afterDot >= 0.25 && afterDot < 0.5){
                afterDot = 0.5;
            }
            else if(afterDot >= 0.5 && afterDot < 0.75){
                afterDot = 0.5;
            }
            else{
                afterDot = 1;
            }

        }

        retval = (Number(beforeDot) + Number(afterDot)) * coeff;
        return retval;
    }

    ListModel{
        id: axeYValueModel;
    }


    Text {
        id: titleText;

        horizontalAlignment: Text.AlignLeft;
        verticalAlignment: Text.AlignVCenter;
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        font.pixelSize: diagram.fontSize;
        font.bold: true;
        font.family: Style.fontFamily;
        color:  diagram.color_text_subtitles;

        text: diagram.title;

    }

    Rectangle {
        id: axeYValuesContainer;

        anchors.left: parent.left;
        anchors.top: legendYText.bottom;
        anchors.topMargin: diagram.size_mainMargin;
        anchors.bottom: legendXText.top;
        anchors.bottomMargin: 10;//diagram.size_mainMargin

        width: sizeText.width + 2 * diagram.size_mainMargin;

        Text {
            id: sizeText;// для определения размера панели значений оси Y

            visible: false;
            horizontalAlignment: Text.AlignLeft;
            verticalAlignment: Text.AlignVCenter;
            wrapMode: Text.NoWrap;
            font.pixelSize: diagram.fontSize;
            font.bold: true;
            font.family: Style.fontFamily;
            color:  diagram.color_text_common;

            text: diagram.maxValue;

        }

        ListView{
            id: axeYValuesList;

            anchors.fill: parent;

            clip: true;
            boundsBehavior: Flickable.StopAtBounds;
            model: axeYValueModel;
            delegate: Item {
                width: axeYValuesList.width;
                height : valueText.height + ((axeYValuesList.height - valueText.height)/4 - valueText.height) * (index !== (axeYValuesList.count - 1));
                Text {
                    id: valueText;

                    anchors.top: parent.top;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignTop;
                    wrapMode: Text.NoWrap;
                    font.pixelSize: diagram.fontSize;
                    font.bold: false;
                    font.family: Style.fontFamily;
                    color:  diagram.color_text_common;
                    text: model.text;

                }

            }

        }

    }

    Rectangle{
        id: axeY;

        anchors.left: axeYValuesContainer.right;
        anchors.top: axeYValuesContainer.top;
        anchors.bottom: axeYValuesContainer.bottom;
        anchors.bottomMargin: 10;//diagram.size_mainMargin

        width: 2;
        visible: diagram.visibleAxeY;
        color: "#C4C4C4";

    }

    Rectangle{
        id: axeX;

        anchors.bottom: axeY.bottom;
        anchors.left: axeY.right;
        anchors.right: parent.right;

        height: 2;
        visible: diagram.visibleAxeX;
        color: "#C4C4C4";

    }

    Text {
        id: legendYText;

        anchors.top: titleText.bottom;
        anchors.topMargin: diagram.size_mainMargin;
        anchors.horizontalCenter: axeY.horizontalCenter;

        horizontalAlignment: Text.AlignLeft;
        verticalAlignment: Text.AlignVCenter;
        wrapMode: Text.NoWrap;
        font.pixelSize: diagram.fontSize;
        font.bold: true;
        font.family: Style.fontFamily;
        color:  diagram.color_text_common;

        text: diagram.legendY;
    }

    Text {
        id: legendXText;

        anchors.bottom:  parent.bottom;
        anchors.bottomMargin: 0;
        anchors.horizontalCenter: barsSpace.horizontalCenter;

        horizontalAlignment: Text.AlignLeft;
        verticalAlignment: Text.AlignVCenter;
        wrapMode: Text.NoWrap;
        font.pixelSize: diagram.fontSize;
        font.bold: true;
        font.family: Style.fontFamily;
        color:  diagram.color_text_common;
        text: diagram.legendX;
    }

    Rectangle{
        id: barsSpace;

        anchors.top: axeYValuesContainer.top;
        anchors.left: axeY.right;
        anchors.right: parent.right;
        anchors.bottom: axeX.top;
        //width: barsList.width + barsList.anchors.leftMargin
        ListView{
            id:barsList;

            //anchors.top: parent.top
            anchors.bottom: parent.bottom;
            anchors.left: parent.left;
            anchors.leftMargin: spacing;
            anchors.right: parent.right;

            //width: contentWidth
            height: parent.height * diagram.maxValue/diagram.maxAxeYValue;
            orientation: ListView.Horizontal;
            clip: true;
            boundsBehavior: Flickable.StopAtBounds;
            spacing: diagram.size_mainMargin;
            model: 0;
            delegate:
                BarChart{

                anchors.bottom: parent.bottom;

                maxBarHeight: barsList.height;
                maxValue: diagram.maxValue;
                barWidth: diagram.barWidth;
                positiveValue: model.positive;
                negativeValue: model.negative;
                color_positive: diagram.colorPositive;
                color_negative: diagram.colorNegative;
            }
        }

    }

}

