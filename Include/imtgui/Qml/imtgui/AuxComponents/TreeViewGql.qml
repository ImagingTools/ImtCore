import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0


Rectangle{
    id: treeViewGql;

    color: "transparent";
    property TreeItemModel model: TreeItemModel{};

    property int shift: 70;
    property string nameId: "Name";
    property alias delegateWidht: list.delegateWidth;
    property alias delegate: list.delegate;

    signal requestSignal(int index, int level);

    signal clicked(int index, int level);
    signal doubleClicked(int index, int level);

    function getIcon(type, isOpen){
        let source = "";
        let imageName = "";
        if(type == "Node"){
            imageName = isOpen ? "Icons/FolderOpened" : "Icons/FolderClosed";
        }
        else if(type == "Doc"){
            imageName = "Icons/New";
        }
        source  =  "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);
        return source;
    }


    Rectangle{
        id: listContainer;

        width: treeViewGql.width -10;
        height: treeViewGql.height -10;
        radius: 4;

        border.color: "lightgrey";

        ListView{
            id: list;

            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;

            boundsBehavior: Flickable.StopAtBounds;
            contentWidth: delegateWidth;
            clip: true;
            property int delegateWidth: 200;

            model: treeViewGql.model;

            delegate: Item{
                id: deleg;

                width: model.Visible ? model.Level * treeViewGql.shift + list.delegateWidth : 0;
                height: model.Visible ? 40 : 0;
                opacity: model.Visible;
                property bool isOpen: model.IsOpen;
                Rectangle{
                    anchors.left: parent.left;
                    anchors.leftMargin: model.Level * treeViewGql.shift;
                    width: list.delegateWidth;
                    height: parent.height;
                    color: "transparent";
                    AuxButton{
                        id: openButton;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 8;

                        width: 16;
                        height: width;
                        radius: width;

                        visible: model.TypeId == undefined ? false : model.TypeId == "Node" ? true : false;
                        enabled: visible;

                        color: "transparent";

                        border.color: "transparent";
                        border.width: 0;
                        hasIcon: true;
                        property string imageName: deleg.isOpen ? "Icons/Down" : "Icons/Right";
                        iconSource:  "../../../" +Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);
                        iconWidth: width;
                        iconHeight: width;

                        onClicked: {
                            if(!deleg.isOpen){
                                if(!model.HasBranch){
                                    treeViewGql.model.SetData("HasBranch", true, model.index);
                                    treeViewGql.requestSignal(model.index, model.Level)
                                }
                                else {
                                    setVisibleElements(true, model.index)
                                }
                                treeViewGql.model.SetData("IsOpen", true, model.index);

                            }
                            else if(deleg.isOpen){
                                treeViewGql.model.SetData("IsOpen", false, model.index);
                                setVisibleElements(false, model.index)
                            }

                        }
                    }

                    Image{
                        id: folderImage;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: openButton.right;
                        anchors.leftMargin: 8;

                        visible: model.TypeId !== undefined;
                        width: 16;
                        height: width;
                        sourceSize.width: width;
                        sourceSize.height: height;
                        source: treeViewGql.getIcon(model.TypeId, deleg.isOpen);
                    }

                    Text{
                        id: nameText;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: model.TypeId == undefined ? folderImage.left : folderImage.right;
                        anchors.leftMargin: model.TypeId == undefined ? 0 : 16;
                        anchors.right: parent.right;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_subtitle !==undefined ? Style.fontSize_subtitle : 18;
                        color: Style.textColor;

                        text: model[treeViewGql.nameId];
                    }
                    MouseArea{
                        id: delegateMA;

                        anchors.top: parent.top;
                        anchors.bottom: parent.bottom;
                        anchors.right: parent.right;
                        anchors.left: nameText.left;

                        hoverEnabled: visible;
                        cursorShape: Qt.PointingHandCursor;
                        onClicked: {
                            treeViewGql.clicked(model.index, model.Level);
                        }
                        onDoubleClicked: {
                            treeViewGql.doubleClicked(model.index, model.Level);
                        }
                    }
                }



            }//delegate

        }//list

        CustomScrollbar{
            id: scrollVert;

            anchors.left: list.right;
            anchors.leftMargin: 1;

            anchors.bottom: list.bottom;

            secondSize: 12;

            targetItem: list;
        }

        CustomScrollbar{
            id: scrollHoriz;

            anchors.right: list.right;
            anchors.top: list.bottom;
            anchors.topMargin: 1 ;

            secondSize: 12;

            vertical: false;
            targetItem: list;
        }
    }//listContainer


    function setContentWidth(){
        let maxWidth = 0;
        for(let i = 0; i < treeViewGql.model.GetItemsCount(); i++){
            let level = treeViewGql.model.GetData("Level",i);
            let visible = treeViewGql.model.GetData("Visible",i);
            let width_ = !visible ? 0 : level * treeViewGql.shift + list.delegateWidth;
            if(width_ > maxWidth){
                maxWidth = width_;
            }
        }

        list.contentWidth = maxWidth;
        if(list.contentWidth > list.width){
            list.contentX = list.contentWidth - list.width + list.originX;
        }
        else {
            list.contentX = list.originX;
        }
    }

    function setVisibleElements(visible, index){
        console.log("SET VISIBLE", visible, index);
        let innerId = treeViewGql.model.GetData("InnerId", index);
        let found = false;
        let foundChangeCount = 0;
        for(let i = 0; i < treeViewGql.model.GetItemsCount(); i++){
            let branchIds = treeViewGql.model.IsValidData("BranchIds", i) ? treeViewGql.model.GetData("BranchIds", i) : "";
            //console.log("branchIds:: ", branchIds)
            let ok = false;
            let arr = branchIds.split(",");
            let arrCounter = 0;
            for(let k = 0; k < arr.length; k++){
                if(arr[k] == innerId){
                    ok = true;
                    if(!found){
                        found = true;
                        foundChangeCount = 1;
                    }
                    break;
                }
                arrCounter++;
            }
            if(arrCounter == arr.length && found){
                foundChangeCount = 2;
            }

            if(foundChangeCount == 2){
                console.log("StopINdex::", i);
                break;
            }

            //
            if(ok){
                treeViewGql.model.SetData("Visible", visible, i);
            }
        }

        setContentWidth();

    }

    function insertTree(index, level, model_){
        console.log("INSERT TREE", index, level);

        let date = new Date();
        let val = date.valueOf();

        let branchIds_parent = treeViewGql.model.IsValidData("BranchIds", index) ? treeViewGql.model.GetData("BranchIds", index) : "";
        let innerId_parent = treeViewGql.model.IsValidData("InnerId", index) ? treeViewGql.model.GetData("InnerId", index) : "";
        let branchIds = branchIds_parent !== "" ? branchIds_parent + "," + innerId_parent: innerId_parent;


        for(let i = 0; i < model_.GetItemsCount(); i++){
            let newIndex =  index + i + 1;
            treeViewGql.model.InsertNewItem(newIndex);
            treeViewGql.model.CopyItemDataFromModel(newIndex, model_, i);
            treeViewGql.model.SetData("Level",level + 1, newIndex);
            treeViewGql.model.SetData("BranchIds", branchIds, newIndex);
            treeViewGql.model.SetData("Visible", true, newIndex);
            treeViewGql.model.SetData("IsOpen", false, newIndex);
            treeViewGql.model.SetData("HasBranch", false, newIndex);
            treeViewGql.model.SetData("InnerId", String(val + newIndex), newIndex);

        }
        setContentWidth();
    }


}
