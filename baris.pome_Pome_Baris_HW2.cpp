//Barış Pome - 31311 - CS204-23 Fall - A-2 Section - Homework 2
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
int flight_id = 0;
struct flight{
    string from;
    string to;
    int hour;
    int min;
    int price;
    int ID;
    flight *next;
    flight *prev;
    //the constructor which is used for the read_files function
    flight (string from, string to, int hour, int min, int price) : from(from), to(to), hour (hour),min(min),price(price), ID(0), next(nullptr), prev(nullptr) {
        ID = 0;
        next = nullptr;
        prev = nullptr;
    }
    flight (string from, string to, int hour, int min, int price, int ID) : from(from), to(to), hour (hour),min(min),price(price), ID(ID), next(nullptr), prev(nullptr) { //the constructor which is used for the add_flight_with_input function
        next = nullptr;
        prev = nullptr;
    }
};
struct airline{
    string name;
    int ID;
    airline *next;
    flight *flights;

    airline (string name, int ID) : name(name), ID(ID), next(nullptr), flights(nullptr){ //the constructor which is used for the read_files function
        next = nullptr;
        flights = nullptr;
    }
};
pair<vector<string>, vector<vector<flight>>> read_files(bool input_done){
    vector<string> airlines;
    vector<vector<flight>> flights(100); //start with a previously sized vector or you get seg fault (for sure)
    while(not input_done){
        string airline_name;
        string filename;
        cout << "Enter the name of airline: ";
        cin >> airline_name;
        cout << "Enter the name of airline file: ";
        cin >> filename;
        ifstream input(filename);
        if(filename == "done"){
            input_done = true;
            return make_pair(airlines, flights);
        }
        while( input.fail() ){
            cout << "Could not open file: " << filename << endl;
            cout << "Enter the name of airline: ";
            cin >> airline_name;
            cout << "Enter the name of airline file: ";
            cin >> filename;
            input.open(filename);
            if(filename == "done"){
                input_done = true;
                return make_pair(airlines, flights);
            }
        }
        airlines.push_back(airline_name);
        cout << "Processing input file: " << filename <<endl;
        int line_no = 0;
        vector<string> lines;
        string line;
        int vector_location = airlines.size() - 1;
        while(getline(input, line)){
            lines.push_back(line);
            line_no += 1;
            if(line_no % 5 == 0){
                flight a_flight(lines[line_no-5], lines[line_no-4], stoi(lines[line_no-3]), stoi(lines[line_no-2]), stoi(lines[line_no-1]));
                flights[vector_location].push_back(a_flight);
            }
        }
    }
    if(input_done){
        cout << "Input files are already read, however you can do manual changes.." << endl;
    }
    return make_pair(airlines, flights);
}

