import QtQuick 2.0

QtObject {
    signal dataChanged(string name, var sender)
    property bool enableNotifications: true
    property var owner

    Component.onCompleted: {
        connectProperties();
    }

    onDataChanged: {
        if(owner && owner.enableNotifications && owner.dataChanged){
            owner.dataChanged(name, sender)
        }
    }

    function connectProperties(){
        let self = this
        let list = getProperties()

        for(let name of list){
            this[name+'Changed'].connect(function(){
                if(self.enableNotifications)
                    self.dataChanged(name, self)
            }
           )
        }
    }

    function createMe(){
        return Qt.createComponent('BaseClass.qml').createObject()
    }

    function IsEqualWithModel(model){
        if (typeof this != typeof model){
            return false;
        }

        let selfKeys = this.getProperties()
        let sourceKeys = model.getProperties()

        console.log("self", this.ToJson())
        console.log("source", model.ToJson())


        if (selfKeys.length !== sourceKeys.length){
            return false;
        }

        for(let i = 0; i < selfKeys.length; i++){
            let key = selfKeys[i];

            if (!sourceKeys.includes(key)){
                return false;
            }

            if(typeof this[key] !== typeof model[key]){
                return false;
            }

            if(typeof this[key] === 'object'){
                let ok = this[key].IsEqualWithModel(model[key]);
                if (!ok){
                    return false;
                }
            }
            else{
                if (this[key] !== model[key]){
                    return false;
                }
            }
        }

        return true;
    }

    function Refresh(){
    }

    function Copy(item){
        return copyFrom(item)
    }

    function CopyMe(){
        let obj = createMe()
        obj.fromJSON(ToJson())
        return obj
    }

    function copyFrom(item){
        fromJSON(item.ToJson())
        return true
    }

    function copyTo(item){
        item.fromJSON(ToJson())
        return true
    }

    function createComponent(name){

    }

    function getJSONKeyForProperty(propertyId){
        return propertyId
    }

    function getProperties(){
        let list = []
        if(Qt.platform.os === 'web'){
            for(let key in this.$properties){
                if(key.indexOf('m_') >= 0 && key !== 'owner' && key !== 'objectName' && key !== 'enableNotifications' && typeof this[key] !== "function"){
                    list.push(key)
                }
            }
        } else {
            for(let key in this){
                if(key !== 'objectName' && key !== 'owner' && key !== 'enableNotifications' && typeof this[key] !== "function"){
                    list.push(key)
                }
            }
        }

        return list
    }

    function CreateFromJson(json){
        return fromJSON(json);
    }

    function ToJson(){
        let list = getProperties()

        let json = '{'
        for(let i = 0; i < list.length; i++){
            let key = list[i]
            if(typeof this[key] === 'object'){
                json += '"' + this.getJSONKeyForProperty(key) + '":' + this[key].ToJson()
            } else {
                json += '"' + this.getJSONKeyForProperty(key) + '":' + (typeof this[key] === 'string' ? '"' + this[key] + '"' : this[key])
            }
            if(i < list.length - 1) json += ','
        }
        json +='}'
        return json
    }

    function fromJSON(json){
        return fromObject(JSON.parse(json))
    }

    function fromObject(sourceObject){
        for(let key in sourceObject){
            let _key = "m_" + key
            if(typeof sourceObject[key] === "object"){
                if(Array.isArray(sourceObject[key])){
                    if(this[_key]){
                        this[_key].clear()
                    } else {
                        let obj = Qt.createComponent('BaseModel.qml').createObject(this)
                        this[_key] = obj
                    }
                    for(let sourceObjectInner of sourceObject[key]){
                        let obj = createComponent(_key).createObject(this)
                        obj.fromObject(sourceObjectInner)
                        this[_key].append({item: obj})
                        obj.owner = this
                    }
                } else {
                    let obj = createComponent(_key).createObject(this)
                    obj.fromObject(sourceObject[key])
                    this[_key] = obj
                    obj.owner = this
                }
            } else {
                this[_key] = sourceObject[key]
            }

        }
    }
}
