///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2021 Advanced Software	Engineering	Limited
//
// You may use and modify the code in this file	in your	application, provided the code and
// its modifications are used only in conjunction with ChartDirector. Usage	of this	software
// is subjected	to the terms and condition of the ChartDirector	license.
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// CChartViewer / CViewPortControl Implementation
//
// The CChartViewer	is a subclass of CStatic for displaying	chart images. It extends CStatic 
// to support to support alpha transparency, image maps, clickable hot spots with tool tips, 
// zooming and scrolling and image update rate control. The CViewPortControl is a subclass of 
// CStatic for visualizing and supporting interactive control of the CChartViewer viewport.
//
// To use these controls in a dialog, in the Dialog Editor, drag a Picture control (CStatic
// control) to the form, configure its type as "Bitmap", and give it a unique ID. Then right 
// click on the control and add a variable to represent the control, using CChartViewer or
// CViewPortControl as the variable type.
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// ****** NOTE FOR PRE-COMPILED HEADER CONFIGURATION ******
// 
// The default pre-compiled header for MFC projects is "stdafx.h". Recent verions of Visual
// Studio may configure new MFC projects to use "pch.h" as the pre-compiled header. In this
// case, you would need to change the following include statement to the pre-compiled header 
// of your project.
//
#include "stdafx.h"
// Some MFC projects may use "pch.h" as the pre-compiled header instead of stdafx.h.
// #include "pch.h"

#include "ChartViewer.h"
#include <stdio.h>

#ifdef _DEBUG
#define	new	DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
//
// CChartViewer
//
/////////////////////////////////////////////////////////////////////////////

//
// Constants used in m_delayChartUpdate
//
enum { NO_DELAY, NEED_DELAY, NEED_UPDATE };
enum { UPDATE_VIEW_PORT_TIMER = 1, DELAYED_MOUSE_MOVE_TIMER = 2 };


static class DPIHelper
{
private :
	HINSTANCE hShCore;
	typedef /*HRESULT*/ long (*GetDpiForMonitor_t)(HMONITOR hmonitor, /*MONITOR_DPI_TYPE*/ int dpiType, UINT* dpiX, UINT* dpiY);
	GetDpiForMonitor_t getDpiForMonitorFunc;

public:
	DPIHelper() { getDpiForMonitorFunc = (hShCore = LoadLibrary(L"shcore.dll")) ? (GetDpiForMonitor_t)(GetProcAddress(0, "GetDpiForMonitor")) : 0; }
	~DPIHelper() { hShCore && FreeLibrary(hShCore); }
	int getDPI(HWND hWnd)
	{
		if (getDpiForMonitorFunc)
		{
			HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
			UINT xScale, yScale;
			if (monitor && (S_OK == getDpiForMonitorFunc(monitor, /*MDT_EFFECTIVE_DPI*/ 0, &xScale, &yScale)))
				return xScale;
		}

		HDC screen = GetDC(NULL);
		double hPixelsPerInch = GetDeviceCaps(screen, LOGPIXELSX);
		ReleaseDC(NULL, screen);
		return (int)(hPixelsPerInch + 0.5);
	}
} dpiHelper;


static class ASE_CursorManager
{
private:
	void* resourceLoader;
	HCURSOR hZoomInCursor;
	HCURSOR hZoomOutCursor;
	HCURSOR hNoZoomCursor;
	HCURSOR hNoMove2DCursor;
	HCURSOR hNoMoveHorizCursor;
	HCURSOR hNoMoveVertCursor;
	HCURSOR getCursor(HCURSOR* cachedCursor, int id)
	{
		if (0 != *cachedCursor)
			return *cachedCursor;
		if (0 == resourceLoader)
			resourceLoader = (HMODULE)Chart::getResourceLoader();
		return *cachedCursor = LoadCursorA((HMODULE)resourceLoader, MAKEINTRESOURCEA(id));
	}
public:
	ASE_CursorManager() : resourceLoader(0), hZoomInCursor(0), hZoomOutCursor(0),
		hNoZoomCursor(0), hNoMove2DCursor(0), hNoMoveHorizCursor(0), hNoMoveVertCursor(0) {}
	~ASE_CursorManager()
	{
		if (0 != hZoomInCursor)
			DestroyCursor(hZoomInCursor);
		if (0 != hZoomOutCursor)
			DestroyCursor(hZoomOutCursor);
		if (0 != hNoZoomCursor)
			DestroyCursor(hNoZoomCursor);
		if (0 != hNoMove2DCursor)
			DestroyCursor(hNoMove2DCursor);
		if (0 != hNoMoveHorizCursor)
			DestroyCursor(hNoMoveHorizCursor);
		if (0 != hNoMoveVertCursor)
			DestroyCursor(hNoMoveVertCursor);
	}
	HCURSOR getZoomInCursor() { return getCursor(&hZoomInCursor, 101); }
	HCURSOR getZoomOutCursor() { return getCursor(&hZoomOutCursor, 102); }
	HCURSOR getNoZoomCursor() { return getCursor(&hNoZoomCursor, 103); }
	HCURSOR getNoMove2DCursor() { return getCursor(&hNoMove2DCursor, 104); }
	HCURSOR getNoMoveHorizCursor() { return getCursor(&hNoMoveHorizCursor, 105); }
	HCURSOR getNoMoveVertCursor() { return getCursor(&hNoMoveVertCursor, 106); }
} cursorManager;


//
// Constructor
//
CChartViewer::CChartViewer()
{
	// current chart and hot spot tester
	m_currentChart = 0;
	m_imageMapChart = 0;
	m_hotSpotTester	= 0;
	m_toolTipHasAttached = false;
	m_CDMLToolTipPrefix = "<*block,bgColor=F0F0F0,edgeColor=808080,margin=5,roundedCorners=3*><*font,color=222222*>";

	// initialize chart	configuration
	m_selectBoxLineColor = RGB(0, 0, 0);
	m_selectBoxLineWidth = 2;
	m_mouseUsage = Chart::MouseUsageDefault;
	m_zoomDirection	= Chart::DirectionHorizontal;
	m_zoomInRatio =	2;
	m_zoomOutRatio = 0.5;
	m_mouseWheelZoomRatio = 1;
	m_scrollDirection =	Chart::DirectionHorizontal;
	m_minDragAmount	= 5;
	m_updateInterval = 20;

	// current state of	the	mouse
	m_isOnPlotArea = false;
	m_isPlotAreaMouseDown =	false;
	m_isDragScrolling =	false;
	m_currentHotSpot = -1;
	m_isClickable =	false;
	m_isMouseTracking = false;
	m_isInMouseMove = false;

	// chart update	rate support
	m_needUpdateChart =	false;
	m_needUpdateImageMap = false;
	m_holdTimerActive =	false;
	m_delayUpdateChart = NO_DELAY;
	m_delayedChart = 0;
	m_lastMouseMove = 0;
	m_hasDelayedMouseMove = false;
	m_delayImageMapUpdate = false;

	// track cursor support
	m_autoHideMsg = 0;
	m_currentMouseX = -0x3fffffff;
	m_currentMouseY = -0x3fffffff;
	m_isInMouseMovePlotArea = false;

	// CViewPortControl support
	m_vpControl = 0;
	m_ReentrantGuard = false;
}

BEGIN_MESSAGE_MAP(CChartViewer, CStaticHelper)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//
// Free	resources
//
void CChartViewer::OnDestroy() 
{
	setViewPortControl(0);
	delete m_hotSpotTester;
	m_hotSpotTester	= 0;
	delete m_delayedChart;
	m_delayedChart = 0;
	delete m_imageMapChart;
	m_imageMapChart = 0;

	CStatic::OnDestroy();
}

