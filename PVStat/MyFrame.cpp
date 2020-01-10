#include "MyFrame.h"
#include "Result.h"
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <ios>
#include <wx/progdlg.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/log.h>>
#include <wx/file.h>
#include <wx/wxprec.h>

#define STAT_IMPORT_BUTTON_ID 10001
#define STAT_OPEN_BUTTON_ID 10002
#define STAT_ANALYSIS_BUTTON_ID 10003
#define STAT_SAVE_CHECKBOX_ID 20001
#define STAT_MENU_SYNTAX 30001

#define SYNTAX_DESC wxT("Colums:\n- Batterieaufladung Standort\n- Batterieversorgung Standort\n- Ladezustand Batterie\n\
- Gesamt-Stromerzeugung Standort\n- Netzeinspeisung Standort\n- Selbstversorgung Standort\n- Gesamt-Stromverbauch Standort\n\
- Stromzukauf Standort\n\n- Auflösung: 15 Minuten\n- Dezimaltrennzeichen: . (Punkt)\n")

/*Implement Event Table*/

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(STAT_IMPORT_BUTTON_ID, importCSVEvent)
EVT_BUTTON(STAT_OPEN_BUTTON_ID, openFileEvent)
EVT_BUTTON(STAT_ANALYSIS_BUTTON_ID, runAnalysisEvent)
EVT_CHECKBOX(STAT_SAVE_CHECKBOX_ID, checkBoxEvent)
EVT_MENU(STAT_MENU_SYNTAX, OnSyntax)
EVT_MENU(wxID_EXIT, OnExit)
EVT_MENU(wxID_ABOUT, OnAbout)
wxEND_EVENT_TABLE()

