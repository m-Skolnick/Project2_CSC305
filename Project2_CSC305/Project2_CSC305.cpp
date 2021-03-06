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
string db_host;
string db_user;
string db_password;
string db_name;
//Sends query to mysql
bool sendQuery(string query) {
	int status;
	string myQuery = query;	
	cout.flush();
	status = mysql_query(conn, myQuery.c_str());
	// if the query didn't work ...
	if (status != 0)
	{
		// ... explain why ...		
		cout << mysql_error(&mysql) << endl;
		cout << "The Query is:" << myQuery << endl;
		return false;  // ... and exit program
	}
	else {
		//cout << "Query succeeded" << endl;
		return true;
	}
}
//reads in a password without echoing it to the screen
string myget_passwd()
{
	string passwd;
	for (;;)
	{
		char ch;
		ch = _getch();           // get char WITHIOUT echo!
		if (ch == 13 || ch == 10) // if done ...
			break;           //  stop reading chars!
		cout << '*';  // dump * to screen
		passwd += ch;   // addd char to password
	}
	cin.sync(); // flush anything else in the buffer (remaining newline)
	cout << endl;  // simulate the enter that the user pressed

	return passwd;
}
//Completes next step after an 'a' is entered to add data
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
		sendQuery("insert into games values(\"" + vtName + "\"," + to_string(vtScore) +
			",\"" + htName + "\"," + to_string(htScore) + ")");
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
void updateRecordTable() {
		//Count/calculate the wins/losses/pf/pa/wp and pd for each team 
	string teamList[100];
	string teamName = "";
	int i = 0;
		//Get all of the records from the records table
	sendQuery("select * from teams");
	res = mysql_store_result(conn);
	// go through each line (row) of the answer table
	for (row = mysql_fetch_row(res);
		row != NULL;
		row = mysql_fetch_row(res))
	{
		//Update records table
		teamName = row[1];
			//Update wins and losses
		sendQuery("update records set w = (select count(vt_name) from games where vt_name = \"" + teamName
			+ "\" && vt_score > ht_score), w = w+(select count(vt_name) from games where ht_name = \"" + teamName
			+ "\" && ht_score > vt_score), l = (select count(vt_name) from games where vt_name = \"" + teamName
			+ "\" && vt_score < ht_score), l = l+(select count(vt_name) from games where ht_name = \"" + teamName
			+ "\" && ht_score < vt_score) where t_name = \"" + teamName + "\"");
			//Update points for points against, winning percentage and point differential
		sendQuery("update records set pf = (select coalesce(sum(vt_score),0) from games where vt_name = \"" + teamName
			+ "\"), pf = pf+(select coalesce(sum(ht_score),0) from games where ht_name = \"" + teamName
			+ "\"), pa = (select coalesce(sum(ht_score),0) from games where vt_name = \"" + teamName
			+ "\"), pa = pa+(select coalesce(sum(vt_score),0) from games where ht_name = \"" + teamName 
			+ "\"), wp = w/(w+l), pd = pf/pa where t_name = \"" + teamName + "\"");
	}
	// clean up the query
	mysql_free_result(res);	
}
void standingsDialog() {
	updateRecordTable();
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
		//update the record table
	updateRecordTable();
	string tName = "";
	cin >> tName;
	//Print the record of the entered team
	sendQuery("select * from records where t_name = \""+tName+"\"");
	// get the query result(s)
	res = mysql_store_result(conn);
	//Print header
	cout << "=Team=============W===L===PF===PA" << endl; //Print data label
	int rowCount = 0;
		 // go through each line (row) of the answer table
	for (row = mysql_fetch_row(res);
		row != NULL;
		row = mysql_fetch_row(res))
	{
		// print out the colums
		cout << " " << left << setw(17) << row[0] << setw(4) << row[1] << setw(4) << row[2]
			<< setw(5) << row[3] << setw(5) << row[4] << endl;
		rowCount++;
	}
	if (rowCount < 1) {
		cout << "Sorry team name: \"" + tName + "\" not found in database";
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
		//Update the city code on the city table
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
	cin >> db_host;
	cout << "Enter MySQL database username:";
	cin >> db_user;
	cout << "Enter MySQL database password:";
	db_password = myget_passwd();
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