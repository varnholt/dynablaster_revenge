#include "menupageitem.h"

MenuPageItem::MenuPageItem()
    : mPageItemType(PageItemTypeUnknown),
      mLayerActive(0),
      mLayerInactive(0),
      mFocussed(false),
      mInteractive(false),
      mActive(false),
      mVisible(true),
      mEnabled(true),
      mTabIndex(-1)
{
}

MenuPageItem::~MenuPageItem()
{
}

MenuPageItem::PageItemType MenuPageItem::getPageItemType() const
{
   return mPageItemType;
}

void MenuPageItem::initialize()
{
}

void MenuPageItem::setActiveLayer(PSDLayer* layer)
{
   mLayerActive = layer;
}

void MenuPageItem::setInactiveLayer(PSDLayer* layer)
{
   mLayerInactive = layer;
}

void MenuPageItem::setFocus(bool focus)
{
   mFocussed = focus;
}

bool MenuPageItem::isFocussed() const
{
   return mFocussed;
}

bool MenuPageItem::isModal() const
{
   return false;
}

void MenuPageItem::activated()
{
   actionSignal(mAction);
}

void MenuPageItem::deactivated()
{
}

void MenuPageItem::setInteractive(bool interactive)
{
   mInteractive = interactive;
}

bool MenuPageItem::isInteractive()
{
   return mInteractive;
}

void MenuPageItem::setAction(const std::string& action)
{
   mAction = action;
}

bool MenuPageItem::isActive()
{
   return mActive;
}

void MenuPageItem::setActive(bool active)
{
   mActive = active;
}

void MenuPageItem::draw()
{
   if (isVisible())
   {
      PSDLayer* psdLayer = getLayer();

      if (psdLayer)
         psdLayer->render();
   }
}

PSDLayer* MenuPageItem::getLayer() const
{
   PSDLayer* layer = isFocussed() ? mLayerActive : mLayerInactive;

   return layer;
}

PSD::Layer* MenuPageItem::getCurrentLayer()
{
   PSDLayer* layer = getLayer();
   if (layer)
      return layer->getLayer();
   else
      return 0;
}

PSDLayer* MenuPageItem::getActiveLayer()
{
   return mLayerActive;
}

PSDLayer* MenuPageItem::getInactiveLayer()
{
   return mLayerInactive;
}

void MenuPageItem::keyPressed(int /*key*/, const std::string& /*text*/)
{
}

void MenuPageItem::setEnabled(bool enabled)
{
   mEnabled = enabled;
}

MenuPageItem* MenuPageItem::getParent() const
{
   return mParent;
}

void MenuPageItem::setParent(MenuPageItem* value)
{
   mParent = value;
}

bool MenuPageItem::hasNestedElements()
{
   return false;
}

bool MenuPageItem::isGrabbingMouseEvents()
{
   return false;
}

void MenuPageItem::mouseMoved(int /*x*/, int /*y*/)
{
}

void MenuPageItem::mousePressed(int /*x*/, int /*y*/)
{
}

void MenuPageItem::mouseReleased()
{
   mouseReleasedSignal();
}

void MenuPageItem::setVisible(bool enabled)
{
   mVisible = enabled;
   visibleSignal(enabled);
}

bool MenuPageItem::isVisible() const
{
   return mVisible;
}

bool MenuPageItem::isEnabled() const
{
   return mEnabled;
}

void MenuPageItem::setTabIndex(int tabIndex)
{
   mTabIndex = tabIndex;
}

int MenuPageItem::getTabIndex() const
{
   return mTabIndex;
}

bool MenuPageItem::isActionRequestOnClickEnabled() const
{
   return true;
}
