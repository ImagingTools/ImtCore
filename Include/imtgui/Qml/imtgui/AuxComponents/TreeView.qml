import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: treeViewContainer;

    color: Style.baseColor;

    property var modelItems;

    property int countItems;

    signal itemTreeViewCheckBoxStateChanged(int state, string packageId, string featureId);

    onModelItemsChanged: {
        treeViewContainer.countItems = modelItems.GetItemsCount();
    }

//    Keys.onPressed: {
//        console.log("TreeView keys pressed")
//        if (event.key === Qt.Key_Tab){
//            console.log('Key tab was pressed');
//            thubnailDecoratorContainer.setFocusOnMenuPanel();
//        }
//        else if (event.key === Qt.Key_Up){
//            console.log('Key up was pressed');
//            treeViewContainer.selectedIndexDecr();
//        }
//        else if (event.key === Qt.Key_Down){
//            console.log('Key down was pressed');

//            treeViewContainer.selectedIndexIncr();
//        }
//        else if (event.key === Qt.Key_Left){
//            console.log('Key left was pressed');
//            if (mainTreeView.currentParentIndex != -1 && mainTreeView.currentChildIndex === -1){
//                treeViewContainer.closeTreeItem(mainTreeView.currentParentIndex);
//            }
//        }
//        else if (event.key === Qt.Key_Space){
//            console.log('Key space was pressed');

//            if (mainTreeView.currentChildIndex !== -1){

//                var parentId = treeViewContainer.modelItems.GetData("Id", mainTreeView.currentParentIndex);
//                var childModel = treeViewContainer.modelItems.GetData("childItemModel", mainTreeView.currentParentIndex);

//                if (!childModel){
//                    return;
//                }

//                var childId = childModel.GetData("Id", mainTreeView.currentChildIndex);
//                var state = childModel.GetData("stateChecked", mainTreeView.currentChildIndex);

//               // treeViewContainer.itemTreeViewCheckBoxStateChanged(2 - state, parentId, childId);
//                treeViewContainer.setStateCheckBox(2 - state);
//            }
//        }
//    }

    TreeItemModel {
        id: treeItemModel;
    }

    function setStateCheckBox(state){
        console.log('TreeView setStateCheckBox', state);

        var childModel = treeViewContainer.modelItems.GetData("childItemModel", mainTreeView.currentParentIndex);

        if (!childModel){
            return;
        }

        childModel.SetData("stateChecked", state, mainTreeView.currentChildIndex);
        treeViewContainer.modelItems.SetData("childItemModel", childModel, mainTreeView.currentParentIndex);

//        treeViewContainer.modelItems.Refresh();
    }

    function openTreeItem(index){
        console.log('TreeView openTreeItem');
        treeViewContainer.modelItems.SetData("isOpened", 1, index);
    }

    function closeTreeItem(index){
        console.log('TreeView closeTreeItem');
        treeViewContainer.modelItems.SetData("isOpened", 0, index);
    }

    function selectedIndexIncr(){
        console.log("PackageCollectionView selectedIndexIncr было", mainTreeView.currentParentIndex, mainTreeView.currentChildIndex);

        if (mainTreeView.currentParentIndex === -1){
            mainTreeView.currentParentIndex++;
            return;
        }

        var modelChild = treeViewContainer.modelItems.GetData("childItemModel", mainTreeView.currentParentIndex);

        if (!modelChild){
            return;
        }

        var countChild = modelChild.GetItemsCount();
        var isOpened = treeViewContainer.modelItems.GetData("isOpened", mainTreeView.currentParentIndex);

        if (!isOpened){
            mainTreeView.currentParentIndex++;
        }
        else{

            if (mainTreeView.currentChildIndex !== countChild - 1){
                mainTreeView.currentChildIndex++;
            }
            else if (mainTreeView.currentChildIndex == countChild - 1 &&
                     mainTreeView.currentParentIndex == treeViewContainer.countItems - 1){
                mainTreeView.currentParentIndex = 0;
                mainTreeView.currentChildIndex = -1;
            }
            else{
                mainTreeView.currentChildIndex = -1;
                mainTreeView.currentParentIndex++;
            }
        }

         console.log("PackageCollectionView selectedIndexIncr стало", mainTreeView.currentParentIndex, mainTreeView.currentChildIndex);
    }

    function selectedIndexDecr(){
        console.log("PackageCollectionView selectedIndexDecr", mainTreeView.currentParentIndex, mainTreeView.currentChildIndex);
        if (mainTreeView.currentParentIndex === -1){
            mainTreeView.currentParentIndex = treeViewContainer.countItems - 1;

            var modelChild = treeViewContainer.modelItems.GetData("childItemModel", mainTreeView.currentParentIndex);

            if (!modelChild){
                return;
            }

            mainTreeView.currentChildIndex = modelChild.GetItemsCount() - 1;
            return;
        }

        var isOpened = true;
        if (mainTreeView.currentParentIndex > 0){
            isOpened = treeViewContainer.modelItems.GetData("isOpened", mainTreeView.currentParentIndex - 1);
        }
        else{
            isOpened = treeViewContainer.modelItems.GetData("isOpened", treeViewContainer.countItems - 1);
        }

        if (mainTreeView.currentChildIndex == -1){
            mainTreeView.currentParentIndex--;
            if (isOpened){

                var modelChild = treeViewContainer.modelItems.GetData("childItemModel", mainTreeView.currentParentIndex);

                if (!modelChild){
                    return;
                }

                mainTreeView.currentChildIndex = modelChild.GetItemsCount() - 1;
            }
        }
        else if (mainTreeView.currentChildIndex == 0){
            mainTreeView.currentChildIndex = -1;
        }
        else{
            mainTreeView.currentChildIndex--;
        }
    }

    ListView {
        id: mainTreeView;

        anchors.fill: treeViewContainer;

        boundsBehavior: Flickable.StopAtBounds;
        model: treeViewContainer.modelItems;

        property int currentParentIndex: -1;
        property int currentChildIndex: -1;

//        delegate: TreeItemDelegate {
//            width: parent.width;
//        }

        delegate: TreeItemDelegateTest {
            width: parent.width;

            onIsOpenedWasChanged: {
                console.log("TreeItemDelegateTest onIsOpenedWasChanged");
                if (!state){
                    treeViewContainer.closeTreeItem(index);
                }
                else{
                    treeViewContainer.openTreeItem(index);
                }
            }

            onSetActiveFocusFromTreeItemDelegate: {
                console.log("TreeView TreeItemDelegateTest onSetActiveFocusFromTreeItemDelegate");

                if (!treeViewContainer.focus){
                    treeViewContainer.forceActiveFocus();
                }
            }
        }

        function changeCheckBoxState(state, packageId, featureId) {
            console.log("TreeView ListView onCheckBoxStateChanged()", state, packageId, featureId);
            treeViewContainer.itemTreeViewCheckBoxStateChanged(state, packageId, featureId);

//            treeViewContainer.setStateCheckBox(state);
        }

    }
}
