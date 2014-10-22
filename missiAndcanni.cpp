#include <iostream>

#include <mutex>

#include <fstream>

#include <string>

#include <vector>

#include <fstream>

#include <sstream>

#include <condition_variable>





#include <pthread.h>



using namespace std;



int counter = 0;

int numPeople = 0;

const int capacity = 3;

int numMissi = 0;

int numCanni = 0;

int boardMissi = 0;

int boardCanni = 0;

int totPeople = 0;



string names[3];

int ids[3];


// The mutex which will control the threads to allow one at a time. 
// These mutex are used whenever we check for a variable, change a variable, and use a function such as print.
pthread_mutex_t test = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t check = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t signal_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t release_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t increment_mutex;// = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t equals_mutex = PTHREAD_MUTEX_INITIALIZER;



void* OnePerson(void* ptr);

void rowBoat();

void MissiArrives(string type, int id);

void CanniArrives(string type, int id);


// Function that splits a string with multiple words in it and seperates it into pieces and store into vector.
vector<string> split(const string& s, const string& pat){

vector<string> answer;

int pos2;

for (unsigned int pos = 0;; pos = pos2 + pat.size()) {

pos2 = s.find(pat, pos);

if (pos2 == -1) {

answer.push_back(s.substr(pos, s.size() + 1 - pos));

break;

}

answer.push_back(s.substr(pos, pos2 - pos));

}

return answer;

}





typedef struct

{

int id;  //id of person

string type;     //0 if missi and 1 if canni

} Arg_Struct;



int main()

{

ifstream MissiAndCanni("/Users/atsuzu/Downloads/input.txt");

string line;


int x = 0;

    vector<Arg_Struct*> ppl_info;

    
//reads in from text, and creates structs of id of person and whether he/she is a cannibal or missionary.
while (getline(MissiAndCanni, line)) {

vector<string> words = split(line, " ");

        Arg_Struct* info = new Arg_Struct;

        info->id = stoi(words[1]);

        info->type = words[0];

ppl_info.push_back(info);

x++;



}

    

    //Arg_Struct* ppl_info2 = new Arg_Struct[x];   //change to vector



    

totPeople = x;

pthread_t* thread = new pthread_t[totPeople];

for (int i = 0; i < x; i++)

pthread_create(thread + i, NULL, OnePerson, (void*)(ppl_info[i]));  //  Basically "Unleashes" the threads.

    

    

for (int i = 0; i < x; i++)

pthread_join(thread[i], NULL);  //  Joins together so the main function won't end before the threads do.

    

    

delete[] thread;

//delete[] ppl_info2;

    

    for(int i = 0; i < totPeople; i++)

    {

        delete ppl_info[i];

    }



    

MissiAndCanni.close();

}


//  Decides whether to put person on boat or wait, depending on if the person is a missionary or cannibal.
void* OnePerson(void* ptr)

