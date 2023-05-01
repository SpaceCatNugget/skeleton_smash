#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <string>
#include <list>
#include <iostream>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

enum JobState
{   Background = 0,
    Foreground = 1,
    Stopped = 2
};

class Command {
// TODO: Add your data members
    const char* cmd_line;
 public:
  Command(const char* cmd_line);
  void setCmdLine(const char* cmd_line_input) {
      cmd_line = cmd_line_input;
  }
  const char* getCmdLine() {
      return cmd_line;
  }
  virtual ~Command();
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line);
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
 public:
  ExternalCommand(const char* cmd_line);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
public:
  ChangeDirCommand(const char* cmd_line);
  virtual ~ChangeDirCommand() = default;
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};


class JobsList;
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members
public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};


class JobsList {

 public:
    class JobEntry {
   // TODO: Add your data members
    int job_id;
    pid_t job_pid;
    JobState job_state;
    std::string cmd;
    time_t time_in_list;
    bool stopped;
    public:
        JobEntry(int job_id, pid_t job_pid, time_t time_in_list, std::string& cmd, bool stopped);

        int getJobId();
        int getJobPID();
        JobState getJobState();
        std::string getCmd();
        time_t getTimeInList();
        bool getStopped();
        void setJobId(int id);
        void setJobPid(pid_t pid);
        void setJobState(JobState state);
        void setCmdLine(std::string cmd);
        void setTimeInList(time_t time);
        void setStop(bool is_stopped);

        ~JobEntry();
  };
private:
    int max_job_id;
    std::list<JobEntry> jobs_list;
 // TODO: Add your data members
 public:
  JobsList();
  ~JobsList() = default;
  void addJob(Command* cmd, pid_t pid,bool last_cmd_in_fg = false, bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  std::list<JobEntry> getJobsList();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line);
  virtual ~JobsCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class TimeoutCommand : public BuiltInCommand {
/* Bonus */
// TODO: Add your data members
 public:
  explicit TimeoutCommand(const char* cmd_line);
  virtual ~TimeoutCommand() {}
  void execute() override;
};

class ChmodCommand : public BuiltInCommand {
  // TODO: Add your data members
 public:
  ChmodCommand(const char* cmd_line);
  virtual ~ChmodCommand() {}
  void execute() override;
};

class ChpromptCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    ChpromptCommand(const char* cmd_line);
    virtual ~ChpromptCommand() {}
    void execute() override;
};

class PWDCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    PWDCommand(const char* cmd_line);
    virtual ~PWDCommand() {}
    void execute() override;
};

class GetFileTypeCommand : public BuiltInCommand {
  // TODO: Add your data members
 public:
  GetFileTypeCommand(const char* cmd_line);
  virtual ~GetFileTypeCommand() {}
  void execute() override;
};

class SetcoreCommand : public BuiltInCommand {
  // TODO: Add your data members
 public:
  SetcoreCommand(const char* cmd_line);
  virtual ~SetcoreCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line);
  virtual ~KillCommand() {}
  void execute() override;
};

class SmallShell {
 private:
  // TODO: Add your data members
    pid_t pid;
    char* prev_directory;
    pid_t curr_process;
    std::string curr_cmd;
    static JobsList job_list;
    int job_id_fg;
    bool last_cmd_in_fg;
    bool is_pipe;
 public:
    std::string smash_prompt;
    SmallShell();
    Command *CreateCommand(const char* cmd_line);
    void executeCommand(const char* cmd_line);

    //SET AND GET COMMANDS
    void setPrompt(std::string prompt);
    void setPid();
    void setPrevDirectory(char* directory);
    char* getPrevDir();
    void setCurrProcess(pid_t pid);
    pid_t getCurrProcess();
    JobsList getJobsList();
    int getJobIdFG();
    void setJobIdFG(int id);
    void setCurrCmd(std::string cmd);
    std::string getCurrCmd();
    bool getLastCmdInFG();
    bool checkIfPipe();
    void setIsPipe(bool is_pipe);
    //END OF SET AND GET COMMANDS

    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    static SmallShell& getInstance() // make SmallShell singleton
    {
      static SmallShell instance; // Guaranteed to be destroyed.
      // Instantiated on first use.
      return instance;
    }
    ~SmallShell();
    // TODO: add extra methods as needed
};

//// built in commands ////



#endif //SMASH_COMMAND_H_
