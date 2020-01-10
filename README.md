Date: 01/08=2020
Author: Anton Stapenhorst

README of the PV Statistics Analyser Version 1.0.1

***Important Information***

This version of the application checks the battery inputs and only creates the optimal CSV if the given File was correctly analysed.

The CSV file that gets saved (if the checkbox is checked) is always saved in the directory of the .exe.

The Application does write a very basic Log File, but as of Version 1.0.1, this File gets written on Exit, so if
it crashes, the Log file does not help with debugging.

The config file contains a few options and settings that can be changed. As of Version 1.0.1, all of the fields 
are default values and not at all necessary to use the program in its entirety, it just prevents the user from
having to re-enter all the values after restart.


***General Workflow***

- start app
- click on "select CSV File" and select a CSV File created by SOLARWATT (for the CSV File Syntax check the menu or press CTRL-S)
- click on "import CSV", Statistics about the data should be visible
- change the Battery Capacity/Power as you wish and enable/disable file output
- click "Run Analyser"
- If successful, more data should appear and if file output was selected, a csv file should be created
- If another file should be analysed, repeat the steps


Any questions or improvements can be written to the following email:

anton.stapen@gmail.com
