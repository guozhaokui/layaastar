
export declare class AStarMap{
    linearizationLen:number;
    constructor(mapdata:Uint32Array, w:number, h:number, posx:number, posy:number, gridw:number, gridh:number);

    /**
     * 
     * @param maxx {number} 最大范围x
     * @param maxy {number} 最大范围y
     */
    setSearchRegion(maxx:number, maxy:number);
    /**
     * 寻路。参数是实际位置，而不是格子。
     * 返回的也是实际节点。
     * @param stx 
     * @param sty 
     * @param targetx 
     * @param targety 
     * @param outbuf  {Uint32Array} 输出路径节点。个数在返回值中，每两个表示一个位置。这个buffer要足够大。
     *                如果只设置一次会更方便一些，但是不太安全，这个buffer是js创建的，可能被释放。
     */
    findPath(stx:number, sty:number, targetx:number, targety:number, outbuf:Uint32Array):number;
}