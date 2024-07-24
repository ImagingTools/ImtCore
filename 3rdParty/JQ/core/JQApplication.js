const MouseController = require('./MouseController')

module.exports = {
    root: null,

    init: function(){ 
        global.JQModules = {
            QtBase: require('../QtBase/QtBase'),
            Qt: require('../Qt/Qt'),
            QtQml: require('../QtQml/QtQml'),
            QtQuick: require('../QtQuick/QtQuick'),
            Qt5Compat: require('../Qt5Compat/Qt5Compat'),
            QtWebSockets: require('../QtWebSockets/QtWebSockets'),
        }

        
        document.head.insertAdjacentHTML("beforeend", `
        <style>
            *{padding:0;margin:0;border:0;overflow:hidden;-ms-overflow-style:none;scrollbar-width:none;user-select:none;-webkit-user-select: none;}
            *::-webkit-scrollbar{width:0;height:0;}
            *,*:before,*:after{-moz-box-sizing:border-box;-webkit-box-sizing:border-box;box-sizing:border-box;}
            :focus,:active{outline:none;}
            a:focus,a:active{outline:none;}
            nav,footer,header,aside{display:block;}
            html,body{height:100%;width:100%;font-size:100%;line-height:1;font-size:14px;-ms-text-size-adjust:100%;-moz-text-size-adjust:100%;-webkit-text-size-adjust:100%;}
            input,button,textarea{font-family:inherit;}
            input::-ms-clear{display:none;}
            button{cursor:pointer;}
            button::-moz-focus-inner{padding:0;border:0;}
            a,a:visited{text-decoration:none;}
            a:hover{text-decoration:none;}
            ul li{list-style:none;}
            img{vertical-align:top;}
            h1,h2,h3,h4,h5,h6{font-size:inherit;font-weight:inherit;}
            br {}

            .Item {
                position: absolute;
                display: flex;
                overflow: unset;
            }
            .Rectangle {
                background-color: white;
            }
            .Repeater {
                display: none;
            }
            .Text, .TextInput, .TextEdit {
                text-align: inherit;
                white-space: pre;
                line-height: normal;
                font-size: 14px;
                font-family: 'Segoe UI';
            }
            .TextInput, .TextEdit {
                flex-direction: column;
            }

            .Row {
                flex-direction: row;
            }
            .Column {
                flex-direction: column;
            }
            .Row > *{
                position: relative;
            }
            .Column > * {
                position: relative;
            }
            .Row > *[no-view] {
                display: none;
            }
            .Column > *[no-view] {
                display: none;
            }


            *[invisible] {
                display: none;
            }
        </style>`)

        this.root = JQModules.QtQuick.Item.create()

        window.addEventListener('resize', ()=>{
            this.root.width = window.innerWidth
            this.root.height = window.innerHeight
        })

        window.addEventListener('load', ()=>{
            this.root.width = window.innerWidth
            this.root.height = window.innerHeight
            this.root.__connectDOM(document.body)
        })

        MouseController.init()
    }
}