//
// MouseMove event handler
//
void CChartViewer::OnMouseMove(UINT	nFlags,	CPoint point) 
{
	// Enable mouse tracking to detect mouse leave events
	if (!m_isMouseTracking)
	{
		TRACKMOUSEEVENT e;		
		e.cbSize = sizeof(e);		
		e.dwFlags = TME_LEAVE;		
		e.hwndTrack = this->m_hWnd;
		m_isMouseTracking = (0 != TrackMouseEvent(&e));
	}

	// On Windows, mouse events can by-pass the event queue. If there are too many mouse events,
	// the event queue may not get processed, preventing other controls from updating. If two mouse
	// events are less than 10ms apart, there is a risk of too many mouse events. So we repost the
	// mouse event as a timer event that is queued up normally, allowing the queue to get processed.
	unsigned int timeBetweenMouseMove = GetTickCount() - m_lastMouseMove;
	if ((m_hasDelayedMouseMove && (timeBetweenMouseMove < 250)) || (timeBetweenMouseMove < 10))
	{
		m_delayedMouseMoveFlag = nFlags;
		m_delayedMouseMovePoint = point;
		if (!m_hasDelayedMouseMove)
		{
			m_hasDelayedMouseMove = true;
			SetTimer(DELAYED_MOUSE_MOVE_TIMER, 1, 0);
		}
	}
	else
		commitMouseMove(nFlags, point);
}

bool GetBitMapTop(HBITMAP hBmp, bool isColorCursor, int* topOffset, int* bottomOffset)
{
	// Get the BITMAP from the HBITMAP
	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);

	//BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmp.bmWidth;
	bi.biHeight = abs(bmp.bmHeight);
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bi.biWidth * bi.biBitCount + 31) / 32) * bi.biHeight;
	int* lpbitmap = (int*)malloc(dwBmpSize * sizeof(int));

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	HDC dc = CreateCompatibleDC(NULL);
	GetDIBits(dc, hBmp, 0, bi.biHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	if (isColorCursor)
	{
		*bottomOffset = bi.biHeight;
		for (int i = 0; i < (int)dwBmpSize; ++i)
		{
			int alpha = (lpbitmap[i] >> 24) & 0xff;
			if (alpha != 0)
			{
				*bottomOffset = bi.biHeight - i / bi.biWidth;
				break;
			}
		}

		*topOffset = 0;
		for (int i = (int)dwBmpSize - 1; i >= 0; --i)
		{
			int alpha = (lpbitmap[i] >> 24) & 0xff;
			if (alpha != 0)
			{
				*topOffset = bi.biHeight - i / bi.biWidth - 1;
				break;
			}
		}
	}
	else
	{
		// AND XOR mask
		// - The bitmap will be twice the height of the cursor. The top half is the AND mask
		//   and the bottom half is the XOR mask. 

		int maskSize = dwBmpSize / 2;
		*bottomOffset = bi.biHeight / 2;
		for (int i = 0; i < maskSize; ++i)
		{
			int ANDmask = lpbitmap[i] & 0xff;
			int XORmask = lpbitmap[i + maskSize] & 0xff;
			if ((ANDmask != 0x00) || (XORmask != 0xff))
			{
				*bottomOffset = bi.biHeight / 2 - i / bi.biWidth;
				break;
			}
		}

		*topOffset = 0;
		for (int i = maskSize - 1; i >= 0; --i)
		{
			int ANDmask = lpbitmap[i] & 0xff;
			int XORmask = lpbitmap[i + maskSize] & 0xff;
			if ((ANDmask != 0x00) || (XORmask != 0xff))
			{
				*topOffset = bi.biHeight / 2 - i / bi.biWidth - 1;
				break;
			}
		}
	}

	free(lpbitmap);
	DeleteDC(dc);
	return true;
}

//
// The method that actually performs MouseMove event processing
//
void CChartViewer::commitMouseMove(UINT nFlags, CPoint point)
{
	// Cancel the delayed mouse event if any
	if (m_hasDelayedMouseMove)
	{
		KillTimer(DELAYED_MOUSE_MOVE_TIMER);
		m_hasDelayedMouseMove = false;
	}

	// Remember the mouse coordinates for later use
	m_currentMouseX = point.x;
	m_currentMouseY = point.y;

	// The chart can be updated more than once during mouse move. For example, it can update due to
	// drag to scroll, and also due to drawing track cursor. So we delay updating the display until
	// all all events has occured.
	m_delayUpdateChart = NEED_DELAY;
	m_isInMouseMove = true;

	// Check if mouse is dragging on the plot area
	m_isOnPlotArea = m_isPlotAreaMouseDown || inPlotArea(toImageX(point.x), toImageY(point.y));
	if (m_isPlotAreaMouseDown)
		OnPlotAreaMouseDrag(nFlags,	point);
	
	// Send CVN_MouseMoveChart
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CVN_MouseMoveChart), 
		(LPARAM)m_hWnd);

	if (inExtendedPlotArea((int)toImageX(point.x), (int)toImageY(point.y)))
	{
		// Mouse wheel events are only sent to the control in focus. So if mouse wheel zooming is 
		// enabled, we must get the focus in order to receive mouse wheel events.
		if (m_mouseWheelZoomRatio != 1)
			SetFocus();

		// Mouse is in extended plot area, send CVN_MouseMovePlotArea
		m_isInMouseMovePlotArea = true;
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CVN_MouseMovePlotArea), 
			(LPARAM)m_hWnd);
	}
	else if (m_isInMouseMovePlotArea)
	{
		// Mouse was in extended plot area, but is not in it now, so send CVN_MouseLeavePlotArea
		m_isInMouseMovePlotArea = false;
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CVN_MouseLeavePlotArea), 
			(LPARAM)m_hWnd);
		applyAutoHide(CVN_MouseLeavePlotArea);
	}

	// Can update chart now
	commitUpdateChart();
	m_isInMouseMove = false;

	if (m_delayImageMapUpdate)
	{
		m_delayImageMapUpdate = false;
		if (!m_isPlotAreaMouseDown)
			updateViewPort(false, true);
	}

	//
	// Show	hot	spot tool tips if necessary
	//
	if (!m_toolTipHasAttached)
	{
		m_toolTipHasAttached = true;

		// Connects	the	CChartViewer to	the	CToolTipCtrl control
		m_ToolTip.Create(this);
		m_ToolTip.Activate(TRUE);
		m_ToolTip.ModifyStyle(0, TTS_NOPREFIX);

		m_ToolTip.AddTool(this);
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
	}

	// Use the ChartDirector ImageMapHandler to	determine if the mouse is over a hot spot
	int	hotSpotNo =	0;
	if (0 != m_hotSpotTester)
		hotSpotNo = m_hotSpotTester->getHotSpot(toImageX(point.x), toImageY(point.y), m_imageMapChart);

	const char* isDynamic = 0;
	if (hotSpotNo != 0)
		isDynamic = m_hotSpotTester->getValue("dynamic");

	// Hide	classical tool tip if mouse button is pressed.
	if (0 != nFlags)
		m_ToolTip.UpdateTipText(_T(""), this);
	
	// If the mouse	is in the same hot spot	since the last mouse move event, there is no need
	// to update the tool tip.
	if ((hotSpotNo != m_currentHotSpot) || ((0 != isDynamic) && (0 != *isDynamic)))
	{
		// Hot spot	has	changed	- update tool tip text
		m_currentHotSpot = hotSpotNo;

		if (hotSpotNo == 0)
		{
			// Mouse is	not	actually on	handler	hot	spot - reset the clickable flag and use 
			// default tool tip text.
			m_isClickable =	false;
			showToolTip(TCHARtoUTF8(m_defaultToolTip));
		}
		else
		{
			// Mouse is	on a hot spot. In this implementation, we consider the hot spot	as 
			// clickable if	its	href ("path") parameter	is not empty.
			const char *path = m_hotSpotTester->getValue("path");
			m_isClickable =	((0	!= path) &&	(0 != *path) && ('~' != *path));
			showToolTip(m_hotSpotTester->getValue("title"));
		}
	}
	
	if (m_enhancedToolTip.getToolTip())
		moveEnhancedToolTip(m_enhancedToolTip.getBounds());

	m_lastMouseMove = GetTickCount();
	CStatic::OnMouseMove(nFlags, point);
}

