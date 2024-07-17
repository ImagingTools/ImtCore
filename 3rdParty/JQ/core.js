const QtBase = require('./QtBase/QtBase')
const Qt = require('./Qt/Qt')
const QtQml = require('./QtQml/QtQml')
const QtQuick = require('./QtQuick/QtQuick')
const Qt5Compat = require('./Qt5Compat/Qt5Compat')
const QtWebSockets = require('./QtWebSockets/QtWebSockets')

global.QtBase = QtBase
global.Qt = Qt
global.QtQml = QtQml
global.QtQuick = QtQuick
global.Qt5Compat = Qt5Compat
global.QtWebSockets = QtWebSockets

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

    </style>`)