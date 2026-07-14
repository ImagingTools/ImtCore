const NoWrap = 0
const WordWrap = 1
const WrapAnywhere = 2
const Wrap = 3
const WrapAtWordBoundaryOrAnywhere = 4

const AutoText = 0
const PlainText = 1
const StyledText = 2
const RichText = 3
const MarkdownText = 4

const ElideNone = 0
const ElideRight = 3

module.exports = {
    tags: ['<a>','<abbr>','<address>','<area>','<article>','<aside>','<audio>','<b>','<base>','<bdi>','<bdo>','<blockquote>','<body>','<br>','<button>','<canvas>','<caption>','<cite>','<code>','<col>','<colgroup>','<data>','<datalist>','<dd>','<del>','<details>','<dfn>','<dialog>','<div>','<dl>','<dt>','<em>','<embed>','<fieldset>','<figcaption>','<figure>','<footer>','<form>','<h1>','<h2>','<h3>','<h4>','<h5>','<h6>','<head>','<header>','<hr>','<html>','<i>','<iframe>','<img>','<input>','<ins>','<kbd>','<label>','<legend>','<li>','<link>','<main>','<map>','<mark>','<meta>','<meter>','<nav>','<noscript>','<object>','<ol>','<optgroup>','<option>','<output>','<p>','<param>','<picture>','<pre>','<progress>','<q>','<ruby>','<rb>','<rt>','<rtc>','<rp>','<s>','<samp>','<script>','<section>','<select>','<small>','<source>','<span>','<strong>','<style>','<sub>','<summary>','<sup>','<table>','<tbody>','<td>','<template>','<textarea>','<tfoot>','<th>','<thead>','<time>','<title>','<tr>','<track>','<u>','<ul>','<var>','<video>','<wbr>'],
    regexp: /<[^<>]+>/g,

    getFontFamily(font){
        if(font && typeof font.family === 'string'){
            let family = font.family.trim()
            if(family) return family
        }
        return 'sans-serif'
    },

    init: function(){
        this.container = document.createElement('div')
        this.container.style.position = 'absolute'
        this.container.style.left = '-100000px'
        this.container.style.top = '0'
        this.container.style.display = 'inline-block'
        this.container.style.visibility = 'hidden'
        this.container.style.pointerEvents = 'none'
        this.container.style.lineHeight = 'normal'

        this.content = document.createElement('span')
        this.content.style.display = 'inline-block'
        this.container.appendChild(this.content)

        document.body.appendChild(this.container)

        this.canvas = document.createElement('canvas')
        this.ctx = this.canvas.getContext("2d")
    },

    measureTextFast(text, font){
        let fontFamily = this.getFontFamily(font)
        this.ctx.font = `${font.italic ? 'italic ' : ''}${font.bold ? 'bold ' : ''}${font.pixelSize}px ${fontFamily}`
        let textMetrics = this.ctx.measureText(text)

        let width = textMetrics.width
        if(Number.isFinite(textMetrics.actualBoundingBoxLeft) && Number.isFinite(textMetrics.actualBoundingBoxRight)){
            width = Math.max(width, textMetrics.actualBoundingBoxLeft + textMetrics.actualBoundingBoxRight)
        }

        let height = 0
        if(Number.isFinite(textMetrics.fontBoundingBoxAscent) && Number.isFinite(textMetrics.fontBoundingBoxDescent)){
            height = textMetrics.fontBoundingBoxAscent + textMetrics.fontBoundingBoxDescent
        } else if(Number.isFinite(textMetrics.actualBoundingBoxAscent) && Number.isFinite(textMetrics.actualBoundingBoxDescent)){
            height = textMetrics.actualBoundingBoxAscent + textMetrics.actualBoundingBoxDescent
        } else {
            height = Math.ceil(font.pixelSize * 1.2)
        }
        
        return {
            width: Math.ceil(width + 1),
            height: Math.ceil(height + 1),
            isHTML: false,
        }
    },

    measureText: function(text, font, maxWidth, wrapMode, isHTML, elide){
        if(!isHTML && maxWidth === 0 && elide === ElideNone){
            return this.measureTextFast(text, font)
        }

        this.container.style.fontFamily = this.getFontFamily(font)
        this.container.style.fontSize = font.pixelSize+'px'
        this.container.style.fontWeight = font.bold ? 'bold' : 'normal'
        this.container.style.fontStyle = font.italic ? 'italic' : 'normal'
        this.container.style.textDecoration = font.underline ? 'underline' : 'unset'
        if(maxWidth){
            this.container.style.maxWidth = maxWidth+'px'
            switch(wrapMode){
                case NoWrap: this.container.style.whiteSpace = 'pre'; this.container.style.wordBreak = 'unset'; break;
                case WordWrap: this.container.style.whiteSpace ='pre-wrap'; this.container.style.wordBreak = 'break-word'; break;
                case WrapAnywhere: this.container.style.whiteSpace ='pre-wrap'; this.container.style.wordBreak = 'break-all'; break;
                case Wrap: this.container.style.whiteSpace ='pre-wrap'; this.container.style.wordBreak = 'break-word'; break;
                case WrapAtWordBoundaryOrAnywhere: this.container.style.whiteSpace ='pre-wrap'; this.container.style.wordBreak = 'break-word'; break;
            }
        } else {
            this.container.style.maxWidth = 'unset'
            this.container.style.whiteSpace = 'pre'; 
            this.container.style.wordBreak = 'unset';
        }

        if(elide === ElideRight){
            this.content.style.textOverflow = 'ellipsis'
            this.content.style.overflow = 'hidden'
            this.content.style.whiteSpace = 'nowrap'
        } else {
            this.content.style.textOverflow = 'unset'
            this.content.style.overflow = 'unset'
            this.content.style.whiteSpace = 'inherit'
        }
 
        if(isHTML){
            this.content.innerHTML = text
        } else {
            this.content.textContent = text
        }
        
        let rect = this.content.getBoundingClientRect()
        return {
            width: Math.ceil(Math.max(rect.width, this.content.scrollWidth) + 1),
            height: Math.ceil(Math.max(rect.height, this.content.scrollHeight) + 1),
            isHTML: isHTML,
        }
    }
}