void CChartViewer::showToolTip(const char* text)
{
	if ((!text) || (!*text) || ((text[0] == '<') && (text[1] == '*')))
	{
		showEnhancedToolTip(text);
		m_ToolTip.UpdateTipText(_T(""), this);
	}
	else
	{
		showEnhancedToolTip(0);
		
		if ((GetKeyState(VK_LBUTTON) < 0) || (GetKeyState(VK_RBUTTON) < 0) || (GetKeyState(VK_MBUTTON) < 0))
			m_ToolTip.UpdateTipText(_T(""), this);
		else
			m_ToolTip.UpdateTipText(UTF8toTCHAR(text), this);
	}
}

void CChartViewer::moveEnhancedToolTip(const CRect& currentPos)
{
	HCURSOR cursor = getHCursor();
	if (NULL == cursor)
		cursor = ::GetCursor();

	int topPos = 0;
	int bottomPos = 0;

	ICONINFO info;
	if (GetIconInfo(cursor, &info) != 0)
	{
		if (info.hbmColor || info.hbmMask)
		{
			bool isColorCursor = 0 != info.hbmColor;
			GetBitMapTop(isColorCursor ? info.hbmColor : info.hbmMask, isColorCursor, &topPos, &bottomPos);

			if (info.hbmColor)
				DeleteObject(info.hbmColor);
			if (info.hbmMask)
				DeleteObject(info.hbmMask);
		}
	}

	POINT p;
	if (!GetCursorPos(&p))
		return;
	ScreenToClient(&p);

	CRect b;
	GetClientRect(&b);

	CRect r = m_enhancedToolTip.getBounds();

	if (p.x + r.Width() > b.right)
	{
		p.x = b.right - r.Width();
		if (p.x < 0)
			p.x = 0;
	}
	if (p.y + bottomPos - (int)info.yHotspot + r.Height() > b.bottom)
	{
		p.y = p.y - r.Height() + topPos - info.yHotspot - 2;
		if (p.y < 0)
			p.y = 0;
	}
	else
	{
		p.y += bottomPos - info.yHotspot + 2;
		if (p.y > b.bottom - b.top - r.Height())
			p.y = b.bottom - b.top - r.Height();
	}
	
	m_enhancedToolTip.setPos(p.x, p.y);
	
	r = m_enhancedToolTip.getBounds();
	if (currentPos.Width() > 0)
		r.UnionRect(r, currentPos);
	if ((r.left -= 5) < 0)
		r.left = 0;
	if ((r.top -= 5) < 0)
		r.top = 0;
	if ((r.right += 5) > b.right)
		r.right = b.right;
	if ((r.bottom += 5) > b.bottom)
		r.bottom = b.bottom;

	redraw(r);
}

void CChartViewer::showEnhancedToolTip(const char *text)
{
	if ((0 == text) || (0 == *text))
	{
		CRect r = m_enhancedToolTip.getBounds();
		m_enhancedToolTip.setToolTip(0);	
		redraw(r);
		return;
	}
	
	CString buffer = UTF8toTCHAR(text);
	buffer.Replace(_T("<*cdml*>"), m_CDMLToolTipPrefix);
	
	CRect moveFrom = m_enhancedToolTip.getBounds();
	
	PieChart c(1, 1, Chart::Transparent);
	c.makeChart()->renderCDML(TCHARtoUTF8(buffer));
	c.setTransparentColor(-2);
	c.setOutputOptions("+alpha=3");
	m_enhancedToolTip.setToolTip(bmpToHBITMAP(renderChart(&c).data));
	c.setOutputOptions("-alpha");

	moveEnhancedToolTip(moveFrom);
}

//
// Delayed MouseMove event handler
//
void CChartViewer::OnDelayedMouseMove() 
{
	if (m_hasDelayedMouseMove)
		commitMouseMove(m_delayedMouseMoveFlag, m_delayedMouseMovePoint);
}

//
// MouseWheel handler
//
BOOL CChartViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// Process the mouse wheel only if the mouse is over the plot area
	if (!isMouseOnPlotArea())
		return FALSE;
	else
		return onMouseWheelZoom(getPlotAreaMouseX(), getPlotAreaMouseY(), zDelta);
}

BOOL CChartViewer::onMouseWheelZoom(int x, int y, short zDelta)
{
	// Zoom ratio = 1 means no zooming
	if (m_mouseWheelZoomRatio == 1)
		return FALSE;

	// X and Y zoom ratios
	double rx = 1;
    double ry = 1;
	if (getZoomDirection() != Chart::DirectionVertical)
		rx = (zDelta > 0) ? m_mouseWheelZoomRatio : 1 / m_mouseWheelZoomRatio;
	if (getZoomDirection() != Chart::DirectionHorizontal)
		ry = (zDelta > 0) ? m_mouseWheelZoomRatio : 1 / m_mouseWheelZoomRatio;

	// Do the zooming
	if (zoomAround(x, y, rx, ry))
	{
		updateViewPort(true, false);
		m_delayImageMapUpdate = true;
	}

	return TRUE;
}

//
// MouseLeave event handler
//
void CChartViewer::OnMouseLeave() 
{
	// Process delayed mouse move, if any
	OnDelayedMouseMove();

	// Mouse tracking is no longer active
	m_isMouseTracking = false;

	if (m_isInMouseMovePlotArea)
	{
		// Mouse was in extended plot area, but is not in it now, so send CVN_MouseLeavePlotArea
		m_isInMouseMovePlotArea = false;
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CVN_MouseLeavePlotArea), 
			(LPARAM)m_hWnd);
		applyAutoHide(CVN_MouseLeavePlotArea);
	}

	// Send CVN_MouseLeaveChart
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CVN_MouseLeaveChart), 
		(LPARAM)m_hWnd);
	applyAutoHide(CVN_MouseLeaveChart);

	// Hide tooltip
	showEnhancedToolTip(0);
}

//
// Intercept WM_SETCURSOR to change	the	mouse cursor.
//
BOOL CChartViewer::OnSetCursor(CWnd* pWnd, UINT	nHitTest, UINT message)	
{
	HCURSOR h = getHCursor();
	if (h != NULL)
	{
		::SetCursor(h);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest,	message);
}

HCURSOR CChartViewer::getHCursor()
{
	if (m_isOnPlotArea)
	{
		switch (m_mouseUsage)
		{
		case Chart::MouseUsageZoomIn:
			if (canZoomIn(m_zoomDirection))
				return cursorManager.getZoomInCursor();
			else
				return cursorManager.getNoZoomCursor();
		case Chart::MouseUsageZoomOut:
			if (canZoomOut(m_zoomDirection))
				return cursorManager.getZoomOutCursor();
			else
				return cursorManager.getNoZoomCursor();
		}
	}

	if (m_isClickable)
		// Hand	cursor = IDC_HAND =	32649
		return AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649));

	return NULL;
}

//
// Mouse left button down event.
//
void CChartViewer::OnLButtonDown(UINT nFlags, CPoint point)	
{
	OnDelayedMouseMove();

	if (inPlotArea(toImageX(point.x), toImageY(point.y)) &&	(m_mouseUsage != Chart::MouseUsageDefault))
	{
		// Mouse usage is for drag to zoom/scroll. Capture the mouse to	prepare	for	dragging and 
		// save	the	mouse down position	to draw	the	selection rectangle.
		SetCapture();
		m_isPlotAreaMouseDown =	true;
		m_plotAreaMouseDownXPos	= point.x;
		m_plotAreaMouseDownYPos	= point.y;
		startDrag();
	}
	else
		CStatic::OnLButtonDown(nFlags, point);
}

