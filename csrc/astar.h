#ifndef _LAYA_A_START_H___
#define _LAYA_A_START_H___

#define MAPDATATYPE unsigned int
#define MAXOUTNUM 100

#include <node.h>
#include <node_object_wrap.h>

struct Vec2 {
    int x, y;
};

struct  MapGrid{
    unsigned short x;
    unsigned short y;
    int     mapinfo;
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
     * @param linedist ֱ�߻����롣����ô���ĸ��ӽ���ֱ�߻���������ô����·����ֱ�߻��ڵ㡣
     * @param pOut ����ڵ㣬ÿ������ʾһ��λ��
     * @param nOutSZ ���buffer�Ĵ�С��������ֹbuffer���
     * @return pOut�ĳ��ȡ������������ڵ㣬�򳤶�Ϊ4
    */
    int findPath(int stx, int sty, int edx, int edy, int maxwidth, int maxheight, int linedist, int* pOut, int nOutSZ);
    int findPath(int stx, int sty, int edx, int edy, int* pOut, int nOutSZ);

    /*
    * �������Ѱ·��Χ����λ���Ǹ��ӡ�
    */
    void setFindRange(int w, int h) {
        mnFindRangeX = w/mnGridWidth;
        mnFindRangeY = h/mnGridHeight;
    };

protected:
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
    * ������pOut�ĳ���
    */
    int linearizationAndToPos(int* pPath,int nNodeNum, int nMaxDist, int* pOut, int nOutSZ);

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
    int mnFindRangeX = 10000;       //���Ѱ·���������������㲻���С���λ�Ǹ��ӡ�
    int mnFindRangeY = 10000;       //���Ѱ·����ߣ�����������㲻���С���λ�Ǹ��ӡ�
    //����������
    int*  mpFindResult = nullptr;
    int   mnFindeSz = 0;
    MapGrid* mpMap=nullptr;
public:
    int mnLinearizationLen = 10000; //���ֱ�߻����롣��λ�Ǹ���
};

#endif

