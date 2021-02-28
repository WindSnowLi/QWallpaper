//
#include "CRipple.h"

/**
 * 功能：水波定时器回调函数，定时器回调函数不能放到类成员中
 * 参数：
 *		[in]		hWnd
 *		[in]		uMsg
 *		[in]		idEvent
 *		[in]		dwTime
 * 返回值：
 *		void
 */

HDC goalhDC = NULL;

static void  CALLBACK WaveTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CRipple* pRipple = (CRipple*)idEvent;

	//水波扩散、渲染
	pRipple->WaveSpread();
	pRipple->WaveRender();

	//HDC hDc = GetDC(hWnd);

	//刷新到屏幕
	pRipple->UpdateFrame(goalhDC);
	//ReleaseDC(hWnd, hDc);
}

//////////////////////////////////////////////////////////////////////
CRipple::CRipple()
{
	m_hWnd = NULL;
	m_hRenderDC = NULL;
	m_hRenderBmp = NULL;
	m_iBmpWidth = 0;
	m_iBmpHeight = 0;
	m_iBytesPerWidth = 0;
	m_pWaveBuf1 = NULL;
	m_pWaveBuf2 = NULL;
	m_pBmpSource = NULL;
	m_pBmpRender = NULL;
	memset(&m_stBitmapInfo, 0, sizeof(m_stBitmapInfo));
}

CRipple::~CRipple()
{
	FreeRipple();
}

/**
 * 功能：初始化水波对象
 * 参数：
 *		[in]		hWnd		窗口句柄
 *		[in]		hBmp		水波背景图片句柄
 *		[in]		uiSpeed		定时器间隔时间（刷新速度）
 * 返回值：
 *		成功true、失败false
 */

UINT tempUiSpeed;

