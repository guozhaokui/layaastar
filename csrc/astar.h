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
    int     mapinfo;
    unsigned short     cost;        //��short���һ��10ms/ǧ�Ρ��������ƴ�С���Ϊ256x256
    unsigned short     camefrom;    //��short���һ��10ms/ǧ�Ρ��������ƴ�С���Ϊ256x256
    unsigned short x;
    unsigned short y;
};

/**
 *@param nOutBufferSize pOutBuffer�Ĵ�С����length������byteLength
*/
int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY,
    const MapGrid* pMap, const int nMapWidth, const int nMapHeight,
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

    AStarMap(const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight, 
        int nPosX, int nPosY, int nGridWidth, int nGridHeight);

    ~AStarMap();

    /**
     * Ѱ·��
     * @param stx,sty ���λ�á����Ǹ���
     * @param edx,edy Ŀ���λ�ã����Ǹ��ӡ�
     * @param maxwidth ���x����
     * @param maxheight ���y����
     * @param linedist ֱ�߻����롣����ô���ĸ��ӽ���ֱ�߻���������ô����·����ֱ�߻��ڵ㡣
     * @param pOut ����ڵ㣬ÿ������ʾһ��λ��
     * @param nOutSZ ���buffer�Ĵ�С��������ֹbuffer���
     * @return pOut�ĳ��ȡ������������ڵ㣬�򳤶�Ϊ4
    */
    int findPath(int stx, int sty, int edx, int edy, int maxwidth, int maxheight, int linedist, 
        int* pOut, int nOutSZ);
    int findPath(int stx, int sty, int edx, int edy, int* pOut, int nOutSZ);

    /*
    * �������Ѱ·��Χ����λ���Ǹ��ӡ�
    */
    void setSearchRegion(int w, int h) {
        mnSearchRegionW = w/mnGridWidth;
        mnSearchRegionH = h/mnGridHeight;
    };

    //������
    int findPathGrid(int sx, int sy, int ex, int ey);
    void saveAsTxt(int stx, int sty, int edx, int edy, int* pOut, int nOutSz);
protected:

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
    * TODO �Ƿ�Ҫ������㣬�����ǲ����ǵġ�
    * pPath������㵽�յ��˳�򣬵��ǲ�����㡣
    *
    * ���⣺�����б�ſ����ߵĻ������ڻ����㷨��8��������߷����ܲ�һ�£����ܻᵼ��һֱ��Ϊ����ײ������� Ӧ�ò����
    */
    int linearizationAndToPos(int* pPath,int nNodeNum, int nMaxDist, int* pOut, int nOutSZ);

protected:
    int     mnGridWidth = 128;
    int     mnGridHeight = 128;
    int     mnPosX = 0;
    int     mnPosY = 0;
    //int   mnStartX = 0;
    //int   mnStartY = 0;
    int     mnTargetX = 0;
    int     mnTargetY = 0;
    int     mnSearchRegionW = 10000;       //���Ѱ·���������������㲻���С���λ�Ǹ��ӡ�
    int     mnSearchRegionH = 10000;       //���Ѱ·����ߣ�����������㲻���С���λ�Ǹ��ӡ�
    //����������
    int*    mpFindResult = nullptr;
    int     mnFindResultCapacity = 0;
    int     mnFindSz = 0;
    MapGrid* mpWorkMap = nullptr;
//DEBUG
public:
    MapGrid* mpMap = nullptr;
public:
    int     mnWidth = 0;
    int     mnHeight = 0;
    int     mnLinearizationLen = 10000; //���ֱ�߻����롣��λ�Ǹ���
};

#endif

