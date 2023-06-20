import QtQuick 2.12

import Acf 1.0
import imtqml 1.0
import imtgui 1.0


Item {
    id: diagram;

    width: 1000;
    height: 500;

    property string title: "Заголовок";
    property string legendX: "Ось абсцисс";
    property string legendY: "Ось ординат";
    property string colorPositive: "yellow";
    property string colorNegative: "blue";
    property int barWidth: 20;
    //property alias model: barsList.model;
    property TreeItemModel model: TreeItemModel{};
    property bool visibleAxeX: false;
    property bool visibleAxeY: true;
    property int fontSize: 18;
    property bool hasMinValue: false;
    property int minValue: 0;
    property real maxValue: 100;
    property real maxAxeYValue: 100;
    property real minWidth: Math.max((axeYValuesContainer.width + axeY.width + barsList.contentWidth + barsList.anchors.leftMargin + barsSpace.anchors.rightMargin + legendXTextRight.width),
                                     titleText.width, legendXText.width, legendYText.width);



    property string backgroundColor: "transparent";
    property string axeColor: "transparent";
    property string titleColor: Style.color_text_subtitles;
    property string valueColor: Style.color_text_common;

    property int spacingX: 20;

    property real xElementRotation: 0;


    onModelChanged: {
        diagram.setMaxValue();
    }

    function setMinValue(){
        if(!diagram.hasMinValue){
            diagram.minValue = 0;
        }
        else{
            var minVal = 0;
            var summ = 0;
            var average = 0;
            var count = diagram.model.GetItemsCount();
            var firstVal;
            var lastVal;
            var delta;
            if(count){
                for(var i = 0; i < diagram.model.GetItemsCount(); i++){
                    var currVal = diagram.model.GetData("positive",i) + diagram.model.GetData("negative",i);
                    summ += currVal;
                    if(i == 0){
                        firstVal = currVal;
                    }
                    if(i == count -1){
                        lastVal = currVal;
                    }
                }
                average = summ/count;
                delta = lastVal - firstVal;
                minVal = Math.trunc(firstVal/10) * 10;
            }

            diagram.minValue = minVal;
        }
    }


    function setMaxValue(){
        if(diagram.model.GetItemsCount()){

            setMinValue();

            var maxVal = 0;
            //for(var i = 0; i < diagram.model.count; i++)
            for(var i = 0; i < diagram.model.GetItemsCount(); i++)
            {
                //console.log(model.get(i).positive, model.get(i).negative);
                //var currVal = diagram.model.get(i).positive + diagram.model.get(i).negative;
                var currVal = diagram.model.GetData("positive",i) + diagram.model.GetData("negative",i) - diagram.minValue;
                if(currVal > maxVal){
                    maxVal = currVal;
                }

            }

            diagram.maxValue = maxVal;
            diagram.fillAxeYModel();
        }



    }

    function fillAxeYModel(){
        axeYValueModel.clear();
        for(var i = 4; i >= 0 ; i--){
            var val;
            if(i == 4){
                val = diagram.roundDigit((diagram.maxValue/4 * i),true);
                diagram.maxAxeYValue = val;
                let valToModel = val + diagram.minValue;
                axeYValueModel.append({"text" : valToModel})
            }
            else {
                val = diagram.roundDigit((diagram.maxValue/4 * i),false);
                let valToModel = val + diagram.minValue;
                axeYValueModel.append({"text" : valToModel})
            }
        }
        diagram.setSizeText();
    }


    function setSizeText(){
        var maxText = "";
        for (var i = 0; i < axeYValueModel.count; i++){
            var currText = String(axeYValueModel.get(i).text);
            if(currText.length > maxText.length){
                maxText = currText;
            }
        }
        sizeText.text = maxText;
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

    ListModel{
        id: axeXValueModel;
    }

    Text {
        id: titleText;

        anchors.top: parent.top;
        anchors.left: parent.left;
//        horizontalAlignment: Text.AlignLeft;
//        verticalAlignment: Text.AlignVCenter;
        wrapMode: Text.WordWrap;
        font.pixelSize: diagram.fontSize;
        font.bold: true;
        font.family: Style.fontFamily;
        color:  diagram.titleColor;

        text: diagram.title;

    }

    Rectangle {
        id: axeYValuesContainer;

        anchors.left: parent.left;
        anchors.top: legendYText.bottom;
        anchors.topMargin: Style.size_mainMargin;
        anchors.bottom: legendXText.top;
        anchors.bottomMargin: 10;//Style.size_mainMargin

        width: sizeText.width + 2 * Style.size_mainMargin;

        color: diagram.backgroundColor;

        Text {
            id: sizeText;// для определения размера панели значений оси Y

            visible: false;
            horizontalAlignment: Text.AlignLeft;
            verticalAlignment: Text.AlignVCenter;
            wrapMode: Text.NoWrap;
            font.pixelSize: diagram.fontSize;
            font.bold: true;
            font.family: Style.fontFamily;
            color:  diagram.valueColor;

            text: diagram.roundDigit(diagram.maxValue);

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
                    color:  diagram.valueColor;
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
        anchors.bottomMargin: 10;//Style.size_mainMargin

        width: 2;
        visible: diagram.visibleAxeY;
        color: diagram.axeColor;

    }

    Rectangle{
        id: axeX;

        anchors.bottom: axeY.bottom;
        anchors.left: axeY.right;
        anchors.right: legendXTextRight.left;
        anchors.rightMargin: 10;

        height: 2;
        visible: diagram.visibleAxeX;
        color: diagram.axeColor;

    }

    Text {
        id: legendYText;

        anchors.top: titleText.bottom;
        anchors.topMargin: Style.size_mainMargin;
        anchors.horizontalCenter: axeY.horizontalCenter;

//        horizontalAlignment: Text.AlignLeft;
//        verticalAlignment: Text.AlignVCenter;
        wrapMode: Text.NoWrap;
        font.pixelSize: diagram.fontSize;
        font.bold: false;
        font.family: Style.fontFamily;
        color:  diagram.valueColor;

        text: diagram.legendY;
    }

    Text {
        id: legendXText;

        anchors.bottom:  parent.bottom;
        anchors.bottomMargin: 0;
        anchors.horizontalCenter: barsSpace.horizontalCenter;
        visible: false;

        horizontalAlignment: Text.AlignLeft;
        verticalAlignment: Text.AlignVCenter;
        wrapMode: Text.NoWrap;
        font.pixelSize: diagram.fontSize;
        font.bold: false;
        font.family: Style.fontFamily;
        color:  diagram.valueColor;
        text: diagram.legendX;

    }

    Text {
        id: legendXTextRight;

        anchors.right: parent.right;
        anchors.verticalCenter: axeX.verticalCenter;

//        horizontalAlignment: Text.AlignLeft;
//        verticalAlignment: Text.AlignVCenter;
        wrapMode: Text.NoWrap;
        font.pixelSize: diagram.fontSize;
        font.bold: false;
        font.family: Style.fontFamily;
        color:  diagram.valueColor;
        text: diagram.legendX;
    }



    Rectangle{
        id: barsSpace;

        anchors.top: axeYValuesContainer.top;
        anchors.left: axeY.right;
        anchors.right: legendXTextRight.left;
        anchors.rightMargin: 10;
        anchors.bottom: axeX.top;
        //width: barsList.width + barsList.anchors.leftMargin
        color: diagram.backgroundColor;
        ListView{
            id: barsList;

            //anchors.top: parent.top
            anchors.bottom: parent.bottom;
            anchors.left: parent.left;
            anchors.leftMargin: 0;//diagram.spacingX/2;
            anchors.right: parent.right;

            //width: contentWidth
            height: parent.height * diagram.maxValue/diagram.maxAxeYValue;
            orientation: ListView.Horizontal;
            clip: true;
            boundsBehavior: Flickable.StopAtBounds;
            model: diagram.model;
            delegate:
                Item{
                //anchors.bottom: parent.bottom;

                width: diagram.barWidth + diagram.spacingX;
                //height: barChart.height;
                height: barsList.height;
                BarChart{
                    id: barChart;

                    anchors.bottom: parent.bottom;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    visible: diagram.visible;

                    maxBarHeight: barsList.height;
                    maxValue: diagram.maxValue;
                    barWidth: diagram.barWidth;
                    positiveValue: (model.positive - diagram.minValue) >= 0 ? (model.positive - diagram.minValue) : 0;
                    negativeValue: (model.negative - diagram.minValue) >= 0 ? (model.negative - diagram.minValue) : 0;
                    color_positive: diagram.colorPositive;
                    color_negative: diagram.colorNegative;

                    addToValue: diagram.minValue;
                }
            }

        }

    }

    ListView{
        id: axeXValuesList;

        anchors.left: barsSpace.left;
        anchors.leftMargin: 0;//diagram.spacingX/2;
        anchors.right: barsSpace.right;
        anchors.top: axeX.bottom;
        anchors.bottom: parent.bottom;
        anchors.topMargin: 4;
        anchors.bottomMargin: 2;

        orientation: ListView.Horizontal;
        clip: true;
        boundsBehavior: Flickable.StopAtBounds;
        model: diagram.model;
        contentX: barsList.contentX;
        onContentXChanged:{
            barsList.contentX = contentX;
        }

        delegate: Rectangle{

            width: diagram.barWidth + diagram.spacingX;
            height: axeXValuesList.height;
            color: "transparent";

            Text{
                id: xValuesText;

                anchors.centerIn: parent;

                color: diagram.valueColor;
                font.family: Style.fontFamily;
                font.pixelSize: 10;

                rotation: diagram.xElementRotation;

                text: model.xValue;
            }

//            Rectangle{//test for web
//                anchors.centerIn: parent;

//                width: xValuesText.width;
//                height: xValuesText.height;

//                color: "red";

//                rotation: diagram.xElementRotation;

//                Text{
//                    id: xValuesText;

//                    //anchors.centerIn: parent;

//                    color: diagram.valueColor;
//                    font.family: Style.fontFamily;
//                    font.pixelSize: 10;

//                    //rotation: diagram.xElementRotation;

//                    text: model.xValue;
//                }
//            }//test for web

        }

    }

}

