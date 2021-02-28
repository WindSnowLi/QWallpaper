//
#include "CRipple.h"

/**
 * ���ܣ�ˮ����ʱ���ص���������ʱ���ص��������ܷŵ����Ա��
 * ������
 *		[in]		hWnd
 *		[in]		uMsg
 *		[in]		idEvent
 *		[in]		dwTime
 * ����ֵ��
 *		void
 */

HDC goalhDC = NULL;

static void  CALLBACK WaveTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CRipple* pRipple = (CRipple*)idEvent;

	//ˮ����ɢ����Ⱦ
	pRipple->WaveSpread();
	pRipple->WaveRender();

	//HDC hDc = GetDC(hWnd);

	//ˢ�µ���Ļ
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
 * ���ܣ���ʼ��ˮ������
 * ������
 *		[in]		hWnd		���ھ��
 *		[in]		hBmp		ˮ������ͼƬ���
 *		[in]		uiSpeed		��ʱ�����ʱ�䣨ˢ���ٶȣ�
 * ����ֵ��
 *		�ɹ�true��ʧ��false
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

	//��ȡλͼ���ߡ�һ�е��ֽ���
	m_iBmpWidth = stBitmap.bmWidth;
	m_iBmpHeight = stBitmap.bmHeight;
	m_iBytesPerWidth = (m_iBmpWidth * 3 + 3) & ~3;		//24λλͼ��һ������ռ3���ֽڣ�һ�е����ֽ���ҪΪ4�ı���

	//���䲨�ܻ�����
	m_pWaveBuf1 = new int[m_iBmpWidth * m_iBmpHeight];
	m_pWaveBuf2 = new int[m_iBmpWidth * m_iBmpHeight];

	//�ռ����ʧ��
	if (m_pWaveBuf1 == NULL || m_pWaveBuf2 == NULL)
		return false;

	memset(m_pWaveBuf1, 0, sizeof(int) * m_iBmpWidth * m_iBmpHeight);
	memset(m_pWaveBuf2, 0, sizeof(int) * m_iBmpWidth * m_iBmpHeight);

	//����λͼ�������ݻ�����
	m_pBmpSource = new BYTE[m_iBytesPerWidth * m_iBmpHeight];
	m_pBmpRender = new BYTE[m_iBytesPerWidth * m_iBmpHeight];

	//�ռ����ʧ��
	if (m_pBmpSource == NULL || m_pBmpRender == NULL)
		return false;

	HDC hdc = GetDC(m_hWnd);

	//��ȾDC
	//�ú�������һ����ָ���豸���ݵ��ڴ��豸�����Ļ�����DC����
	m_hRenderDC = CreateCompatibleDC(hdc);


	//�ú������ڴ�����ָ�����豸������ص��豸���ݵ�λͼ��
	//��CreateCompatibleBitmap����������λͼ����ɫ��ʽ���ɲ���hdc��ʶ���豸����ɫ��ʽƥ�䣬
	//��λͼ����ѡ�������ڴ��豸�����У������ڴ��豸���������ɫ�͵�ɫ����λͼ��
	m_hRenderBmp = CreateCompatibleBitmap(hdc, m_iBmpWidth, m_iBmpHeight);

	//SelectObject�������������Ժ������ú���ѡ��һ����ָ�����豸�����Ļ����У����¶����滻��ǰ����ͬ���͵Ķ���
	SelectObject(m_hRenderDC, m_hRenderBmp);

	//��ʼ��BITMAPINFO�ṹ
	m_stBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_stBitmapInfo.bmiHeader.biWidth = m_iBmpWidth;
	m_stBitmapInfo.bmiHeader.biHeight = -m_iBmpHeight;
	m_stBitmapInfo.bmiHeader.biPlanes = 1;
	m_stBitmapInfo.bmiHeader.biBitCount = 24;					//24λλͼ
	m_stBitmapInfo.bmiHeader.biSizeImage = 0;
	m_stBitmapInfo.bmiHeader.biCompression = BI_RGB;

	//������ʱ�ڴ�DC����ͼƬ
	HDC hMemDC = CreateCompatibleDC(hdc);
	SelectObject(hMemDC, hBmp);
	ReleaseDC(m_hWnd, hdc);

	//��ȡλͼ����
	//int GetDIBits(HDC hdc, HBITMAP hbmp, UINT uStartScan, UINT cScanLines, LPVOID lpvBits, LPBITMAPINFO lpbi, UINT uUsage)��
	/*GetDIBits������ȡָ������λͼ��λ��Ȼ������һ��DIB���豸�޹�λͼ��Device-Independent Bitmap��ʹ�õ�ָ����ʽ���Ƶ�һ���������С�
	*cScanLines��ָ��������ɨ��������
	*lpvBits��ָ����������λͼ���ݵĻ�������ָ�롣����˲���ΪNULL����ô��������λͼ��ά�����ʽ���ݸ�lpbi����ָ���BITMAPINFO�ṹ��
	*lpbi��ָ��һ��BITMAPINFO�ṹ��ָ�룬�˽ṹȷ�����豸����λͼ�����ݸ�ʽ��
	*uUsage��ָ��BITMAPINFO�ṹ��bmiColors��Ա�ĸ�ʽ��������Ϊ����ȡֵ��
	*DIB_PAL_COLORS����ɫ����ָ��ǰ�߼���ɫ���16λ����ֵ���鹹�ɡ�
	*DIB_RGB_COLORS����ɫ���ɺ졢�̡�����RGB������ֱ��ֵ���ɡ�
	*����ֵ�����lpvBits�����ǿգ����Һ������óɹ�����ô����ֵΪ��λͼ���Ƶ�ɨ��������
	*/
	GetDIBits(hMemDC, hBmp, 0, m_iBmpHeight, m_pBmpSource, &m_stBitmapInfo, DIB_RGB_COLORS);
	GetDIBits(hMemDC, hBmp, 0, m_iBmpHeight, m_pBmpRender, &m_stBitmapInfo, DIB_RGB_COLORS);

	//��ȡ��λͼ���ݣ��ͷ��ڴ�DC
	DeleteDC(hMemDC);

	//���ö�ʱ��
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
 * ���ܣ��ͷ�ˮ��������Դ
 * ������
 *		void
 * ����ֵ��
 *		void
 */
