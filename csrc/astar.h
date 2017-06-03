#ifndef _LAYA_A_START_H___
#define _LAYA_A_START_H___

#define MAPDATATYPE unsigned int
#define USEINT
#define MAXOUTNUM 100

#include <node.h>
#include <node_object_wrap.h>

struct Vec2 {
    int x, y;
};

int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY,
    const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight,
    int* pOutBuffer, const int nOutBufferSize);

void test();

/**
*/
class AStarMap :public node::ObjectWrap{
public:
    //V8 ����
    static void Init(v8::Local<v8::Object> exports);
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;
    //

    AStarMap(const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight, int nPosX, int nPosY, int nGridWidth, int nGridHeight);
    ~AStarMap();
    /**
     * Ѱ·��
     * @param stx,sty ���λ�á����Ǹ���
     * @param edx,edy Ŀ���λ�ã����Ǹ��ӡ�
     * @param maxwidth ���Ѱ·���������������㲻����
     * @param maxheight ���Ѱ·����ߣ�����������㲻����
     * @param linedist ֱ�߻����롣����ô���ĸ��ӽ���ֱ�߻���������ô����·����ֱ�߻��ڵ㡣
    */
    void findPath(int stx, int sty, int edx, int edy, int maxwidth, int maxheight, int linedist);

protected:
    /**
     * @param nStartX ��ʼλ�ã����Ǹ��ӣ���λ�á�
     * @param nTargetX Ŀ��λ�ã����Ǹ��ӣ���λ�á�
    */
    int _findPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);

    /**
    * @param nStartX ��ʼλ�ã��Ǹ��ӡ� 
    * @param nTargetX Ŀ��λ�ã��Ǹ���
    */
    int _findPathGrid(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);

    /**
     * ֱ�߼�⡣
     * @param x1 ��ʼ����
     * @param x2 Ŀ�����
     * @param hitx ��ײ��λ��
     * @return {bool} �Ƿ�����ײ��false�Ļ���hitx��hity�����塣
    */
    bool _rayCast(const int x1, const int y1, const int x2, const int y2, int& hitx, int& hity);

    /**
    * ֱ�߻������ 
    */
    void linearization(Vec2* pPath,int nNodeNum, int nMaxDist);

    int* mpOutBuffer=nullptr;
    int mnOutBufferSz = 0;
    int mnWidth = 0;
    int mnHeight = 0;
    int mnGridWidth = 128;
    int mnGridHeight = 128;
    int mnPosX = 0;
    int mnPosY = 0;
    //int mnStartX = 0;
    //int mnStartY = 0;
    int mnTargetX = 0;
    int mnTargetY = 0;
    //���������Ӻ����������λ�ýڵ�
    Vec2* mpFindResult = nullptr;
    int   mnFindeSz = 0;
    unsigned int* mpMap=nullptr;
public:
    Vec2 mPathOut[MAXOUTNUM];
    int  mnPathOutSz = 0;
};

#endif

