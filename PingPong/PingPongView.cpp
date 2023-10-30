
// PingPongView.cpp: реализация класса CPingPongView
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS можно определить в обработчиках фильтров просмотра реализации проекта ATL, эскизов
// и поиска; позволяет совместно использовать код документа в данным проекте.
#ifndef SHARED_HANDLERS
#include "PingPong.h"
#endif

#include "MainFrm.h"

#include "PingPongDoc.h"
#include "PingPongView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum { GLF_START_LIST = 1000 };

// CPingPongView

IMPLEMENT_DYNCREATE(CPingPongView, CView)

BEGIN_MESSAGE_MAP(CPingPongView, CView)
	// Стандартные команды печати
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPingPongView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// Создание или уничтожение CPingPongView

CPingPongView::CPingPongView() noexcept
{
	m_FPSTimer = 0;
	pl1Score = pl2Score = 0;
	zoom = 1.0f;
	desiredFPS = 60;
	pause = false;
}

CPingPongView::~CPingPongView()
{
	HGLRC hrc = ::wglGetCurrentContext();
	::wglMakeCurrent(NULL, NULL);
	
	if (hrc)
		::wglDeleteContext(hrc);
}

int CPingPongView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!InitializeOpenGL())
	{
		AfxMessageBox(_T("Помилка ініціалізації OpenGL"),MB_ICONINFORMATION);
		return -1;
	}

	return 0;
}

BOOL CPingPongView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CPingPongView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	srand((unsigned int)time(NULL));

	SetPlayerOne();
	SetPlayerTwo();
	CreateBall();
}

// Рисование CPingPongView

void CPingPongView::OnDraw(CDC* pDC)
{
	CPingPongDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	glPushMatrix();
	glScalef(zoom, zoom, zoom);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glClearColor(48.0/255.0f, 53.0/255.0f, 161.0/255.0f, 0);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawBuffer(GL_BACK);

	DrawBackground(RGB(42, 0, 76), RGB(30, 7, 41));

	DrawAxes();

	RenderNumber(pl1Score, -0.2, windowHeight - 0.025);
	RenderNumber(pl2Score, 0.1, windowHeight - 0.025);
	
	DrawRacket(&playerOne);
	DrawRacket(&playerTwo);

	glPopMatrix();
	DrawBall();

	glPopMatrix();
	glPopAttrib();
	glFinish();
	SwapBuffers(wglGetCurrentDC());

	CMainFrame* pMainFrame = (CMainFrame*)GetParentFrame();

	CString widowCaption = L"Рахунок: " + (_bstr_t)pl1Score + L" : " + (_bstr_t)pl2Score + L"   Сторона відбиття: ";
	if (ball.GetSideCollision() & BOTTOM) {
		widowCaption += L"Нижня";
	}

	if (ball.GetSideCollision() & TOP) {
		widowCaption += L"Верхня";
	}

	if (ball.GetSideCollision() & CENTER) {
		widowCaption += L"Центер";
	}

	pMainFrame->SetWindowText(widowCaption);
}


// Печать CPingPongView


void CPingPongView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPingPongView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// подготовка по умолчанию
	return DoPreparePrinting(pInfo);
}

void CPingPongView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте дополнительную инициализацию перед печатью
}

void CPingPongView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте очистку после печати
}

void CPingPongView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPingPongView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Диагностика CPingPongView

#ifdef _DEBUG
void CPingPongView::AssertValid() const
{
	CView::AssertValid();
}

void CPingPongView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPingPongDoc* CPingPongView::GetDocument() const // встроена неотлаженная версия
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPingPongDoc)));
	return (CPingPongDoc*)m_pDocument;
}
#endif //_DEBUG


