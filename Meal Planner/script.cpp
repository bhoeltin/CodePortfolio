// Elisabetta Caldesi, Anthony DiFalco, Brianna Hoelting, Sophie Lancaster
// DINING HALL MEAL PLANNER
// 5/5/17
// Final Project code
// script.cpp

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <sstream>
#include <cstring>
#include <cctype>

using namespace std;

// struct for each food with name and calories attributes
struct Food {
  string name;
  int calories;
};

// prototypes for the sorting fuctions
bool sort1(Food, Food, vector <string>); // sorts based on preferences
bool sort2(Food, Food, double, int); // sorts based on calories


int main() {

	vector<Food> foodVect; // create vector of Food struct that stores of all the foods
  	string textFile; 
  	vector<string> prefs; // vector that stores preferences of food

  	char meal;
  	string foodFile;
	cout << endl;
  	cout << "Enter Meal Preference (B, L, D): "; // prompt user for meal
  	cin >> meal; // read in meal
	
	// if breakfast is chosen, read in from Breakfast text file
	if (meal == 'B') {
	    foodFile = "SDHbreakfast.txt";

	// if lunch is chosen, read in from Lunch text file
	} else if (meal == 'L') {
	    foodFile = "SDHlunch.txt";
	
	// if dinner is chosen, read in from Dinner text file
	} else if (meal == 'D') {
	    foodFile = "SDHdinner.txt";
	}

	// read in foods in csv format
  	string preference = " ";
  	cout << "Enter comma seperated list: " << endl;
  	cin >> preference;
  
	// parse the foods input by user by comma
  	istringstream iss(preference);
  	string token;

  	while (getline(iss, token, ',')) {
    		prefs.push_back(token); // push foods entered into the preferences vector
  	}

	// prompt user for calories wanted per meal
	double cals;
  	cout << "Enter the Number of Calories for Meal: ";
  	cin >> cals;

	// open text file containing foods and calories
  	string line;
  	fstream myfile (foodFile);
  	if (myfile.is_open()) { // check if file could be opened
    		while (getline(myfile, line)) { // read each line from text file
     			Food food;
	      		istringstream iss(line); // parse the line so that name and calories are stored into the struct food
	      		string token;
      			while (getline(iss, token, '!')) {
        			if (isalpha(token[0])) {
          				food.name = token; // set name of food in the food struct
        			} else {
          				food.calories = stoi(token); // set calories of food in food struct
        			}
      			}
      			foodVect.push_back(food); // push back each food into the vector storing all of the food structs
    		}
    		myfile.close(); // close file
  	} else {
		cout << "Unable to open file" << endl; // if file couldn't be opened
	}


	// sort foods by preferences
  	Food key;
    	int i, j;
	// go through each food struct
    	for (i = 1; i < foodVect.size(); i++) {
       		key = foodVect[i];
       		j = i-1;
       		while (j >= 0 && sort1(foodVect[j],key, prefs)) { // sort the food vector based on calories
           		foodVect[j+1] = foodVect[j];
           		j = j-1;
       		}
       		foodVect[j+1] = key; // most relevant foods will be at the bottom of the vector
    	}

	bool match = true;
	vector<Food> foodMatch; // vector that stores all of the food matches
	while (match) {
		// go through the vector containing all of the food
  		for (int i = 0; i < foodVect.size(); i++) {
			// go through the vector containing the preferences
    			for (int j = 0; j < prefs.size(); j++) {
      				string comp = prefs[j];

      				for (int k = 0; k < foodVect[i].name.length(); k++) {
        				foodVect[i].name[k] = tolower(foodVect[i].name[k]); // convert every name in the food structs contained in food vector to lowercase
      				}

      				if (strstr(foodVect[i].name.c_str(), comp.c_str())) { // check if the food in preference matches any food in the food vector
        				if (foodMatch.size() == 0){
          					foodMatch.push_back(foodVect[i]); // fill in the vector containing matches with the element from the food vector
        				}else{ // if the size of food Match isn't 0
          					bool flag = true;
          					for (int e = 0; e < foodMatch.size(); e++){ // go through the vector of matches
              						if (strcmp(foodMatch[e].name.c_str(), foodVect[i].name.c_str()) == 0){ // checks if that food is already in the match vector 
                						flag = false; // sets flag to false
              						}
          					}
          					if (flag) foodMatch.push_back(foodVect[i]); // only pushes the specific food in the match vector if the flag is true, so all of the
											    // elements of foodMatch are unique
        				}
      				}
    			}
  		}
  		if (foodMatch.size() != 0) {
    			// exit while loop
			match = false;
  		} else {
			// re ask user to input preferences if no matches were found
    			cout << "No matches were found... Please enter new preferences: ";
			cin >> preference;
			// parse in preferences
			istringstream iss(preference);
			string token;
			while (getline(iss, token, ',')) {
      				prefs.push_back(token); // fill in preferences vector
    			}
  		}
	}

	// sort foods by calories
   	for (i = 1; i < foodMatch.size(); i++) {
       		key = foodMatch[i];
       		j = i-1;
       		while (j >= 0 && sort2(foodMatch[j],key, cals, prefs.size())) {
           		foodMatch[j+1] = foodMatch[j];
           		j = j-1;
       		}
       		foodMatch[j+1] = key; // most relevant foods are at the bottom of the array
   	}


  	vector<Food> menu1; // set vector that contains the recommended foods
   
   	for (int i = foodMatch.size()-1; i > -1; i--){
     		if(menu1.size() == prefs.size()) break;
			// if the size of the menu1 vector is 0, fill menu with the first element of food Match 
     			if (menu1.size() == 0){
       				menu1.push_back(foodMatch[i]);
       				i--;
     			}
     			bool found = false;
     			if(menu1.size() != prefs.size()){
				// go through preferences vector
       				for(int k = 0; k <prefs.size(); k++){
					// check if the preferences match any food in the food vector
	 				if(strstr(foodMatch[i].name.c_str(), prefs[k].c_str())){
						// go through menu vector
	   					for (int e = 0; e < menu1.size(); e++){
							// if that food is already in the menu vector, set found equal to true
	     						if (strstr(menu1[e].name.c_str(), prefs[k].c_str())){
	       							found = true;
	     						}
	   					}
	 				}
       				}
				// if found is false, then add food to the menu vector
       				if(!found)  menu1.push_back(foodMatch[i]);
     			}
   	}

	
	cout << endl;
   	int totCals = 0;

   	for (int i = menu1.size() - 1; i > -1; i--) {
        	totCals += menu1[i].calories; // find total calories of menu 
   	}
	// if the total calories are less than the number of calories desired 
   	if (totCals < cals){
	   	cout << totCals << "!" << "nope" << endl; // not over calorie count
   	} else{
	   	cout << totCals << "!" << "over" << endl; // over calorie count
   	}

	// print menu
   	for (int i = menu1.size() - 1; i > -1; i--) {
    		cout << menu1[i].name << "!" << menu1[i].calories << endl;
   	}
  	
	return 0;
}



