#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include "command.hpp"
#include "commands/sign_toml_command.hpp"
#include "commands/sign_command.hpp"
#include "commands/verify_command.hpp"
#include "commands/add_command.hpp"
#include "commands/ls_on_command.hpp"
#include "commands/ls_nodes_command.hpp"
#include "commands/ls_command.hpp"
#include "commands/add_sig_command.hpp"
#include "commands/rm_sig_command.hpp"
#include "commands/ls_sig_command.hpp"
#include "commands/show_command.hpp"
#include "commands/view_command.hpp"
#include "commands/solve_command.hpp"
#include "commands/template_options_file_command.hpp"
#include "commands/compose_command.hpp"
#include "commands/hash_object_command.hpp"

namespace wot {

// Container of commands
struct Commands {
  std::vector<std::unique_ptr<Command>> all;

  Commands() {
    all.push_back(std::make_unique<SignTomlCommand>());
    all.push_back(std::make_unique<SignCommand>());
    all.push_back(std::make_unique<VerifyCommand>());
    all.push_back(std::make_unique<AddCommand>());
    all.push_back(std::make_unique<LsRulesCommand>());
    all.push_back(std::make_unique<LsNodesCommand>());
    all.push_back(std::make_unique<LsCommand>());
    all.push_back(std::make_unique<AddSigCommand>());
    all.push_back(std::make_unique<RmSigCommand>());
    all.push_back(std::make_unique<LsSigCommand>());
    all.push_back(std::make_unique<ViewCommand>());
    all.push_back(std::make_unique<SolveCommand>());
    all.push_back(std::make_unique<TemplateOptionsFileCommand>());
    all.push_back(std::make_unique<ComposeCommand>());
    all.push_back(std::make_unique<HashObjectCommand>());
    all.push_back(std::make_unique<ShowCommand>());
  }
};

} // namespace wot
