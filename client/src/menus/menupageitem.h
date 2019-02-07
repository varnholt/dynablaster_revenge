#ifndef MENUPAGEITEM_H
#define MENUPAGEITEM_H

#include "psdlayer.h"

// Qt
#include <QObject>

class MenuPageItem : public QObject
{
   Q_OBJECT

   public:

      //! page item types
      enum PageItemType
      {
         PageItemTypeUnknown,
         PageItemTypeButton,
         PageItemTypeCheckbox,
         PageItemTypeCombobox,
         PageItemTypeEditableCombobox,
         PageItemTypeLabel,
         PageItemTypeList,
         PageItemTypeListElement,
         PageItemTypePixmap,
         PageItemTypeRadioButton,
         PageItemTypeScrollbar,
         PageItemTypeScrollImage,
         PageItemTypeSlider,
         PageItemTypeTextedit
      };

      //!
      MenuPageItem(QObject* parent = 0);

      virtual ~MenuPageItem();

      PageItemType getPageItemType() const;


      // layer information

      virtual void setActiveLayer(PSDLayer* layer);

      virtual void setInactiveLayer(PSDLayer* layer);

      virtual PSDLayer* getLayer() const;

      virtual PSD::Layer* getCurrentLayer();

      virtual PSDLayer* getActiveLayer();

      virtual PSDLayer* getInactiveLayer();


      // main

      //! initialize menupage item
      virtual void initialize();

      //! draw menupage item
      virtual void draw();


      // behaviour

      //! setter for interactive flag
      void setInteractive(bool);

      //! getter for interactive flag
      bool isInteractive();

      //! menupage item is focussed
      bool isFocussed() const;

      //! menu page item is modal
      virtual bool isModal() const;

      //! setter for action identifier
      void setAction(const QString&);

      //! menu page item has integrated items
      virtual bool hasNestedElements();

      //! menu page item is grabbing mouse events
      virtual bool isGrabbingMouseEvents();

      //! check if item is active
      bool isActive();

      //! getter for visible flag
      bool isVisible() const;

      //! check if item is enabled
      bool isEnabled() const;

      //! setter for tab index
      void setTabIndex(int tabIndex);

      //! getter for tab index
      int getTabIndex() const;

      //! check if action request on click is enabled
      virtual bool isActionRequestOnClickEnabled() const;


      // event handlers

      //! mouse moved
      virtual void mouseMoved(int x, int y);

      //! mouse pressed
      virtual void mousePressed(int x, int y);

      //! mouse released
      virtual void mouseReleased();

      //! setter for parent item
      MenuPageItem *getParent() const;

      //! getter for parent item
      void setParent(MenuPageItem *value);


   signals:

      //! action was triggered
      void action(const QString&);

      //! mouse was released
      void signalMouseReleased();

      //! visibility changed
      void visible(bool enabled);


   public slots:

      //! setter for visible flag
      void setVisible(bool);

      //! item was set active
      virtual void setActive(bool);

      virtual void setFocus(bool);

      virtual void activated();

      virtual void deactivated();

      virtual void keyPressed(int key, const QString& text);

      virtual void paste(const QString& /*text*/) {}

      virtual void animate(float /*time*/) {}

      virtual void setEnabled(bool enabled);


   protected:

      //! page item type
      PageItemType mPageItemType;


      // behaviour

      QString mAction;


      // layer info

      PSDLayer* mLayerActive;

      PSDLayer* mLayerInactive;

      // states

      bool mFocussed;

      bool mInteractive;

      bool mActive;

      bool mVisible;

      bool mEnabled;


      // item tab index

      int mTabIndex;      


      // items may have a parent item

      //! parent item
      MenuPageItem* mParent;
};

#endif // MENUPAGEITEM_H