void CChartViewer::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
	CStatic::OnLButtonDblClk(nFlags, point);
}

//
// Mouse left button up	event.
//
void CChartViewer::OnLButtonUp(UINT	nFlags,	CPoint point) 
{
	OnDelayedMouseMove();

	if (m_isPlotAreaMouseDown)
	{
		// Release the mouse capture.
		ReleaseCapture();
		m_isPlotAreaMouseDown =	false;
		setRectVisible(false);
		bool hasUpdate = false;

		switch (m_mouseUsage)
		{
		case Chart::MouseUsageZoomIn :
			if (canZoomIn(m_zoomDirection))
			{
				if (isDrag(m_zoomDirection, point))
				{
					int minX, minY, spanX, spanY;
					getDragZoomRect(point.x, point.y, &minX, &minY, &spanX, &spanY);

					// Zoom	to the drag	selection rectangle.
					hasUpdate = zoomTo(m_zoomDirection,
						toImageX(minX), toImageY(minY), toImageX(minX + spanX), toImageY(minY + spanY));
				}
				else
					// User	just click on a	point. Zoom-in around the mouse	cursor position.
					hasUpdate =	zoomAt(m_zoomDirection,	toImageX(point.x), toImageY(point.y), m_zoomInRatio);
			}
			break;
		case Chart::MouseUsageZoomOut:
			// Zoom	out	around the mouse cursor	position.
			if (canZoomOut(m_zoomDirection))
				hasUpdate =	zoomAt(m_zoomDirection,	toImageX(point.x), toImageY(point.y), m_zoomOutRatio);
			break;
		default	:
			if (m_isDragScrolling)
				// Drag	to scroll. We can update the image map now as scrolling	has	finished.
				updateViewPort(false, true);
			else
				// Is not zooming or scrolling,	so is just a normal	click event.
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),	BN_CLICKED), (LPARAM)m_hWnd);
			break;
		}

		m_isDragScrolling =	false;
		if (hasUpdate)
			// View	port has changed - update it.
			updateViewPort(true, true);
	}
	else
		CStatic::OnLButtonUp(nFlags, point);
}