MyFrame::MyFrame()
	: wxFrame(nullptr, wxID_ANY, "PV Statistics Analyser", wxPoint(30, 30), wxSize(1200, 750))
{
	/*Create the Log file and set the main logger to it*/
	this->logfile = new std::ofstream;
	this->logfile->open("Log.txt", std::ios_base::out | std::ios_base::trunc);

	this->logStream = new wxLogStream(this->logfile);
	wxLog::SetActiveTarget(this->logStream);

	wxLogMessage("Building Frame...");
	/*Creating the menu bar*/
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(STAT_MENU_SYNTAX, "&Syntax\tCtrl-S",
		"Click to Display the required CSV Syntax");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("Welcome to the PV Statistics Analyser!");
	// Create the necessary Buttons
	this->b_import = new wxButton(this, STAT_IMPORT_BUTTON_ID, "Import CSV", wxPoint(10,60), wxSize(70,30));
	this->b_open = new wxButton(this, STAT_OPEN_BUTTON_ID, "Select File", wxPoint(210, 30), wxSize(60, 20));
	this->b_analyserRun = new wxButton(this, STAT_ANALYSIS_BUTTON_ID, "Run Analysis", wxPoint(10, 350), wxSize(80, 30));
	// Create the Checkbox
	this->c_save = new wxCheckBox(this, STAT_SAVE_CHECKBOX_ID, "Save CSV", wxPoint(10, 295));
	this->c_save->SetValue(true);
	//Create the TextCtrl and StaticText Objects THESE DO NOT HAVE EVENT HANDLING
	this->t_path = new wxTextCtrl(this, wxID_ANY, "", wxPoint(5, 30), wxSize(200, 20));
	this->t_desc = new wxStaticText(this, wxID_ANY, "Select the CSV File:", wxPoint(5, 5), wxSize(200, 20));
	this->t_givenInfoL = new wxStaticText(this, wxID_ANY, "Stats of the given CSV File:", wxPoint(320,10), wxSize(200, 20));
	this->t_givenInfo = new wxTextCtrl(this, wxID_ANY, "", wxPoint(320, 30), wxSize(400, 620), wxTE_MULTILINE | wxTE_READONLY);
	this->t_givenInfo->SetBackgroundColour(*wxWHITE);
	this->t_analyserInfoL = new wxStaticText(this, wxID_ANY, "Stats of the created CSV File:", wxPoint(750, 10), wxSize(200, 20));
	this->t_analyserInfo = new wxTextCtrl(this, wxID_ANY, "", wxPoint(750, 30), wxSize(400, 620), wxTE_MULTILINE | wxTE_READONLY);
	this->t_analyserInfo->SetBackgroundColour(*wxWHITE);
	this->t_batteryCL = new wxStaticText(this, wxID_ANY, "Battery Capacity (kWh):", wxPoint(10, 270), wxSize(160, 20));
	this->t_batteryCap = new wxTextCtrl(this, wxID_ANY, "", wxPoint(180, 270), wxSize(50, 20));
	this->t_batteryPL = new wxStaticText(this, wxID_ANY, "max. Battery Discharge (W):", wxPoint(10, 240), wxSize(160, 20));
	this->t_batteryPower = new wxTextCtrl(this, wxID_ANY, "", wxPoint(180, 240), wxSize(50, 20));
	this->t_nameL = new wxStaticText(this, wxID_ANY, "Name of the CSV:", wxPoint(10, 320), wxSize(100, 20));
	this->t_name = new wxTextCtrl(this, wxID_ANY, "", wxPoint(110, 320), wxSize(80, 20));
	this->t_nameEnding = new wxStaticText(this, wxID_ANY, ".csv", wxPoint(190, 320), wxSize(30, 20));
	this->d_syntaxDialog = new wxDialog(this, wxID_ANY, "Syntax of the CSV File", wxPoint(150, 150), wxSize(400, 300), wxCAPTION | wxCLOSE_BOX);
	this->t_syntaxDesc = new wxStaticText(this->d_syntaxDialog,wxID_ANY, SYNTAX_DESC, wxPoint(10, 50), wxSize(300, 200));
	this->t_syntaxDescL = new wxStaticText(this->d_syntaxDialog, wxID_ANY, "The CSV File has to have the following properties, otherwise the analyser will not work:", wxPoint(10, 10), wxSize(300, 40));
	this->t_syntaxDesc->SetBackgroundColour(*wxWHITE);
	wxLogMessage("Building Frame Complete...");
	//Get global config and set members
	wxConfigBase *conf = wxConfigBase::Get(false);
	wxString true_val;
	if (conf) {
		//Check every config setting/option
		bool flag = (conf->Read((const wxString &)wxT("options/defaultSaveCSV"), (wxString *)&true_val) == 1);
		if (flag) {
			/*Config read was successfull, use read value*/
			wxLogMessage("Config Entry for defaultSaveCSV found: %s", true_val);
			this->c_save->SetValue((true_val == "true" ? true : false));
			if (!this->c_save->IsChecked()) {
				this->t_name->Enable(false);
			}
		}
		else {
			/*Otherwise, set true*/
			wxLogMessage("No Config Entry found for defaultSaveCSV, using true...");
			this->c_save->SetValue(true);
		}
		wxString defaultCSVPath;
		flag = (conf->Read(wxT("settings/defaultCSVPath"), (wxString *)&defaultCSVPath) == 1);
		if (flag) {
			/*Config read was successfull, use read value*/
			wxLogMessage("Config Entry for defaultCSVPath found: %s", defaultCSVPath);
			this->t_path->SetValue(defaultCSVPath);
		}
		else {
			wxLogMessage("No Config Entry found for defaultCSVPath, using \"\"...");
			this->t_path->SetValue(wxT(""));
		}
		wxString defaultBatteryCapacity;
		flag = (conf->Read(wxT("settings/defaultBatteryCapacity"), (wxString *)&defaultBatteryCapacity) == 1);
		if (flag) {
			/*Config read was successfull, use read value*/
			wxLogMessage("Config Entry for defaultBatteryCapacity found: %s", defaultBatteryCapacity);
			this->t_batteryCap->SetValue(defaultBatteryCapacity);
		}
		else {
			wxLogMessage("No Config Entry found for defaultBatteryCapacity, using \"\"...");
			this->t_batteryCap->SetValue(wxT(""));
		}
		wxString defaultBatteryPower;
		flag = (conf->Read(wxT("settings/defaultBatteryPower"), (wxString *)&defaultBatteryPower) == 1);
		if (flag) {
			/*Config read was successfull, use read value*/
			wxLogMessage("Config Entry for defaultBatteryPower found: %s", defaultBatteryPower);
			this->t_batteryPower->SetValue(defaultBatteryPower);
		}
		else {
			wxLogMessage("No Config Entry found for defaultBatteryPower, using \"\"...");
			this->t_batteryPower->SetValue(wxT(""));
		}
		wxString defaultCSVName;
		flag = (conf->Read(wxT("settings/defaultCSVName"), (wxString *)&defaultCSVName) == 1);
		if (flag) {
			/*Config read was successfull, use read value*/
			wxLogMessage("Config Entry for defaultCSVName found: %s", defaultCSVName);
			this->t_name->SetValue(defaultCSVName);
		}
		else {
			wxLogMessage("No Config Entry found for defaultCSVName, using \"\"...");
			this->t_name->SetValue(wxT(""));
		}
	}
}