BOOL CPingPongView::InitializeOpenGL() {
	HGLRC hrc;
	
	if (!SetupPixelFormat())
		return FALSE;

	hrc = wglCreateContext(GetDC()->GetSafeHdc());
	wglMakeCurrent(GetDC()->GetSafeHdc(), hrc);

	CFont font;
	font.CreatePointFont(50, _T("Arial"));

	SelectObject(wglGetCurrentDC(), font.m_hObject);
	wglUseFontOutlinesA(wglGetCurrentDC(), 0, 255, GLF_START_LIST, 0, 0, WGL_FONT_POLYGONS, 0);

	//glEnable(GL_DEPTH_TEST);

	m_FPSTimer = 1;

	delay = 1000 / desiredFPS;

	deltaTime = (double)delay / 1000.0;
	
	SetTimer(m_FPSTimer, delay, NULL);
	return TRUE;
}

BOOL CPingPongView::SetupPixelFormat()
{
	CMainFrame* pMainFrame = (CMainFrame*)this->GetParentFrame();

	int pixelformat;

	if ((pixelformat = ChoosePixelFormat(GetDC()->GetSafeHdc(), &pMainFrame->pfd)) == 0)
	{
		AfxMessageBox(L"Помилка ChoosePixelFormat");
		return FALSE;
	}

	if (SetPixelFormat(GetDC()->GetSafeHdc(), pixelformat, &pMainFrame->pfd) == FALSE)
	{
		AfxMessageBox(L"Помилка SetPixelFormat");
		return FALSE;
	}

	::DescribePixelFormat(GetDC()->GetSafeHdc(), pixelformat, sizeof(pMainFrame->pfd), &(pMainFrame->pfd));

	if (pMainFrame->pfd.cColorBits < 16) {
		AfxMessageBox(L"Помилка ColorBits < 16");
		return FALSE;
	}

	return TRUE;
}

void CPingPongView::DrawBackground(COLORREF color1, COLORREF color2) {
	
	int r1, g1, b1;

	r1 = GetRValue(color1);
	g1 = GetGValue(color1);
	b1 = GetBValue(color1);

	int r2, g2, b2;

	r2 = GetRValue(color2);
	g2 = GetGValue(color2);
	b2 = GetBValue(color2);

	double x = 0;
	double y = 0;

	double halfWidth = windowWidth;
	double halfHeight = windowHeight;

	double coordsXY[][2] =
	{
		{x - halfWidth, y + halfHeight},
		{x + halfWidth, y + halfHeight},
		{x - halfWidth, y - halfHeight},
		{x + halfWidth, y - halfHeight}
	};


	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i < 4; i++)
	{
		if((i + 1) % 2 == 0) glColor3ub(r1, g1, b1);
		else glColor3ub(r2, g2, b2);
		glVertex3d(coordsXY[i][0], coordsXY[i][1], 0);

	}
	glEnd();
	glLineWidth(5);

	glEnable(GL_SMOOTH);
	double delta = 0.05;
	// Малюємо квадратну рамку
	glBegin(GL_LINE_LOOP);

	glColor3ub(255, 17, 131);
	glVertex2f(-windowWidth - delta , -windowHeight - delta);

	glColor3ub(193, 17, 255);
	glVertex2f(windowWidth + delta, -windowHeight - delta);

	glColor3ub(17, 141, 255);
	glVertex2f(windowWidth + delta, windowHeight + delta);

	glColor3ub(17, 255, 69);
	glVertex2f(-windowWidth - delta, windowHeight + delta);
	glEnd();

	glDisable(GL_SMOOTH);
	glPopMatrix();
}

void CPingPongView::DrawAxes() {

	float delta = 0.035;
	
	glPushMatrix();
	glScalef(zoom, zoom, zoom);
	glEnable(GL_LINE_STIPPLE);
	glLineWidth(5);
    glLineStipple(15, 0xAAAF); // Шаблон: 1010101010101010 (16 біт)
   
	glBegin(GL_LINES);

	glColor3ub(241, 255, 158);
	glVertex2f(0, windowHeight);

	glColor3ub(255, 210, 158);
	glVertex2f(0, -windowHeight);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glPopMatrix();
}

