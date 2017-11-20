/* 
	Author Name: Micaiah Skolnick 
			based on SQL connection code by Dr. Stephen Blythe
           Date: 11/19/17
*/
#include <iostream>
#include <iomanip>
#include <string>

#include <my_global.h>
#include <mysql.h>

#include <conio.h>

using namespace std;
	//Global variables
// mysql connection and query variables
MYSQL *conn, // actual mysql connection
mysql;   // local mysql data
MYSQL_RES *res; // mysql query results
MYSQL_ROW row;  // one row of a table (result)
				// strings for mysql hostname, userid, ...
string db_host = "lindenwoodcshome.ddns.net";
string db_user = "skolnickm";
string db_password = "mskolnick";
string db_name = "skolnickm";
bool sendQuery(string query) {
	int status;
	string myQuery = query;
	cout << "The Query is:" << myQuery << endl;
	//cout << "Sending query ..."; 
	cout.flush();
	status = mysql_query(conn, myQuery.c_str());
	//cout << "Done" << endl;
	// if the query didn't work ...
	if (status != 0)
	{
		// ... explain why ...
		cout << mysql_error(&mysql) << endl;
		return false;  // ... and exit program
	}
	else {
		cout << "Query succeeded" << endl;
		return true;
	}
}
// reads in a password without echoing it to the screen
//string myget_passwd()
//{ //****************************************NEED TO CHANGE THIS BACK to ask for password/username BEFORE SUBMITTING TO BLYTHE*******
//	string passwd;
//
//	for (;;)
//	{
//		char ch;
//		ch = _getch();           // get char WITHIOUT echo!
//		if (ch == 13 || ch == 10) // if done ...
//			break;           //  stop reading chars!
//		cout << '*';  // dump * to screen
//		passwd += ch;   // addd char to password
//	}
//	cin.sync(); // flush anything else in the buffer (remaining newline)
//	cout << endl;  // simulate the enter that the user pressed
//
//	return passwd;
//}
void addDialog() {
	char code2 = 'q';
	string cCode, cName, tName, vtName, htName;
	int vtScore, htScore;
	cin >> code2; //Get next code for what will be added
	switch (code2) { //Enter the correct dialog based on the character entered
	case 'c':
		cin >> cCode >> ws; //Get the city code
		getline(cin, cName); //Get the city name
			//Write the new city/name to the database
		sendQuery("insert into cities values(\"" + cCode + "\",\"" + cName + "\")");
		break;
	case 't':
		cin >> cCode; //Get the city code for the new team
		cin >> tName; //Get the team name for the new team
					  //Write the new team/city code to the file
		if(sendQuery("insert into teams values(\"" + cCode + "\",\"" + tName + "\")")) {
				//If the team was successfully added to the team database, add it to the records database
			sendQuery("insert into records values(\"" + tName + "\", 0, 0, 0, 0, 0, 0)");
		}
		break;
	case 'g':
			//Get the visiting team name,team score,home team name, and home team score
		cin >> vtName >> vtScore >> htName >> htScore;
			//Add the game to the database
		if (sendQuery("insert into games values(\"" + vtName + "\"," + to_string(vtScore) + 
			",\"" + htName + "\"," + to_string(htScore) + ")")) {
			if (htScore > vtScore) {
					//Update the records table if home team won
				sendQuery("update records set w = w+1, pf = pf+"+to_string(htScore)+
					", pa = pa+"+to_string(vtScore)+", wp = w/(w+l), pd = pf/pa where t_name = \"" + htName + "\"");
				sendQuery("update records set l = l+1, pf = pf+" + to_string(vtScore) +
					", pa = pa+" + to_string(htScore) + ", wp = w/(w+l), pd = pf/pa where t_name = \"" + vtName + "\"");
			}
			else if (vtScore > htScore) {
					//Update the records table if visiting team won
				sendQuery("update records set w = w+1, pf = pf+" + to_string(vtScore) +
					", pa = pa+" + to_string(htScore) + ", wp = w/(w+l), pd = pf/pa where t_name = \"" + vtName + "\"");
				sendQuery("update records set l = l+1, pf = pf+" + to_string(htScore) +
					", pa = pa+" + to_string(vtScore) + ", wp = w/(w+l), pd = pf/pa where t_name = \"" + htName + "\"");
			}			
		}
		break;
	}
}
void printCities() {		 
	//Print each of the cities which were entered into the database
	sendQuery("select * from cities");	
	//Print header
	cout << "= City Name ======== Code" << endl;
	// get the query result(s)
	res = mysql_store_result(conn);
	// go through each line (row) of the answer table
	for (row = mysql_fetch_row(res);
		row != NULL;
		row = mysql_fetch_row(res))
	{
		// print out the first 2 colums; they are stored in
		//    an "array-like" manner
		cout << " " << left << setw(20) << row[1] << row[0] << endl;
	}
	// clean up the query
	mysql_free_result(res);
	cout << endl; // Add a line after printing data	
}
void printTeams() {
	//Print each of the cities which were entered into the database
	sendQuery("select teams.t_name, cities.c_name from teams, cities where teams.c_code = cities.c_code");
	//Print header
	cout << "= Team Name ========= City Name" << endl;
	// get the query result(s)
	res = mysql_store_result(conn);
	// go through each line (row) of the answer table
	for (row = mysql_fetch_row(res);
		row != NULL;
		row = mysql_fetch_row(res))
	{
		// print out the first 2 colums; they are stored in
		//    an "array-like" manner
		cout << left << "  " << setw(20) << row[1] << row[0] << endl;
	}
	// clean up the query
	mysql_free_result(res);
	cout << endl; // Add a line after printing data	
}
void printGames() {
	//Print each of the cities which were entered into the database
	sendQuery("select * from games");
	// get the query result(s)
	res = mysql_store_result(conn);
	//Print header
	cout << "= Visiting Team = Score == Home Team ==== Score" << endl; //Print data header	
	// go through each line (row) of the answer table
	for (row = mysql_fetch_row(res);
		row != NULL;
		row = mysql_fetch_row(res))
	{
		// print out the first 2 colums; they are stored in
		//    an "array-like" manner
		cout << "  " << left << setw(18) << row[0] << setw(7) << row[1]
			<< setw(17) << row[2] << row[3] << endl;
	}
	// clean up the query
	mysql_free_result(res);
	cout << endl; // Add a line after printing data		
}
void listDialog() {
	char code2 = 'q';
	cin >> code2; //Get next code
	switch (code2) { //Enter the correct dialog based on the character entered
	case 'c': 
		printCities();
		break;
	case 't':
		printTeams();
		break;
	case 'g': 
		printGames();
		break;
	}
}
void populateRecordTable() {
		//Count/calculate the wins/losses/pf/pa/wp and pd for each team 
	sendQuery("select sum(pf) from games where ");

}
void standingsDialog() {
	//Print the standings from each of the teams
	sendQuery("select * from records order by wp DESC, pd DESC, t_name DESC");
	// get the query result(s)
	res = mysql_store_result(conn);
	//Print header
	cout << "=Team=============W===L===PF===PA" << endl; //Print data label
		 // go through each line (row) of the answer table
	for (row = mysql_fetch_row(res);
		row != NULL;
		row = mysql_fetch_row(res))
	{
		// print out the colums
		cout << " " << left << setw(17) << row[0] << setw(4) << row[1] << setw(4) << row[2]
			<< setw(5) << row[3] << setw(5) << row[4] << endl;
	}
	// clean up the query
	mysql_free_result(res);
	cout << endl; // Add a line after printing data		
}
void recordDialog() {
	string tName = "";
	cin >> tName;
	//Print the record of the entered team
	sendQuery("select * from records where t_name = \""+tName+"\"");
	// get the query result(s)
	res = mysql_store_result(conn);
	//Print header
	cout << "=Team=============W===L===PF===PA" << endl; //Print data label
														 // go through each line (row) of the answer table
	for (row = mysql_fetch_row(res);
		row != NULL;
		row = mysql_fetch_row(res))
	{
		// print out the colums
		cout << " " << left << setw(17) << row[0] << setw(4) << row[1] << setw(4) << row[2]
			<< setw(5) << row[3] << setw(5) << row[4] << endl;
	}
	// clean up the query
	mysql_free_result(res);
	cout << endl; // Add a line after printing data	
}
void deleteDialog() {
	string tName = "";
	cin >> tName;
		//Delete the entered team from everything
	sendQuery("delete from teams where t_name = \"" + tName + "\"");
	sendQuery("delete from records where t_name = \"" + tName + "\"");
	sendQuery("delete from games where vt_name = \"" + tName + "\"");	
	sendQuery("delete from games where ht_name = \"" + tName + "\"");
}
void moveDialog() {
	string tName = "",cCode = "";
	cin >> tName >> cCode;
	//Delete the entered team from everything
	sendQuery("update teams set c_code = \"" + cCode + "\" where t_name = \"" + tName + "\"");
	
}
int main()
{
	// set up the client (this machine) to use mysql
	cout << "initializing client DB subsystem ..."; cout.flush();
	mysql_init(&mysql);
	cout << "Done!" << endl;

	// get user credentials and mysql server info
	cout << "Enter MySQL database hostname (or IP adress):";
	//cin >> db_host; //****************************************NEED TO CHANGE THIS BACK to ask for password/username BEFORE SUBMITTING TO BLYTHE*******

	cout << "Enter MySQL database username:";
	//cin >> db_user; //****************************************NEED TO CHANGE THIS BACK to ask for password/username BEFORE SUBMITTING TO BLYTHE*******

	cout << "Enter MySQL database password:";
	//db_password = myget_passwd(); //****************************************NEED TO CHANGE THIS BACK to ask for password/username BEFORE SUBMITTING TO BLYTHE*******
	// could also prompt for this, if desired
	db_name = db_user;

	// go out and connect to the mysql server
	cout << "Connecting to remote DB ..."; cout.flush();
	conn = mysql_real_connect(&mysql,
		db_host.c_str(), db_user.c_str(), db_password.c_str(), db_user.c_str(),
		0, 0, 0); // last three are usually 0's

		// if we couldn't setup the connection ...
	if (conn == NULL)	{
		// print out the error message as to why ...
		cout << mysql_error(&mysql) << endl;
		return 1; // ... and exit the program. 
	}
	else
		cout << "DB connection established" << endl;
	// now, send mysql our query ...
	int status;

	/*

	string myQuery = "select * from brandNewTable";
	cout << "What is the author name?";
	string authName = "skolnickm"; //****************************************NEED TO CHANGE THIS BACK to just declaration BEFORE SUBMITTING TO BLYTHE*******
	//cin >> authName; //*******************************NEED TO CHANGE THIS BACK to ask for password/username BEFORE SUBMITTING TO BLYTHE*******

	myQuery += " where Name= \"";
	myQuery += authName + "\"";
	*/
		//Create tables for database
	sendQuery("create table if not exists cities (c_code char(100) not null primary key, c_name char(100))");	
	sendQuery("create table if not exists teams (c_code char(100) not null primary key, t_name char(100))");
	sendQuery("create table if not exists games (vt_name char(100), vt_score int, ht_name char(100), ht_score int)");	
	sendQuery("create table if not exists records (t_name char(100) not null primary key, w float, l float, pf float, pa float, wp float, pd float)");
	
	char code = 'q'; //Start with q, so if nothing is added, code quits
					 //Read instructions to perform until an instruction to quit has been reached
	do {
		cout << ">>>"; //Prompt the user for input
		cin >> code; //Get the first code
		//buildTeamList(); //Build a list of teams already in the file
		switch (code) { //Enter the correct dialog based on the character entered
		case 'a': addDialog(); break;
		case 'l': listDialog(); break;
		case 'r': recordDialog(); break;
		case 's': standingsDialog(); break;
		case 'd': deleteDialog(); break;
		case 'm': moveDialog(); break;
		}
	} while (code != 'q'); //Keep reading instructions until a code of "q" is reached
	mysql_close(conn); // clean up the connection
	return 0;
}