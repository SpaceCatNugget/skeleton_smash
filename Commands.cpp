#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
//#include <direct.h>
#include <sstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <ctype.h>

using namespace std;

const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

string _ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
  return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  if(!cmd_line) {
      return 0;
  }
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = nullptr;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundCommand(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

bool check_if_number(std::string s ) {
    for( int i = 0; i < s.length(); i++ ) {
        if( !isdigit( s[i] )) {
            return false;
        }
    }
    return true;
}

char **init_args(const char *cmd_line, int *num_of_args) {
    char **args = (char **) malloc(COMMAND_MAX_ARGS * sizeof(char **));
    if (!args)
        return nullptr;
    for (int i = 0; i < COMMAND_MAX_ARGS; i++) {
        args[i] = nullptr;
    }
    int num = _parseCommandLine(cmd_line, args);
    if (num == -1)
        args = nullptr;
    *num_of_args = num;
    return args;
}

void free_args(char **args, int arg_num) {
    if (!args)
        return;

    for (int i = 0; i < arg_num; i++) {
        if (args[i])
            free(args[i]);
    }
    free(args);
}

// TODO: Add your implementation for classes in Commands.h

//SET AND GET COMMANDS
//for shell
void SmallShell::setPrompt(std::string prompt) {
    smash_prompt = prompt + "> ";
}
void SmallShell::setPid() {
    pid = getppid();
}
void SmallShell::setPrevDirectory(char* directory) {
    prev_directory = directory;
}
char* SmallShell::getPrevDir() {
    return prev_directory;
}
void SmallShell::setCurrProcess(pid_t pid) {
    curr_process = pid;
}
pid_t SmallShell::getCurrProcess(){
    return curr_process;
}
JobsList SmallShell::getJobsList() {
    return job_list;
}
int SmallShell::getJobIdFG() {
    return job_id_fg;
}
void SmallShell::setJobIdFG(int id) {
    job_id_fg = id;
}
void SmallShell::setCurrCmd(std::string cmd) {
    curr_cmd = cmd;
}
std::string SmallShell::getCurrCmd() {
    return curr_cmd;
}
bool SmallShell::getLastCmdInFG() {
    return last_cmd_in_fg;
}
bool SmallShell::checkIfPipe() {
    return is_pipe;
}
void SmallShell::setIsPipe(bool is_pipe) {
    is_pipe = is_pipe;
}

//for job entry
int JobsList::JobEntry::getJobId() {
    return job_id;
}
int JobsList::JobEntry::getJobPID() {
    return job_pid;
}
JobState JobsList::JobEntry::getJobState() {
    return job_state;
}
std::string JobsList::JobEntry::getCmd() {
    return cmd;
}
time_t JobsList::JobEntry::getTimeInList() {
    return time_in_list;
}
bool JobsList::JobEntry::getStopped() {
    return stopped;
};
void JobsList::JobEntry::setJobId(int id) {
    this->job_id = id;
}
void JobsList::JobEntry::setJobPid(pid_t pid) {
    this->job_pid = pid;
}
void JobsList::JobEntry::setJobState(JobState state) {
    this->job_state = state;
}
void JobsList::JobEntry::setCmdLine(std::string cmd) {
    this->cmd = cmd;
}
void JobsList::JobEntry::setTimeInList(time_t time) {
    this->time_in_list = time;
}
void JobsList::JobEntry::setStop(bool is_stopped) {
    this->stopped = is_stopped;
}

std::list<JobsList::JobEntry> JobsList::getJobsList() {
    return jobs_list;
}

JobsList::JobEntry* JobsList::getJobById(int jobId) {

    for (auto& job : jobs_list) {
        if (job.getJobId() == jobId) {
            return &job;
        }
    }
    return nullptr;
}

char* RedirectionCommand::getCmd() {
    return cmd;
}
void RedirectionCommand::setCmd(std::string command) {
    this->cmd = (char *) malloc(sizeof(char) * (strlen(command.c_str()) + 1));
    strcpy(this->cmd, command.c_str());
}
char* RedirectionCommand::getFilename() {
    return filename;
}
void RedirectionCommand::setFilename(std::string file_name) {
    this->filename = (char *) malloc(sizeof(char) * (strlen(file_name.c_str()) + 1));
    strcpy(filename, file_name.c_str());
}
std::string PipeCommand::getCmd1() {
    return this->cmd1;
}
std::string PipeCommand::getCmd2() {
    return this->cmd2;
}
std::string PipeCommand::getDelimiter() {
    return this->delimiter;
}

void PipeCommand::setCmd1(std::string command1) {
    this->cmd1 = command1;
}
void PipeCommand::setCmd2(std::string command2) {
    this->cmd2 = command2;
}
void PipeCommand::setDelimiter(std::string del) {
    this->delimiter = del;
}


//END OF SET AND GET COMMANDS

JobsList SmallShell::job_list;

SmallShell::SmallShell() {
    this->setPrompt("smash");
    this->setPid();
    this->setPrevDirectory(nullptr);
    this->setCurrProcess(-1);
    this->setJobIdFG(-1);
    this->setIsPipe(false);
}
// TODO: add your implementation


SmallShell::~SmallShell() {
// TODO: add your implementation
    if (prev_directory) {
        free(prev_directory);
    }
}

JobsList::JobEntry::~JobEntry() = default;

JobsList::JobsList() : jobs_list(), max_job_id(1) {}

Command::Command(const char *cmd_line){
    setCmdLine(cmd_line);
}

Command::~Command() {
    free((void *) cmd_line);
}

void throw_invalid_args_error(std::string cmd) {
    std::cerr << "smash error: " << cmd << ": invalid arguments" << std::endl;
}


/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
    // For example:
    std::string cmd_s = _trim(string(cmd_line));
    std::string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    if (firstWord.compare("chprompt") == 0 || firstWord.compare("chprompt&") == 0) {
        return new ChpromptCommand(cmd_line);
    }
    if (firstWord.compare("showpid") == 0 || firstWord.compare("showpid&") == 0) {
        return new ShowPidCommand(cmd_line);
    }
    if (firstWord.compare("pwd") == 0 || firstWord.compare("pwd&") == 0) {
        return new PWDCommand(cmd_line);
    }
    if (firstWord.compare("cd") == 0 || firstWord.compare("cd&") == 0) {
        return new ChangeDirCommand(cmd_line);
    }
    if (firstWord.compare("jobs") == 0 || firstWord.compare("jobs&") == 0) {
        return new JobsCommand(cmd_line);
    }
    if (firstWord.compare("bg") == 0 || firstWord.compare("bg&") == 0) {
        return new BackgroundCommand(cmd_line);
    }
    if (firstWord.compare("kill") == 0 || firstWord.compare("kill&") == 0) {
        return new KillCommand(cmd_line);
    }
    if (strstr(cmd_line, ">") != nullptr || strstr(cmd_line, ">>") != nullptr) {
        return new RedirectionCommand(cmd_line);
    }

    if (strstr(cmd_line, "|") != nullptr || strstr(cmd_line, "|&")) {
        return new PipeCommand(cmd_line);
    }
    if (firstWord.compare("chmod") == 0 || firstWord.compare("chmod&") == 0) {
        return new ChmodCommand(cmd_line);
    }
/*

  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else if ...
  .....
  else {
    return new ExternalCommand(cmd_line);
  }
  */
    return nullptr;
}