//
// Chart hold timer.
//
void CChartViewer::OnTimer(UINT_PTR nIDEvent)	
{
	if (nIDEvent == DELAYED_MOUSE_MOVE_TIMER)
	{
		// Is a delayed mouse move event
		OnDelayedMouseMove();
	}
	else
	{	
		// Is a delayed view port update
		m_holdTimerActive =	false;

		// Reset the timer
		KillTimer(nIDEvent);
		
		// If has pending chart	view port update request, handles them now.
		if (m_needUpdateChart || m_needUpdateImageMap)
			updateViewPort(m_needUpdateChart, m_needUpdateImageMap);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChartViewer	overrides

BOOL CChartViewer::PreTranslateMessage(MSG*	pMsg) 
{
	// Remember	to forward mouse messages to the CToolTipCtrl
	BOOL res = CStatic::PreTranslateMessage(pMsg);
	if (this->m_toolTipHasAttached)
		m_ToolTip.RelayEvent(pMsg);
	return res;
}

/////////////////////////////////////////////////////////////////////////////
// CChartViewer	properties

//
// Set the chart to	the	control
//
void CChartViewer::setChart(BaseChart *c)
{
	// In case the user	forgets	to check the "Notify" check	box	in the Dialog editor, we set it
	// ourselves so	the	CChartViewer control can receive mouse events.
	if ((GetStyle()	& SS_NOTIFY) ==	0)
		ModifyStyle(0, SS_NOTIFY);
	if ((GetStyle() & SS_TYPEMASK) != SS_BITMAP)
		ModifyStyle(SS_TYPEMASK, SS_BITMAP);

	if (m_currentChart != c)
		setImageMap(0);

	m_currentChart = c;
	delete m_imageMapChart;
	m_imageMapChart = c ? new BaseChart(c) : 0;

	if (0 != c)
	{
		commitPendingSyncAxis(c);
		if (m_delayUpdateChart != NO_DELAY)
			c->makeChart();
	}

	updateDisplay();
}

//
// Get back	the	same BaseChart pointer provided	by the previous	setChart call.
//
BaseChart *CChartViewer::getChart()
{
	return m_currentChart;
}

//
// Set the CViewPortControl to be associated with this control
//
void CChartViewer::setViewPortControl(CViewPortControl *vpc)
{
	if (m_ReentrantGuard)
		return;
	m_ReentrantGuard = true;
	
	if (0 != m_vpControl)
		m_vpControl->setViewer(0);
	m_vpControl = vpc;
	if (0 != m_vpControl)
		m_vpControl->setViewer(this);

	m_ReentrantGuard = false;
}


//
// Get the CViewPortControl that is associated with this control
//
CViewPortControl *CChartViewer::getViewPortControl()
{
	return m_vpControl;
}

//
// Set image map used by the chart
//
void CChartViewer::setImageMap(const char *imageMap)
{
	//delete the existing ImageMapHandler
	if (0 != m_hotSpotTester)
		delete m_hotSpotTester;
	m_currentHotSpot = -1;
	m_isClickable =	false;
	
	//create a new ImageMapHandler to represent	the	image map
	if ((0 == imageMap)	|| (0 == *imageMap))
		m_hotSpotTester	= 0;
	else
		m_hotSpotTester	= new ImageMapHandler(imageMap, this);
}

//
// Get the image map handler for the chart
//
ImageMapHandler	*CChartViewer::getImageMapHandler()
{
	return m_hotSpotTester;
}

//
// Set the default tool	tip	to use
//
void CChartViewer::setDefaultToolTip(LPCTSTR text)
{
	m_defaultToolTip = text;
}

//
// Set the CDML tool tip prefix
//
void CChartViewer::setCDMLToolTipPrefix(LPCTSTR prefix)
{
	m_CDMLToolTipPrefix = prefix;
}

//
// Get the CToolTipCtrl	for	managing tool tips.
//
CToolTipCtrl *CChartViewer::getToolTipCtrl()
{
	return &m_ToolTip;
}

//
// Set the border width	of the selection box
//
void CChartViewer::setSelectionBorderWidth(int width)
{
	m_selectBoxLineWidth = width;
}

//
// Get the border with of the selection	box.
//
int	CChartViewer::getSelectionBorderWidth()
{
	return m_selectBoxLineWidth;
}

//
// Set the border color	of the selection box
//
void CChartViewer::setSelectionBorderColor(COLORREF	c)
{
	m_selectBoxLineColor = c;
	if (m_TopLine.m_hWnd !=	0)
	{
		m_TopLine.SetColor(c);
		m_LeftLine.SetColor(c);
		m_BottomLine.SetColor(c);
		m_RightLine.SetColor(c);
	}
}

//
// Get the border color	of the selection box.
//
COLORREF CChartViewer::getSelectionBorderColor()
{
	return m_selectBoxLineColor;
}

//
// Set the mouse usage mode
//
void CChartViewer::setMouseUsage(int mouseUsage)
{
	m_mouseUsage = mouseUsage;
}

//
// Get the mouse usage mode
//
int	CChartViewer::getMouseUsage()
{
	return m_mouseUsage;
}

//
// Set the zoom	direction
//
void CChartViewer::setZoomDirection(int	direction)
{
	m_zoomDirection	= direction;
	setKeepAspectRatio(m_zoomDirection == Chart::KeepAspectRatio);
}

//
// Get the zoom	direction
//
int	CChartViewer::getZoomDirection()
{
	return m_zoomDirection;
}

//
// Set the scroll direction
//
void CChartViewer::setScrollDirection(int direction)
{
	m_scrollDirection =	direction;
}

//
// Get the scroll direction
//
int	CChartViewer::getScrollDirection()
{
	return m_scrollDirection;
}

//
// Set the zoom-in ratio for mouse click zoom-in
//
void CChartViewer::setZoomInRatio(double ratio)
{
	m_zoomInRatio =	ratio;
}

//
// Get the zoom-in ratio for mouse click zoom-in
//
double CChartViewer::getZoomInRatio()
{
	return m_zoomInRatio;
}

//
// Set the zoom-out	ratio
//
void CChartViewer::setZoomOutRatio(double ratio)
{
	m_zoomOutRatio = ratio;
}

//
// Get the zoom-out	ratio
//
double CChartViewer::getZoomOutRatio()
{
	return m_zoomOutRatio;	
}

//
// Set the mouse wheel zoom ratio 
//
void CChartViewer::setMouseWheelZoomRatio(double ratio)
{
	m_mouseWheelZoomRatio = ratio;
}

//
// Get the mouse wheel zoom ratio 
//
double CChartViewer::getMouseWheelZoomRatio()
{
	return m_mouseWheelZoomRatio;	
}

//
// Set the minimum mouse drag before the dragging is considered	as real. This is to	avoid small	
// mouse vibrations	triggering a mouse drag.
//
void CChartViewer::setMinimumDrag(int offset)
{
	m_minDragAmount	= offset;
}

//
// Get the minimum mouse drag before the dragging is considered	as real.
//
int	CChartViewer::getMinimumDrag()
{
	return m_minDragAmount;
}

//
// Set the minimum interval	between	ViewPortChanged	events.	This is	to avoid the chart being 
// updated too frequently. (Default	is 20ms	between	chart updates.)	Multiple update	events
// arrived during the interval will	be merged into one chart update	and	executed at	the	end
// of the interval.
//
void CChartViewer::setUpdateInterval(int interval)
{
	m_updateInterval = interval;
}

//
// Get the minimum interval	between	ViewPortChanged	events.	
//
int	CChartViewer::getUpdateInterval()
{
	return m_updateInterval;
}

//
// Check if	there is a pending chart update	request. 
//
bool CChartViewer::needUpdateChart()
{
	return m_needUpdateChart;
}

//
// Check if	there is a pending image map update	request. 
//
bool CChartViewer::needUpdateImageMap()
{
	return m_needUpdateImageMap;
}

//
// Get the current mouse x coordinate when used in a mouse move event handler
//
int CChartViewer::getChartMouseX()
{
	if (m_currentMouseX < -0x10000000)
		return getPlotAreaLeft() + getPlotAreaWidth();
	else
		return (int)(toImageX(m_currentMouseX) + 0.5);
}

//
// Get the current mouse y coordinate when used in a mouse move event handler
//
int CChartViewer::getChartMouseY()
{
	if (m_currentMouseY < -0x10000000)
		return getPlotAreaTop() + getPlotAreaHeight();
	else
		return (int)(toImageY(m_currentMouseY) + 0.5);
}

//
// Get the current mouse x coordinate bounded to the plot area when used in a mouse event handler
//
int CChartViewer::getPlotAreaMouseX()
{
	int ret = getChartMouseX();
	if (ret < getPlotAreaLeft())
		ret = getPlotAreaLeft();
	if (ret > getPlotAreaLeft() + getPlotAreaWidth())
		ret = getPlotAreaLeft() + getPlotAreaWidth();
	return ret;
}

//
// Get the current mouse y coordinate bounded to the plot area when used in a mouse event handler
//
int CChartViewer::getPlotAreaMouseY()
{
	int ret = getChartMouseY();
	if (ret < getPlotAreaTop())
		ret = getPlotAreaTop();
	if (ret > getPlotAreaTop() + getPlotAreaHeight())
		ret = getPlotAreaTop() + getPlotAreaHeight();
	return ret;
}

//
// Check if mouse is on the extended plot area
//
bool CChartViewer::isMouseOnPlotArea()
{
	if (this->m_isMouseTracking)
		return inExtendedPlotArea(getChartMouseX(), getChartMouseY());
	else
		return false;
}

//
// Check if mouse is dragging to scroll or to select the zoom rectangle
//
bool CChartViewer::isMouseDragging()
{
	return m_isPlotAreaMouseDown;
}

//
// Check if is currently processing a mouse move event
//
bool CChartViewer::isInMouseMoveEvent()
{
	return m_isInMouseMove;
}

/////////////////////////////////////////////////////////////////////////////
// CChartViewer	methods

//
// Update the display
//
void CChartViewer::updateDisplay()
{
	if (m_delayUpdateChart == NO_DELAY)
		commitUpdateChart();
	else
	{
		m_delayUpdateChart = NEED_UPDATE;
		delete m_delayedChart;
		m_delayedChart = (0 != m_currentChart) ? new BaseChart(m_currentChart) : 0;
	}
}

//
// Commit chart to display
//
void CChartViewer::commitUpdateChart()
{
	if (m_delayUpdateChart == NEED_DELAY)
	{
		// No actual update occur
		m_delayUpdateChart = NO_DELAY;
		return;
	}

	// Display the chart
	BaseChart *c = (m_delayUpdateChart == NEED_UPDATE) ? m_delayedChart : m_currentChart;
	displayChart(c);
	setChartMetrics((0 != c) ? c->getChartMetrics() : 0);

	// Remember to update the selection rectangle
	if ((0 != m_TopLine.m_hWnd) && m_TopLine.IsWindowVisible())
	{
		m_TopLine.Invalidate();
		m_LeftLine.Invalidate();
		m_RightLine.Invalidate();
		m_BottomLine.Invalidate();
		m_TopLine.UpdateWindow();
		m_LeftLine.UpdateWindow();
		m_RightLine.UpdateWindow();
		m_BottomLine.UpdateWindow();
	}

	// Any delayed chart has been committed
	m_delayUpdateChart = NO_DELAY;
	delete m_delayedChart;
	m_delayedChart = 0;
}

void CChartViewer::redraw(const CRect& r)
{
	InvalidateRect(&r, FALSE);

	// Remember to also redraw the selection rectangle
	if ((0 != m_TopLine.m_hWnd) && m_TopLine.IsWindowVisible())
	{
		m_TopLine.Invalidate();
		m_LeftLine.Invalidate();
		m_RightLine.Invalidate();
		m_BottomLine.Invalidate();
	}
}

HBITMAP CChartViewer::getDynamicTip(CPoint* dc)
{
	HBITMAP toolTip = m_enhancedToolTip.getToolTip();
	if (0 != toolTip)
		*dc = m_enhancedToolTip.getBounds().TopLeft();
	return toolTip;
}

//
// Set the message used to remove the dynamic layer
//
void CChartViewer::removeDynamicLayer(int msg)
{
	m_autoHideMsg = msg;
	if (msg == -1)
		applyAutoHide(msg);
}

//
// Attempt to hide the dynamic layer using the specified message
//
void CChartViewer::applyAutoHide(int msg)
{
	if (m_autoHideMsg == msg)
	{
		if (0 != m_currentChart)
			m_currentChart->removeDynamicLayer();
		m_autoHideMsg = 0;

	    updateDisplay();
	}
}

//
// Create the edges	for	the	selection rectangle
//
void CChartViewer::initRect()
{
	m_TopLine.Create(GetParent(), m_selectBoxLineColor);
	m_LeftLine.Create(GetParent(), m_selectBoxLineColor);
	m_BottomLine.Create(GetParent(), m_selectBoxLineColor);
	m_RightLine.Create(GetParent(), m_selectBoxLineColor);
}

//
// Set selection rectangle position	and	size
//
void CChartViewer::drawRect(int	x, int y, int width, int height)
{
	// Create the edges	of the rectangle if	not	already	created
	if (m_TopLine.m_hWnd ==	0)
		initRect();

	// width < 0 is	interpreted	as the rectangle extends to	the	left or	x.
	// height <0 is	interpreted	as the rectangle extends to	above y.
	if (width <	0)
		x -= (width	= -width);
	if (height < 0)
		y -= (height = -height);

	// Compute the position	of the selection rectangle as relative to the parent window
	RECT rect;
	rect.left =	x;
	rect.top = y;
	rect.right = x + width;
	rect.bottom	= y	+ height;
	MapWindowPoints(m_TopLine.GetParent(), &rect);

	// Put the edges along the sides of	the	rectangle
	m_TopLine.MoveWindow(rect.left,	rect.top, rect.right - rect.left, m_selectBoxLineWidth);
	m_LeftLine.MoveWindow(rect.left, rect.top, m_selectBoxLineWidth, rect.bottom - rect.top);
	m_BottomLine.MoveWindow(rect.left, rect.bottom - m_selectBoxLineWidth +	1, 
		rect.right - rect.left,	m_selectBoxLineWidth);
	m_RightLine.MoveWindow(rect.right -	m_selectBoxLineWidth + 1, rect.top,	
		m_selectBoxLineWidth, rect.bottom -	rect.top);
}

//
// Show/hide selection rectangle
//
void CChartViewer::setRectVisible(bool b)
{
	// Create the edges	of the rectangle if	not	already	created
	if (b && (m_TopLine.m_hWnd == 0)) 
		initRect();

	// Show/hide the edges
	if (m_TopLine.m_hWnd != 0)
	{
		int	state =	b ?	SW_SHOW	: SW_HIDE;
		m_TopLine.ShowWindow(state);
		m_LeftLine.ShowWindow(state);
		m_BottomLine.ShowWindow(state);
		m_RightLine.ShowWindow(state);
	}
}

//
// Determines if the mouse is dragging.
//
bool CChartViewer::isDrag(int direction, CPoint	point)
{
	// We only consider	the	mouse is dragging it is	has	dragged	more than m_minDragAmount. This	is
	// to avoid	small mouse	vibrations triggering a	mouse drag.
	int	spanX =	abs(point.x	- m_plotAreaMouseDownXPos);
	int	spanY =	abs(point.y	- m_plotAreaMouseDownYPos);
	return ((direction != Chart::DirectionVertical)	&& (spanX >= m_minDragAmount)) ||
		((direction	!= Chart::DirectionHorizontal) && (spanY >=	m_minDragAmount));
}

void CChartViewer::getDragZoomRect(int mx, int my, int* x, int* y, int* w, int* h)
{
	*x = (mx < m_plotAreaMouseDownXPos) ? mx : m_plotAreaMouseDownXPos;
	*y = (my < m_plotAreaMouseDownYPos) ? my : m_plotAreaMouseDownYPos;
	*w = abs(mx - m_plotAreaMouseDownXPos);
	*h = abs(my - m_plotAreaMouseDownYPos);

	if (getZoomXYRatio() > 0)
	{
		double imageXYRatio = getPlotAreaWidth() / (double)getPlotAreaHeight();

		double whDelta = toImageX(*w) - toImageY(*h) * imageXYRatio;
		if (whDelta < 0)
			*w = toDisplayX(toImageY(*h) * imageXYRatio);
		else if (whDelta > 0)
			*h = toDisplayY(toImageX(*w) / imageXYRatio);

		if (*x == mx)
			*x = m_plotAreaMouseDownXPos - *w;
		if (*y == my)
			*y = m_plotAreaMouseDownYPos - *h;
	}
}

//
// Process mouse dragging over the plot	area
//
void CChartViewer::OnPlotAreaMouseDrag(UINT	/* nFlags */, CPoint point)
{
	switch (m_mouseUsage)
	{
		case Chart::MouseUsageZoomIn :
		{
			//
			// Mouse is	used for zoom in. Draw the selection rectangle if necessary.
			//

			bool isDragZoom	= canZoomIn(m_zoomDirection) &&	isDrag(m_zoomDirection,	point);
			if (isDragZoom)
			{
				int minX, minY, spanX, spanY;
				getDragZoomRect(point.x, point.y, &minX, &minY, &spanX, &spanY);

				switch (m_zoomDirection)
				{
				case Chart::DirectionHorizontal:
					drawRect(minX, toDisplayY(getPlotAreaTop()), spanX, toDisplayY(getPlotAreaHeight()));
					break;
				case Chart::DirectionVertical:
					drawRect(toDisplayX(getPlotAreaLeft()), minY, toDisplayX(getPlotAreaWidth()), spanY);
					break;
				default:
					drawRect(minX, minY, spanX, spanY);
					break;
				}
			}
			setRectVisible(isDragZoom);
			break;
		}
		case Chart::MouseUsageScroll :
		{
			//
			// Mouse is	used for drag scrolling. Scroll	and	update the view	port.
			//

			if (m_isDragScrolling || isDrag(m_scrollDirection, point))
			{
				m_isDragScrolling =	true;
				switch (m_scrollDirection)
				{
				case Chart::DirectionHorizontal:
					::SetCursor(cursorManager.getNoMoveHorizCursor());
					break;
				case Chart::DirectionVertical:
					::SetCursor(cursorManager.getNoMoveVertCursor());
					break;
				default	:
					::SetCursor(cursorManager.getNoMove2DCursor());
					break;
				}
								
				if (dragTo(m_scrollDirection, 
					toImageX(point.x - m_plotAreaMouseDownXPos), toImageY(point.y - m_plotAreaMouseDownYPos)))
					updateViewPort(true, false);
			}
		}
	}
}

//
// Trigger the ViewPortChanged event
//
void CChartViewer::updateViewPort(bool needUpdateChart,	bool needUpdateImageMap)
{
	// Merge the current update	requests with any pending requests.
	m_needUpdateChart =	m_needUpdateChart || needUpdateChart;
	m_needUpdateImageMap = needUpdateImageMap;

	// Hold	timer has not expired, so do not update	chart immediately. Keep	the	requests pending.
	if (m_holdTimerActive)
		return;

	// The chart can be updated more than once during mouse move. For example, it can update due to
	// drag to scroll, and also due to drawing track cursor. So we delay updating the display until
	// all all updates has occured.
	int hasDelayUpdate = (m_delayUpdateChart != NO_DELAY);
	if (!hasDelayUpdate)
		m_delayUpdateChart = NEED_DELAY;
	
	// Can trigger the ViewPortChanged event.
	validateViewPort();
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),	CVN_ViewPortChanged), 
		(LPARAM)m_hWnd);
	
	if (m_needUpdateChart && (0 != m_vpControl))
		m_vpControl->updateDisplay();

	// Can update chart now
	if (!hasDelayUpdate)
		commitUpdateChart();

	// Clear any pending updates.
	m_needUpdateChart =	false;
	m_needUpdateImageMap = false;

	// Set hold	timer to prevent multiple chart	updates	within a short period.
	if (m_updateInterval > 0)
	{
		m_holdTimerActive = true;
		SetTimer(UPDATE_VIEW_PORT_TIMER, m_updateInterval, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRectCtrl
//
// A rectangle with	a background color.	Use	as thick edges for the selection
// rectangle.
//

//
// Create control with a given color
//
BOOL CRectCtrl::Create(CWnd	*pParentWnd, COLORREF c)
{
	SetColor(c);

	RECT r;
	r.left = r.top = r.right = r.bottom	= 0;
	return CStatic::Create(0, WS_CHILD,	r, pParentWnd);
}

BEGIN_MESSAGE_MAP(CRectCtrl, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRectCtrl message handlers

HBRUSH CRectCtrl::CtlColor(CDC*	/* pDC */, UINT	/* nCtlColor */) 
{
	return (HBRUSH)m_Color.m_hObject;
}

/////////////////////////////////////////////////////////////////////////////
// CRectCtrl proporties

//
// Set the background color
//
void CRectCtrl::SetColor(COLORREF c)
{
	m_Color.CreateSolidBrush(c);
}


/////////////////////////////////////////////////////////////////////////////
//
// CStaticHelper
//
/////////////////////////////////////////////////////////////////////////////

CStaticHelper::CStaticHelper() : m_currentHBITMAP(0), m_WCCisV6(0), m_testMode(false),
	m_dpi(0), m_detectedDpi(0), m_bgColor(-1), m_toImageScaleX(1), m_toImageScaleY(1)
{
}

BEGIN_MESSAGE_MAP(CStaticHelper, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CStaticHelper::setDPI(int dpi)
{
	m_dpi = dpi;
}

int CStaticHelper::getDPI()
{
	int dpi = m_dpi;
	if (dpi < 0)
		return 96;
	if (dpi == 0)
	{
		if (0 == m_detectedDpi)
			m_detectedDpi = dpiHelper.getDPI(m_hWnd);
		dpi = m_detectedDpi;
	}
	return (dpi < 24) ? 24 : ((dpi > 384) ? 384 : dpi);
}

void CStaticHelper::setBgColor(int c)
{
	m_bgColor = c;
}

int CStaticHelper::getParentBgColor()
{
	CWnd* p = GetParent();
	HBRUSH hBrush = (HBRUSH)p->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)GetDC(), (LPARAM)m_hWnd);
	LOGBRUSH LogBrush;
	::GetObject(hBrush, sizeof(LOGBRUSH), &LogBrush);
	int ret = LogBrush.lbColor;
	return ((ret & 0xff) << 16) | (ret & 0xff00) | ((ret & 0xff0000) >> 16);
}

void CStaticHelper::setHBITMAP(HBITMAP newBitMap)
{
	HBITMAP oldHBITMAP = SetBitmap(newBitMap);
	if ((0 != oldHBITMAP) && (oldHBITMAP != m_currentHBITMAP))
	{
		m_WCCisV6 = 1;
		DeleteObject(oldHBITMAP);
	}

	if (0 != m_currentHBITMAP)
        DeleteObject(m_currentHBITMAP);
	m_currentHBITMAP = newBitMap;
}

HBITMAP CStaticHelper::bmpToHBITMAP(const char *data)
{
	if (0 == data)
		return 0;
    const BITMAPINFO *bmp = (const BITMAPINFO *)(data + 14);
    CDC	*cdc = GetDC();
	void *buffer;
	HBITMAP ret = CreateDIBSection(cdc->m_hDC, bmp, DIB_RGB_COLORS, &buffer, NULL, 0x0);
	memcpy(buffer, data + *(int *)(data + 10), bmp->bmiHeader.biSizeImage);
	ReleaseDC(cdc);
    return ret;
}

void CStaticHelper::paintBMP(CDC* dc, HBITMAP hBmp, int x, int y)
{
	if (0 == hBmp)
		return;

	CBitmap cbmp;
	if (!cbmp.Attach(hBmp))
		return;

	BITMAP bmp;
	cbmp.GetBitmap(&bmp);

	CDC dcMem;
	if (!dcMem.CreateCompatibleDC(dc))
		return;
	CBitmap* oldCbmp = dcMem.SelectObject(&cbmp);
	
	if (bmp.bmBitsPixel == 32)
	{
		BLENDFUNCTION bfn = { 0 };
		bfn.BlendOp = AC_SRC_OVER;
		bfn.BlendFlags = 0;
		bfn.SourceConstantAlpha = 255;
		bfn.AlphaFormat = AC_SRC_ALPHA;
		AlphaBlend(dc->GetSafeHdc(), x, y, bmp.bmWidth, bmp.bmHeight,
			dcMem.GetSafeHdc(), 0, 0, bmp.bmWidth, bmp.bmHeight, bfn);
	}
	else
		dc->BitBlt(x, y, bmp.bmWidth, bmp.bmHeight, &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(oldCbmp);
	cbmp.Detach();
}

void CStaticHelper::OnPaint()
{
	CPaintDC pDC(this);
	if (m_testMode)
		return;

	CRect clientR;
	GetClientRect(clientR);
	pDC.IntersectClipRect(clientR);

	CPoint dynLayerXY;
	HBITMAP dynLayer = getDynamicTip(&dynLayerXY);

	BITMAP bmp;
	GetObject(GetBitmap(), sizeof(BITMAP), &bmp);

	bool needBackground = (bmp.bmBitsPixel == 32) && (m_bgColor != Chart::Transparent);
	bool doubleBuffer = dynLayer || needBackground;

	CDC* dc = &pDC;
	CDC bufferDC;
	CBitmap bufferBMP;
	CBitmap* oldBMP = 0;
	if (doubleBuffer)
	{
		if (!bufferBMP.CreateCompatibleBitmap(dc, clientR.Width(), clientR.Height()))
			return;
		if (!bufferDC.CreateCompatibleDC(dc))
			return;
		bufferDC.IntersectClipRect(&pDC.m_ps.rcPaint);
		oldBMP = bufferDC.SelectObject(&bufferBMP);
		dc = &bufferDC;
	}
	
	if (needBackground)
	{
		if (m_bgColor == -1)
		{
			POINT ptOffset;
			ptOffset.x = 0;
			ptOffset.y = 0;
			MapWindowPoints(GetParent(), &ptOffset, 1);

			HDC hdc = dc->GetSafeHdc();
			POINT ptOrig;
			OffsetWindowOrgEx(hdc, ptOffset.x, ptOffset.y, &ptOrig);
			GetParent()->SendMessage(WM_ERASEBKGND, (WPARAM)hdc, 0);
			SetWindowOrgEx(hdc, ptOrig.x, ptOrig.y, 0);
		}
		else
		{
			CBrush backBrush(RGB((m_bgColor >> 16) & 0xff, (m_bgColor >> 8) & 0xff, m_bgColor & 0xff));
			CBrush* pOldBrush = dc->SelectObject(&backBrush);
			CRect rect;
			dc->GetClipBox(&rect);
			dc->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
			dc->SelectObject(pOldBrush);
		}
	}

	paintBMP(dc, GetBitmap(), 0, 0);
	if (dynLayer)
		paintBMP(dc, dynLayer, dynLayerXY.x, dynLayerXY.y);

	if (doubleBuffer)
	{
		pDC.BitBlt(0, 0, clientR.Width(), clientR.Height(), dc, 0, 0, SRCCOPY);
		bufferDC.SelectObject(oldBMP);
	}
}

HBITMAP CStaticHelper::getDynamicTip(CPoint *pos)
{
	return 0;
}

MemBlock CStaticHelper::renderChart(BaseChart* c)
{
	if (0 == c)
		return MemBlock();

	int dpi = getDPI();
	if (96 != dpi)
	{
		char buffer[1024];
		sprintf(buffer, "+dpi=%d", dpi);
		c->setOutputOptions(buffer);
	}

	MemBlock m = c->makeChart(Chart::BMP);
	c->setOutputOptions("-dpi");
	return m;
}

void CStaticHelper::displayChart(BaseChart *c)
{
	if (0 == c)
	{
		setHBITMAP(0);
		return;
	}

	c->setTransparentColor(-2);
	c->setOutputOptions((m_WCCisV6 >= 0) ? "+alpha" : "+alpha=3");
	MemBlock m = renderChart(c);

	if ((m_WCCisV6 == 0) && (((const BITMAPINFO *)(m.data + 14))->bmiHeader.biBitCount == 32))
	{
        m_WCCisV6 = -1;

	    m_testMode = true;
		PieChart testC(1, 1, 0x7fffffff);
        displayChart(&testC);
        displayChart(0);
		m_testMode = false;

        if (m_WCCisV6 <= 0)
		{
			displayChart(c);
            return;
		}
    }

	c->setOutputOptions("-alpha");

	setHBITMAP(bmpToHBITMAP(m.data));

	if (0 == c)
		m_toImageScaleX = m_toImageScaleY = 1;
	else
	{
		CRect r;
		GetClientRect(&r);
		DrawArea* d = c->getDrawArea();
		m_toImageScaleX = d->getWidth() / (double)r.Width();
		m_toImageScaleY = d->getHeight() / (double)r.Height();
	}
}

//
// High DPI Support
//
double CStaticHelper::toImageX(int x)
{
	return x * m_toImageScaleX;
}
double CStaticHelper::toImageY(int y)
{
	return y * m_toImageScaleY;
}
int CStaticHelper::toDisplayX(double x)
{
	double ret = x / m_toImageScaleX;
	return (int)((ret >= 0) ? ret + 0.5 : (ret - 0.5));
}
int CStaticHelper::toDisplayY(double y)
{
	double ret = y / m_toImageScaleY;
	return (int)((ret >= 0) ? ret + 0.5 : (ret - 0.5));
}


/////////////////////////////////////////////////////////////////////////////
//
// CViewPortControl
//
/////////////////////////////////////////////////////////////////////////////

//
// Constructor
//
CViewPortControl::CViewPortControl()
{
	m_ChartViewer = 0;
	m_Chart = 0;
	m_ReentrantGuard = false;

	m_mouseDownX = 0;
	m_mouseDownY = 0;
}

BEGIN_MESSAGE_MAP(CViewPortControl, CStaticHelper)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

//
// Free	resources
//
void CViewPortControl::OnDestroy() 
{
	setViewer(0);
	displayChart(0);
	
	CStatic::OnDestroy();
}

//
// Set the CChartViewer to be associated with this control
//
void CViewPortControl::setViewer(CChartViewer *viewer)
{
	if (m_ReentrantGuard)
		return;
	m_ReentrantGuard = true;

	if (0 != m_ChartViewer)
		m_ChartViewer->setViewPortControl(0);
	m_ChartViewer = viewer;
	setViewPortManager(viewer);
	if (0 != m_ChartViewer)
		m_ChartViewer->setViewPortControl(this);

	m_ReentrantGuard = false;
	
	updateDisplay();
}

//
// Get back	the	same CChartViewer pointer provided	by the previous	setViewer call.
//
CChartViewer *CViewPortControl::getViewer()
{
	return m_ChartViewer;
}

//
// Set the chart to	be displayed in the	control
//
void CViewPortControl::setChart(BaseChart *c)
{
	// In case the user	forgets	to check the "Notify" check	box	in the Dialog editor, we set it
	// ourselves so	the	CChartViewer control can receive mouse events.
	if ((GetStyle()	& SS_NOTIFY) ==	0)
		ModifyStyle(0, SS_NOTIFY);
	if ((GetStyle() & SS_TYPEMASK) != SS_BITMAP)
		ModifyStyle(SS_TYPEMASK, SS_BITMAP);

	m_Chart = c;
	ViewPortControlBase::setChart(c);
	updateDisplay();
}

//
// Get back	the	same BaseChart pointer provided	by the previous	setChart call.
//
BaseChart *CViewPortControl::getChart()
{
	return m_Chart;
}

//
// Display the chart
//
void CViewPortControl::paintDisplay()
{
	displayChart(getChart()); 
}

//
// Mouse left button down event.
//
void CViewPortControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);

	// Enable mouse capture for drag support
	SetCapture();

	// Remember current mouse position
	m_mouseDownX = point.x;
	m_mouseDownY = point.y;

	// Get the CChartViewer zoom/scroll state to determine which type of mouse action is allowed
	syncState();

	// Handle the mouse down event
	handleMouseDown(toImageX(point.x), toImageY(point.y));

	// Update the chart viewer if the viewport has changed
	updateChartViewerIfNecessary();
}

//
// MouseMove event handler
//
void CViewPortControl::OnMouseMove(UINT nFlags, CPoint point)
{
	// Get the CChartViewer zoom/scroll state to determine which type of mouse action is allowed
	syncState();

	// Handle the mouse move event
	handleMouseMove(toImageX(point.x), toImageY(point.y), isDrag(point));

	// Update the chart viewer if the viewport has changed
	updateChartViewerIfNecessary();

	// Mouse wheel events are only sent to the control in focus. So if mouse wheel zooming is 
	// enabled, we must get the focus in order to receive mouse wheel events.
	if ((0 != m_ChartViewer) && (m_ChartViewer->getMouseWheelZoomRatio() != 1)
		&& isOnPlotArea(toImageX(point.x), toImageY(point.y)))
		SetFocus();

	// Update the mouse cursor
	HCURSOR cursor = getHCursor(getCursor());
	if (cursor != NULL)
		::SetCursor(cursor);

	// Update the display
	if (needUpdateDisplay())
		paintDisplay();
}

//
// Mouse left button up event.
//
void CViewPortControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonUp(nFlags, point);
	ReleaseCapture();

	// Get the CChartViewer zoom/scroll state to determine which type of mouse action is allowed
	syncState();

	// Handle the mouse up event
	handleMouseUp(toImageX(point.x), toImageY(point.y));

	// Update the chart viewer if the viewport has changed
	updateChartViewerIfNecessary();
}