void CPingPongView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	float fAspect;

	if (cx == 0 || cy == 0) cx = cy = 1;

	glViewport(0, 0, cx, cy);
	fAspect = 1.0f * cx / cy;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	double mn = 1.0;

	if (cx <= cy) {
		glOrtho(-mn, mn, -mn / fAspect, mn / fAspect, -mn, mn);
		windowHeight = mn / fAspect;
		windowWidth = mn;

		playerOne.SetX(-windowWidth + 0.15);
		playerTwo.SetX(windowWidth - 0.15);
	}

	else {
		glOrtho(-mn * fAspect, mn * fAspect, -mn, mn, -mn, mn);
		
		windowHeight = mn;
		windowWidth = mn * fAspect;

		playerOne.SetX(-windowWidth + 0.15);	
		playerTwo.SetX(windowWidth - 0.15);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Invalidate(false); //Відключено через те, що гра вже має фрейм рейт в 60 FPS
}


void CPingPongView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_FPSTimer) {
		UpdateGame();
		Invalidate(false);								//Наявність 60 FPS
	}

	CView::OnTimer(nIDEvent);
}

void CPingPongView::DrawRacket(Racket* racket) {

	COLORREF color = racket->GetColor();
	int r, g, b;

	bool reversed = racket->GetX() > 0;

	r = GetRValue(color);
	g = GetGValue(color);
	b = GetBValue(color);

	double x = racket->GetX();
	double y = racket->GetY();

	double halfWidth = racket->GetWidth() / 2;
	double halfHeight = racket->GetHeight() / 2;

	double coordsXY[][2] =
	{
		{x - halfWidth, y + halfHeight},
		{x + halfWidth, y + halfHeight}, 
		{x - halfWidth, y - halfHeight}, 
		{x + halfWidth, y - halfHeight}
	};

	glPushMatrix();
	glScalef(zoom, zoom, zoom);
	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i < 4; i++)
	{
		if (!reversed) {
			if ((i + 1) % 2 == 0) glColor3ub(r, g, b);
			else glColor3ub(r - 100, g - 100, b - 100);
		}
		else {
			if ((i + 1) % 2 == 0)  glColor3ub(r - 100, g - 100, b - 100); 
			else glColor3ub(r, g, b);
		}

		glVertex3d(coordsXY[i][0], coordsXY[i][1], 0);

	}
	glEnd();
	glPopMatrix();
}

void CPingPongView::DrawBall() {
	COLORREF color = ball.GetColor();
	int r, g, b;

	r = GetRValue(color);
	g = GetGValue(color);
	b = GetBValue(color);

	double centerX = ball.GetX();
	double centerY = ball.GetY();

	double radius = ball.GetWidth() / 2;
	int segments = 4;

	glPushMatrix();
	glScalef(zoom, zoom, zoom);
	glEnable(GL_SMOOTH);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(centerX, centerY);

	glColor3ub(r, g, b);

	for (int i = 0; i <= segments; i++) {

		double angle = 2.0f * 3.1415926 * i /segments;
		if (ball.GetRotateAngle() < 0) angle = -angle;

		angle += ball.GetRotateAngle();

		glVertex2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
	}

//	glRotatef(ball.GetRotateAngle(), zoom, zoom, 0.0f);
	glEnd();
	glDisable(GL_SMOOTH);
	glPopMatrix();
}

