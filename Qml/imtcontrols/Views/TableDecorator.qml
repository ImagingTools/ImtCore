import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

TreeItemModel {
    id: tableDecorator;
    property bool compl: false;

    Component.onCompleted: {
        var index = tableDecorator.InsertNewItem();

        var headers = tableDecorator.AddTreeModel("Headers");
        var cells = tableDecorator.AddTreeModel("Cells");
        var cellWidth = tableDecorator.AddTreeModel("CellWidth");

        //headers
        headers.SetData("Color","#E5E5E5");
        headers.SetData("FontSize",16);
        headers.SetData("FontBold",true);
        headers.SetData("FontColor","#686671");
        headers.SetData("TextPosition",Text.AlignHCenter);

        headers.SetData("CellRadius",8);
        headers.SetData("LeftTopRound",false);
        headers.SetData("RightTopRound",false);
        headers.SetData("LeftBottomRound",false);
        headers.SetData("RightBottomRound",false);

        headers.SetData("BorderColorHorizontal","#686671");
        headers.SetData("BorderColorVertical","#686671");
        headers.SetData("HorizontalBorderSize",1);
        headers.SetData("VerticalBorderSize",1);

        headers.SetData("VisibleLeftBorderFirst",false);
        headers.SetData("VisibleRightBorderLast",false);
        headers.SetData("VisibleTopBorderFirst", false);
        headers.SetData("VisibleBottomBorderLast",true);

        //headers.SetData("WrapMode",Text.NoWrap);
        //headers.SetData("ElideMode",Text.ElideRight);
        headers.SetData("IsRightBorder",false);

        //cells
        cells.SetData("Color","#FFFFFF");
        cells.SetData("FontSize",18);
        cells.SetData("FontBold",false);
        cells.SetData("FontColor","#686671");
        cells.SetData("TextPosition",Text.AlignHCenter);

        cells.SetData("CellRadius",0);
        cells.SetData("LeftTopRound",false);
        cells.SetData("RightTopRound",false);
        cells.SetData("LeftBottomRound",false);
        cells.SetData("RightBottomRound",false);

        cells.SetData("BorderColorHorizontal","#E5E5E5");
        cells.SetData("BorderColorVertical","#E5E5E5");
        cells.SetData("HorizontalBorderSize",1);
        cells.SetData("VerticalBorderSize",1);

        cells.SetData("VisibleLeftBorderFirst",true);
        cells.SetData("VisibleRightBorderLast",false);
        cells.SetData("VisibleTopBorderFirst", false);
        cells.SetData("VisibleBottomBorderLast",true);

        cells.SetData("WrapMode",Text.NoWrap);
        cells.SetData("ElideMode",Text.ElideRight);
        cells.SetData("IsRightBorder",false);

        cellWidth.SetData("Width", -1);
        cellWidth.SetData("WidthPercent", -1);

        //
        tableDecorator.compl = true;
    }

    function setDefault(model, count){
        var headers = model.AddTreeModel("Headers");
        var cells = model.AddTreeModel("Cells");
        var cellWidth = model.AddTreeModel("CellWidth");

        for(var i = 0; i < count; i++){
            headers.InsertNewItem();
            cells.InsertNewItem();
            cellWidth.InsertNewItem();
            headers.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("Headers"));
            cells.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("Cells"));
            cellWidth.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("CellWidth"));
        }

    }

}