bool CRipple::InitRipple(HWND hWnd, HBITMAP hBmp, UINT uiSpeed)
{
	m_hWnd = hWnd;
	BITMAP stBitmap;

	if (GetObject(hBmp, sizeof(stBitmap), &stBitmap) == 0)
	{
		return false;
	}

	//获取位图宽、高、一行的字节数
	m_iBmpWidth = stBitmap.bmWidth;
	m_iBmpHeight = stBitmap.bmHeight;
	m_iBytesPerWidth = (m_iBmpWidth * 3 + 3) & ~3;		//24位位图，一个像素占3个字节，一行的总字节数要为4的倍数

	//分配波能缓冲区
	m_pWaveBuf1 = new int[m_iBmpWidth * m_iBmpHeight];
	m_pWaveBuf2 = new int[m_iBmpWidth * m_iBmpHeight];

	//空间分配失败
	if (m_pWaveBuf1 == NULL || m_pWaveBuf2 == NULL)
		return false;

	memset(m_pWaveBuf1, 0, sizeof(int) * m_iBmpWidth * m_iBmpHeight);
	memset(m_pWaveBuf2, 0, sizeof(int) * m_iBmpWidth * m_iBmpHeight);

	//分配位图像素数据缓冲区
	m_pBmpSource = new BYTE[m_iBytesPerWidth * m_iBmpHeight];
	m_pBmpRender = new BYTE[m_iBytesPerWidth * m_iBmpHeight];

	//空间分配失败
	if (m_pBmpSource == NULL || m_pBmpRender == NULL)
		return false;

	HDC hdc = GetDC(m_hWnd);

	//渲染DC
	//该函数创建一个与指定设备兼容的内存设备上下文环境（DC）。
	m_hRenderDC = CreateCompatibleDC(hdc);


	//该函数用于创建与指定的设备环境相关的设备兼容的位图。
	//由CreateCompatibleBitmap函数创建的位图的颜色格式与由参数hdc标识的设备的颜色格式匹配，
	//该位图可以选入任意内存设备环境中，并且内存设备环境允许彩色和单色两种位图。
	m_hRenderBmp = CreateCompatibleBitmap(hdc, m_iBmpWidth, m_iBmpHeight);

	//SelectObject，计算机编程语言函数，该函数选择一对象到指定的设备上下文环境中，该新对象替换先前的相同类型的对象。
	SelectObject(m_hRenderDC, m_hRenderBmp);

	//初始化BITMAPINFO结构
	m_stBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_stBitmapInfo.bmiHeader.biWidth = m_iBmpWidth;
	m_stBitmapInfo.bmiHeader.biHeight = -m_iBmpHeight;
	m_stBitmapInfo.bmiHeader.biPlanes = 1;
	m_stBitmapInfo.bmiHeader.biBitCount = 24;					//24位位图
	m_stBitmapInfo.bmiHeader.biSizeImage = 0;
	m_stBitmapInfo.bmiHeader.biCompression = BI_RGB;

	//创建临时内存DC保存图片
	HDC hMemDC = CreateCompatibleDC(hdc);
	SelectObject(hMemDC, hBmp);
	ReleaseDC(m_hWnd, hdc);

	//获取位图数据
	//int GetDIBits(HDC hdc, HBITMAP hbmp, UINT uStartScan, UINT cScanLines, LPVOID lpvBits, LPBITMAPINFO lpbi, UINT uUsage)；
	/*GetDIBits函数获取指定兼容位图的位，然后将其作一个DIB—设备无关位图（Device-Independent Bitmap）使用的指定格式复制到一个缓冲区中。
	*cScanLines：指定检索的扫描线数。
	*lpvBits：指向用来检索位图数据的缓冲区的指针。如果此参数为NULL，那么函数将把位图的维数与格式传递给lpbi参数指向的BITMAPINFO结构。
	*lpbi：指向一个BITMAPINFO结构的指针，此结构确定了设备所在位图的数据格式。
	*uUsage：指定BITMAPINFO结构的bmiColors成员的格式。它必须为下列取值：
	*DIB_PAL_COLORS：颜色表由指向当前逻辑调色板的16位索引值数组构成。
	*DIB_RGB_COLORS：颜色表由红、绿、蓝（RGB）三个直接值构成。
	*返回值：如果lpvBits参数非空，并且函数调用成功，那么返回值为从位图复制的扫描线数。
	*/
	GetDIBits(hMemDC, hBmp, 0, m_iBmpHeight, m_pBmpSource, &m_stBitmapInfo, DIB_RGB_COLORS);
	GetDIBits(hMemDC, hBmp, 0, m_iBmpHeight, m_pBmpRender, &m_stBitmapInfo, DIB_RGB_COLORS);

	//获取完位图数据，释放内存DC
	DeleteDC(hMemDC);

	//设置定时器
	tempUiSpeed = uiSpeed;
	//SetTimer(m_hWnd, (UINT_PTR)this, uiSpeed, WaveTimerProc);
	return true;
}

void CRipple::startTimer() {
	goalhDC = ::GetDC(workerw);
	SetTimer(m_hWnd, (UINT_PTR)this, tempUiSpeed, WaveTimerProc);
}

void CRipple::cancelTimer() {
	KillTimer(m_hWnd, (UINT_PTR)this);
	ReleaseDC(workerw, goalhDC);
	DeleteDC(goalhDC);
}


/**
 * 功能：释放水波对象资源
 * 参数：
 *		void
 * 返回值：
 *		void
 */
void CRipple::FreeRipple()
{
	//释放资源
	if (m_hRenderDC != NULL)
	{
		DeleteDC(m_hRenderDC);
	}
	if (m_hRenderBmp != NULL)
	{
		DeleteObject(m_hRenderBmp);
	}
	if (m_pWaveBuf1 != NULL)
	{
		delete[]m_pWaveBuf1;
	}
	if (m_pWaveBuf2 != NULL)
	{
		delete[]m_pWaveBuf2;
	}
	if (m_pBmpSource != NULL)
	{
		delete[]m_pBmpSource;
	}
	if (m_pBmpRender != NULL)
	{
		delete[]m_pBmpRender;
	}
	//杀定时器
//	KillTimer(m_hWnd, (UINT_PTR)this);
}

/**
 * 功能：水波扩散
 * 参数：
 *		void
 * 返回值：
 *		void
 */