BOOL CPingPongView::PreTranslateMessage(MSG* pMsg)
{
	int x = (int)pMsg->wParam;

	if (pMsg->message == WM_KEYDOWN) {
		if (x == 27) {
			pause = !pause;
		}
		
		if (x == 17) {
			ctrl = true;
		}

		if (x == 87 && !playerOne.CheckTopCollision()) {		//W - button
			playerOne.SetMovingUp(true);
		}

		if (x == 83 && !playerOne.CheckBottomCollision()) {		//S - button
			playerOne.SetMovingDown(true);
		}

		if (x == 38 && !playerTwo.CheckTopCollision()) {		//Up Arrow - button
			playerTwo.SetMovingUp(true);
		}

		if (x == 40 && !playerTwo.CheckBottomCollision()) {		//Down Arrow - button
			playerTwo.SetMovingDown(true);
		}
	}

	if (pMsg->message == WM_KEYUP) {

		if (x == 17) {
			ctrl = false;
		}

		if (x == 87) {
			playerOne.SetMovingUp(false);
		}

		if (x == 83) {
			playerOne.SetMovingDown(false);
		}

		if (x == 38) {
			playerTwo.SetMovingUp(false);
		}

		if (x == 40) {
			playerTwo.SetMovingDown(false);
		}
	}

	
	return CView::PreTranslateMessage(pMsg);
}



void CPingPongView::SetPlayerOne() {
	playerOne.SetY(0);
	playerOne.SetDefault();

	playerOne.SetStep(desiredFPS);
	//playerOne.TurnOnAi();
}

void CPingPongView::SetPlayerTwo() {
	playerTwo.SetY(0);
	playerTwo.SetDefault();

	playerTwo.SetStep(desiredFPS);

	playerTwo.TurnOnAi();
}

void CPingPongView::CreateBall() {
	double speedX = -1;
	if (ball.GetLeftSide() & RIGHT) speedX = -speedX;

	ball.SetDefault();
	
	ball.SetX(0);
	ball.SetY(0);
	ball.SetSpeedX(speedX);

	ball.SetStep(desiredFPS);
	ball.BackToGame();
}

void CPingPongView::UpdateGame() {

	if (pause) return;

	playerOne.SetTopBoundary(windowHeight);
	playerOne.SetBottomBoundary(-windowHeight);

	playerTwo.SetTopBoundary(windowHeight);
	playerTwo.SetBottomBoundary(-windowHeight);

	ball.SetWindowHeight(windowHeight);
	ball.SetWindowWidth(windowWidth);

	playerOne.CheckCollision();
	playerTwo.CheckCollision();

	if (!playerOne.GetAiStatus()) {
		playerOne.UpdatePosition(deltaTime);
	}
	else {
		playerOne.AutoPlay(ball.GetInfoStruct(), deltaTime);
	}

	if (!playerTwo.GetAiStatus()) {
		playerTwo.UpdatePosition(deltaTime);
	}
	else {
		playerTwo.AutoPlay(ball.GetInfoStruct(), deltaTime);
	}

	ball.Move(&playerOne,&playerTwo);

	double angle = ball.GetRotateAngle();

	if (ball.GetRotateAngle() < 0) {
		angle -= 0.25;
	}

	else {
		angle += 0.25;
	}

	ball.SetRotateAngle(angle);

	if (!ball.InGame()) {
		if (ball.GetLeftSide() & LEFT) {
			pl2Score++;
		}
		else {
			pl1Score++;
		}

		CreateBall();
		SetPlayerOne();
		SetPlayerTwo();
	}
}