MyFrame::~MyFrame()
{
	delete this->given;
	delete this->optimum;
	/*Save the config file*/
	wxLogMessage("Saving Config File...");
	wxConfigBase *conf = wxConfigBase::Get(false);
	if (conf) {
		conf->Write(wxT("options/defaultSaveCSV"), this->c_save->IsChecked() ? "true" : "false");
		conf->Write(wxT("settings/defaultCSVPath"), this->t_path->GetValue());
		conf->Write(wxT("settings/defaultBatteryCapacity"), this->t_batteryCap->GetValue());
		conf->Write(wxT("settings/defaultBatteryPower"), this->t_batteryPower->GetValue());
		conf->Write(wxT("settings/defaultCSVName"), this->t_name->GetValue());
	}
	wxLogMessage("Closing Logging...");
	/*close logging*/
	this->logfile->close();
	delete this->logfile;
	this->logfile = nullptr;
	delete this->logStream;
	this->logStream = nullptr;
}

void MyFrame::importCSVEvent(wxCommandEvent & event)
{
	wxLogMessage("Importing CSV...");
	/* Open the selected File and load the Table*/
	std::string path = std::string((this->t_path->GetValue()).mb_str());
	wxLogMessage("Using CSV Path: %s", path);
	wxDialog progress(this, wxID_ANY, "Parsing CSV File...", wxPoint(200,200), wxDefaultSize, wxCAPTION);
	progress.Show(true);
	this->given = new Table(path);
	progress.Show(false);
	if (!*given) {
		//wxMessageBox("Error: CSV File is corrupted.");
		wxLogWarning("Error: CSV File Syntax wrong or File corrupted!");
		wxMessageBox("Error: CSV File Syntax wrong or File corrupted!");
		this->CSVFileValid = false;
		this->CSVFileAnalysed = false;
	}
	else {
		std::stringstream amount;
		amount << "Success! Parsed " << this->given->size() << " Lines!" << std::endl;
		wxLogMessage("Success! Parsed %d Lines...", this->given->size());
		wxMessageBox(amount.str());
		/*Analyse the given CSV*/
		Result result;
		this->CSVFileValid = true;
		wxLogMessage("Analysing the given CSV...");
		result = this->given->analyse();
		if (result.valid) {
			this->CSVFileAnalysed = true;
			wxLogMessage("Finished Analysing, printing Results...");
			/*Fill the Static Text*/
			fillGivenInfo(result);
			wxLogMessage("Printed Results of given CSV File...");
		}
		else {
			this->CSVFileAnalysed = false;
			wxLogMessage("Error: CSV File Analysis failed!");
			wxMessageBox("Error: CSV File Analysis failed!");
		}
		
	}

}

void MyFrame::openFileEvent(wxCommandEvent & event)
{
	wxLogMessage("Setting CSV File Path...");
	wxFileDialog openFileDialog(this, "Open CSV File", "", "", "CSV files (*.csv)|*.csv", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		/*The user clicked cancel...*/
		wxLogMessage("User cancelled File Dialog...");
		return;
	}
	/* We actually just want the Path...*/
	wxString filePath = openFileDialog.GetPath();
	wxLogMessage("Success! File Path: %s", filePath);
	this->t_path->SetValue(filePath);
}