void insert_sorted(airline*& airline_current, flight*& flight_current, flight*& new_flight) { //to insert the new flight to the correct place
    if ((new_flight->hour < flight_current->hour) || ((new_flight->hour == flight_current->hour) && (new_flight->min < flight_current->min))) { //if it must be the first element
        new_flight->next = flight_current; //to connect the new flight to the next flight
        new_flight->prev = nullptr; //to connect the new flight to nullptr because it is the first element
        flight_current->prev = new_flight; //to connect the next flight to the new flight
        flight_current = new_flight; //to make the new flight the first element
        airline_current->flights = flight_current; //to connect the airline to the first element
    }
    else { //if it is in the middle or last
        while ((flight_current->next != nullptr) && ((new_flight->hour > flight_current->hour) || (new_flight->hour == flight_current->hour && new_flight->min > flight_current->min))) {  // to find the correct place and to make it points to the previous flight that the new flight will be connected after that
            flight_current = flight_current->next;
        }
        if (flight_current->next == nullptr && ((new_flight->hour > flight_current->hour) || (new_flight->hour == flight_current->hour && new_flight->min > flight_current->min))) { //if it must be the last element of the flights of airline
            flight_current->next = new_flight; //to connect the previous flight to the new flight
            new_flight->prev = flight_current; //to connect the new flight to the previous flight
            new_flight->next = nullptr; //to connect the new flight to nullptr because it is the last element
            while (flight_current->prev != nullptr) {  //to points to the first element of connected flights because of the remaining flights that must be sorted of the airline
                flight_current = flight_current->prev;
            }
            airline_current->flights = flight_current; //to connect the airline to the first element
        }
        else { //if it is in the middle of the flights of airline
            new_flight->next = flight_current; //to connect the new flight to the next flight
            new_flight->prev = flight_current->prev; //to connect the new flight to the previous flight
            flight_current->prev = new_flight;  //to connect the current flight to the new flight
            new_flight->prev->next = new_flight; //to connect the previous flight to the new flight
            while (flight_current->prev != nullptr) { //to points to the first element of connected flights because of the remaining flights that must be sorted of the airline
                flight_current = flight_current->prev;
            }
            airline_current->flights = flight_current; //to connect the airline to the first element
        }
    }

}
airline* make_linked_list_structure(vector<string> airlines, vector<vector<flight>> flights, int& flight_id){ // for making airlines basic linked list and flights linked doubly list
    airline* airline_head = new airline(airlines[0], 0); //to create the first airline
    airline* airline_current = airline_head; //to make the first airline the current airline
    for(int i = 1; i < airlines.size(); i++){ //to create the remaining airlines linked list
        airline* new_airline = new airline(airlines[i], i);
        airline_current->next = new_airline; //to connect the previous airline to the new airline
        new_airline->next = nullptr; //to connect the new airline to nullptr because it is the last airline
        airline_current = new_airline; //to make the new airline the current airline
    }
    for (int i = 0; i < airlines.size(); i++) { //to create the flights linked list
        airline* airline_current = airline_head;
        while(airline_current->name != airlines[i]){ //to find the correct airline
            airline_current = airline_current->next;
        }
        flight* flight_current = airline_current->flights;
        for (int j = 0; j < flights[i].size(); j++) {
            flight* new_flight = new flight(flights[i][j].from, flights[i][j].to, flights[i][j].hour, flights[i][j].min, flights[i][j].price, flight_id);
            if (airline_current->flights == nullptr){ //if there is no flight in the airline before that
                airline_current->flights = new_flight;  //to connect the airline to the first element
                new_flight->prev = nullptr; //to connect the new flight to nullptr because it is the first element
                new_flight->next = nullptr; //to connect the new flight to nullptr because it is the last element
                flight_current = new_flight; //to make the new flight the first element
            }
            else { //if there is a flight in the airline before that algorithm to put it correct place by looking time and sorting it by time
                insert_sorted(airline_current, flight_current, new_flight); //to insert the new flight to the correct place
            }
            flight_id += 1;
        }
    }
    return airline_head;
}
void add_flight_with_input(airline* head){
    cout << "Adding manual entry:" << endl;
    string from , airline_name, to;
    int hour, min, price;
    cout << "AIRLINE: ";
    cin >> airline_name;
    cout << "FROM: ";
    cin >> from;
    cout << "TO: ";
    cin >> to;
    cout << "HOUR: ";
    cin >> hour;
    cout << "MIN: ";
    cin >> min;
    cout << "PRICE: ";
    cin >> price;  //to get the input from the user
    airline* airline_current = head;
    while (airline_current->next != nullptr){
        if (airline_current->name == airline_name && airline_current->flights != nullptr){ //if there is a flight in the airline before that algorithm to put it correct place by looking time and sorting it by time
            flight* flight_current = airline_current->flights; //to make the first flight the current flight
            flight* new_flight = new flight(from, to, hour, min, price); //to create a new flight
            new_flight->ID = flight_id;
            insert_sorted(airline_current, flight_current, new_flight); //to insert the new flight to the correct place
            break;
        }
        airline_current = airline_current->next; //to pass the next airline if the airline is not the airline that we are looking for
    }
    if (airline_current->next == nullptr){ //if the airline is the last airline and if there is no flight in the airline before that
        flight* new_flight = new flight(from, to, hour, min, price);
        new_flight->ID = flight_id;
        airline* new_airline = new airline(airline_name, airline_current->ID+1); //to create a new airline
        airline_current->next = new_airline;
        new_airline->next = nullptr; //to connect the new airline to nullptr because it is the last airline
        airline_current = new_airline;
        airline_current->flights = new_flight; //to connect the airline to the first element
        new_flight->prev = nullptr; //to connect the new flight to nullptr because it is the first element
        new_flight->next = nullptr; //to connect the new flight to nullptr because it is the last element
    }
    cout << "Flight ID " << flight_id << " is added to the list.." << endl;
    flight_id += 1;
}
void print_all(airline* head){
    if (head == nullptr){ //if the list is empty (in sample runs if before reading files option 2 is selected or after deleting data option 0 is selected)
        cout << "List is empty.." << endl;
        return;
    }

    airline* airline_temp = head;
    while (airline_temp){ //to print all airlines and flights
        if (airline_temp->flights != nullptr) { //if there is no flight in the airline sample runs will not print the airline it passes to the next airline
            cout << "###################################" << endl;
            cout << "### AIRLINE ID: " << airline_temp->ID << " ###" << endl;
            cout << "NAME:" << airline_temp->name << endl;
            cout << "FLIGHTS: ";
            flight* flight_temp = airline_temp->flights;
            while (flight_temp){ //to print all flights of the airline
                cout << "#[" << flight_temp->ID << "|" << flight_temp->from << "->" << flight_temp->to << "|" << flight_temp->hour << ":" << flight_temp->min << "|" << flight_temp->price << "TRY]#";
                flight_temp = flight_temp->next;
            }
            cout << endl;
        }
        airline_temp = airline_temp->next; //to pass the next airline
    }
}
void delete_linked_list(airline* &head){
    while (head){ //to delete the all airlines
        airline* temp = head; //to delete the airline without losting the head
        head = head->next;
        while (temp->flights){ //to delete the all flights of the airline
            flight* temp_flight = temp->flights;
            temp->flights = temp->flights->next;
            delete temp_flight;
        }
        delete temp;
    }
}
void remove_flight_with_input(airline* &head) {
    if (head == nullptr) {
        cout << "The list is empty." << endl;
        return;
    }
    int flight_number;
    cout << "Flight id to remove: ";
    cin >> flight_number;
    airline* airline_current = head;
    while (airline_current) {
        flight* flight_current = airline_current->flights;
        flight* prev_flight = nullptr;
        while (flight_current) {
            if (flight_current->ID == flight_number) { //if the flight is found
                if (prev_flight == nullptr) { // if it is the first element
                    if (flight_current->next) {
                        flight_current->next->prev = nullptr;
                    }
                    airline_current->flights = flight_current->next;
                } else if (flight_current->next == nullptr) { // if it is the last element
                    prev_flight->next = nullptr;
                } else { // if it is in the middle
                    prev_flight->next = flight_current->next;
                    flight_current->next->prev = prev_flight;
                }
                delete flight_current;
                cout << "Flight ID " << flight_number << " is removed from the list.." << endl;
                return;
            }
            prev_flight = flight_current; //to pass the next flight
            flight_current = flight_current->next; //to pass the next flight
        }
        airline_current = airline_current->next; //to pass the next airline
    }
    cout << "Flight ID " << flight_number << " not found in the list." << endl;
}

