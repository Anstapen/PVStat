#include "MyApp.h"
#include "MyFrame.h"
#include <wx/stdpaths.h>
#include <wx/fileconf.h>



wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
	// create INI file in user's directory

	wxString inifile_dir = wxStandardPaths::Get().GetDataDir();

	//wxLogMessage("Using .INI File: %s", inifile_dir + wxT("\\config.ini"));

	m_fileconfig = new wxFileConfig(wxT(""), wxT(""), inifile_dir + wxT("\\config.ini"));
	wxConfigBase::Set(m_fileconfig);

	

	/*Create the Main Frame*/
	MyFrame *frame = new MyFrame();
	frame->Show(true);
	return true;
}

int MyApp::OnExit()
{
	if (m_fileconfig) {
		m_fileconfig->Flush();		// not really needed
		delete m_fileconfig;
		m_fileconfig = NULL;
	}
	wxConfigBase::Set(NULL);

	
	return 0;
}