//
// MouseWheel handler
//
BOOL CViewPortControl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// Process the mouse wheel only if the mouse is over the plot area
	ScreenToClient(&pt);
	if ((0 == m_ChartViewer) || (!isOnPlotArea(toImageX(pt.x), toImageY(pt.y))))
		return FALSE;

	// Ask the CChartViewer to zoom around the center of the chart
	int x = m_ChartViewer->getPlotAreaLeft() + m_ChartViewer->getPlotAreaWidth() / 2;
	int y = m_ChartViewer->getPlotAreaTop() + m_ChartViewer->getPlotAreaHeight() / 2;
	return m_ChartViewer->onMouseWheelZoom(x, y, zDelta);
}

//
// Get the CChartViewer zoom/scroll state
//
void CViewPortControl::syncState()
{
	CChartViewer *viewer = getViewer();
	if (0 != viewer)
		setZoomScrollDirection(viewer->getZoomDirection(), viewer->getScrollDirection());
}

//
// Determines if the mouse is dragging.
//
bool CViewPortControl::isDrag(CPoint point)
{
	CChartViewer *viewer = getViewer();
	if (0 == viewer)
		return false;

	int minimumDrag = viewer->getMinimumDrag();
	int moveX = abs(m_mouseDownX - point.x);
	int moveY = abs(m_mouseDownY - point.y);
	return (moveX >= minimumDrag) || (moveY >= minimumDrag);
}