int pathfinder(airline* head, const string& from, const string& to, int max_transfers, vector<flight>& current_path, vector<flight>& cheapest_path, int& min_total) { //to find the cheapest path
    if (max_transfers < 0) { //if the max transfers is less than 0 it means that there is no path between the from and to
        return INT_MAX;
    }
    int local_min_total = INT_MAX;
    for (airline* airline_current = head; airline_current != nullptr; airline_current = airline_current->next) { //to find the correct airline it is sth like a search algorithm while (airline_current)
        for (flight* flight_current = airline_current->flights; flight_current != nullptr; flight_current = flight_current->next) { //to find the correct flight it is sth like a search algorithm while (flight_current)
            if (flight_current->from == from) { //if the flight is from the from
                current_path.push_back(*flight_current); //to add the flight to the current path
                if (flight_current->to == to) { //if the flight is to the to
                    int path_current_price = 0; //to calculate the total price of the current path
                    for (const flight& f : current_path) {
                        path_current_price += f.price;
                    }
                    if (path_current_price < min_total) {
                        min_total = path_current_price; //to update the min total price
                        cheapest_path = current_path; //to update the cheapest path
                    }
                    local_min_total = min(local_min_total, path_current_price); //to update the local min total price
                }
                else {
                    int remaining_path_price = pathfinder(head, flight_current->to, to, max_transfers - 1, current_path, cheapest_path, min_total); //to find the remaining path price assuming new from is the to of the current flight
                    local_min_total = min(local_min_total, remaining_path_price); //to update the local min total price
                }
                current_path.pop_back(); //to delete the flight from the current path
            }
        }
    }
    return local_min_total; //to return the local min total price
}
void pathfinder_assistant(airline* head) { //to get the input from the user and to call the pathfinder function and to print the cheapest path if there is a path, it was necessary to find the path because it helps to find the cheapest path recursively
    string from, to;
    int max_transfers;
    vector<flight> cheapest_path, current_path;
    int min_total = INT_MAX;
    cout << "Where are you now? " << endl;
    cin >> from;
    cout << "Where do you want to go? "<< endl;
    cin >> to;
    cout << "Maximum number of transfers: "<< endl;
    cin >> max_transfers; //to get the input from the user
    int path_found_price = pathfinder(head, from, to, max_transfers, current_path, cheapest_path, min_total);   //to call the pathfinder function
    if (path_found_price != INT_MAX) { //if there is a path
        cout << "##Best price path##\n";
        for (const auto& f : cheapest_path) {
            cout << "[" << f.ID << "|" << f.from << "->" << f.to << "|" << f.hour << ":" << f.min << "|" << f.price;
        }
        cout << " $TOTAL PRICE: " << path_found_price << endl;
    }
    else { //if there is no path
        cout << "No path found between " << from << " and " << to << endl;
    }
}
//void printMainMenu() {
    //cout << endl;
    //cout <<"I***********************************************I"<<endl
         //<<"I               0 - DELETE DATA                 I"<<endl
         //<<"I               1 - READ FILES                  I"<<endl
         //<<"I               2 - PRINT ALL FLIGHTS           I"<<endl
         //<<"I               3 - ADD FLIGHT                  I"<<endl
         //<<"I               4 - REMOVE FLIGHT               I"<<endl
         //<<"I               5 - PATH FINDER                 I"<<endl
         //<<"I               6 - EXIT                        I"<<endl
         //<<"I***********************************************I"<<endl
         //<<">>";
    //cout << endl;
//}
void processMainMenu() {
    pair<vector<string>, vector<vector<flight>>> lines_flights;
    airline *head;
    char input;
    bool input_done = false;
    do {
        //printMainMenu();
        cout << ">>"<< endl;
        cout << "Please enter your option " << endl;
        cin >> input;
        switch (input) {
            case '0':
                delete_linked_list(head);
                cout << "Data is destroyed.." << endl;
                input_done = false;
                break;
            case '1':
                if (not input_done) {
                    lines_flights = read_files(input_done);
                    head = make_linked_list_structure(lines_flights.first, lines_flights.second, flight_id);
                    cout << "Files have read.." << endl;
                } else {
                    cout << "Files are already read.." << endl;
                }
                input_done = true;
                break;
            case '2':
                print_all(head);
                break;
            case '3':
                add_flight_with_input(head);
                break;
            case '4':
                remove_flight_with_input(head);
                break;
            case '5':
                pathfinder_assistant(head);
                break;
            case '6':
                cout << "Exiting.." << endl;
                exit(0);
            default:
                cout << "Invalid option: please enter again" << endl;
        }
    } while (true);
}
int main() {
    processMainMenu();
    return 0;
}

