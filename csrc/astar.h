#ifndef _LAYA_A_START_H___
#define _LAYA_A_START_H___

#define MAPDATATYPE unsigned int

int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY,
    const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight,
    int* pOutBuffer, const int nOutBufferSize);

void test();

/**
* Ϊ�����Ч�ʣ�����һ�������ߡ����Բ���ֱ�ӷ��ظ������ݡ��������λ����û�й�ϵ��
*/
class AStarMap {
public:
    AStarMap(const MAPDATATYPE* pMap, const int nMapWidth, const int nMapHeight, int nPosX, int nPosY, int nGridWidth, int nGridHeight);
    ~AStarMap();

protected:
    /**
     * @param nStartX ��ʼλ�ã����Ǹ��ӣ���λ�á�
     * @param nTargetX Ŀ��λ�ã����Ǹ��ӣ���λ�á�
    */
    int _findPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);

    /**
    * @param nStartX ��ʼλ�ã��Ǹ��ӡ� ���ⲿ���ӣ����ڲ����ӵ�xy����1
    * @param nTargetX Ŀ��λ�ã��Ǹ���
    */
    int _findPathGrid(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);

    /**
     * ֱ�߼�⡣
     * @param stx ���ӣ��Ǳ���
    */
    int _rayCast(const int stx, const int sty, const int edx, const int edy);

    int* mpOutBuffer=nullptr;
    int mnOutBufferSz = 0;
    int mnWidth = 0;
    int mnHeight = 0;
    int mnGridWidth = 128;
    int mnGridHeight = 128;
    int mnPosX = 0;
    int mnPosY = 0;
    MAPDATATYPE* mpMap=nullptr;

};

#endif

