///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2021 Advanced Software Engineering Limited
//
// You may use and modify the code in this file in your application, provided the code and
// its modifications are used only in conjunction with ChartDirector. Usage of this software
// is subjected to the terms and condition of the ChartDirector license.
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "include/chartdir.h"
#include <afxmt.h>

//
// Constants
//
#ifdef CD_NAMESPACE
namespace CD_NAMESPACE
{
#endif

namespace Chart
{
	//
	// Mouse usage mode constants
	//
	enum
	{
		MouseUsageDefaultCapture = -1,
		MouseUsageDefault = 0,
		MouseUsageScroll = 1,
		MouseUsageZoomIn = 3,
		MouseUsageZoomOut = 4,

		AutoDpi = 0,
	};
}

#ifdef CD_NAMESPACE
}
#endif

//
// Forward declarations
//
class CViewPortControl;

//
// Utility to convert from UTF8 string to MFC TCHAR string.
//
class UTF8toTCHAR
{
public :
	UTF8toTCHAR(const char *utf8_string) : t_string(0), needFree(false)
	{
		if (0 == utf8_string)
			t_string = 0;
		else if (0 == *utf8_string)
			t_string = _T("");
		else if ((sizeof(TCHAR) == sizeof(char)) && isPureAscii(utf8_string))
			// No conversion needed for pure ASCII text
			t_string = (TCHAR *)utf8_string;
		else
		{
			// Either TCHAR = Unicode (2 bytes), or utf8_string contains non-ASCII characters.
			// Needs conversion
			needFree = true;

			// Convert to Unicode (2 bytes)
			int string_len = (int)strlen(utf8_string);
			wchar_t *buffer = new wchar_t[string_len + 1];
			MultiByteToWideChar(CP_UTF8, 0, utf8_string, -1, buffer, string_len + 1);
			buffer[string_len] = 0;
					
#ifdef _UNICODE
			t_string = buffer;
#else
			// TCHAR is MBCS - need to convert back to MBCS
			t_string = new char[string_len * 2 + 2];
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, t_string, string_len * 2 + 1, 0, 0);
			t_string[string_len * 2 + 1] = 0;
			delete[] buffer;
#endif
		}

	}

	operator const TCHAR*()
	{
		return t_string;
	}

	~UTF8toTCHAR()
	{
		if (needFree)
			delete[] t_string;
	}

private :
	TCHAR *t_string;
	bool needFree;

	//
	// helper utility to test if a string contains only ASCII characters
	//
	bool isPureAscii(const char *s)
	{
		while (*s != 0) { if (*(s++) & 0x80) return false; }
		return true;
	}

	//disable assignment
	UTF8toTCHAR(const UTF8toTCHAR &rhs);
	UTF8toTCHAR &operator=(const UTF8toTCHAR &rhs);
};

//
// Utility to convert from MFC TCHAR string to UTF8 string
//
class TCHARtoUTF8
{
public :
	TCHARtoUTF8(const TCHAR *t_string) : utf8_string(0), needFree(false)
	{
		if (0 == t_string)
			utf8_string = 0;
		else if (0 == *t_string)
			utf8_string = "";
		else if ((sizeof(TCHAR) == sizeof(char)) && isPureAscii((char *)t_string))
			// No conversion needed for pure ASCII text
			utf8_string = (char *)t_string;
		else
		{
			// TCHAR is non-ASCII. Needs conversion.
	
			needFree = true;
			int string_len = (int)_tcslen(t_string);

#ifndef _UNICODE
			// Convert to Unicode if not already in unicode.
			wchar_t *w_string = new wchar_t[string_len + 1];
			MultiByteToWideChar(CP_ACP, 0, t_string, -1, w_string, string_len + 1);
			w_string[string_len] = 0;
#else
			wchar_t *w_string = (wchar_t*)t_string;
#endif

			// Convert from Unicode (2 bytes) to UTF8
			utf8_string = new char[string_len * 3 + 1];
			WideCharToMultiByte(CP_UTF8, 0, w_string, -1, utf8_string, string_len * 3 + 1, 0, 0);
			utf8_string[string_len * 3] = 0;
					
			if (w_string != (wchar_t *)t_string)
				delete[] w_string;
		}

	}

