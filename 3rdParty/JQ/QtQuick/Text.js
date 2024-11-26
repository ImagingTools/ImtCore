const Item = require('./Item')
const Property = require('../QtQml/Property')
const String = require('../QtQml/String')
const Color = require('../QtQml/Color')
const Real = require('../QtQml/Real')
const Font = require('../QtQml/Font')
const Signal = require('../QtQml/Signal')

const tags = ['<a>','<abbr>','<address>','<area>','<article>','<aside>','<audio>','<b>','<base>','<bdi>','<bdo>','<blockquote>','<body>','<br>','<button>','<canvas>','<caption>','<cite>','<code>','<col>','<colgroup>','<data>','<datalist>','<dd>','<del>','<details>','<dfn>','<dialog>','<div>','<dl>','<dt>','<em>','<embed>','<fieldset>','<figcaption>','<figure>','<footer>','<form>','<h1>','<h2>','<h3>','<h4>','<h5>','<h6>','<head>','<header>','<hr>','<html>','<i>','<iframe>','<img>','<input>','<ins>','<kbd>','<label>','<legend>','<li>','<link>','<main>','<map>','<mark>','<meta>','<meter>','<nav>','<noscript>','<object>','<ol>','<optgroup>','<option>','<output>','<p>','<param>','<picture>','<pre>','<progress>','<q>','<ruby>','<rb>','<rt>','<rtc>','<rp>','<s>','<samp>','<script>','<section>','<select>','<small>','<source>','<span>','<strong>','<style>','<sub>','<summary>','<sup>','<table>','<tbody>','<td>','<template>','<textarea>','<tfoot>','<th>','<thead>','<time>','<title>','<tr>','<track>','<u>','<ul>','<var>','<video>','<wbr>']
const regexp = /<[^<>]+>/g

class Text extends Item {
    static AlignLeft = 0
    static AlignRight = 1
    static AlignHCenter = 2
    static AlignJustify = 3
    static AlignTop = 4
    static AlignBottom = 5
    static AlignVCenter = 6

    static NoWrap = 0
    static WordWrap = 1
    static WrapAnywhere = 2
    static Wrap = 3
    static WrapAtWordBoundaryOrAnywhere = 4

    static AutoText = 0
    static PlainText = 1
    static StyledText = 2
    static RichText = 3
    static MarkdownText = 4

    static ElideNone = 0
    static ElideLeft = 1
    static ElideMiddle = 2
    static ElideRight = 3

    static html = []

    static meta = Object.assign({}, Item.meta, {
        text: { type: String, value: '', signalName: 'textChanged' },
        color: { type: Color, value: 'black', signalName: 'colorChanged' },
        font: { type: Font, signalName: 'fontChanged' },
        contentWidth: { type: Real, value: 0, signalName: 'contentWidthChanged' },
        contentHeight: { type: Real, value: 0, signalName: 'contentHeightChanged' },
        horizontalAlignment: { type: Property, value: Text.AlignLeft, signalName: 'horizontalAlignmentChanged' },
        verticalAlignment: { type: Property, value: Text.AlignTop, signalName: 'verticalAlignmentChanged' },
        wrapMode: { type: Property, value: Text.NoWrap, signalName: 'wrapModeChanged' },
        textFormat: { type: Property, value: Text.AutoText, signalName: 'textFormatChanged' },
        elide: { type: Property, value: Text.ElideNone, signalName: 'elideChanged' },
        lineHeight: { type: Real, value: 0, signalName: 'lineHeightChanged' },

        textChanged: {type:Signal, slotName:'onTextChanged', args:[]},
        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
        fontChanged: {type:Signal, slotName:'onFontChanged', args:[]},
        contentWidthChanged: {type:Signal, slotName:'onContentWidthChanged', args:[]},
        contentHeightChanged: {type:Signal, slotName:'onContentHeightChanged', args:[]},
        horizontalAlignmentChanged: {type:Signal, slotName:'onHorizontalAlignmentChanged', args:[]},
        verticalAlignmentChanged: {type:Signal, slotName:'onVerticalAlignmentChanged', args:[]},
        wrapModeChanged: {type:Signal, slotName:'onWrapModeChanged', args:[]},
        textFormatChanged: {type:Signal, slotName:'onTextFormatChanged', args:[]},
        elideChanged: {type:Signal, slotName:'onElideChanged', args:[]},
        lineHeightChanged: {type:Signal, slotName:'onLineHeightChanged', args:[]},

        lineLaidOut: {type:Signal, slotName:'onLineLaidOut', args:['line']},
        linkActivated: {type:Signal, slotName:'onLinkActivated', args:['link']},
        LinkHovered: {type:Signal, slotName:'onLinkHovered', args:['link']},
    })


    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.__DOM.classList.add('Text')
        obj.__createImpl()

