#include <iostream>

#include <program.hpp>
#include <math.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

int Program::exec(string & output) {

    string command = cli + ">" + tmp_filename;

    LOG << "Command: " << command;
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
  const wot::Node & n,
  const string & main_command
) {
    // Check if electrum is there
    if (!is_present()) {
      cerr << hint_if_no_executable( action, n, main_command );

      return false;
    }
    return true;
}

// A filter based on CLI (please sanitize before use)
string Program::cli_filter(const string & in, const string & command) {
  ofstream f;
  const string tmp_file1 = "/tmp/in."+sha256(in);
  const string tmp_file2 = "/tmp/out."+sha256(in);
  LOG << "Writing to " << tmp_file1;
  f.open (tmp_file1); f << in; f.close();

  string cli = "cat "+tmp_file1+" | "+command+" > "+tmp_file2;

  LOG << "Command: " << cli;
  system( cli.c_str() );

  ifstream f2 = ifstream(tmp_file2);
  stringstream buffer;
  buffer << f2.rdbuf();
  remove(tmp_file1.c_str());
  remove(tmp_file2.c_str());

  return buffer.str();
}
