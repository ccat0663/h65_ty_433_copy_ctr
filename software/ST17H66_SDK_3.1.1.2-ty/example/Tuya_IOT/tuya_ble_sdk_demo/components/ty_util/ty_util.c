#include "ty_util.h"




/*********************************************************************
 * LOCAL CONSTANT
 */

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
uint8_t ty_util_check_sum8(uint8_t* buf, uint32_t size)
{
    uint8_t sum = 0;
    for(uint32_t idx=0; idx<size; idx++) {
        sum += buf[idx];
    }
    return sum;
}

/*********************************************************
FN: 
*/
uint16_t ty_util_check_sum16(uint8_t* buf, uint32_t size)
{
    uint16_t sum = 0;
    for(uint32_t idx=0; idx<size; idx++) {
        sum += buf[idx];
    }
    return sum;
}

/*********************************************************
FN: 
*/
uint16_t ty_util_crc16(uint8_t* buf, uint32_t size, uint16_t* p_crc)
{
    uint16_t poly[2] = {0, 0xa001}; //0x8005 <==> 0xa001
    uint16_t crc = 0xffff;
    int i, j;
    
    for(j=size; j>0; j--)
    {
        uint8_t ds = *buf++;
        for(i=0; i<8; i++)
        {
            crc = (crc >> 1) ^ poly[(crc ^ ds) & 1];
            ds = ds >> 1;
        }
    }
    return crc;
}

/*********************************************************
FN: 
*/
uint32_t ty_util_crc32(uint8_t* buf, uint32_t size, uint32_t* p_crc)
{
    uint32_t crc = (p_crc == NULL) ? 0xFFFFFFFF : ~(*p_crc);
    for (uint32_t i = 0; i < size; i++)
    {
        crc = crc ^ buf[i];
        for (uint32_t j = 8; j > 0; j--)
        {
            crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
        }
    }
    return ~crc;
}

/*********************************************************
FN: 
*/
uint32_t ty_util_intArray2int(uint8_t* intArray, uint32_t startIdx, uint32_t size)
{
    if(startIdx >= size) {
        return (uint32_t)-1;
    }
    
	uint32_t num = 0;
	for (uint32_t idx=startIdx; idx<startIdx+size; idx++) {
		num = (num*10) + intArray[idx];
	}
	return num;
}

/*********************************************************
FN: 
*/
uint32_t ty_util_int2intArray(uint32_t num, uint8_t* intArray, uint32_t size)
{
    uint32_t idx = 0;
    uint32_t tmp = 0;
    
    tmp = num;
    do {
        tmp /= 10;
        idx++;
    } while(tmp != 0);
    
    if(size < idx) {
        return 0;
    }
    
    tmp = num;
    for(idx=0; tmp!=0; idx++)
    {
        intArray[idx] = tmp % 10;
        tmp /= 10;
    }
    
    ty_util_reverse_byte(intArray, idx);
    
    return idx;
}

/*********************************************************
FN: 
PM: buf - void*防止调用该API时产生警告
*/
uint32_t ty_util_reverse_byte(void* buf, uint32_t size)
{
    uint8_t* p_tmp = buf;
    uint8_t  tmp;
    for(uint32_t idx=0; idx<size/2; idx++)
    {
        tmp = *(p_tmp+idx);
        *(p_tmp+idx) = *(p_tmp+size-1-idx);
        *(p_tmp+size-1-idx) = tmp;
    }
    return 0;
}

/*********************************************************
FN: 统计整数二进制表示中1的个数
AT: hamming_weight
*/
uint32_t ty_util_count_one_in_num(uint32_t num)
{
    num = (num&0x55555555) + ((num>>1)&0x55555555);
    num = (num&0x33333333) + ((num>>2)&0x33333333);
    num = (num&0x0f0f0f0f) + ((num>>4)&0x0f0f0f0f);
    num = (num&0x00ff00ff) + ((num>>8)&0x00ff00ff);
    num = (num&0x0000ffff) + ((num>>16)&0x0000ffff);

    return num;
}

/*********************************************************
FN: 
*/
uint32_t ty_util_shell_sort(int* buf, int size)
{
    int i;
    int j;
    int temp;  
    int gap;  //步长
    for (gap = size / 2; gap >= 1; gap /= 2) {  // 步长初始化为数组长度的一半，每次遍历后步长减半
        for (i = 0 + gap; i < size; i += gap) {   //对步长为gap的元素进行直插排序，当gap为1时，就是直插排序
            temp = buf[i];  //备份a[i]的值
            j = i - gap;  //j初始化为i的前一个元素（与i相差gap长度）
            while (j >= 0 && buf[j] > temp) {
                buf[j + gap] = buf[j];  //将在a[i]前且比temp的值大的元素向后移动一位
                j -= gap;
            }
            buf[j + gap] = temp; 
        }
    }
    return 0;
}




/*********************************************************  string  *********************************************************/