	operator const char*()
	{
		return utf8_string;
	}

	~TCHARtoUTF8()
	{
		if (needFree)
			delete[] utf8_string;
	}

private :
	char *utf8_string;
	bool needFree;

	//
	// helper utility to test if a string contains only ASCII characters
	//
	bool isPureAscii(const char *s)
	{
		while (*s != 0) { if (*(s++) & 0x80) return false; }
		return true;
	}
	
	//disable assignment
	TCHARtoUTF8(const TCHARtoUTF8 &rhs);
	TCHARtoUTF8 &operator=(const TCHARtoUTF8 &rhs);
};

/////////////////////////////////////////////////////////////////////////////
// CRectCtrl window

//
// A rectangle with a background color. Use as thick edges for the selection
// rectangle.
//

class CRectCtrl : public CStatic
{
public:
	BOOL Create(CWnd* pParentWnd, COLORREF c);
	void SetColor(COLORREF c);

protected:
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

private :
	CBrush m_Color;
};


/////////////////////////////////////////////////////////////////////////////
// CStaticHelper utiltiies

class CStaticHelper : public CStatic
{
private:
	HBITMAP m_currentHBITMAP;
	int m_WCCisV6;
	bool m_testMode;
	int m_dpi;
	int m_detectedDpi;
	int m_bgColor;

	double m_toImageScaleX;
	double m_toImageScaleY;

	void setHBITMAP(HBITMAP newBitMap);

public:
	CStaticHelper();

	virtual void setDPI(int dpi);
	virtual int getDPI();

	virtual void setBgColor(int c);
	virtual int getParentBgColor();

	virtual double toImageX(int x);
	virtual double toImageY(int y);
	virtual int toDisplayX(double x);
	virtual int toDisplayY(double y);

protected:
	virtual MemBlock renderChart(BaseChart* c);
	HBITMAP bmpToHBITMAP(const char* data);
	virtual void displayChart(BaseChart* c);
	virtual HBITMAP getDynamicTip(CPoint* pos);
	virtual void paintBMP(CDC* dc, HBITMAP hBmp, int x, int y);

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CChartViewer window

//
// Event message ID
//
#define CVN_ViewPortChanged	1000			// View port has changed
#define CVN_MouseMoveChart 1001				// Mouse moves over the chart
#define CVN_MouseLeaveChart 1002			// Mouse leaves the chart
#define CVN_MouseMovePlotArea 1003			// Mouse moves over the extended plot area
#define CVN_MouseLeavePlotArea 1004			// Mouse leaves the extended plot area


class CEnhancedToolTip
{
public:
	CEnhancedToolTip() { setToolTip(0);  }
	~CEnhancedToolTip() { setToolTip(0); }
	void setToolTip(HBITMAP h) {
		if ((0 != toolTipHandle) && (h != toolTipHandle)) {
			DeleteObject(toolTipHandle);
			toolTipW = toolTipH = 0;
		}
		if (0 != (toolTipHandle = h)) {
			BITMAP b;
			GetObject(h, sizeof(BITMAP), &b);
			toolTipW = b.bmWidth;
			toolTipH = b.bmHeight;
		}
	}
	void setPos(int x, int y) { toolTipX = x; toolTipY = y; }
	HBITMAP getToolTip() { return toolTipHandle; }
	CRect getBounds() { return CRect(toolTipX, toolTipY, toolTipX + toolTipW, toolTipY + toolTipH); }
private:
	int toolTipX;
	int toolTipY;
	int toolTipW;
	int toolTipH;
	HBITMAP toolTipHandle;
};


class CChartViewer : public CStaticHelper, public ViewPortManager
{
public:
	CChartViewer();

	//
	// CChartViewer properties
	//

	virtual void setChart(BaseChart *c);
	virtual BaseChart *getChart();

	virtual void setViewPortControl(CViewPortControl *vpc);
	virtual CViewPortControl *getViewPortControl();

	virtual void setImageMap(const char *imageMap);
	virtual ImageMapHandler *getImageMapHandler();

	virtual void setDefaultToolTip(LPCTSTR text);
	virtual void setCDMLToolTipPrefix(LPCTSTR text);
	virtual CToolTipCtrl *getToolTipCtrl();

