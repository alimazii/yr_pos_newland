// QR_Encode.h : CQR_Encode

#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3


#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3


#define QR_VRESION_S	0 // 1 - 9
#define QR_VRESION_M	1 // 10 - 26
#define QR_VRESION_L	2 // 27 - 40

#define MAX_ALLCODEWORD	 3706
#define MAX_DATACODEWORD 2956
#define MAX_CODEBLOCK	  153
#define MAX_MODULESIZE	  177

#define MAX_INPUTDATASIZE  512
#define MAX_BMPBUFFSIZE	8192

#define N_TIMES		4	//打印放大倍数

#define min(a,b)    (((a) < (b)) ? (a) : (b))

/////////////////////////////////////////////////////////////////////////////
typedef struct 
{
	int nLen;										//数据长度
	int nLevel;									//纠错级别
	int nMask;									//掩码号
	int nVersion;									//型号
	char szInputData[MAX_INPUTDATASIZE];			//要进行编码的数据
} DataInfo;

typedef struct
{
	int xsize;
	int ysize;
	char bmpbuff[MAX_BMPBUFFSIZE];
}BmpInfo;
/////////////////////////////////////////////////////////////////////////////
typedef struct tagRS_BLOCKINFO
{
	int ncRSBlock;
	int ncAllCodeWord;
	int ncDataCodeWord;

} RS_BLOCKINFO;

/////////////////////////////////////////////////////////////////////////////

typedef struct tagQR_VERSIONINFO
{
	int nVersionNo;	   // 1-40
	int ncAllCodeWord;

	// (0 = L, 1 = M, 2 = Q, 3 = H)
	int ncDataCodeWord[4];

	int ncAlignPoint;
	int nAlignPoint[6];

	RS_BLOCKINFO RS_BlockInfo1[4];
	RS_BLOCKINFO RS_BlockInfo2[4];

} QR_VERSIONINFO;


int EncodeDataAndGenerateBmp(DataInfo* stDataInfo, BmpInfo* BmpBuff);
int GetEncodeVersion(DataInfo* stDataInfo);
int EncodeSourceData(DataInfo* stDataInfo, int nVerGroup);
int GetBitLength(unsigned char nMode, int ncData, int nVerGroup);
int SetBitStream(int nIndex, unsigned short wData, int ncData);
int IsNumeralData(unsigned char c);
int IsAlphabetData(unsigned char c);
int IsKanjiData(unsigned char c1, unsigned char c2);
unsigned char AlphabetToBinaly(unsigned char c);
unsigned short KanjiToBinaly(unsigned short wc);
void GetRSCodeWord(unsigned char* lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);
void FormatModule();
void SetFunctionModule();
void SetFinderPattern(int x, int y);
void SetAlignmentPattern(int x, int y);
void SetVersionPattern();
void SetCodeWordPattern();
void SetMaskingPattern(int nPatternNo);
void SetFormatInfoPattern(int nPatternNo);
int CountPenalty();

void LcdToBmp(const char *lcdBuf,char *bmpBuf, int w, int h);
void AsciiToUtf8(DataInfo* stDataInfo);
char* BmpZoomIn(int ntimes);
	

