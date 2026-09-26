#include "menu.h"

// Qt
#include <QStringList>

#include <cstdlib>

Menu* Menu::lInstance = 0;

Menu::Menu() : mCurrentPage(0), mBackground(0), mMenuWorkflow(0)
{
   mSettings = std::make_unique<Settings>("data/menus/menu.ini", Settings::IniFormat);

   lInstance = this;
}

Menu::Menu(const Menu& /*menu*/) : QList<MenuPage*>(), mCurrentPage(0), mBackground(0), mMenuWorkflow(0)
{
   std::abort();
}

Menu::~Menu()
{
   while (!isEmpty())
      delete takeFirst();
}

void Menu::initialize()
{
   // assign 1 psd for each page
   mSettings->beginGroup("pages");
   QStringList childKeys = mSettings->childKeys();

   QString filename;
   bool defaultAssigned = false;
   foreach (const QString& childKey, childKeys)
   {
      filename = mSettings->value(childKey).toString();

      // create a new page
      MenuPage* page = new MenuPage();

      page->setTitle(childKey);
      page->setFilename(filename);
      page->initialize();

      // connect page actions to outside world
      page->actionRequestSignal.connect([this](const std::string& p, const std::string& action) { actionRequestSignal(p, action); });

      page->actionKeyPressedSignal.connect([this](const std::string& p, const std::string& item, int key)
                                           { actionKeyPressedSignal(p, item, key); });

      page->layerFocussedSignal.connect([this](const std::string& p, const std::string& item) { layerFocussedSignal(p, item); });

      // store page
      push_back(page);

      // assign "special" pages
      if (filename.contains("background"))
      {
         mBackground = page;
      }
      else
      {
         if (!defaultAssigned)
         {
            // assign default page
            mCurrentPage = page;
            mCurrentPage->setActive(true);

            // default page is now assigned
            defaultAssigned = true;
         }
      }
   }

   if (mBackground)
   {
      takeAt(indexOf(mBackground));
      push_front(mBackground);
   }

   mSettings->endGroup();
}

Menu* Menu::getInstance()
{
   return lInstance;
}

void Menu::mouseMoved(int x, int y)
{
   if (mCurrentPage)
   {
      mCurrentPage->mouseMoved(x, y);
   }
}

void Menu::mousePressed(int x, int y)
{
   if (mCurrentPage)
   {
      mCurrentPage->mousePressed(x, y);
   }
}

void Menu::mouseReleased()
{
   if (mCurrentPage)
   {
      mCurrentPage->mouseReleased();
   }
}

void Menu::keyPressed(int key, const std::string& text)
{
   if (mCurrentPage)
   {
      mCurrentPage->keyPressed(key, text);
   }
}

void Menu::paste(const std::string& text)
{
   if (mCurrentPage)
   {
      mCurrentPage->paste(text);
   }
}

void Menu::setCurrentPage(MenuPage* page)
{
   mCurrentPage = page;
}

MenuPage* Menu::getCurrentPage()
{
   return mCurrentPage;
}

MenuPage* Menu::getBackground()
{
   return mBackground;
}

void Menu::setMenuWorkflow(MenuWorkflow* workflow)
{
   mMenuWorkflow = workflow;
}

void Menu::actionResponse(
   const QString& /*page*/,
   const QString& /*action*/,
   bool /*ok*/
)
{
}

MenuPage* Menu::getPageByName(const QString& pageName)
{
   MenuPage* page = 0;

   for (int i = 0; i < size(); i++)
   {
      page = at(i);

      if (page->getFilename() == pageName)
      {
         break;
      }
   }

   return page;
}

MenuWorkflow* Menu::getMenuWorkflow() const
{
   return mMenuWorkflow;
}
