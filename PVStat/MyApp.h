#pragma once
#include <wx/wx.h>
#include <wx/fileconf.h>


class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
	// MyApp member variable:
	wxFileConfig *m_fileconfig = nullptr;
};
