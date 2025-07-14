const Item = require('./Item')
const Property = require('../QtQml/Property')
const String = require('../QtQml/String')
const Color = require('../QtQml/Color')
const Real = require('../QtQml/Real')
const Geometry = require('../QtQml/Geometry')
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

        // let textMetrics = JQApplication.TextController.measureText(this.text, this.font, this.width, this.wrapMode, this.textFormat, this.elide)

        if(isHTML){
            this.__impl.innerHTML = this.text.replaceAll('<br>', '\r')
        } else {
            this.__impl.innerText = this.text.replaceAll('<br>', '\r')
        }

        //temp
        if(this.wrapMode === Text.NoWrap && this.elide === Text.ElideNone && !isHTML){
            let textMetrics = JQApplication.TextController.measureTextFast(this.text, this.font)

            this.contentWidth = textMetrics.width
            this.contentHeight = textMetrics.height

            Geometry.setAuto(this.__self, 'height', textMetrics.height, this.__self.constructor.meta.height)
            Geometry.setAuto(this.__self, 'width', textMetrics.width, this.__self.constructor.meta.width)
        } else {
            let textMetrics = JQApplication.TextController.measureText(this.text, this.font, !this.width__prevent ? 0 : this.width, this.wrapMode, isHTML, this.elide)

            this.contentWidth = textMetrics.width
            this.contentHeight = textMetrics.height

            Geometry.setAuto(this.__self, 'height', textMetrics.height, this.__self.constructor.meta.height)
            Geometry.setAuto(this.__self, 'width', textMetrics.width, this.__self.constructor.meta.width)
        }
    }

    SLOT_widthChanged(oldValue, newValue){
        super.SLOT_widthChanged(oldValue, newValue)

        JQApplication.updateLater(this)
    }

    SLOT_textChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_colorChanged(oldValue, newValue){
        let rgba = Color.getRGBA(this.__proxy, 'color', this.__self.constructor.meta)
        this.__setDOMStyle({
            opacity: 1,
            color: `rgba(${rgba.r},${rgba.g},${rgba.b},${newValue === 'transparent' ? 0 : rgba.a * this.__proxy.opacity})`
        })
    }

    SLOT_opacityChanged(oldValue, newValue){
        let rgba = Color.getRGBA(this.__proxy, 'color', this.__self.constructor.meta)
        this.__setDOMStyle({
            opacity: 1,
            color: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.__proxy.color === 'transparent' ? 0 : rgba.a * newValue})`
        })
    }

    SLOT_visibleChanged(oldValue, newValue){
        super.SLOT_visibleChanged()

        JQApplication.updateLater(this)
    }

    SLOT_elideChanged(oldValue, newValue){
        if(newValue === Text.ElideRight){
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

        JQApplication.updateLater(this)
    }

    SLOT_horizontalAlignmentChanged(oldValue, newValue){
        switch(newValue){
            case Text.AlignLeft: this.__setDOMStyle({ justifyContent: 'flex-start', textAlign: 'start' }); break;
            case Text.AlignRight: this.__setDOMStyle({ justifyContent: 'flex-end', textAlign: 'end' }); break;
            case Text.AlignHCenter: this.__setDOMStyle({ justifyContent: 'center', textAlign: 'center' }); break;
            case Text.AlignJustify: this.__setDOMStyle({ justifyContent: 'normal', textAlign: 'justify' }); break;
        }

        JQApplication.updateLater(this)
    }

    SLOT_verticalAlignmentChanged(oldValue, newValue){
        switch(newValue){
            case Text.AlignTop: this.__setDOMStyle({ alignItems: 'flex-start' }); break;
            case Text.AlignBottom: this.__setDOMStyle({ alignItems: 'flex-end' }); break;
            case Text.AlignVCenter: this.__setDOMStyle({ alignItems: 'center' }); break;
        }

        JQApplication.updateLater(this)
    }

    SLOT_wrapModeChanged(oldValue, newValue){
        switch(newValue){
            case Text.NoWrap: this.__setDOMStyle({ whiteSpace: 'pre' }); break;
            case Text.WordWrap: this.__setDOMStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-word' }); break;
            case Text.WrapAnywhere: this.__setDOMStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-all' }); break;
            case Text.Wrap: this.__setDOMStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-word' }); break;
            case Text.WrapAtWordBoundaryOrAnywhere: this.__setDOMStyle({ whiteSpace: 'pre-wrap', wordBreak: 'break-word' }); break;
        }

        JQApplication.updateLater(this)
    }

    onFontChanged(oldValue, newValue){
        let font = this.font
        this.__setDOMStyle({
            fontWeight: font.bold == true ? 'bold' : 'normal',
            fontSize: font.pixelSize+'px',
            fontFamily: `'${font.family}'`,
            textDecoration: font.underline == true ? 'underline' : 'unset',
        })

        JQApplication.updateLater(this)
    }



    __endUpdate(){
        super.__endUpdate()
        this.__updateGeometry()
    }
}



module.exports = Text