void CRipple::FreeRipple()
{
	//�ͷ���Դ
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
	//ɱ��ʱ��
//	KillTimer(m_hWnd, (UINT_PTR)this);
}

/**
 * ���ܣ�ˮ����ɢ
 * ������
 *		void
 * ����ֵ��
 *		void
 */
void CRipple::WaveSpread()
{


	int* lpWave1 = m_pWaveBuf1;
	int* lpWave2 = m_pWaveBuf2;

	for (int i = m_iBmpWidth; i < (m_iBmpHeight - 1) * m_iBmpWidth; i++)
	{
		//������ɢ
		lpWave2[i] = ((lpWave1[i - 1] + lpWave1[i - m_iBmpWidth] +
			lpWave1[i + 1] + lpWave1[i + m_iBmpWidth]) >> 1) - lpWave2[i];

		//����˥��
		lpWave2[i] -= (lpWave2[i] >> 5);
	}

	//����������
	m_pWaveBuf1 = lpWave2;
	m_pWaveBuf2 = lpWave1;
}

/**
 * ���ܣ�����ˮ��������Ⱦˮ��λͼ����
 * ������
 *		void
 * ����ֵ��
 *		void
 */
void CRipple::WaveRender()
{

	int iPtrSource = 0;
	int iPtrRender = 0;
	int lineIndex = m_iBmpWidth;
	int iPosX = 0;
	int iPosY = 0;

	//ɨ��λͼ
	for (int y = 1; y < m_iBmpHeight - 1; y++)
	{
		for (int x = 0; x < m_iBmpWidth; x++)
		{
			//���ݲ�������λͼ����ƫ��ֵ����Ⱦ�㣨x��y)��ӦԭʼͼƬ��iPosX��iPosY��
			iPosX = x + (m_pWaveBuf1[lineIndex - 1] - m_pWaveBuf1[lineIndex + 1]);
			iPosY = y + (m_pWaveBuf1[lineIndex - m_iBmpWidth] - m_pWaveBuf1[lineIndex + m_iBmpWidth]);
			//����һ�ּ���ƫ�Ƶķ���
			//int waveData = (1024 - m_pWaveBuf1[lineIndex]);
			//iPosX = (x - m_iBmpWidth/2)*waveData/1024 + m_iBmpWidth/2;
			//iPosY = (y - m_iBmpHeight/2)*waveData/1024 + m_iBmpHeight/2;

			if (0 <= iPosX && iPosX < m_iBmpWidth &&
				0 <= iPosY && iPosY < m_iBmpHeight)
			{
				//�ֱ����ԭʼλͼ��iPosX��iPosY������Ⱦλͼ��x��y)��Ӧ����ʼλͼ����
				iPtrSource = iPosY * m_iBytesPerWidth + iPosX * 3;
				iPtrRender = y * m_iBytesPerWidth + x * 3;
				//��Ⱦλͼ�����´������
				for (int c = 0; c < 3; c++)
				{
					m_pBmpRender[iPtrRender + c] = m_pBmpSource[iPtrSource + c];
				}
			}

			lineIndex++;
		}
	}
	//������Ⱦ���λͼ
	SetDIBits(m_hRenderDC, m_hRenderBmp, 0, m_iBmpHeight, m_pBmpRender, &m_stBitmapInfo, DIB_RGB_COLORS);
}



/**
 * ���ܣ�ˢ��ˮ����hDc��
 * ������
 *		[in]		hDc			ˢ��Ŀ��DC��һ��Ϊ��ĻDC��Ҫ��ʾ�����
 * ����ֵ��
 *		void
 */


void CRipple::UpdateFrame(HDC hDc)
{
	BitBlt(hDc, 0, 0, m_iBmpWidth, m_iBmpHeight, m_hRenderDC, 0, 0, SRCCOPY);
}

/**
 * ���ܣ���ʯ�ӣ��趨��Դ��
 * ������
 *		[in]		x				ʯ��λ��x
 *		[in]		y				ʯ��λ��y
 *		[in]		stoneSize		ʯ�Ӵ�С���뾶��
 *		[in]		stoneWeight		ʯ������
 * ����ֵ��
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

			//���Ʒ�Χ�����ܳ���ͼƬ
			if (posX < 0 || posX >= m_iBmpWidth ||
				posY < 0 || posY >= m_iBmpHeight)
			{
				continue;
			}

			//��һ��Բ�������ڣ���ʼ�����ܻ�����1
			if (i * i + j * j <= stoneSize * stoneSize)
			{
				m_pWaveBuf1[posY * m_iBmpWidth + posX] = stoneWeight;
			}
		}
	}
}