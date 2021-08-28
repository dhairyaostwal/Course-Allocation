#include<bits/stdc++.h>

using namespace std;

const int MAX_N = 2e3 + 5;
vector<vector<int>> courseGraph(MAX_N); // Graph storing the courses and its connections
int n;
map<string, int> days;
map<string, int> courseInd;
map<string, int> studentsInd;
int interactions;
int studentsCount;

class Course {
public:
    string name;
    int capacity;
    int availability;
    int day;
    int timeStartInMins;
    int timeEndInMins;
    set<string> registeredStudents; // Set is used to keep track of the registered students for a course
    queue<string> waitingList;   // Queue is used for maintaining the waiting list for a particular course
};

class Student {
public:
    string regNo;
    set<int> registeredCourses; // Set is used to keep a list of registered courses
};

void precompute() {
    days["Mon"] = 0, days["Tue"] = 1, days["Wed"] = 2, days["Thu"] = 3;
    days["Fri"] = 4, days["Sat"] = 5, days["Sun"] = 6;
}

vector<int> findTimeOfDayInMins(string timeslot) {
    vector<int> time(2, 0);

    int i = 0, sz = timeslot.size();
    while (i < sz && timeslot[i] != ' ') {
        i++;
    }
    i++;
    string startHour = "";
    while (i < sz && timeslot[i] != ':') {
        startHour += timeslot[i];
        i++;
    }
    i++;
    string startMins = "";
    int startMorning = 1;
    while (i < sz && timeslot[i] >= '0' && timeslot[i] <= '9') {
        startMins += timeslot[i];
        i++;
    }
    if (timeslot[i] == 'p') {
        startMorning = 0;
    }
    time[0] = (stoi(startHour) % 12) * 60 + stoi(startMins);
    if (!startMorning) {
        time[0] += 12 * 60; // Adding 12 hours to the time in "pm" for the startTime
    }
    while (i < sz && timeslot[i] != '-') {
        i++;
    }
    i++;
    string endHour = "";
    while (i < sz && timeslot[i] != ':') {
        endHour += timeslot[i];
        i++;
    }
    i++;
    string endMins = "";
    int isMorning = 1;
    while (i < sz && timeslot[i] >= '0' && timeslot[i] <= '9') {
        endMins += timeslot[i];
        i++;
    }
    if (timeslot[i] == 'p') {
        isMorning = 0;
    }
    time[1] = (stoi(endHour) % 12) * 60 + stoi(endMins);
    if (!isMorning) {
        time[1] += 12 * 60; // Adding 12 hours to the time in "pm" for the endTime
    }

    return time;
}

