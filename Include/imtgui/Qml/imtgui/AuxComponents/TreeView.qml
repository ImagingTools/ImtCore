import QtQuick 2.15
import Acf 1.0

AuxTable {
    id: treeView;

    property int basePadding: 30;

    function collapse(index){

    }

    function expand(index){

    }

    delegate: TreeDelegate {
        id: treeDelegate;

        x: treeDelegate.depth * treeView.basePadding

        height: treeView.itemHeight;

        basePadding: treeView.basePadding;
        width: treeView.width - x;


        minHeight: treeView.itemHeight;

        //!!!
        cellDecorator: treeView.cellDecorator;
        widthDecorator: treeView.widthDecorator;

        borderColorHorizontal: treeView.borderColorHorizontal_deleg;
        borderColorVertical: treeView.borderColorVertical_deleg;
        horizontalBorderSize: treeView.horizontalBorderSize_deleg;
        verticalBorderSize: treeView.verticalBorderSize_deleg;

        visibleLeftBorderFirst: treeView.visibleLeftBorderFirst_deleg;
        visibleRightBorderLast: treeView.visibleRightBorderLast_deleg;
        visibleTopBorderFirst: treeView.visibleTopBorderFirst_deleg;
        visibleBottomBorderLast: treeView.visibleBottomBorderLast_deleg;

        canSetBorderParams: treeView.canSetBorderParams_deleg;
        wrapMode:  treeView.wrapMode_deleg;
        elideMode: treeView.elideMode_deleg;
        isRightBorder: treeView.isRightBorder_deleg;

        textMarginHor: treeView.textMarginHor_deleg;
        textMarginVer: treeView.textMarginVer_deleg;
        //!!!

        Component.onCompleted: {
            for(var i = 0; i < treeView.headers.GetItemsCount(); i++){
                treeDelegate.addToArray(model[treeView.headers.GetData("Id", i)]);
            }
        }

        onExpanded: {
            treeView.expand(model.index);
        }

        onCollapsed: {
            treeView.collapse(model.index);
        }

        onClicked: {
            treeView.tableSelection.singleSelect(model.index);

            treeView.elementsList.forceActiveFocus();
        }

        onRightButtonMouseClicked: {

            var point = mapToItem(null, mX, mY);
            treeView.rightButtonMouseClicked(point.x, point.y);
        }

        onDoubleClicked: {
            treeView.selectedIndex = model.index;

            var point = mapToItem(null, mX, mY);
            treeView.doubleClicked(point.x, point.y)
            treeView.selectItem(model.Id, model.Name);
        }
    }
}