void CRipple::WaveSpread()
{


	int* lpWave1 = m_pWaveBuf1;
	int* lpWave2 = m_pWaveBuf2;

	for (int i = m_iBmpWidth; i < (m_iBmpHeight - 1) * m_iBmpWidth; i++)
	{
		//波能扩散
		lpWave2[i] = ((lpWave1[i - 1] + lpWave1[i - m_iBmpWidth] +
			lpWave1[i + 1] + lpWave1[i + m_iBmpWidth]) >> 1) - lpWave2[i];

		//波能衰减
		lpWave2[i] -= (lpWave2[i] >> 5);
	}

	//交换缓冲区
	m_pWaveBuf1 = lpWave2;
	m_pWaveBuf2 = lpWave1;
}

/**
 * 功能：根据水波波幅渲染水波位图数据
 * 参数：
 *		void
 * 返回值：
 *		void
 */
void CRipple::WaveRender()
{

	int iPtrSource = 0;
	int iPtrRender = 0;
	int lineIndex = m_iBmpWidth;
	int iPosX = 0;
	int iPosY = 0;

	//扫描位图
	for (int y = 1; y < m_iBmpHeight - 1; y++)
	{
		for (int x = 0; x < m_iBmpWidth; x++)
		{
			//根据波幅计算位图数据偏移值，渲染点（x，y)对应原始图片（iPosX，iPosY）
			iPosX = x + (m_pWaveBuf1[lineIndex - 1] - m_pWaveBuf1[lineIndex + 1]);
			iPosY = y + (m_pWaveBuf1[lineIndex - m_iBmpWidth] - m_pWaveBuf1[lineIndex + m_iBmpWidth]);
			//另外一种计算偏移的方法
			//int waveData = (1024 - m_pWaveBuf1[lineIndex]);
			//iPosX = (x - m_iBmpWidth/2)*waveData/1024 + m_iBmpWidth/2;
			//iPosY = (y - m_iBmpHeight/2)*waveData/1024 + m_iBmpHeight/2;

			if (0 <= iPosX && iPosX < m_iBmpWidth &&
				0 <= iPosY && iPosY < m_iBmpHeight)
			{
				//分别计算原始位图（iPosX，iPosY）和渲染位图（x，y)对应的起始位图数据
				iPtrSource = iPosY * m_iBytesPerWidth + iPosX * 3;
				iPtrRender = y * m_iBytesPerWidth + x * 3;
				//渲染位图，重新打点数据
				for (int c = 0; c < 3; c++)
				{
					m_pBmpRender[iPtrRender + c] = m_pBmpSource[iPtrSource + c];
				}
			}

			lineIndex++;
		}
	}
	//设置渲染后的位图
	SetDIBits(m_hRenderDC, m_hRenderBmp, 0, m_iBmpHeight, m_pBmpRender, &m_stBitmapInfo, DIB_RGB_COLORS);
}



/**
 * 功能：刷新水波到hDc上
 * 参数：
 *		[in]		hDc			刷新目标DC（一般为屏幕DC，要显示出来嘛）
 * 返回值：
 *		void
 */


void CRipple::UpdateFrame(HDC hDc)
{
	BitBlt(hDc, 0, 0, m_iBmpWidth, m_iBmpHeight, m_hRenderDC, 0, 0, SRCCOPY);
}

/**
 * 功能：扔石子（设定波源）
 * 参数：
 *		[in]		x				石子位置x
 *		[in]		y				石子位置y
 *		[in]		stoneSize		石子大小（半径）
 *		[in]		stoneWeight		石子重量
 * 返回值：
 *		void
 */
void CRipple::DropStone(int x, int y, int stoneSize, int stoneWeight)
{
	int posX = 0;
	int posY = 0;

	for (int i = -stoneSize; i < stoneSize; i++)
	{
		for (int j = -stoneSize; j < stoneSize; j++)
		{
			posX = x + i;
			posY = y + j;

			//控制范围，不能超出图片
			if (posX < 0 || posX >= m_iBmpWidth ||
				posY < 0 || posY >= m_iBmpHeight)
			{
				continue;
			}

			//在一个圆形区域内，初始化波能缓冲区1
			if (i * i + j * j <= stoneSize * stoneSize)
			{
				m_pWaveBuf1[posY * m_iBmpWidth + posX] = stoneWeight;
			}
		}
	}
}