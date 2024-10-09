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

module.exports = {
    tags: ['<a>','<abbr>','<address>','<area>','<article>','<aside>','<audio>','<b>','<base>','<bdi>','<bdo>','<blockquote>','<body>','<br>','<button>','<canvas>','<caption>','<cite>','<code>','<col>','<colgroup>','<data>','<datalist>','<dd>','<del>','<details>','<dfn>','<dialog>','<div>','<dl>','<dt>','<em>','<embed>','<fieldset>','<figcaption>','<figure>','<footer>','<form>','<h1>','<h2>','<h3>','<h4>','<h5>','<h6>','<head>','<header>','<hr>','<html>','<i>','<iframe>','<img>','<input>','<ins>','<kbd>','<label>','<legend>','<li>','<link>','<main>','<map>','<mark>','<meta>','<meter>','<nav>','<noscript>','<object>','<ol>','<optgroup>','<option>','<output>','<p>','<param>','<picture>','<pre>','<progress>','<q>','<ruby>','<rb>','<rt>','<rtc>','<rp>','<s>','<samp>','<script>','<section>','<select>','<small>','<source>','<span>','<strong>','<style>','<sub>','<summary>','<sup>','<table>','<tbody>','<td>','<template>','<textarea>','<tfoot>','<th>','<thead>','<time>','<title>','<tr>','<track>','<u>','<ul>','<var>','<video>','<wbr>'],
    regexp: /<[^<>]+>/g,

    init: function(){
        this.container = document.createElement('div')
        this.container.style.position = 'absolute'
        this.container.style.display = 'inline'
        this.container.style.opacity = 0
        this.container.style.lineHeight = 'normal'
        document.body.appendChild(this.container)
    },

    measureText: function(text, font, maxWidth, wrapMode, textFormat){
        this.container.style.fontFamily = font.family
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
        
        let isHTML = false
        if(textFormat === undefined || textFormat === PlainText){
            isHTML = false
        } else if(textFormat === AutoText){
            isHTML = false
            let result = text.match(this.regexp)
            if(result){
                for(let res of result){
                    if(this.tags.indexOf(res) >= 0){
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

        if(isHTML){
            this.container.innerHTML = text
        } else {
            this.container.innerText = text
        }
        
        let rect = this.container.getBoundingClientRect()
        rect.isHTML = isHTML
        return rect
    }
}