/*********************************************************
FN: 字符转int
*/
uint8_t ty_util_str_hexChar2int(uint8_t hexChar)
{
    switch(hexChar){
        case '0':return 0;
        case '1':return 1;
        case '2':return 2;
        case '3':return 3;
        case '4':return 4;
        case '5':return 5;
        case '6':return 6;
        case '7':return 7;
        case '8':return 8;
        case '9':return 9;
        case 'a':
        case 'A':return 10;
        case 'b':
        case 'B':return 11;
        case 'c':
        case 'C':return 12;
        case 'd':
        case 'D':return 13;
        case 'e':
        case 'E':return 14;
        case 'f':
        case 'F':return 15;
        default: return (uint8_t)-1;
    }
}

/*********************************************************
FN: int转字符
*/
uint8_t ty_util_str_int2hexChar(bool isHEX, uint8_t intNum)
{
    switch(intNum){
        case 0:return '0';
        case 1:return '1';
        case 2:return '2';
        case 3:return '3';
        case 4:return '4';
        case 5:return '5';
        case 6:return '6';
        case 7:return '7';
        case 8:return '8';
        case 9:return '9';
        case 10:return (isHEX ? 'A' : 'a');
        case 11:return (isHEX ? 'B' : 'b');
        case 12:return (isHEX ? 'C' : 'c');
        case 13:return (isHEX ? 'D' : 'd');
        case 14:return (isHEX ? 'E' : 'e');
        case 15:return (isHEX ? 'F' : 'f');
        default:return (uint8_t)-1;
    }
}

/*********************************************************
FN: 
*/
uint32_t ty_util_str_hexStr2int(uint8_t* hexStr, uint32_t size, uint32_t* num)
{
    *num = 0;
    for(uint32_t idx=0; idx<size; idx++)
    {
        uint8_t tmp = ty_util_str_hexChar2int(hexStr[idx]);
        if(tmp == (uint8_t)-1) {
            return 1;
        }
        
        (*num) = (*num)<<4;
        (*num) += tmp;
    }
    return 0;
}

/*********************************************************
FN: 
RT: hexStr的实际长度
*/
uint32_t ty_util_str_int2hexStr(bool isHEX, uint32_t num, uint8_t* hexStr, uint32_t size)
{
    uint32_t idx = 0;
    hexStr[idx++] = ty_util_str_int2hexChar(isHEX, num%16);
    num = num/16;
    
    while(num >= 16) {
        hexStr[idx++] = ty_util_str_int2hexChar(isHEX, num%16);
        num = num/16;
    }
    
    hexStr[idx++] = ty_util_str_int2hexChar(isHEX, num);
    
    if(idx < size) {
        memset(&hexStr[idx], '0', size-idx);
    }
    
    ty_util_reverse_byte(hexStr, size);
    
    return idx;
}

/*********************************************************
FN: 
*/
uint32_t ty_util_str_intStr2int(uint8_t* intStr, uint32_t size, uint32_t* num)
{
    *num = 0;
    for(uint32_t idx=0; idx<size; idx++)
    {
        uint8_t tmp = ty_util_str_hexChar2int(intStr[idx]);
        if(tmp == (uint8_t)-1 || tmp > 9) {
            return 1;
        }
        
        (*num) = (*num)*10;
        (*num) += tmp;
    }
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_util_str_int2intStr(uint32_t num, uint8_t* intStr, uint32_t size)
{
    uint32_t idx = 0;
    uint32_t tmp = 0;
    
    tmp = num;
    do {
        tmp /= 10;
        idx++;
    } while(tmp != 0);
    
    if(size < idx) {
        return 0;
    }
    
    tmp = num;
    for(idx=0; tmp!=0; idx++)
    {
        intStr[idx] = ty_util_str_int2hexChar(true, tmp % 10);
        tmp /= 10;
    }
    
    ty_util_reverse_byte(intStr, idx);
    
    return idx;
}

/*********************************************************
FN: 
*/
uint32_t ty_util_str_hexStr2hexArray(uint8_t* hexStr, uint32_t size, uint8_t* hexArray)
{
    uint8_t hex_num = 0;
	for(uint32_t idx=0; idx<size; idx++)
    {
        uint8_t tmp = ty_util_str_hexChar2int(hexStr[idx]);
        if(tmp == (uint8_t)-1) {
            return 1;
        }
        
        hex_num <<= 4;
        hex_num |= tmp;
        
        if((idx & 1) == 1){
            hexArray[idx>>1] = hex_num;
            hex_num = 0;
        }
	}
	return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_util_str_hexArray2hexStr(bool isHEX, uint8_t* hexArray, uint32_t size, uint8_t* hexStr)
{
    uint32_t idx;
	for(idx=0; idx<size; idx++)
    {
        uint8_t high = hexArray[idx]>>4;
        uint8_t low  = hexArray[idx]&0x0F;
        
        hexStr[idx*2] = ty_util_str_int2hexChar(isHEX, high);
        hexStr[idx*2+1] = ty_util_str_int2hexChar(isHEX, low);
	}
//    hexstr[idx*2] = '0'; //防止hexStr数组的长度不足，需要可加上
	return 0;
}






