#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	// TODO: Add your implementation
    cout << "smash: got ctrl-Z" << endl;
    SmallShell& smash = SmallShell::getInstance();
    if(smash.getCurrProcess() != -1) {
        bool temp = smash.getLastCmdInFG();
        Command* command = smash.CreateCommand((smash.getCurrCmd()).c_str());
        if(temp) { // if last command was fg so insert with already given job id
            smash.getJobsList().addJob(command, smash.getCurrProcess(), true, true);
        } else {
            smash.getJobsList().addJob(command, smash.getCurrProcess(), false, true);
        }
        kill(smash.getCurrProcess(), SIGSTOP);
        cout << "smash: process " << smash.getCurrProcess() << " was stopped" << endl;
        smash.setCurrProcess(-1);
        smash.setCurrCmd("");
        delete command;
    }
}

void ctrlCHandler(int sig_num) {
  // TODO: Add your implementation
    cout << "smash: got ctrl-C" << endl;
    SmallShell& smash = SmallShell::getInstance();
    if(smash.getCurrProcess() != -1) {
        int process_num = smash.getCurrProcess();
        smash.setCurrProcess(-1);
        smash.setCurrCmd("");
        kill(process_num, SIGINT);
        cout << "smash: process " << process_num << " was killed" << endl;
    }
    exit(0);
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

