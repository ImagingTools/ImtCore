import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0


Rectangle{
    id: treeViewGql;

    color: "transparent";
    property TreeItemModel model: TreeItemModel{};

    property int shift: 70;
    property alias delegateWidht: list.delegateWidth;
    property alias delegate: list.delegate;

    signal requestSignal(int index, int level);

    Component.onCompleted: {//TEST
        let date = new Date();
        let val = date.valueOf();
        let index;

        index = testInsertModelLevel0.InsertNewItem();
        testInsertModelLevel0.SetData("Id", String(val + index), index);
        testInsertModelLevel0.SetData("Name","ул. Ленина", index);
        testInsertModelLevel0.SetData("HasChildren", true, index);

        index = testInsertModelLevel0.InsertNewItem();
        testInsertModelLevel0.SetData("Id", String(val + index), index);
        testInsertModelLevel0.SetData("Name","ул. Маркса", index);
        testInsertModelLevel0.SetData("HasChildren", true, index);

        index = testInsertModelLevel0.InsertNewItem();
        testInsertModelLevel0.SetData("Id", String(val + index), index);
        testInsertModelLevel0.SetData("Name","ул. Менделеева", index);
        testInsertModelLevel0.SetData("HasChildren", true, index);

        index = testInsertModelLevel0.InsertNewItem();
        testInsertModelLevel0.SetData("Id", String(val + index), index);
        testInsertModelLevel0.SetData("Name","ул. Королёва", index);
        testInsertModelLevel0.SetData("HasChildren", true, index);

        index = testInsertModelLevel0.InsertNewItem();
        testInsertModelLevel0.SetData("Id", String(val + index), index);
        testInsertModelLevel0.SetData("Name","ул. Гагарина", index);
        testInsertModelLevel0.SetData("HasChildren", true, index);

        index = testInsertModelLevel0.InsertNewItem();
        testInsertModelLevel0.SetData("Id", String(val + index), index);
        testInsertModelLevel0.SetData("Name","ул. Мира", index);
        testInsertModelLevel0.SetData("HasChildren", false, index);

        insertTree(-1,-1, testInsertModelLevel0);//!!!

        index = testInsertModel.InsertNewItem();
        testInsertModel.SetData("Name", "дом 1" , index);
        testInsertModel.SetData("HasChildren", true, index);

        index = testInsertModel.InsertNewItem();
        testInsertModel.SetData("Name","дом 2" , index);
        testInsertModel.SetData("HasChildren", true, index);

        index = testInsertModel.InsertNewItem();
        testInsertModel.SetData("Name", "дом 3" , index);
        testInsertModel.SetData("HasChildren", false, index);


    }

    TreeItemModel{
        id: testInsertModel;
    }

    TreeItemModel{
        id: testInsertModelLevel0;
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

                width: visible ? model.Level * treeViewGql.shift + list.delegateWidth : 0;
                height: visible ? 40 : 0;
                visible: model.Visible;
                property bool isOpen: false;
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
                        anchors.leftMargin: Style.size_smallMargin;

                        width: 16;
                        height: width;
                        radius: width;

                        visible: model.HasChildren == undefined ? false : model.HasChildren;
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

                        }
                    }

                    Image{
                        id: folderImage;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: openButton.right;
                        anchors.leftMargin: Style.size_smallMargin;

                        visible: model.HasChildren == undefined ? false : model.HasChildren;

                        width: 16;
                        height: width;
                        sourceSize.width: width;
                        sourceSize.height: height;
                        property string imageName: deleg.isOpen ? "Icons/FolderOpened" : "Icons/FolderClosed";

                        source:  "../../../" +Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);

                    }

                    Text{
                        id: nameText;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: model.HasChildren == undefined ? folderImage.right : model.HasChildren ? folderImage.right : folderImage.left;
                        anchors.leftMargin: model.HasChildren == undefined ? 0 : model.HasChildren ? Style.size_mainMargin : 0;
                        anchors.right: parent.right;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_subtitle;
                        color: Style.textColor;

                        text: model.Name;
                    }
                    MouseArea{
                        anchors.fill: parent;

                        visible: model.HasChildren == undefined ? false : model.HasChildren;

                        onClicked: {
                            if(!deleg.isOpen){
                                if(!model.HasBranch){
                                    treeViewGql.model.SetData("HasBranch", true, model.index);
                                    treeViewGql.requestSignal(model.index, model.Level)
                                    insertTree(model.index, model.Level, testInsertModel);
                                }
                                else {
                                    setVisibleElements(true, model.index)
                                }
                                deleg.isOpen = true;
                            }
                            else if(deleg.isOpen){
                                deleg.isOpen = false;
                                setVisibleElements(false, model.index)
                            }

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
        let id = treeViewGql.model.GetData("Id", index);
        let found = false;
        let foundChangeCount = 0;
        for(let i = 0; i < treeViewGql.model.GetItemsCount(); i++){
            let parentId = treeViewGql.model.IsValidData("ParentId", i) ? treeViewGql.model.GetData("ParentId", i) : "";
            //console.log("parentId:: ", parentId)
            let ok = false;
            let arr = parentId.split(",");
            let arrCounter = 0;
            for(let k = 0; k < arr.length; k++){
                if(arr[k] == id){
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
        let parentId_parent = treeViewGql.model.IsValidData("ParentId", index) ? treeViewGql.model.GetData("ParentId", index) : "";
        let id_parent = treeViewGql.model.GetData("Id", index);
        let parentId = parentId_parent !== "" ? parentId_parent + "," + id_parent: id_parent;

        let date = new Date();
        let val = date.valueOf();
        for(let i = 0; i < model_.GetItemsCount(); i++){
            let newIndex =  index + i + 1;
            treeViewGql.model.InsertNewItem(newIndex);
            treeViewGql.model.CopyItemDataFromModel(newIndex, model_, i);
            treeViewGql.model.SetData("Level",level + 1, newIndex);
            treeViewGql.model.SetData("ParentId", parentId, newIndex);
            treeViewGql.model.SetData("Visible", true, newIndex);
            treeViewGql.model.SetData("HasBranch", false, newIndex);

            //УБРАТЬ!!!(TEST)
            //console.log(String(val + newIndex))
            treeViewGql.model.SetData("Id", String(val + newIndex), newIndex);

        }
        setContentWidth();
    }


}
