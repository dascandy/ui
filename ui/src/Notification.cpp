#include "ui/Notification.hpp"
#include "process.hpp"

namespace Notification {
  void Send(const Notification& n) {
    std::vector<std::string> args;
    if (not n.applicationName.empty()) args.push_back("--app-name=" + n.applicationName);
    for (auto& action : n.actions) {
      args.push_back("--action=" + action.name + "=" + action.text);
    }
    if (not n.icon.empty()) args.push_back("--icon=" + n.icon.string());
    if (not n.isPersistent) args.push_back("--transient");

    args.push_back(n.summary);
    args.push_back(n.body);

    std::vector<uint8_t> action;
    Run(Process("notify-send", args).out(action));

    // We assume the action returned may have whitespace padding of sorts.
    std::string action_str((const char*)action.data(), action.size());
    printf("|%s|\n", action_str.c_str());
    for (auto& a : n.actions) {
      if (action_str.find(a.name) != std::string::npos) {
        a.onAction();
      }
    }
  }
}