{

    //pthread_mutex_lock(&check);

    

    pthread_mutex_lock(&equals_mutex);

Arg_Struct* info;

info = (Arg_Struct*)ptr;

    pthread_mutex_unlock(&equals_mutex);

    

    pthread_mutex_lock(&increment_mutex);

    if(info->type == "Missionary")

    {

        MissiArrives(info->type, info->id);

    }

    if(info->type == "Cannibal")

    {

        CanniArrives(info->type, info->id);

    }

    pthread_mutex_unlock(&increment_mutex);



    

    pthread_mutex_lock(&check);

    while((numPeople >= capacity) || ((boardMissi == 1) && info->type == "Cannibal") || (boardCanni > 1 && info->type == "Missionary"))

    {



        pthread_cond_wait(&condition_cond, &check);

//        pthread_mutex_lock(&print_mutex);

//        cout  << info->type << " " << info->id << " is waiting" << endl;

//        cout << "Number of Cannibals: " << boardCanni << endl;

//        cout << "Number of Missionary: " << boardMissi << endl;

//        cout << "Number of People: " << numPeople << endl;

//        pthread_mutex_unlock(&print_mutex);

        

        //pthread_mutex_lock(&check);

        if(info->type == "Cannibal" && numPeople < capacity && (boardMissi == 0 || boardMissi == 2))

        {

            break;

        }



        if(info->type == "Missionary" && boardCanni <= 1 && numPeople < capacity)

        {

            break;

        }



        //pthread_mutex_unlock(&check);

    }

    pthread_mutex_unlock(&check);

    



    pthread_mutex_lock(&check);

    if(info->type == "Missionary")

        boardMissi++;

    if(info->type == "Cannibal")

        boardCanni++;

    numPeople++;

    pthread_mutex_lock(&print_mutex);

    //cout << "NumPeople : " << numPeople << endl;

    cout << info->type << " " << info->id << " is on the boat" << endl;

    names[numPeople-1] = info->type;

    ids[numPeople-1] = info->id;

    pthread_mutex_unlock(&print_mutex);

    pthread_mutex_unlock(&check);



    //pthread_mutex_lock(&increment_mutex);

    pthread_mutex_lock(&check);

    if((boardMissi + boardCanni) == 3)

    {

        pthread_mutex_lock(&print_mutex);

        //cout << "---" << numPeople << "---" << endl;

        //cout << "----->People released<-----" << endl;

        pthread_mutex_unlock(&print_mutex);

    

        //pthread_mutex_lock(&test);

        pthread_mutex_lock(&release_mutex);

        rowBoat();

        pthread_mutex_unlock(&release_mutex);

        //pthread_mutex_unlock(&test);

    }

    pthread_mutex_unlock(&check);

    

    

    pthread_mutex_lock(&test);

    if(boardCanni <= 1)

    {

        pthread_cond_broadcast(&condition_cond);

        pthread_mutex_lock(&print_mutex);

        //cout << "check" << endl;

        pthread_mutex_unlock(&print_mutex);

    }

    pthread_mutex_unlock(&test);

    

    //pthread_mutex_unlock(&increment_mutex);



    //pthread_mutex_unlock(&test);

    //pthread_mutex_unlock(&check);

    return NULL;

}



void MissiArrives(string type, int id)

{

    pthread_mutex_lock(&print_mutex);

    cout << "<Arrived> Person type is " << type << " ID is " << id << endl;

    pthread_mutex_unlock(&print_mutex);

    

    

    pthread_mutex_lock(&increment_mutex);

    numMissi++;

    pthread_mutex_unlock(&increment_mutex);

}



void CanniArrives(string type, int id)

{

    pthread_mutex_lock(&print_mutex);

    cout << "<Arrived> Person type is " << type << " ID is " << id << endl;

    pthread_mutex_unlock(&print_mutex);

    

    pthread_mutex_lock(&increment_mutex);

    numCanni++;

    pthread_mutex_unlock(&increment_mutex);

}


//  Carries the boat over to the other side onced the boat has 3 people on it
void rowBoat()

{



    //pthread_mutex_lock(&test);

    pthread_mutex_lock(&print_mutex);

    //cout << "---" << numPeople << "---" << endl;

    cout << "----->People released<-----" << endl;

    pthread_mutex_unlock(&print_mutex);

    

    //pthread_mutex_lock(&check);

    //pthread_mutex_lock(&release_mutex);

    numPeople = 0;

    numMissi -= boardMissi;

    numCanni -= boardCanni;

    boardMissi = 0;

    boardCanni = 0;

    //pthread_mutex_unlock(&release_mutex);

    //pthread_mutex_unlock(&test);

    

    pthread_mutex_lock(&print_mutex);

    ofstream outfile("/Users/atsuzu/Documents/Project1-1/out.txt", ofstream::app);

    

for(int i = 0; i < 3; i++)

{

cout << names[i] << " " << ids[i] << " ";

outfile << names[i] << " " << ids[i] << " ";

}

    cout << " is leaving" << endl;

    outfile << " is leaving" << endl;

pthread_mutex_unlock(&print_mutex);

    

    //pthread_mutex_unlock(&release_mutex);

    pthread_mutex_lock(&test);

    pthread_cond_broadcast(&condition_cond);

    pthread_mutex_unlock(&test);



    }