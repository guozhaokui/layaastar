
#ifndef __V8_ARGS_MACRO_H___
#define __V8_ARGS_MACRO_H___

#define GETARRAYBUFFER(o,arg) \
if (args[arg]->IsArrayBuffer()) {\
    o = Local<ArrayBuffer>::Cast(args[arg]);\
}\
else if (args[arg]->IsArrayBufferView()) {\
    o= Local<ArrayBufferView>::Cast(args[arg])->Buffer();\
}\
else {\
    printf("param %d error, need ArrayBuffer\n",arg);\
    return;\
}

#define GETNUMBER(type,o,arg) \
if (args[arg]->IsNumber()) {\
    o = (type)(args[arg]->ToNumber()->Value());\
}\
else {\
    printf("param ##arg## error: width {number}\n");\
return;\
}

#define GETI32(o,arg) \
if (args[arg]->IsInt32()) {\
    o =  args[arg]->Int32Value();\
}\
else if(args[arg]->IsNumber()){\
    o = (int)(args[arg]->ToNumber()->Value());\
}else {\
    printf("param ##arg## error: width {number}\n");\
return;\
}


#endif