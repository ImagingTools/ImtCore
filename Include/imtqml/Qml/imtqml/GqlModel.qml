import QtQuick 2.0

TreeItemModel {
    id: container;
    
    function SetGqlQuery(gqlData){
        console.log("SetGqlQuery",gqlData)
        this.state = "Loading"
        var xhr = new XMLHttpRequest;
        xhr.open("POST", "../../graphql");
        xhr.send(gqlData);

        xhr.onreadystatechange = () => {
//        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText;
                this.updateTreeItemJSONModel()
                this.state = "Ready"
            }
        }
    }

    function updateTreeItemJSONModel() {
        if ( this.json === "" )
            return;

        this.clear();
        var d1 = new Date()
        var objectArray = this.parseJSONString(this.json, this.inquiry);
        var dict = {};
        for ( var key in objectArray ) {
            var jo = objectArray[key];
            if(typeof jo === 'object' && jo._qmlName !== 'TreeItemModel.qml'){
                var retModel
                retModel = this.createComponent("imtqml/TreeItemModel.qml", this);
                retModel.append(jo);
                jo = retModel
            }
            dict[key] = jo
        }
            this.append(dict);
        var d2 = new Date()
        console.log(d2.getMilliseconds() - d1.getMilliseconds())
    }
}
