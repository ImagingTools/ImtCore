import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: treeViewContainer;
    color: Style.baseColor;

//    property alias modelItems: mainTreeView.model;
    property var modelItems;

    property int countItems: modelItems.GetItemsCount();

    signal itemTreeViewCheckBoxStateChanged(int state, string packageId, string featureId);

    onModelItemsChanged: {
    }

    Keys.onPressed: {
        console.log("TreeView keys pressed")
        if (event.key === Qt.Key_Tab){
            console.log('Key tab was pressed');
            thubnailDecoratorContainer.setFocusOnMenuPanel();
        }
        else if (event.key === Qt.Key_Up){
            console.log('Key up was pressed');
            treeViewContainer.selectedIndexDecr();
        }
        else if (event.key === Qt.Key_Down){
            console.log('Key down was pressed');

            treeViewContainer.selectedIndexIncr();
        }
        else if (event.key === Qt.Key_Left){
            console.log('Key left was pressed');
            if (mainTreeView.currentParentIndex != -1 && mainTreeView.currentChildIndex === -1){
                treeViewContainer.closeTreeItem(mainTreeView.currentParentIndex);
            }
        }
        else if (event.key === Qt.Key_Right){
            console.log('Key right was pressed');
            if (mainTreeView.currentParentIndex != -1 && mainTreeView.currentChildIndex === -1){
                treeViewContainer.openTreeItem(mainTreeView.currentParentIndex);
            }
        }
    }

    TreeItemModel {
        id: treeItemModel;
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
        console.log("PackageCollectionView selectedIndexIncr");

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

//        if (mainTreeView.currentChildIndex == -1){
//            mainTreeView.currentParentIndex--;

//            var modelChild = treeViewContainer.modelItems.GetData("childItemModel", mainTreeView.currentParentIndex);

//            if (!modelChild){
//                return;
//            }

//            mainTreeView.currentChildIndex = modelChild.GetItemsCount() - 1;
//        }
//        else if (mainTreeView.currentChildIndex > -1){
//            mainTreeView.currentChildIndex--;
//        }
//        else if (mainTreeView.currentChildIndex == 0){
//            mainTreeView.currentChildIndex = -1;
//        }

//        if (!isOpened){

//            if (mainTreeView.currentParentIndex > 0){
//                mainTreeView.currentParentIndex--;
//                mainTreeView.currentChildIndex = -1;
//            }
//            else{
//                mainTreeView.currentParentIndex = treeViewContainer.countItems - 1;
//            }
//        }
//        else{
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
//        }
    }

    ListView {
        id: mainTreeView;
        anchors.fill: treeViewContainer;
        boundsBehavior: Flickable.StopAtBounds;

        model: treeViewContainer.modelItems;

        onModelChanged: {
            console.log("TreeView ListView onModelChanged");

//            for (var i = 0; i < countItems; i++){
//                treeViewContainer.modelItems.SetData("isOpened", true, i)

//            }
        }

        property int currentParentIndex: -1;
        property int currentChildIndex: -1;

        delegate: TreeItemDelegateTest {
            width: parent.width;
            //childItemModel: model.childItemModel;

//            listViewItem: mainTreeView;

//            onCheckBoxState: {
//                console.log("TreeView onCheckBoxState", state, packageId, featureId);
//            }

            onIsOpenedWasChanged: {
                console.log("TreeItemDelegateTest onIsOpenedWasChanged");
                if (!state){
                    treeViewContainer.closeTreeItem(index);
                }
                else{
                    treeViewContainer.openTreeItem(index);
                }
            }
        }

        function changeCheckBoxState(state, packageId, featureId) {
            console.log("TreeView ListView onCheckBoxStateChanged()", state, packageId, featureId);
            treeViewContainer.itemTreeViewCheckBoxStateChanged(state, packageId, featureId);
        }

    }
}
