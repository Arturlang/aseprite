// Aseprite
// Copyright (C) 2020-2022  Igara Studio S.A
// Copyright (C) 2001-2015  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "app/ui/main_menu_bar.h"

#include "app/app.h"
#include "app/app_menus.h"
#include "app/extensions.h"
#include "ui/manager.h"
#include "ui/message.h"

namespace app {

MainMenuBar::MainMenuBar()
  // We process Alt+mnemonics with ShowMenu commands (instead of the
  // integrated method in ui::MenuBox::onProcessMessage()).
  : MenuBar(MenuBar::ProcessTopLevelShortcuts::kNo)
{
  Extensions& extensions = App::instance()->extensions();

  // Reload the main menu if there are changes in keyboard shortcuts
  // or scripts when extensions are installed/uninstalled or
  // enabled/disabled, enqueued to avoid multiple reloads.
  m_extKeys = extensions.KeysChange.connect([&] { queueReload(); });
  m_extScripts = extensions.ScriptsChange.connect([&] { queueReload(); });
}

void MainMenuBar::queueReload()
{
  if (m_queuedReload)
    return;

  m_queuedReload = true;

  auto* msg = new CallbackMessage([this] { reload(); });
  msg->setRecipient(this);
  Manager::getDefault()->enqueueMessage(msg);
}

void MainMenuBar::reload()
{
  m_queuedReload = false;
  setMenu(nullptr);

  // Reload all menus.
  AppMenus::instance()->reload();

  setMenu(AppMenus::instance()->getRootMenu());
  if (auto p = parent())
    p->layout();
}

} // namespace app