	virtual void setSelectionBorderWidth(int width);
	virtual int getSelectionBorderWidth();

	virtual void setSelectionBorderColor(COLORREF c);
	virtual COLORREF getSelectionBorderColor();
	
	virtual void setMouseUsage(int mouseUsage);
	virtual int getMouseUsage();

	virtual void setZoomDirection(int direction);
	virtual int getZoomDirection();
	
	virtual void setScrollDirection(int direction);
	virtual int getScrollDirection();

	virtual void setZoomInRatio(double ratio);
	virtual double getZoomInRatio();

	virtual void setZoomOutRatio(double ratio);
	virtual double getZoomOutRatio();

	virtual void setMouseWheelZoomRatio(double ratio);
	virtual double getMouseWheelZoomRatio();

	virtual void setMinimumDrag(int offset);
	virtual int getMinimumDrag();

	virtual void setUpdateInterval(int interval);
	virtual int getUpdateInterval();

	virtual bool needUpdateChart();
	virtual bool needUpdateImageMap();

	virtual bool isMouseOnPlotArea();
	virtual bool isInMouseMoveEvent();
	virtual bool isMouseDragging();

	//
	// CChartViewer methods
	//

	// Trigger the ViewPortChanged event
	virtual void updateViewPort(bool needUpdateChart, bool needUpdateImageMap);
	
	// Request CChartViewer to redisplay the chart
	virtual void updateDisplay();

	// Handles mouse wheel zooming
	virtual BOOL onMouseWheelZoom(int x, int y, short zDelta);

	// Set the message used to remove the dynamic layer
	virtual void removeDynamicLayer(int msg);
	
	// Get the mouse coordinates
	virtual int getChartMouseX();
	virtual int getChartMouseY();

	// Get the mouse coordinates bounded to the plot area
	virtual int getPlotAreaMouseX();
	virtual int getPlotAreaMouseY();

	// Utility to obtain the viewport right/top
	virtual double getViewPortRight() { return getViewPortLeft() + getViewPortWidth(); }
	virtual double getViewPortBottom() { return getViewPortTop() + getViewPortHeight(); }

	virtual void redraw(const CRect& r);

	// Overrides
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual HBITMAP getDynamicTip(CPoint* pos);

	// Generated message map functions
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDelayedMouseMove();
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

private:

	//
	// CChartViewer configurable properties
	//

	BaseChart *m_currentChart;			// Current BaseChart object
	CString m_defaultToolTip;			// Default tool tip text
	CString m_CDMLToolTipPrefix;		// CDML tool tip prefix
	CToolTipCtrl m_ToolTip;				// CToolTipCtrl for managing tool tips
	CEnhancedToolTip m_enhancedToolTip;	// To support enhanced tool tips
	bool m_toolTipHasAttached;			// Tooltip controls have attached to CChartViewer
	COLORREF m_selectBoxLineColor;		// Selectiom box border color
	int m_selectBoxLineWidth;			// Selectiom box border width
	int m_mouseUsage;					// Mouse usage mode
	int m_zoomDirection;				// Zoom direction
	int m_scrollDirection;				// Scroll direction
	double m_zoomInRatio;				// Click zoom in ratio
	double m_zoomOutRatio;				// Click zoom out ratio
	double m_mouseWheelZoomRatio;		// Mouse wheel zoom ratio
	int m_minDragAmount;				// Minimum drag amount
	int m_updateInterval;				// Minimum interval between chart updates
	bool m_needUpdateChart;				// Has pending chart update request
	bool m_needUpdateImageMap;			// Has pending image map udpate request

	//
	// Keep track of mouse states
	//

	int m_currentHotSpot;				// The hot spot under the mouse cursor.
	bool m_isClickable;					// Mouse is over a clickable hot spot.
	bool m_isOnPlotArea;				// Mouse is over the plot area.
	bool m_isPlotAreaMouseDown;			// Mouse left button is down in the plot area.
	bool m_isDragScrolling;				// Is drag scrolling the chart.
	bool m_isMouseTracking;				// Is tracking mouse leave event.
    bool m_isInMouseMove;				// Is in mouse moeve event handler
	HCURSOR getHCursor();				// The current mouse cursor

	//
	// Dragging support
	//

