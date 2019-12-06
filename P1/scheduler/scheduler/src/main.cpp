#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>

using namespace std;


enum class Action { arrive=0, finish=1 }; // Action Enum for getting check times for the output as Arrive or Finish

class Instruction {  // Instruction for calculating the exact timings of the avaliable state

public:
    string name;
    int exec_time;
    Instruction(string name, int exec_time) {
        this->name = name;
        this->exec_time = exec_time;
    }
};


class Process { //Process Class for hold the values and calculate Turnaround time etc.
public:
    string name;
    int priority;
    string code_file;
    int arrival_time;

    vector<Instruction*> instructions;

    int next_instruction_index = 0;
    int remain_exec_time = 0;
    int total_exec_time = 0;
    int proc_timer;
    int finish_time;

    Process(string name, int priority, string code_file, int arrival_time) {
        this->name = name;
        this->priority = priority;
        this->code_file = code_file;
        this->arrival_time = arrival_time;
    }

};


class Event { // Event Class for which events are being made on the CPU

public:
    Action action;
    int event_time;
    Process* process;

    Event(int event_time, Action action, Process* process) {
        this->event_time = event_time;
        this->action = action;
        this->process = process;
    }
};


class Cpu { // Cpu Class for hold the current process and get the exact check times


public:

    Process* current_process;
    bool available;

    Cpu() {
        current_process = NULL;
        available = true;
    }

    void set_process(int time, Process* process) {
        this->current_process = process;
        this->available = false;
        process->proc_timer = time;
    }

};


struct CompareTime { //Time Checker Struct
    bool operator()(const Event e1, const Event e2) {
        if(e1.event_time == e2.event_time) {
            return (static_cast<int>(e1.action) > static_cast<int>(e2.action));
        }
        return e1.event_time > e2.event_time;
    }
};

struct ComparePriority { // Priority Checker Struct
    bool operator()(const Process* p1, const Process* p2) {
        return p1->priority > p2->priority;
    }
};


priority_queue<Event, vector<Event>, CompareTime > event_times; // Arrival time min heap
priority_queue<Process*, vector<Process*>, ComparePriority > ready_queue; // Priority max heap


vector<Process*> process_list;
Cpu cpu = Cpu();


void print_state(int time) { // Function for printing the standard output
    string s = "";
    s += to_string(time) + ":HEAD-";

    if(!cpu.available) {
        s += cpu.current_process->name + "[" + to_string(cpu.current_process->next_instruction_index+1) + "]-";
    } else {
        s += "-";
    }

    priority_queue<Process*, vector<Process*>, ComparePriority > pq = ready_queue;


    if(!pq.empty()) {
        for (int i = 0; i < ready_queue.size(); i++) {
            Process *process = pq.top();
            pq.pop();
            s += process->name + "[" + to_string(process->next_instruction_index+1) + "]-";
        }
    }
    s += "TAIL\n";
    cout << s;
}



string print_file(int time) { // Function for printing the output
    string s = "";
    s += to_string(time) + ":HEAD-";

    if(!cpu.available) {
        s += cpu.current_process->name + "[" + to_string(cpu.current_process->next_instruction_index+1) + "]-";
    } else {
        s += "-";
    }

    priority_queue<Process*, vector<Process*>, ComparePriority > pq = ready_queue;


    if(!pq.empty()) {
        for (int i = 0; i < ready_queue.size(); i++) {
            Process *process = pq.top();
            pq.pop();
            s += process->name + "[" + to_string(process->next_instruction_index+1) + "]-";
        }
    }
    s += "TAIL\n";
    return s;
}