// function that sorts foods based on preferences
bool sort1(Food a, Food b, vector <string> prefs){
	string comp;
	int count1 = 0;
  	int count2 = 0;
	// go through preference vector
	for (int i = 0; i < prefs.size(); i++){
    		comp = prefs[i];
    		for (int j = 0; j < a.name.length(); j++) {
      			a.name[j] = tolower(a.name[j]); // convert food name to lowercase
    		}
		// if the preferece is found in the name of the food add to count1
		if (strstr(a.name.c_str(), comp.c_str())){
			count1++; 
		}
    		for (int k = 0; k < b.name.length(); k++) {
      			b.name[k] = tolower(b.name[k]); // convert food name to lowercase
    		}
		// if the preferece is found in the name of the food add to count2
		if (strstr(b.name.c_str(), comp.c_str())){
			count2++;
		}
	}

	if (count1 > count2){
		return true;
	}else{
		return false;
	}
}




// function that sorts foods based on calories
bool sort2(Food a, Food b, double cals, int n){
	cals = cals/n; // divide calories by number of preferences
  	int count1 = 0;
  	int count2 = 0;
	int calsUpper = cals + 50; // set boundaries
	int calsLower = cals - 100;

	// increase count1 if the calories of the food are less than calsUpper but higher than calsLower
	if (a.calories <= calsUpper && a.calories >= calsLower){
    		count1++;
	}
	
	// increase count2 if the calories of the food are higher than calsUpper but lower than calsLower
	if (b.calories <= calsUpper && b.calories >= calsLower){
		count2++;
	}

	if (count1 > count2){
		return true;
	}else{
		return false;
	}
}
