
// PingPongView.h: интерфейс класса CPingPongView
//

#pragma once

#include "Ball.h"

class CPingPongView : public CView
{
protected: // создать только из сериализации
	CPingPongView() noexcept;
	DECLARE_DYNCREATE(CPingPongView)

// Атрибуты
public:
	CPingPongDoc* GetDocument() const;
	int m_FPSTimer, desiredFPS, delay, pl1Score, pl2Score;
	double deltaTime, windowHeight, windowWidth, zoom;
	Racket playerOne;
	Racket playerTwo;
	Ball ball;

	bool pause, ctrl;
// Операции
public:
	BOOL InitializeOpenGL();
	BOOL SetupPixelFormat();
	void DrawAxes();
	void DrawRacket(Racket *racket);
	void DrawBall();
	void DrawBackground(COLORREF color1, COLORREF color2);

	void SetPlayerOne();
	void SetPlayerTwo();
	void CreateBall();

	void UpdateGame();

	void RenderNumber(int number, double x, double y);
	void DrawSquare(double x, double y, double size);

public:
	virtual void OnDraw(CDC* pDC);  // переопределено для отрисовки этого представления
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Реализация
public:
	virtual ~CPingPongView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // версия отладки в PingPongView.cpp
inline CPingPongDoc* CPingPongView::GetDocument() const
   { return reinterpret_cast<CPingPongDoc*>(m_pDocument); }
#endif

