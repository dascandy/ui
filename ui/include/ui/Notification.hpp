#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <filesystem>
#include <vector>

namespace Notification {
  struct ApplicationName {
    std::string_view name;
  };

  struct Summary {
    std::string_view name;
  };
  struct Body {
    std::string_view name;
  };
  struct Action {
    std::string name;
    std::string text;
    std::function<void()> onAction;
  };
  struct DefaultAction {
    std::string text;
    std::function<void()> onAction;
  };
  struct NotificationIcon {
    std::filesystem::path path;
  };
  struct Persistent {};

  struct Notification {
    std::string applicationName;
    std::string summary;
    std::string body;
    std::vector<Action> actions;
    std::filesystem::path icon;
    bool isPersistent = false;
    void Set(const ApplicationName& appname) {
      applicationName = std::string(appname.name);
    }
    void Set(const Summary& s) {
      summary = std::string(s.name);
    }
    void Set(const Body& b) {
      body = std::string(b.name);
    }
    void Set(Action&& action) {
      actions.push_back(std::move(action));
    }
    void Set(DefaultAction&& action) {
      actions.push_back(Action{"default", std::move(action.text), std::move(action.onAction)});
    }
    void Set(NotificationIcon& i) {
      icon = std::move(i.path);
    }
    void Set(Persistent) {
      isPersistent = true;
    }
  };

  void Send(const Notification& n);
  template <typename... Ts>
  void SendNotification(Ts... args) {
    Notification n;
    (n.Set(std::move(args)), ...);
    Send(n);
  }
}