	int m_plotAreaMouseDownXPos;		// The starting x coordinate of the mouse drag.
	int m_plotAreaMouseDownYPos;		// The starting y coordinate of the mouse drag.
	bool isDrag(int direction, CPoint point);				// Check if mouse is dragging
	void OnPlotAreaMouseDrag(UINT nFlags, CPoint point);	// Process mouse dragging

	//
	// Selection rectangle
	//

	CRectCtrl m_LeftLine;				// Left edge of selection rectangle
	CRectCtrl m_RightLine;				// Right edge of selection rectangle
	CRectCtrl m_TopLine;				// Top edge of selection rectangle
	CRectCtrl m_BottomLine;				// Bottom edge of selection rectangle
	
	void initRect();					// Initialize selection rectangle edges
	void drawRect(int x, int y, int width, int height);		// Draw selection rectangle
	void setRectVisible(bool b);		// Show/hide selection rectangle
	void getDragZoomRect(int px, int py, int* x, int* y, int* w, int* h);


	//
	// Chart update rate control
	//

	bool m_holdTimerActive;				// Delay chart update to limit update frequency
	
	int m_delayUpdateChart;				// Delay chart update until the mouse event is completed
	BaseChart *m_delayedChart;			// The chart to be used for delayed update.
	void commitUpdateChart();			// Commit updating the chart.

	unsigned int m_lastMouseMove;		// The timestamp of the last mouse move event.
	bool m_hasDelayedMouseMove;			// Delay the mouse move event to allow other updates
	UINT m_delayedMouseMoveFlag;		// The mouse key flags of the delayed mouse move event.
	CPoint m_delayedMouseMovePoint;		// The mouse coordinates of the delayed mouse move event.
	void commitMouseMove(UINT nFlags, CPoint point);    // Raise the delayed mouse move event.

	bool m_delayImageMapUpdate;			// Delay image map update until mouse moves on plot area

	//
	// Track Cursor support
	//

	int m_currentMouseX;				// Get the mouse x-pixel coordinate
	int m_currentMouseY;				// Get the mouse y-pixel coordinate
	int m_isInMouseMovePlotArea;		// flag to indicate if is in a mouse move plot area event.

	//
	// Tooltip Support
	//
	ImageMapHandler* m_hotSpotTester;   // ImageMapHander representing the image map
	BaseChart* m_imageMapChart;         // BaseChart object for handling dynamic map
	void showToolTip(const char* text);
	void showEnhancedToolTip(const char* text);
	void moveEnhancedToolTip(const CRect &currentPos);

	//
	// Dynamic Layer support
	//

	int m_autoHideMsg;					// The message that will trigger removing the dynamic layer.
	void applyAutoHide(int msg);		// Attempt to remove the dynamic layer with the given message.

	//
	// CViewPortControl support
	//
	
	CViewPortControl *m_vpControl;		// Associated CViewPortControl      
	bool m_ReentrantGuard;				// Prevents infinite calling loops
public:
	afx_msg void OnBnClickedButton1();
};

/////////////////////////////////////////////////////////////////////////////
// CViewPortControl window

class CViewPortControl : public CStaticHelper, public ViewPortControlBase
{
public:
	CViewPortControl();

	// Set the chart to be displayed in the control
	virtual void setChart(BaseChart *c);
	virtual BaseChart *getChart();
	
	// Associated CChartViewer
	virtual void setViewer(CChartViewer *viewer);
	virtual CChartViewer *getViewer();

	// Request the CViewPortControl to update its contents 
	virtual void updateDisplay();

protected:

	// Generated message map functions
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()

private:

	CChartViewer *m_ChartViewer;		// Associated CChartViewer
	BaseChart *m_Chart;					// BaseChart object displayed in the control
	bool m_ReentrantGuard;				// Prevents infinite calling loops

	int m_mouseDownX;					// Current mouse x coordinates
	int m_mouseDownY;					// Current mouse y coordinates
	bool isDrag(CPoint point);			// Check if mouse is dragging

	HCURSOR getHCursor(int position);	// Get the mouse cursor
	void paintDisplay();				// Paint the display
	void syncState();					// Synchronize the CViewPortControl with CChartViewer
	void updateChartViewerIfNecessary();	// Update CChartViewer if viewport has changed
};
