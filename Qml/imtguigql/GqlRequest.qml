import QtQuick 2.12

QtObject {
    id: root;

    property string json;
    property string state;

    function SetGqlQuery(gqlData){
        root.state = "Loading"

        let xhr = new XMLHttpRequest
        xhr.open("POST", "../../graphql")
        xhr.send(gqlData)

        xhr.onreadystatechange = function(){
            if (xhr.readyState === XMLHttpRequest.DONE){
                root.json = xhr.responseText;

                root.state = "Ready";
            }
        }
    }
}
