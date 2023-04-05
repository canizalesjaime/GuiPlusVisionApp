#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ConvertMatToBitmap.h"

class wxImagePanel : public wxPanel
{
	wxImage image;
	wxBitmap resized;
	int w, h;

public:
	wxImagePanel(wxFrame* parent, wxString file, wxBitmapType format);

	void paintEvent(wxPaintEvent & evt);
	void paintNow();
	void OnSize(wxSizeEvent& event);
	void render(wxDC& dc);

	// some useful events
	/*
	 void mouseMoved(wxMouseEvent& event);
	 void mouseDown(wxMouseEvent& event);
	 void mouseWheelMoved(wxMouseEvent& event);
	 void mouseReleased(wxMouseEvent& event);
	 void rightClick(wxMouseEvent& event);
	 void mouseLeftWindow(wxMouseEvent& event);
	 void keyPressed(wxKeyEvent& event);
	 void keyReleased(wxKeyEvent& event);
	 */

	DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
// some useful events
/*
 EVT_MOTION(wxImagePanel::mouseMoved)
 EVT_LEFT_DOWN(wxImagePanel::mouseDown)
 EVT_LEFT_UP(wxImagePanel::mouseReleased)
 EVT_RIGHT_DOWN(wxImagePanel::rightClick)
 EVT_LEAVE_WINDOW(wxImagePanel::mouseLeftWindow)
 EVT_KEY_DOWN(wxImagePanel::keyPressed)
 EVT_KEY_UP(wxImagePanel::keyReleased)
 EVT_MOUSEWHEEL(wxImagePanel::mouseWheelMoved)
 */

 // catch paint events
	EVT_PAINT(wxImagePanel::paintEvent)
	//Size event
	EVT_SIZE(wxImagePanel::OnSize)
	END_EVENT_TABLE()


	// some useful events
	/*
	 void wxImagePanel::mouseMoved(wxMouseEvent& event) {}
	 void wxImagePanel::mouseDown(wxMouseEvent& event) {}
	 void wxImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
	 void wxImagePanel::mouseReleased(wxMouseEvent& event) {}
	 void wxImagePanel::rightClick(wxMouseEvent& event) {}
	 void wxImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
	 void wxImagePanel::keyPressed(wxKeyEvent& event) {}
	 void wxImagePanel::keyReleased(wxKeyEvent& event) {}
	 */

	wxImagePanel::wxImagePanel(wxFrame* parent, wxString file, wxBitmapType format) :
	wxPanel(parent)
{
	// load the file... ideally add a check to see if loading was successful
	image.LoadFile(file, format);
	w = -1;
	h = -1;
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void wxImagePanel::paintEvent(wxPaintEvent & evt)
{
	// depending on your system you may need to look at double-buffered dcs
	wxPaintDC dc(this);
	render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void wxImagePanel::paintNow()
{
	// depending on your system you may need to look at double-buffered dcs
	wxClientDC dc(this);
	render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void wxImagePanel::render(wxDC&  dc)
{
	int neww, newh;
	dc.GetSize(&neww, &newh);

	if (neww != w || newh != h)
	{
		resized = wxBitmap(image.Scale(neww, newh /*, wxIMAGE_QUALITY_HIGH*/));
		cv::Mat img = cv::Mat::zeros(cv::Size(neww, newh), CV_8UC3);
		cv::circle(img, cv::Point2f(100, 100), 50, cv::Scalar(255, 0, 0), cv::FILLED, cv::LINE_8);
		ConvertMatBitmapTowxBitmap(img, resized);
		w = neww;
		h = newh;
		dc.DrawBitmap(resized, 0, 0, false);
	}
	else {
		dc.DrawBitmap(resized, 0, 0, false);
	}
}

/*
 * Here we call refresh to tell the panel to draw itself again.
 * So when the user resizes the image panel the image should be resized too.
 */
void wxImagePanel::OnSize(wxSizeEvent& event) 
{
	Refresh();
	//skip the event.
	event.Skip();
}

class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};

enum
{
	ID_Hello = 1
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Hello, MyFrame::OnHello)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
//wxIMPLEMENT_APP(visionApp);

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
		"Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("COPYRIGHT");
}

void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("Gotta think of something to write here.",
		"About App", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Gotta think of something to write here.");
}

class MyApp : public wxApp
{

	wxFrame *frame;
	wxImagePanel * drawPane;
public:
	bool OnInit()
	{
		// make sure to call this first
		wxInitAllImageHandlers();

		wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		MyFrame *frame = new MyFrame("Jaime App", wxPoint(50, 50), wxSize(450, 340));

		// then simply create like this
		drawPane = new wxImagePanel(frame, wxT("C:\\Users\\Caniz\\source\\reposgithub\\apollo_detection_systems\\3d_net\\dataset\\chair.png"), wxBITMAP_TYPE_PNG);
		sizer->Add(drawPane, 1, wxEXPAND);

		frame->SetSizer(sizer);

		frame->Show();
		return true;
	}

};

IMPLEMENT_APP(MyApp)