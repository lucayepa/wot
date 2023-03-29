#include <program.hpp>

int Program::exec(string & output) {
    string command = cli + ">" + tmp_filename;

    //cerr << "Command: " << command;
    int result = system( command.c_str() );

    ifstream f = ifstream(tmp_filename);
    stringstream buffer;
    buffer << f.rdbuf();
    remove(tmp_filename.c_str());

    output = buffer.str();

    return result;
}

inline bool Program::is_present() {
  string line =  "which " + name + " > /dev/null 2>&1";
  return !system(line.c_str());
}

bool Program::check_and_suggest_cli(
  const string & action,
  const Node & n,
  const string & main_command
) {
    // Check if electrum is there
    if (!is_present()) {
      //cerr << hint_if_no_executable(action,n,main_command);

      return false;
    }
    return true;
}