void CPingPongView::RenderNumber(int number, double x, double y) {

	double size = 0.05;	

	CString str = _bstr_t(number);
	
	double step = x;
	
	glPushMatrix();
	glScalef(zoom, zoom, zoom);

	if(x < 0)
		step = x * str.GetLength();

	for (int i = 0; i < str.GetLength(); i++) {
		wchar_t firstChar = str.GetAt(i);
		number = _wtoi(&firstChar);
		
		switch (number) {
		case 0:
			DrawSquare(step, y, size);
			DrawSquare(step + size, y, size);
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step, y - size, size);
			DrawSquare(step + size * 2, y - size, size);

			DrawSquare(step, y - size * 2, size);
			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step, y - size * 3, size);
			DrawSquare(step + size * 2, y - size * 3, size);

			DrawSquare(step, y - size * 4, size);
			DrawSquare(step + size, y - size * 4, size);
			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		case 1:
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step + size, y - size, size);
			DrawSquare(step + size * 2, y - size, size);

			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step + size * 2, y - size * 3, size);

			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		case 2:
			DrawSquare(step, y, size);
			DrawSquare(step + size, y, size);
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step + size * 2, y - size, size);

			DrawSquare(step, y - size * 2, size);
			DrawSquare(step + size, y - size * 2, size);
			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step, y - size * 3, size);

			DrawSquare(step, y - size * 4, size);
			DrawSquare(step + size, y - size * 4, size);
			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		case 3:
			DrawSquare(step, y, size);
			DrawSquare(step + size, y, size);
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step + size * 2, y - size, size);

			DrawSquare(step, y - size * 2, size);
			DrawSquare(step + size, y - size * 2, size);
			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step + size * 2, y - size * 3, size);

			DrawSquare(step, y - size * 4, size);
			DrawSquare(step + size, y - size * 4, size);
			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		case 4:
			DrawSquare(step, y, size);
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step, y - size, size);
			DrawSquare(step + size * 2, y - size, size);

			DrawSquare(step, y - size * 2, size);
			DrawSquare(step + size, y - size * 2, size);
			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step + size * 2, y - size * 3, size);

			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		case 5:
			DrawSquare(step, y, size);
			DrawSquare(step + size, y, size);
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step, y - size, size);

			DrawSquare(step, y - size * 2, size);
			DrawSquare(step + size, y - size * 2, size);
			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step + size * 2, y - size * 3, size);

			DrawSquare(step, y - size * 4, size);
			DrawSquare(step + size, y - size * 4, size);
			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		case 6:
			DrawSquare(step, y, size);
			DrawSquare(step + size, y, size);
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step, y - size, size);

			DrawSquare(step, y - size * 2, size);
			DrawSquare(step + size, y - size * 2, size);
			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step, y - size * 3, size);
			DrawSquare(step + size * 2, y - size * 3, size);

			DrawSquare(step, y - size * 4, size);
			DrawSquare(step + size, y - size * 4, size);
			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		case 7:
			DrawSquare(step, y, size);
			DrawSquare(step + size, y, size);
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step + size * 2, y - size, size);

			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step + size * 2, y - size * 3, size);

			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		case 8:
			DrawSquare(step, y, size);
			DrawSquare(step + size, y, size);
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step, y - size, size);
			DrawSquare(step + size * 2, y - size, size);

			DrawSquare(step, y - size * 2, size);
			DrawSquare(step + size, y - size * 2, size);
			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step, y - size * 3, size);
			DrawSquare(step + size * 2, y - size * 3, size);

			DrawSquare(step, y - size * 4, size);
			DrawSquare(step + size, y - size * 4, size);
			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		case 9:
			DrawSquare(step, y, size);
			DrawSquare(step + size, y, size);
			DrawSquare(step + size * 2, y, size);

			DrawSquare(step, y - size, size);
			DrawSquare(step + size * 2, y - size, size);

			DrawSquare(step, y - size * 2, size);
			DrawSquare(step + size, y - size * 2, size);
			DrawSquare(step + size * 2, y - size * 2, size);

			DrawSquare(step + size * 2, y - size * 3, size);

			DrawSquare(step + size * 2, y - size * 4, size);
			break;
		}

		if (x < 0)
			step -= x;
		else
			step += x * 2;
	}
	glPopMatrix();
}

void CPingPongView::DrawSquare(double x, double y, double size) {
	
	double coordsXY[][2] =
	{
		{x - size/2, y + size/2},
		{x + size / 2, y + size / 2},
		{x - size / 2, y - size / 2},
		{x + size / 2, y - size / 2}
	};

	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i < 4; i++)
	{
		glVertex3d(coordsXY[i][0], coordsXY[i][1], 0);
	}
	glEnd();
}

BOOL CPingPongView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (ctrl) {
		if (zDelta > 0 && zoom < 1.0) {
			zoom += 0.02;
		}
		if (zDelta < 0 && zoom > 0.02) {
			zoom -= 0.02;
		}
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
