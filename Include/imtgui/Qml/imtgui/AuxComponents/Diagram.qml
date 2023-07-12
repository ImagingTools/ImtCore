import QtQuick 2.12

import Acf 1.0
import imtqml 1.0
import imtgui 1.0


Item {
    id: diagram;

    width: 1000;
    height: 500;

    property string title: "Заголовок";
    property string diagramName: "Название диаграммы";
    property string legendX: "Ось абсцисс";
    property string legendY: "Ось ординат";
    property string colorPositive: "yellow";
    property string colorNegative: "blue";
    property int barWidth: 15;
    property TreeItemModel model: TreeItemModel{};
    property TreeItemModel axeYValueModel: TreeItemModel{};
    property bool visibleAxeX: false;
    property bool visibleAxeY: true;
    property int fontSize: 18;
    property bool hasMinValue: false;
    property int minValue: 0;
    property real maxValue: 100;
    property real maxAxeYValue: 100;


    property real minWidth: Math.max((axeYValuesContainer.width + axeY.width + barsList.contentWidth + barsList.anchors.leftMargin + barsSpace.anchors.rightMargin + legendXTextRight.width),
                                        400);


    property string backgroundColor: "transparent";
    property string axeColor: "transparent";
    property string titleColor: Style.color_text_subtitles;
    property string valueColor: Style.color_text_common;

    property int spacingX: 25;

    property real xElementRotation: 0;

    property real parentY: 0;

    onParentYChanged: {console.log("ParentYChanged");}


    onModelChanged: {
        //diagram.setMaxValue();
    }

    function setMinValue(){
        if(!diagram.hasMinValue){
            diagram.minValue = 0;
        }
        else{
            var minVal;
            var firstVal;
            var count = diagram.model.GetItemsCount();
            if(count){
                firstVal = Number(diagram.model.GetData("positive",0)) + Number(diagram.model.GetData("negative",0));
                minVal = firstVal;
                for(var i = 0; i < diagram.model.GetItemsCount(); i++){
                    var currVal = Number(diagram.model.GetData("positive",i)) + Number(diagram.model.GetData("negative",i));
                    if(currVal < minVal){
                        minVal = currVal;
                    }
                }
                minVal = Math.trunc(minVal);
            }

            diagram.minValue = minVal;
        }
    }


    function setMaxValue(){
        if(diagram.model.GetItemsCount()){

            setMinValue();

            var maxVal = 0;
            for(var i = 0; i < diagram.model.GetItemsCount(); i++)
            {
                var currVal = Number(diagram.model.GetData("positive",i)) + Number(diagram.model.GetData("negative",i)) - Number(diagram.minValue);
                if(currVal > maxVal){
                    maxVal = currVal;
                }
            }
            diagram.maxValue = maxVal;
            diagram.fillAxeYModel();
        }



    }

    function fillAxeYModel(){
        diagram.axeYValueModel.Clear();
        for(var i = 4; i >= 0 ; i--){
            var val;
            if(i == 4){
                val = diagram.roundDigit((diagram.maxValue/4 * i),true);
                diagram.maxAxeYValue = val;
                let valToModel = val + diagram.minValue;
                var index = diagram.axeYValueModel.InsertNewItem();
                diagram.axeYValueModel.SetData("text", valToModel, index);
            }
            else {
                val = diagram.roundDigit((diagram.maxValue/4 * i),false);
                let valToModel = Math.trunc((val + diagram.minValue)*100)/100;
                index = diagram.axeYValueModel.InsertNewItem();
                diagram.axeYValueModel.SetData("text", valToModel, index);
            }
        }
        diagram.setSizeText();
    }


    function setSizeText(){
        var maxText = "";
        for (var i = 0; i < diagram.axeYValueModel.GetItemsCount(); i++){
            var currText = String(diagram.axeYValueModel.GetData("text",i));
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
        id: axeXValueModel;
    }

    Text {
        id: titleText;

        anchors.top: parent.top;

        anchors.left: parent.left;
        anchors.right: parent.right;

        width: parent.width/2;
        wrapMode: Text.WordWrap;
        font.pixelSize: diagram.fontSize;
        font.bold: true;
        font.family: Style.fontFamily;
        color:  diagram.titleColor;

        text: diagram.title;

    }

    Text {
        id: diagramNameText;

        anchors.top: parent.top;
        anchors.topMargin: legendYText.y;
        anchors.right: parent.right;

        width: parent.width/2;
        horizontalAlignment: Text.AlignRight;
        wrapMode: Text.WordWrap;
        font.pixelSize: diagram.fontSize;
        font.bold: false;
        font.family: Style.fontFamily;
        color:  diagram.valueColor;

        text: diagram.diagramName;

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

            text: Math.trunc(diagram.maxValue*100)/100;

        }

        ListView{
            id: axeYValuesList;

            anchors.fill: parent;

            clip: true;
            boundsBehavior: Flickable.StopAtBounds;
            model: diagram.axeYValueModel;
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
                    font.pixelSize: diagram.fontSize -2;
                    font.bold: false;
                    font.family: Style.fontFamily;
                    color:  diagram.valueColor;
                    text: model.text !== undefined ? model.text : "";

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

                    parentY: diagram.parentY;
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

        }

    }

}

