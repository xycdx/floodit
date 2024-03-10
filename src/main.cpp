#include <graph.h>
#include <unionfind.h>
#include <solver.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <io.h>
#include <fcntl.h>
using namespace std;

int totalTestCase = 50;
int generate(int n, int k)
{
    int numTestCases = totalTestCase;
    int rows = n;
    int columns = n;
    int numColors = k;

    random_device r;
    seed_seq seed1{r(), r(), r(), r(), r(), r(), r(), r()};
    mt19937 mt(seed1);
    uniform_int_distribution<int> uniform_dist(0, numColors - 1);

    for (int testCase = 1; testCase <= numTestCases; ++testCase)
    {
        ostringstream puzzle;
        puzzle << rows << " " << columns << " " << '\n';
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < columns; ++j)
                puzzle << uniform_dist(mt) << " ";
            puzzle << '\n';
        }
        string filename = "../test/testcase_" + to_string(testCase);
        ofstream file(filename);
        if (file)
        {
            file << puzzle.str();
            file.close();
            cout << "Test case " << testCase << " written to " << filename << endl;
        }
        else
        {
            cerr << "Failed to write test case " << testCase << " to " << filename << endl;
        }
    }
    return 0;
}

void GetAllFileNames(const std::string& path, std::vector<std::string>& filenames) {
    intptr_t handle;
    struct _finddata_t fileinfo;

    handle = _findfirst((path + "\\*").c_str(), &fileinfo);
    if (handle == -1) {
        std::cerr << "No files found in the directory." << std::endl;
        return;
    }

    do {
        if (!(fileinfo.attrib & _A_SUBDIR)) {
            filenames.push_back(path + "/" + fileinfo.name);
        }
    } while (_findnext(handle, &fileinfo) == 0);

    _findclose(handle);
}

atomic<bool> stopTimer(false);
atomic<bool> timeoutOccurred(false);
atomic<int> successCount(0);
atomic<int> stateCount(0);
atomic<double> totalDuration(0.0);

void handle_timeout() {
    cout << "Timeout occurred" << endl;
    timeoutOccurred = true;
}

void run_testcase(int testcase, int row, int col, vector<vector<int>>& data) {
    thread timerThread([](){
        double seconds = 0.1;
        while (true) {
            if(stopTimer) {
                return;
            }
            seconds += 0.1;
            if(seconds >= 180) {
                break;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }

        handle_timeout();
    });
    auto start = chrono::steady_clock::now();
    auto result = solvebyAstar(row, col, data, timeoutOccurred);
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    stopTimer = true;
    timerThread.join();
    for(int i:get<0>(result)) {
        cout<<i<<" ";
    }
    cout<<endl;

    if (timeoutOccurred) {
        cout << "Execution timeout" << endl;
    } else {
        cout << "Execution time: " << duration << " mileseconds" << endl;
        successCount++;
        stateCount = stateCount + get<1>(result);
        totalDuration = totalDuration + duration;
    }
}
double test_rate[30][10],test_time[30][10], test_state_time[30][10];
int test_state[30][10];

void test(int n, int k) {
    vector<string> filenames;
    GetAllFileNames("../99problems", filenames);
    stateCount = 0;
    successCount = 0;
    totalDuration = 0.0;
    for(int testcase = 0; testcase < filenames.size(); testcase++) {
        cout << "testcase: " << filenames[testcase] << endl;
        ifstream file(filenames[testcase]);
        if (file.is_open()) {
            int row, col, color;
            file >> row >> col >> color;
            vector<vector<int>> data(row, vector<int>(col, 0));
            for(int i = 0; i < row; i++) {
                for(int j = 0; j < col; j++) {
                    file >> data[i][j];
                }
            }

            stopTimer = false;
            timeoutOccurred = false;
            run_testcase(testcase, row, col, data);

            file.close();
        } else {
            cout << "no such file" << endl;
        }
    }

    test_rate[n][k] = 1.0 * successCount / totalTestCase;
    test_time[n][k] = (successCount == 0 ? -1 : totalDuration / successCount);
    test_state[n][k] = (successCount == 0 ? -1 : stateCount / successCount);
    test_state_time[n][k] = (successCount == 0 ? -1 : totalDuration / stateCount);
}


int main() {
    test(0, 0);
    cout<<test_time[0][0]<<endl;
    // for(int i = 15; i <= 20; i++) {
    //     for(int j = 2; j <= min(6, i * i); j++) {
    //         generate(i, j);
    //         test(i, j);
    //     }
    // }

    // for(int i = 15; i <= 20; i++) {
    //     for(int j = 2; j <= min(6, i * i); j++) {
    //         cout<<test_rate[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }
    // cout<<endl;
    // for(int i = 15; i <= 20; i++) {
    //     for(int j = 2; j <= min(6, i * i); j++) {
    //         cout<<test_time[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }
    // cout<<endl;
    // for(int i = 15; i <= 20; i++) {
    //     for(int j = 2; j <= min(6, i * i); j++) {
    //         cout<<test_state[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }
    // cout<<endl;
    // for(int i = 15; i <= 20; i++) {
    //     for(int j = 2; j <= min(6, i * i); j++) {
    //         cout<<test_state_time[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }
    // cout<<endl;
    // return 0;
}