int main() {

    ofstream myfile;
    myfile.open ("../output.txt");

    ifstream infile("../definition.txt", ios::in);
    string line;
    while (getline(infile, line)) { // While for reading the input
        istringstream iss(line);
        string x,y;
        int a, b;
        if (!(iss >> x >> a >> y >> b)) { break; }
        Process* process = new Process(x, a, y, b);
        process_list.push_back(process);
        event_times.push(Event(process->arrival_time, Action::arrive, process));
    }
    infile.close();


    for (int i = 0; i < process_list.size(); ++i) { //While for reading the input for processes
        Process* process = process_list[i];
        ifstream infile("../" + process->code_file + ".txt", ios::in);
        string line;
        while (getline(infile, line)) {
            istringstream iss(line);
            string x;
            int a;
            if (!(iss >> x >> a)) { break; }
            Instruction* instruction = new Instruction(x, a);
            process->instructions.push_back(instruction);
            process->remain_exec_time += instruction->exec_time;
        }
        process->total_exec_time = process->remain_exec_time;
        infile.close();
    }


    int finished_process_count = 0;
    int time = 0;
    print_state(time);
    myfile << print_file(time);
    while(!event_times.empty()) { // While continues to loop until all events are stopped
        Event event = event_times.top();
        event_times.pop();
        time = event.event_time;
        Process* next_process = event.process;
        Action action = event.action;

        //cout << time << " " << to_string(static_cast<int>(action)) << " " << next_process->name << endl;

        if(action == Action::arrive) {

            if(cpu.available) { // If cpu is avaliable
                cpu.set_process(time, next_process);
                print_state(time);
                myfile << print_file(time);
            } else {
                Process* curr_process = cpu.current_process;

                if(next_process->priority < curr_process->priority) { // If a higher priority process comes
                    vector<int> arr;
                    arr.push_back(curr_process->proc_timer);
                    for(int i=curr_process->next_instruction_index, j=1; i<curr_process->instructions.size(); i++, j++) {
                        arr.push_back(curr_process->instructions[i]->exec_time + arr[j-1]);
                    }


                    int last_instruction_idx = lower_bound(arr.begin(), arr.end(), time) - arr.begin();
                    curr_process->next_instruction_index += last_instruction_idx;
                    curr_process->remain_exec_time -= arr[last_instruction_idx] - arr[0];

                    //cout << last_instruction_idx << " " << arr[last_instruction_idx] << " " << next_process->name << endl;
                    ready_queue.push(curr_process);
                    cpu.set_process(arr[last_instruction_idx], next_process);
                    event_times.push(Event(next_process->proc_timer+next_process->remain_exec_time, Action::finish, next_process));
                    print_state(arr[last_instruction_idx]);
                    myfile << print_file(arr[last_instruction_idx]);
                } else { // If a lower priority process comes
                    vector<int> arr;
                    arr.push_back(curr_process->proc_timer);
                    for(int i=curr_process->next_instruction_index, j=1; i<curr_process->instructions.size(); i++, j++) {
                        arr.push_back(curr_process->instructions[i]->exec_time + arr[j-1]);
                    }


                    int last_instruction_idx = lower_bound(arr.begin(), arr.end(), time) - arr.begin();
                    //cout << last_instruction_idx << " " << arr[last_instruction_idx] << " " << next_process->name << endl;


                    curr_process->next_instruction_index += last_instruction_idx;
                    curr_process->remain_exec_time -= arr[last_instruction_idx] - arr[0];
                    ready_queue.push(next_process);
                    print_state(arr[last_instruction_idx]);
                    myfile << print_file(arr[last_instruction_idx]);
                }
            }
        } else { // I a process is finished

            Process* curr_process = cpu.current_process;
            if(curr_process->name != next_process->name) { continue; }

            curr_process->finish_time = time;

            if(!ready_queue.empty()) {
                Process *ready_process = ready_queue.top();
                ready_queue.pop();
                cpu.set_process(time, ready_process);
                event_times.push(Event(ready_process->proc_timer+ready_process->remain_exec_time, Action::finish, ready_process));
                print_state(time);
                myfile << print_file(time);
            } else {
                cpu.current_process = NULL;
                cpu.available = true;
            }


            finished_process_count++;
            if(finished_process_count == process_list.size()) {
                print_state(time);
                myfile << print_file(time);
                break;
            }

        }


    }

    cout << endl;


    for(int i=0; i<process_list.size(); i++) { // Calculaion of the Turnaround Time and Waiting time
        Process* process = process_list[i];
        cout << "Turnaround time for " << process->name << " = " << process->finish_time-process->arrival_time << " ms" << endl;
        cout << "Waiting time for " << process->name << " = " << process->finish_time-process->arrival_time-process->total_exec_time << endl;
        myfile << "" << endl;
        myfile << "Turnaround time for " << process->name << " = " << process->finish_time-process->arrival_time << " ms" << endl;
        myfile << "Waiting time for " << process->name << " = " << process->finish_time-process->arrival_time-process->total_exec_time << endl;
    }



    myfile.close();

    return 0;
}



	