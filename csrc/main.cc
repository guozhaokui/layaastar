// hello.cc
#include <node.h>
#include <stdio.h>
#include <algorithm>
#include "astar.h"

namespace laya {
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;
    using v8::ArrayBufferView;
    using v8::ArrayBuffer;

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

    /**
     * @param buffer {ArrayBuffer}
     * @param w {number}
     * @param h {number}
     * @param r {number} roughness
     * @param obuff {ArrayBuffer} out buffer
    */
    void _Prefilter(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        float* ibuff = nullptr;
        int width = 0;
        int height = 0;
        float roughness = 0.0f;
        float* obuff = nullptr;
        int obuffsz = 0;

        Local<ArrayBuffer> ijsbuff;
        GETARRAYBUFFER(ijsbuff, 0);

        v8::ArrayBuffer::Contents icontents = ijsbuff->GetContents();
        int ilen = (int)icontents.ByteLength();
        ibuff = (float*)icontents.Data();

        if (args[1]->IsNumber()) {
            width = args[1]->ToInt32()->Value();
        }
        else {
            printf("param 2 error: width {number}\n");
            return;
        }
        if (args[2]->IsNumber()) {
            height = args[2]->ToInt32()->Value();
        }
        else {
            printf("param 3 error: height {number}\n");
            return;
        }
        if (args[3]->IsNumber()) {
            roughness = (float)(args[3]->ToNumber()->Value());
        }
        else {
            printf("param 4 error: roughness {number}\n");
            return;
        }

        Local<ArrayBuffer> ojsbuff;
        if (args[4]->IsArrayBuffer()) {
            ojsbuff = Local<ArrayBuffer>::Cast(args[4]);
        }
        else if (args[4]->IsArrayBufferView()) {
            ojsbuff = Local<ArrayBufferView>::Cast(args[4])->Buffer();
        }
        else {
            printf("param 5 error: obuff {ArrayBuffer}\n");
            return;
        }

        v8::ArrayBuffer::Contents ocontents = ojsbuff->GetContents();
        obuffsz = (int)ocontents.ByteLength();
        obuff = (float*)ocontents.Data();
        //Prefilter(ibuff, width, height, roughness, obuff, obuffsz);
        //args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
    }

    /**
     * @param ab {ArrayBuffer}
     * @param w  {number}
     * @param h {number}
    */
    void _test1(const FunctionCallbackInfo<Value>& args){
        Local<ArrayBuffer> ijsbuff;
        GETARRAYBUFFER(ijsbuff, 0);
        v8::ArrayBuffer::Contents icontents = ijsbuff->GetContents();
        float* ibuff = (float*)icontents.Data();

        int width = 0;
        int height = 0;
        if (args[1]->IsNumber()) {
            width = (int)(args[1]->ToNumber()->Value());
        }
        else {
            printf("param 1 error: width {number}\n");
            return;
        }

        if (args[2]->IsNumber()) {
            height = (int)(args[2]->ToNumber()->Value());
        }
        else {
            printf("param 2 error: height {number}\n");
            return;
        }

    }

    void _test2(const FunctionCallbackInfo<Value>& args) {
        test();
    }

    void init(Local<Object> exports) {
        NODE_SET_METHOD(exports, "prefilter", _Prefilter);
        NODE_SET_METHOD(exports, "test1", _test1);
        NODE_SET_METHOD(exports, "testastar", _test2);
    }

    //必须要有一个初始化函数。
    //NODE_MODULE(模块名，初始化函数)
    NODE_MODULE(addon, init)

}