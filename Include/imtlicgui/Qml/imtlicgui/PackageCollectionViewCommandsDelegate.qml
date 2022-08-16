import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    Component.onCompleted: {
//        collectionViewBase.commands.onItemsChanged.connect(updateTreeView);
    }

    function updateTreeView(){
//        Events.sendEvent("TreeViewModelUpdate");

//        updateGui();
    }
}