void SmallShell::executeCommand(const char *cmd_line) {
    // TODO: Add your implementation here
    Command *cmd = CreateCommand(cmd_line);
    if(!cmd) {
        return;
    }
    cmd->execute();
}

///////// BUILT-IN COMMANDS /////////

BuiltInCommand::BuiltInCommand(const char *cmd_line) : Command(cmd_line){
    std::string cmd(cmd_line);
    if (_isBackgroundCommand(cmd.c_str())) {
        char cmd_line_copy[COMMAND_ARGS_MAX_LENGTH];
        strcpy(cmd_line_copy, cmd.c_str());
        _removeBackgroundSign(cmd_line_copy);
        cmd = cmd_line_copy;
    }
    char *cmd_copy = (char *) malloc(sizeof(char) * (cmd.length() + 1));
    strcpy(cmd_copy, cmd.c_str());
    this->setCmdLine(cmd_line);
}
ChpromptCommand::ChpromptCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}
ShowPidCommand::ShowPidCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}
PWDCommand::PWDCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}
ChangeDirCommand::ChangeDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}
JobsCommand::JobsCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}
BackgroundCommand::BackgroundCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}
KillCommand::KillCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}

///////// END OF BUILT-IN COMMANDS /////////

///////// SPECIAL COMMANDS /////////
RedirectionCommand::RedirectionCommand(const char *cmd_line) : Command(cmd_line) {
    string cmd_line_copy(cmd_line);
    int num_of_args;
    char **args = init_args(this->getCmdLine(), &num_of_args);
    string str_to_parse = string(cmd_line);
    string delimiter;
    if (str_to_parse.find(">>") != std::string::npos) {
        delimiter = ">>";
    } else {
        delimiter = ">";
    }

    string trimmed_command = _trim(str_to_parse.substr(0, str_to_parse.find(delimiter)));
    setCmd(trimmed_command.c_str());

    string trimmed_filename = _trim(str_to_parse.substr(str_to_parse.find(delimiter) + delimiter.length(), str_to_parse.length()));
    if (_isBackgroundCommand(trimmed_filename.c_str())) {
        char trimmed_copy[FILENAME_MAX];
        strcpy(trimmed_copy, trimmed_filename.c_str());
        _removeBackgroundSign(trimmed_copy);
        trimmed_filename = _trim(string(trimmed_copy));
    }
    setFilename(trimmed_filename);

    char double_arrow[] = ">>";
    if (strcmp(delimiter.c_str(), double_arrow) == 0) {
        this->append = true;
    } else {
        this->append = false;
    }
    prepare();

    free_args(args, num_of_args);
}
PipeCommand::PipeCommand(const char *cmd_line) : Command(cmd_line) {
    string str_to_parse = string(cmd_line);
    if(str_to_parse.find("|&") == std::string::npos) {
        setDelimiter("|");
        setCmd2(str_to_parse.substr(str_to_parse.find(delimiter) + 1, str_to_parse.length()));
    } else {
        setDelimiter("|&");
        setCmd2(str_to_parse.substr(str_to_parse.find(delimiter) + 2, str_to_parse.length()));
    }
    setCmd1(str_to_parse.substr(0, str_to_parse.find(delimiter)));
}

