#pragma once
#include <wx/wx.h>
#include <wx/valnum.h>
#include <wx/fileconf.h>
#include <wx/log.h>
#include <wx/file.h>
#include "Table.h"
#include <fstream>

class MyFrame : public wxFrame
{
public:
	MyFrame();
	~MyFrame();

public:
	wxStaticText *t_desc = nullptr;
	wxStaticText *t_givenInfoL = nullptr;
	wxTextCtrl *t_givenInfo = nullptr;
	wxStaticText *t_analyserInfoL = nullptr;
	wxTextCtrl *t_analyserInfo = nullptr;
	wxStaticText *t_batteryPL = nullptr;
	wxStaticText *t_batteryCL = nullptr;
	wxStaticText *t_nameL = nullptr;
	wxStaticText *t_nameEnding = nullptr;
	wxButton *b_open = nullptr;
	wxButton *b_import = nullptr;
	wxButton *b_analyserRun = nullptr;
	wxTextCtrl *t_path = nullptr;
	wxTextCtrl *t_batteryCap = nullptr;
	wxTextCtrl *t_batteryPower = nullptr;
	wxTextCtrl *t_name = nullptr;
	wxCheckBox *c_save = nullptr;
	wxDialog *d_syntaxDialog = nullptr;
	wxStaticText *t_syntaxDesc = nullptr;
	wxStaticText *t_syntaxDescL = nullptr;
	/*Table Objects*/
	Table *given = nullptr;
	Table *optimum = nullptr;

public:
	void importCSVEvent(wxCommandEvent &event);
	void openFileEvent(wxCommandEvent &event);
	void runAnalysisEvent(wxCommandEvent &event);
	void checkBoxEvent(wxCommandEvent &event);

	//Declare Events macro
	wxDECLARE_EVENT_TABLE();
private:
	void fillGivenInfo(Result &result);
	void fillAnalyserInfo(Result &result);

	void OnSyntax(wxCommandEvent &event);
	void OnSyntaxConfirm(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);

	/*private data members*/
	float batteryCap = 0.0f;
	float batteryPower = 0.0f;
	wxLogStream *logStream = nullptr;
	std::ofstream *logfile = nullptr;
	bool CSVFileValid = false;
	bool CSVFileAnalysed = false;
};
