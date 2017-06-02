//@ts-check
'use strict';
var fs = require('fs');
var path = require('path');
var astar= require('./build/Release/layaastar');

var args = process.argv;

/**
 * @return {string}
 */
function getFinalePath(f){
    if(f)
        return path.isAbsolute(f)?f:path.resolve(process.cwd(), f);        
    return null;
}

var st = Date.now();
astar.testastar();
var dt = Date.now()-st;

console.log('tm='+dt);