ChmodCommand::ChmodCommand(const char *cmd_line) : BuiltInCommand(cmd_line)  {};

///////// END OF SPECIAL COMMANDS /////////

void ChpromptCommand::execute() {
    int num_of_args;
    char **args = init_args(this->getCmdLine(), &num_of_args);
    if (!args) {
        perror("smash error: malloc failed");
        return;
    }
    SmallShell &shell = SmallShell::getInstance();
    if (num_of_args == 0) {
        shell.setPrompt("smash> ");
    } else {
        shell.setPrompt(args[1]);
    }
    free_args(args, num_of_args);
}

void ShowPidCommand::execute() {
    SmallShell &shell = SmallShell::getInstance();
    std::cout << "smash pid is " << getpid() << std::endl;
}

std::string GetCurrentWorkingDir() {
    char buff[FILENAME_MAX];
    getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

void PWDCommand::execute() {
    std::string string_to_print = GetCurrentWorkingDir();
    std::cout << string_to_print << std::endl;
}

void ChangeDirCommand::execute() {
    int num_of_args;
    char **args = init_args(this->getCmdLine(), &num_of_args);
    if (!args) {
        perror("smash error: malloc failed");
        return;
    }

    SmallShell &shell = SmallShell::getInstance();
    if (num_of_args > 2) {
        std::cerr << "smash error: cd: too many arguments" << std::endl;
    } else if (num_of_args == 2) {
        long max_size = pathconf(".", _PC_PATH_MAX);
        char *curr_working_dir = (char *) malloc((size_t) max_size);
        if (!curr_working_dir) {
            perror("smash error: malloc failed");
            free_args(args, num_of_args);
            return;
        }
        if (getcwd(curr_working_dir, (size_t) max_size) == nullptr) {
            perror("smash error: getcwd failed");
            free(curr_working_dir);
            free_args(args, num_of_args);
            return;
        }

        std::string second_word = args[1];
        if (second_word.compare("-") == 0) {
            if (shell.getPrevDir() == nullptr) {
                std::cerr << "smash error: cd: OLDPWD not set" << std::endl;
                free(curr_working_dir);
            } else {
                if (chdir(shell.getPrevDir()) == -1) {
                    perror("smash error: chdir failed");
                    free(curr_working_dir);
                    free_args(args, num_of_args);
                    return;
                }
                shell.setPrevDirectory(curr_working_dir);
            }
        } else {
            shell.setPrevDirectory(curr_working_dir);

            if (chdir(args[1]) == -1) {
                perror("smash error: chdir failed");
                free_args(args, num_of_args);
                return;
            }
        }
    } else {
        free_args(args, num_of_args);
        return;
    }
    free_args(args, num_of_args);
}

void JobsCommand::execute() {
    SmallShell &shell = SmallShell::getInstance();

    (shell.getJobsList()).removeFinishedJobs();
    for (auto iterator = ((shell.getJobsList()).getJobsList()).begin(); iterator != ((shell.getJobsList()).getJobsList()).end(); iterator++) {
        auto job = *iterator;
        if (job.getStopped()) {
            cout << "[" << job.getJobId() << "] " << job.getCmd() << " : " << job.getJobPID()
                 << " " << difftime(time(nullptr), job.getTimeInList()) << " secs (stopped)" << endl;
        } else {
            cout << "[" << job.getJobId() << "] " << job.getCmd() << " : " << job.getJobPID()
                 << " " << difftime(time(nullptr), job.getTimeInList()) << " secs" << endl;
        }
    }
}

void BackgroundCommand::execute() {
    int num_of_args;
    char **args = init_args(this->getCmdLine(), &num_of_args);
    if (!args) {
        perror("smash error: malloc failed");
        return;
    }
    SmallShell &shell = SmallShell::getInstance();
    (shell.getJobsList()).removeFinishedJobs();

    if (num_of_args == 1) { //case: just bg command, will continue last sent to bg
        for (auto iterator = ((shell.getJobsList()).getJobsList()).end(); iterator != ((shell.getJobsList()).getJobsList()).begin(); --iterator) {
            auto job = *iterator;
            if (job.getStopped()) {
                std::cout << job.getCmd() << " : " << job.getJobPID() << std::endl;
                return;
            }
        }
        std::cerr << "smash error: bg: there is no stopped jobs to resume" << std::endl;
    } else if(num_of_args == 2) { //case: bg+job_id
        if(!check_if_number(args[1])) {
            throw_invalid_args_error("bg");
            return;
        }
        for (auto iterator = ((shell.getJobsList()).getJobsList()).end(); iterator != ((shell.getJobsList()).getJobsList()).begin(); --iterator) {
            auto job = *iterator;
            if (job.getStopped() && job.getJobId() == *args[1]) {
                std::cout << job.getCmd() << " : " << job.getJobPID() << std::endl;
                return;
            } else if (job.getJobId() == *args[1]) {
                std::cerr << "smash error: bg: job-id " << job.getJobId() << "is already running in the background" <<std::endl;
                return;
            }
        }
        std::cerr << "smash error: bg: job-id " << args[1] << "does not exist" << std::endl;
    } else {
        throw_invalid_args_error("bg");
    }
}

void KillCommand::execute() {
    int num_of_args;
    char **args = init_args(this->getCmdLine(), &num_of_args);
    if (!args) {
        perror("smash error: malloc failed");
        return;
    }
    SmallShell &shell = SmallShell::getInstance();
    (shell.getJobsList()).removeFinishedJobs();

    if(num_of_args != 3) {
        throw_invalid_args_error("kill");
        return;
    }

    if (!check_if_number(args[2])) {
        throw_invalid_args_error("kill");
        free_args(args, num_of_args);
        return;
    }
    char first_char = string(args[1]).at(0);
    char dash = '-';
    if (first_char != dash) {
        throw_invalid_args_error("kill");
        free_args(args, num_of_args);
        return;
    }
    int job_id_to_kill = std::stoi(args[2]);
    if (!check_if_number(string(args[1]).erase(0, 1))) {
        throw_invalid_args_error("kill");
        free_args(args, num_of_args);
        return;
    }
    int signum = std::stoi(string(args[1]).erase(0, 1));

    if (JobsList::JobEntry *job_to_kill = (shell.getJobsList()).getJobById(job_id_to_kill)) {
        pid_t job_pid_to_kill = job_to_kill->getJobPID();

        if (kill(job_pid_to_kill, signum) == -1) {
            perror("smash error: kill failed");
            free_args(args, num_of_args);
            return;
        }
        std::cout << "signal number " << signum << " was sent to pid " << job_pid_to_kill << std::endl;

        if (signum == SIGTSTP) {
            job_to_kill->setStop(true);
        } else if (signum == SIGCONT) {
            job_to_kill->setStop(false);
        }
    } else {
        std::cerr << "smash error: kill: job-id " << job_id_to_kill << " does not exist" << std::endl;
    }
}

JobsList::JobEntry::JobEntry(int job_id, pid_t job_pid, time_t time_in_list, std::string &cmd, bool stopped) {
    setJobId(job_id);
    setJobPid(job_pid);
    setTimeInList(time_in_list);
    setCmdLine(cmd);
    setStop(stopped);
}

void JobsList::addJob(Command *cmd, pid_t pid,bool last_cmd_in_fg, bool isStopped) {
    removeFinishedJobs();
    string cmd_line(cmd->getCmdLine());
    SmallShell &shell = SmallShell::getInstance();
    //get index of the job id
    auto iterator = jobs_list.begin();
    if(last_cmd_in_fg) {
        for (; iterator != jobs_list.end(); ++iterator) {
            auto job = *iterator;
            if (job.getJobId() > shell.getJobIdFG()) break;
        }
        jobs_list.insert(iterator, JobEntry(shell.getJobIdFG(), pid, time(nullptr), cmd_line, isStopped));
        return;
    }
    jobs_list.push_back(JobEntry(max_job_id, pid, time(nullptr), cmd_line, isStopped));
    max_job_id++;
}

void JobsList::removeFinishedJobs() {
    if (jobs_list.empty()) {
        max_job_id = 1;
        return;
    }
    SmallShell &shell = SmallShell::getInstance();

    for (auto iterator = jobs_list.begin(); iterator != jobs_list.end(); iterator++) {
        if(!shell.checkIfPipe()) {
            auto curr_job = *iterator;
            int status;
            int ret_wait = waitpid(curr_job.getJobPID(), &status, WNOHANG);
            if (ret_wait == curr_job.getJobPID() || ret_wait == -1) {
                jobs_list.erase(iterator);
                iterator--;
            }
        }
    }

    int curr_max_id = 0;
    //for (auto it = job_list.begin(); it != job_list.end(); ++it) {
    for(auto& curr_job : jobs_list){
        //auto job = *it;
        if (curr_job.getJobId() > curr_max_id) {
            curr_max_id = curr_job.getJobId();
        }
    }
    max_job_id = curr_max_id + 1;
}

// ------------------------------------ special commands ------------------------------------------------

void RedirectionCommand::execute() {
    if (is_redirection_success) {
        SmallShell &shell = SmallShell::getInstance();
        shell.executeCommand(getCmd());
    }
    cleanup();
}

void RedirectionCommand::prepare() {
    stdout_copy = dup(1);
    if (close(1) == -1) {
        perror("smash error: close failed");
        return;
    }
    if (this->append) {
        this->fd = open(filename, O_WRONLY | O_APPEND | O_CREAT, 0666);


    } else {
        this->fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    if (this->fd == -1) {
        perror("smash error: open failed");
        is_redirection_success = false;
    } else {
        is_redirection_success = true;
    }

}

void RedirectionCommand::cleanup() {
    free(this->filename);
    free(this->cmd);
    if (is_redirection_success) {
        if (close(this->fd) == -1) {
            perror("smash error: close failed");
        }
    }
    if (dup2(stdout_copy, 1) == -1) {
        perror("smash error: dup2 failed");
    }
    if (close(stdout_copy) == -1) {
        perror("smash error: close failed");
    }
}

void PipeCommand::execute() {
// TODO: Check number of arguments
    int filedest[2];
    pipe(filedest);
    SmallShell &shell = SmallShell::getInstance();
    pid_t pid1 = fork(), pid2;
    if (pid1 == -1) {
        perror("smash error: fork failed");
        if (close(filedest[0]) == -1) {
            perror("smash error: close failed");
        }
        if (close(filedest[1]) == -1) {
            perror("smash error: close failed");
        }
        return;
    }
    if (pid1 == 0) { //first son
        if (setpgrp() == -1) {
            perror("smash error: setpgrp failed");
            if (close(filedest[0]) == -1) {
                perror("smash error: close failed");
            }
            if (close(filedest[1]) == -1) {
                perror("smash error: close failed");
            }
            return;
        }
        if (getDelimiter() == "|") {
            if (dup2(filedest[1], 1) == -1) {
                perror("smash error: dup2 failed");
                if (close(filedest[0]) == -1) {
                    perror("smash error: close failed");
                }
                if (close(filedest[1]) == -1) {
                    perror("smash error: close failed");
                }
                return;
            }
        } else {
            if (dup2(filedest[1], 2) == -1) {
                perror("smash error: dup2 failed");
                if (close(filedest[0]) == -1) {
                    perror("smash error: close failed");
                }
                if (close(filedest[1]) == -1) {
                    perror("smash error: close failed");
                }
                return;
            }
        }
        if (close(filedest[0]) == -1) {
            perror("smash error: close failed");
        }
        if (close(filedest[1]) == -1) {
            perror("smash error: close failed");
        }
        shell.setIsPipe(true);
        shell.executeCommand(getCmd1().c_str());
        exit(0);
    }
    pid2 = fork();
    if (pid2 == -1) {
        perror("smash error: fork failed");
        if (close(filedest[0]) == -1) {
            perror("smash error: close failed");
        }
        if (close(filedest[1]) == -1) {
            perror("smash error: close failed");
        }
        return;
    }
    if (pid2 == 0) { //second son
        if (setpgrp() == -1) {
            perror("smash error: setpgrp failed");
            if (close(filedest[0]) == -1) {
                perror("smash error: close failed");
            }
            if (close(filedest[1]) == -1) {
                perror("smash error: close failed");
            }
            return;
        }
        if (dup2(filedest[0], 0) == -1) {
            perror("smash error: dup2 failed");
            if (close(filedest[0]) == -1) {
                perror("smash error: close failed");
            }
            if (close(filedest[1]) == -1) {
                perror("smash error: close failed");
            }
            return;
        }
        if (close(filedest[0]) == -1) {
            perror("smash error: close failed");
        }
        if (close(filedest[1]) == -1) {
            perror("smash error: close failed");
        }
        shell.setIsPipe(true);
        shell.executeCommand(getCmd2().c_str());
        exit(0);
    }
    if (close(filedest[0]) == -1) {
        perror("smash error: close failed");
    }
    if (close(filedest[1]) == -1) {
        perror("smash error: close failed");
    }
    if (waitpid(pid1,nullptr, WUNTRACED) == -1) {
        perror("smash error: waitpid failed");
        return;
    }
    if (waitpid(pid2,nullptr, WUNTRACED) == -1) {
        perror("smash error: waitpid failed");
        return;
    }
}

void ChmodCommand::execute() {
    int num_of_args;
    char **args = init_args(this->getCmdLine(), &num_of_args);
    if (!args) {
        perror("smash error: malloc failed");
        return;
    }
    SmallShell &shell = SmallShell::getInstance();

    if(num_of_args != 3) {
        throw_invalid_args_error("chmod");
        free_args(args, num_of_args);
        return;
    }

    int num[3];
    if (!check_if_number(args[1])) {
        throw_invalid_args_error("chmod");
        free_args(args, num_of_args);
        return;
    } else {
        int helper = 10;
        int num_to_array = stoi(args[1]);
        for (int i = 0; i < 3; i++) {
            num[2-i] = num_to_array%helper;
            helper = helper*10;
            num_to_array = num_to_array/10;
        }
        if (num_to_array != 0) {
            throw_invalid_args_error("chmod");
            free_args(args, num_of_args);
            return;
        }
    }

    mode_t mode_num = num[0]*100 + num[1]*10 + num[2];

    char *file_path = args[2];
    if(access(file_path, F_OK) == -1) {
        throw_invalid_args_error("chmod");
        free_args(args, num_of_args);
        return;
    }

    if (chmod(file_path, mode_num) == -1) {
        perror("smash error: chmod failed");
        free_args(args, num_of_args);
        return;
    }

    free_args(args, num_of_args);

//    for (int i = 0; i < 3; i++) {
//        switch(num[i]) {
//            case 0:
//                //place holder
//                break;
//            case 1:
//                //place holder
//                break;
//            case 2:
//                //place holder
//                break;
//            case 3:
//                //place holder
//                break;
//            case 4:
//                //place holder
//                break;
//            case 5:
//                //place holder
//                break;
//            case 6:
//                //place holder
//                break;
//            case 7:
//                //place holder
//                break;
//        }
//    }

}