        return obj
    }

    __createImpl(){
        let dom = this.__getDOM()
        let impl = document.createElement('div')
        impl.classList.add('impl')
        dom.appendChild(impl)
        this.__impl = impl
    }

    __setImplStyle(style){
        if(this.__impl) {
            for(let name in style){
                this.__impl.style[name] = style[name]
            }
        }
    }

    __updateGeometry(){
        let isHTML = false
        if(this.textFormat === undefined || this.textFormat === Text.PlainText){
            isHTML = false
        } else if(this.textFormat === Text.AutoText){
            isHTML = false
            let result = this.text.match(regexp)
            if(result){
                for(let res of result){
                    if(tags.indexOf(res) >= 0){
                        isHTML = true
                        break
                    }
                }
                
            } else {
                isHTML = false
            }
        } else {
            isHTML = true
        }

        // let textMetrics = JQApplication.TextController.measureText(this.text, this.font, this.__getDataQml('width').__auto ? 0 : this.width, this.wrapMode, this.textFormat, this.elide)

        if(isHTML){
            this.__impl.innerHTML = this.text.replaceAll('<br>', '\r')
        } else {
            this.__impl.innerText = this.text.replaceAll('<br>', '\r')
        }

        if(this.wrapMode === Text.NoWrap && this.elide === Text.ElideNone && !isHTML){
            let textMetrics = JQApplication.TextController.measureTextFast(this.text, this.font)

            this.contentWidth = textMetrics.width
            this.contentHeight = textMetrics.height

            this.__getDataQml('height').__setAuto(textMetrics.height)
            this.__getDataQml('width').__setAuto(textMetrics.width)
        } else {
            let textMetrics = JQApplication.TextController.measureText(this.text, this.font, this.__getDataQml('width').__auto ? 0 : this.width, this.wrapMode, isHTML, this.elide)

            this.contentWidth = textMetrics.width
            this.contentHeight = textMetrics.height

            this.__getDataQml('height').__setAuto(textMetrics.height)
            this.__getDataQml('width').__setAuto(textMetrics.width)
        }

        
        
        // if(this.__getDataQml('width').__auto){
        //     this.contentWidth = Math.max(textMetrics.width, this.__impl.scrollWidth)
        //     this.contentHeight = Math.max(textMetrics.height, this.__impl.scrollHeight)
        // } else {
        //     this.contentWidth = textMetrics.width
        //     this.contentHeight = textMetrics.height
        // }
        
    }

    onWidthChanged(){
        super.onWidthChanged()

        this.__updateGeometry()
    }

    onTextChanged(){
        this.__updateGeometry()
    }

    onColorChanged(){
        let rgba = this.__getDataQml('color').__toRGBA()
        this.__setDOMStyle({
            opacity: 1,
            color: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity})`
        })
    }

    onOpacityChanged(){
        let rgba = this.__getDataQml('color').__toRGBA()
        this.__setDOMStyle({
            opacity: 1,
            color: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity})`
        })
    }

    onVisibleChanged(){
        super.onVisibleChanged()

        this.__updateGeometry()
    }

    onElideChanged(){
        if(this.elide === Text.ElideRight){
            this.__setImplStyle({
                textOverflow: 'ellipsis',
                overflow: 'auto',
            })
        } else {
            this.__setImplStyle({
                textOverflow: 'unset',
                overflow: 'unset',
            })
        }

        this.__updateGeometry()
    }

    onHorizontalAlignmentChanged(){
        switch(this.horizontalAlignment){
            case Text.AlignLeft: this.__setDOMStyle({ justifyContent: 'flex-start', textAlign: 'start' }); break;
            case Text.AlignRight: this.__setDOMStyle({ justifyContent: 'flex-end', textAlign: 'end' }); break;
            case Text.AlignHCenter: this.__setDOMStyle({ justifyContent: 'center', textAlign: 'center' }); break;
            case Text.AlignJustify: this.__setDOMStyle({ justifyContent: 'normal', textAlign: 'justify' }); break;
        }

        this.__updateGeometry()
    }

    onVerticalAlignmentChanged(){
        switch(this.verticalAlignment){
            case Text.AlignTop: this.__setDOMStyle({ alignItems: 'flex-start' }); break;
            case Text.AlignBottom: this.__setDOMStyle({ alignItems: 'flex-end' }); break;
            case Text.AlignVCenter: this.__setDOMStyle({ alignItems: 'center' }); break;
        }

        this.__updateGeometry()
    }

    onWrapModeChanged(){
        switch(this.wrapMode){
            case Text.NoWrap: this.__setDOMStyle({ whiteSpace: 'pre' }); break;
            case Text.WordWrap: this.__setDOMStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-word' }); break;
            case Text.WrapAnywhere: this.__setDOMStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-all' }); break;
            case Text.Wrap: this.__setDOMStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-word' }); break;
            case Text.WrapAtWordBoundaryOrAnywhere: this.__setDOMStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-word' }); break;
        }

        this.__updateGeometry()
    }

    onFontChanged(){
        this.__setDOMStyle({
            fontWeight: this.font.bold == true ? 'bold' : 'normal',
            fontSize: this.font.pixelSize+'px',
            fontFamily: `'${this.font.family}'`,
            textDecoration: this.font.underline == true ? 'underline' : 'unset',
        })

        this.__updateGeometry()
    }
}

Text.initialize()

module.exports = Text