void MyFrame::runAnalysisEvent(wxCommandEvent & event)
{
	/*Check if the given CSV File has been analysed...*/
	if (!this->CSVFileAnalysed) {
		wxMessageBox("Error: CSV File has to be analysed first!");
		wxLogMessage("Error: CSV File has to be analysed first!");
		return;
	}
	/*Several Things have to be checked...*/
	wxLogMessage("Checking Battery Capacity Value...");
	double batteryCap = 0.0f;
	if (!(this->t_batteryCap->GetValue().ToDouble(&batteryCap))) {
		wxMessageBox("Error: Battery Capacity is not a Number!");
		wxLogMessage("Error: Battery Capacity is not a Number!");
		return;
	}
	/*Value has to be positive*/
	if (batteryCap < 1) {
		wxMessageBox("Error: Battery Capacity must be greater than 0!");
		wxLogMessage("Error: Battery Capacity must be greater than 0!");
		return;
	}
	wxLogMessage("Checking battery Power Value...");
	double batteryPower = 0.0f;
	if (!(this->t_batteryPower->GetValue().ToDouble(&batteryPower))) {
		wxMessageBox("Error: Battery Power is not a Number!");
		wxLogMessage("Error: Battery Power is not a Number!");
		return;
	}
	/*Value has to be positive*/
	if (batteryPower < 1) {
		wxMessageBox("Error: Battery Power must be greater than 0!");
		wxLogMessage("Error: Battery Power must be greater than 0!");
		return;
	}
	Result ret;
	Table *optimum = new Table;
	wxDialog progress(this, wxID_ANY, "Creating optimal CSV...", wxPoint(200, 200), wxDefaultSize, wxCAPTION);
	progress.Show(true);
	wxLogMessage("Creating optimal CSV...");
	this->given->getOptimum(ret,*optimum,batteryCap, (float)(batteryPower / 4.0f));
	progress.Show(false);
	if (!ret.valid) {
		wxMessageBox("Error: Could not create optimal CSV!");
		wxLogMessage("Error: Could not create optimal CSV!");
		return;
	}
	else {
		wxMessageBox("Finished!");
		wxLogMessage("Finished creating optimal CSV...");
		wxLogMessage("Filling Statistics...");
		fillAnalyserInfo(ret);
	}
	/*If save file is checked save the file with the given name*/
	if (this->c_save->IsChecked()) {
		wxLogMessage("Checking if File has to be saved...");
		if (this->t_name->IsEmpty()) {
			wxLogMessage("Saving File...");
			wxMessageBox("Error: No Name provided!");
			wxLogMessage("Error: No Name provided!");
			return;
		}
		else {
			/*Convert textbox input to std::string*/
			wxString path = (this->t_name->GetValue()).mb_str();
			std::string path_str = std::string(path.mb_str());
			/*Append the file extension*/
			std::stringstream sstream;
			sstream << path_str;
			sstream << ".csv";
			path_str = sstream.str();
			wxLogMessage("File Path: %s", path_str);
			wxDialog progress(this, wxID_ANY, "Saving CSV...", wxPoint(200, 200), wxDefaultSize, wxCAPTION);
			progress.Show(true);
			int ret = optimum->saveCSV(path_str);
			progress.Show(false);
			if (ret == -1) {
				wxMessageBox("Error: Could not save CSV File!");
				wxLogMessage("Error: Could not save CSV File!");
			}
			else {
				sstream.str("");
				sstream << "Saved CSV as " << path_str << "!";
				wxMessageBox(sstream.str());
				wxLogMessage("Success! Saved File...");
			}
		}
	}
}

void MyFrame::checkBoxEvent(wxCommandEvent & event)
{
	if (this->c_save->IsChecked()) {
		this->t_name->Enable(true);
	}
	else {
		this->t_name->Enable(false);
	}
}

void MyFrame::fillGivenInfo(Result & result)
{
	std::stringstream test;
	/*Add the contents*/
	std::ostream stream(this->t_givenInfo);
	
	for (int i = 0; i < result.mean.size(); i++) {
		test << "Column " << result.spaceDesc[i + 1] << ": " << std::endl;
		test << "Average: " << result.mean[i] << " Wh" << std::endl;
		test << "Maximum: " << result.max[i] << " Wh" << std::endl;
		test << "Total: " << (int)(result.total[i] / 4.0f) << " Wh" << std::endl;
		test << std::endl;
	}
	test << "Buy/Sell Values: " << std::endl;
	test << "Sold Energy: " << result.sellValue << " Euro " << std::endl;
	test << "Bought Energy: " << result.buyValue << " Euro " << std::endl;
	test << "Profit: " << result.profitValue << " Euro " << std::endl;
	std::string str(test.str());
	wxString toSet(str.c_str());
	stream << str;
}

void MyFrame::fillAnalyserInfo(Result & result)
{
	std::stringstream test;
	/*Add the contents*/
	std::ostream stream(this->t_analyserInfo);

	for (int i = 0; i < result.mean.size(); i++) {
		test << "Column " << result.spaceDesc[i + 1] << ": " << std::endl;
		test << "Average: " << result.mean[i] << " Wh" << std::endl;
		test << "Maximum: " << result.max[i] << " Wh" <<  std::endl;
		test << "Total: " << (int)(result.total[i] / 1.0f) << " Wh" << std::endl;
		test << std::endl;
	}
	test << "Buy/Sell Values: " << std::endl;
	test << "Sold Energy: " << result.sellValue << " Euro " << std::endl;
	test << "Bought Energy: " << result.buyValue << " Euro " << std::endl;
	test << "Profit: " << result.profitValue << " Euro " << std::endl;
	std::string str(test.str());
	wxString toSet(str.c_str());
	stream << str;
}

void MyFrame::OnSyntax(wxCommandEvent & event)
{
	//wxMessageBox(wxT("Syntax Event"), wxT("CSV Syntax"));
	
	this->d_syntaxDialog->Show(true);
}

void MyFrame::OnSyntaxConfirm(wxCommandEvent & event)
{
	this->d_syntaxDialog->Show(false);
}

void MyFrame::OnExit(wxCommandEvent & event)
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent & event)
{
	wxMessageBox(wxT("Author: Anton Stapenhorst\nVersion: 1.0.2"), wxT("About"));
}