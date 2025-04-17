import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

TreeItemModel {
    id: tableDecorator;
    property bool compl: false;

    Component.onCompleted: {
        var index = tableDecorator.insertNewItem();

        var headers = tableDecorator.addTreeModel("Headers");
        var cells = tableDecorator.addTreeModel("Cells");
        var cellWidth = tableDecorator.addTreeModel("CellWidth");

        //headers
        headers.setData("Color","#E5E5E5");
        headers.setData("FontSize",16);
        headers.setData("FontBold",true);
        headers.setData("FontColor","#686671");
        headers.setData("TextPosition",Text.AlignLeft);

        headers.setData("CellRadius",2);
        headers.setData("LeftTopRound",true);
        headers.setData("RightTopRound",true);
        headers.setData("LeftBottomRound",false);
        headers.setData("RightBottomRound",false);

        headers.setData("BorderColorHorizontal","#686671");
        headers.setData("BorderColorVertical","#686671");
        headers.setData("HorizontalBorderSize",1);
        headers.setData("VerticalBorderSize",1);

        headers.setData("VisibleLeftBorderFirst",false);
        headers.setData("VisibleRightBorderLast",false);
        headers.setData("VisibleTopBorderFirst", false);
        headers.setData("VisibleBottomBorderLast",true);

        //headers.setData("WrapMode",Text.NoWrap);
        //headers.setData("ElideMode",Text.ElideRight);
        headers.setData("IsRightBorder",false);

        //cells
        cells.setData("Color","#FFFFFF");
        cells.setData("FontSize",18);
        cells.setData("FontBold",false);
        cells.setData("FontColor","#686671");
        cells.setData("TextPosition",Text.AlignLeft);

        cells.setData("CellRadius",0);
        cells.setData("LeftTopRound",false);
        cells.setData("RightTopRound",false);
        cells.setData("LeftBottomRound",false);
        cells.setData("RightBottomRound",false);

//        cells.setData("BorderColorHorizontal","#E5E5E5");
//        cells.setData("BorderColorVertical","#E5E5E5");
        cells.setData("BorderColorHorizontal","transparent");
        cells.setData("BorderColorVertical","transparent");
        cells.setData("HorizontalBorderSize",1);
        cells.setData("VerticalBorderSize",1);

        cells.setData("VisibleLeftBorderFirst",true);
        cells.setData("VisibleRightBorderLast",false);
        cells.setData("VisibleTopBorderFirst", false);
        cells.setData("VisibleBottomBorderLast",true);

        cells.setData("WrapMode",Text.NoWrap);
        cells.setData("ElideMode",Text.ElideRight);
        cells.setData("IsRightBorder",false);

        cellWidth.setData("Width", -1);
        cellWidth.setData("WidthPercent", -1);
        cellWidth.setData("MinWidth", 100);

        //
        tableDecorator.compl = true;
    }

    function setDefault(model, count){
        var headers = model.addTreeModel("Headers");
        var cells = model.addTreeModel("Cells");
        var cellWidth = model.addTreeModel("CellWidth");

        for(var i = 0; i < count; i++){
            headers.insertNewItem();
            cells.insertNewItem();
            cellWidth.insertNewItem();
            headers.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Headers"));
            cells.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Cells"));
            cellWidth.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("CellWidth"));
        }

    }

}