//
// Update the display
//
void CViewPortControl::updateDisplay()
{
	paintViewPort();
	paintDisplay();
}

//
// Update the CChartViewer if the viewport has changed
//
void CViewPortControl::updateChartViewerIfNecessary()
{
	CChartViewer *viewer = getViewer();
	if (0 == viewer)
		return;
	
	if (needUpdateChart() || needUpdateImageMap())
		viewer->updateViewPort(needUpdateChart(), needUpdateImageMap());
}

//
// The the mouse cursor
//
HCURSOR CViewPortControl::getHCursor(int position)
{
	switch (position)
	{
	case Chart::Left:
	case Chart::Right:
		//IDC_SIZEWE = 32644
		return AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32644));
	case Chart::Top:
	case Chart::Bottom:
		//IDC_SIZENS = 32645
		return AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32645));
	case Chart::TopLeft:
	case Chart::BottomRight:
		//IDC_SIZENWSE = 32642
		return AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32642));
	case Chart::TopRight:
	case Chart::BottomLeft:
		//IDC_SIZENESW = 32643
		return AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32643));
	}

	//IDC_ARROW = 32512
	return AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32512));
}

//
// Intercept WM_SETCURSOR to change	the	mouse cursor.
//
BOOL CViewPortControl::OnSetCursor(CWnd* pWnd, UINT	nHitTest, UINT message)	
{
	HCURSOR cursor = getHCursor(getCursor());
	if (cursor != NULL)
	{
		::SetCursor(cursor);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest,	message);
}


void CChartViewer::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