int main() {
    precompute();
    cout << "* * * * * Course Registration * * * * *\n";
    cout << "Enter the number of courses\n";
    cin >> n;
    vector<Course> courses(n);
    for (int i = 0; i < n; i++) {
        string timeslot;
        cout << "Enter the name of the course " << (i + 1) << " in camelCase(Eg: OperatingSystems)\n";
        string courseName;
        cin >> courseName;
        courses[i].name = courseName;
        courseInd[courses[i].name] = i + 1;
        cout << "Enter the course capacity\n";
        cin >> courses[i].capacity;
        cout << "Enter the available seats\n";
        cin >> courses[i].availability;
        cout << "Enter the timeslot for the course(Eg: \"Monday 4:30pm-5:30pm\")\n";
        cin.ignore();
        getline(cin, timeslot);

        courses[i].day = days[timeslot.substr(0, 3)];

        vector<int> timeInMins = findTimeOfDayInMins(timeslot);
        courses[i].timeStartInMins = timeInMins[0];
        courses[i].timeStartInMins += 24 * courses[i].day;
        courses[i].timeEndInMins = timeInMins[1];
        courses[i].timeEndInMins += 24 * courses[i].day;
        cout << "\n";
    }

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (!(courses[i].timeEndInMins <= courses[j].timeStartInMins ||
                  courses[j].timeEndInMins <= courses[i].timeStartInMins)) {
                courseGraph[i].push_back(j);
                courseGraph[j].push_back(i);
            }
        }
    }
    cout << "Enter the number of students that can participate in the registration\n";
    cin >> studentsCount;
    vector<Student> students(studentsCount);
    for (int i = 0; i < studentsCount; i++) {
        cout << "Enter registration number of student " + i + 1 << "\n";
        cin >> students[i].regNo;
        studentsInd[students[i].regNo] = i + 1;
    }

    cout << "\n";
    cout << "Course Registration begins\n";
    cout << "Enter the number of interactions by the students in order\n";
    cin >> interactions;
    for (int i = 0; i < interactions; i++) {
        string regNo;
        cout << "Enter student's registration number\n";
        cin >> regNo;
        if (studentsInd[regNo] == 0) {
            cout << "This student is not allowed to take part in the registration process\n";
            cout << "\n";
            continue;
        }
        int studentIndex = studentsInd[regNo] - 1;
        cout << "1. Register for a course\n2. Delete a course\n";
        cout << "Enter interaction type:";
        int type;
        cin >> type;
        if (type == 1) {
            cout << "Enter the name of the course in camelCase(Eg: OperatingSystems)\n";
            string name;
            cin >> name;
            if (courseInd[name] == 0) {
                cout << "Course doesn't exist\n";
                cout << "\n";
                continue;
            }
            int courseIndex = courseInd[name] - 1;
            if (courses[courseIndex].registeredStudents.find(regNo) != courses[courseIndex].registeredStudents.end()) {
                cout << "Course is already registered by the student\n";
                cout << "\n";
                continue;
            }
            bool canRegister = true;
            for (auto i: courseGraph[courseIndex]) {
                if (students[studentIndex].registeredCourses.find(i) !=
                    students[studentIndex].registeredCourses.end()) {
                    canRegister = false;
                    break;
                }
            }
            if (!canRegister) {
                cout << "This course clashes with other registered course(s) and hence registration is unsuccessful\n";
                cout << "\n";
                continue;
            }
            if (courses[courseIndex].registeredStudents.size() == courses[courseIndex].availability) {
                cout << "This course has no seats left. The student is added to the waiting list";
                courses[courseIndex].waitingList.push(regNo);
            } else {
                courses[courseIndex].registeredStudents.insert(regNo);
                students[studentIndex].registeredCourses.insert(courseIndex);
                cout << "Course is successfully registered\n";
            }
        } else if (type == 2) {
            cout << "Enter the name of the course in camelCase(Eg: OperatingSystems)\n";
            string name;
            cin >> name;
            if (courseInd[name] == 0) {
                cout << "Course doesn't exist\n";
                cout << "\n";
                continue;
            }
            int courseIndex = courseInd[name] - 1;
            if (courses[courseIndex].registeredStudents.find(regNo) == courses[courseIndex].registeredStudents.end()) {
                cout << "Course is not registered by the student\n";
                cout << "\n";
                continue;
            }
            courses[courseIndex].registeredStudents.erase(courses[courseIndex].registeredStudents.find(regNo));
            students[studentIndex].registeredCourses.erase(students[studentIndex].registeredCourses.find(courseIndex));
            if (courses[courseIndex].registeredStudents.size() ==
                courses[courseIndex].availability - 1) { // Handling of waiting list
                if (courses[courseIndex].waitingList.size() > 0) {
                    courses[courseIndex].registeredStudents.insert(courses[courseIndex].waitingList.front());
                    students[studentsInd[courses[courseIndex].waitingList.front()] - 1].registeredCourses.insert(
                            courseIndex);
                    courses[courseIndex].waitingList.pop();
                }
            }
            cout << "Course is successfully deleted\n";
        } else {
            cout << "Invalid choice by the student\n";
        }
        cout << "\n";
    }

    return 0;
}
