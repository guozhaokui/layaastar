#!/usr/bin/env node
var fs = require('fs');
var path = require('path');
if (process.argv.length < 2) {
    console.log(`
用法：   pbrtools cmd cmdparams;
    cmd: converthdr
`
    );
    process.exit(1);
}

var cmd = process.argv[1];

var curpath = process.cwd();
try{
    var func = require('../lib/'+cmd).mainfunc;
    func.apply(null,process.argv.slice(2));
}catch(e){
    console.log(' no this command :'+cmd);
}
