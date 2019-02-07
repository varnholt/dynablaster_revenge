#ifndef MENUPAGE_H
#define MENUPAGE_H

// Qt
#include <QImage>
#include <QList>
#include <QObject>
#include <QSettings>

// menus
#include "image/psd.h"

// forward declarations
class MenuPageItem;
class MenuPageAnimation;
class PSDLayer;

class MenuPage : public QObject, public PSD
{
   Q_OBJECT

   public:

      MenuPage(QObject* parent = 0);

      virtual ~MenuPage();

      void setTitle(const QString&);

      void setFilename(const QString&);

      QString getFilename() const;

      void initialize();

      QList<MenuPageItem*>* getPageItems();

      void setActive(bool);

      bool isActive();

      void setAnimation(MenuPageAnimation*);

      MenuPageAnimation* getAnimation();

      MenuPageItem* getPageItem(const QString& layerName) const;

      MenuPageItem* processTableScrollButtons(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processCheckBox(
         PSDLayer* layer,
         QString layerNameWithoutPostfix,
         QString layerName
      );

      MenuPageItem* processLabel(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processLineEdit(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processBackground(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processPixmap(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processTableMain(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processTableScrollBar(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processTableScrollBarSlider(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processSliderScrollBarIcons(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processScrollImage(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processDefaultItem(
         PSDLayer* layer,
         QString layerName
      );

      MenuPageItem* processButton(
         PSDLayer* layer,
         QString layerName,
         QString layerNameWithoutPostfix
      );

      MenuPageItem* processComboBox(
         PSDLayer *layer,
         QString layerName
      );

      MenuPageItem* processEditableComboBox(
         PSDLayer *layer,
         QString layerName
      );

      //! getter for the active item
      MenuPageItem *getActiveItem() const;

      //! setter for the active item
      void setActiveItem(MenuPageItem *value);

      //! getter for pageitem at given position
      QList<MenuPageItem*> getItemsAt(int x, int y) const;

      //! getter for the focussed item
      MenuPageItem *getFocussedItem() const;


signals:

      //! send out an action request
      void actionRequest(
         const QString& page,
         const QString& actionRequest
      );

      //! a key was pressed while an item was focussed
      void actionKeyPressed(
         const QString& page,
         const QString& itemName,
         int key
      );

      //! a layer has been focussed
      void layerFocussed(
         const QString& page,
         const QString& itemName
      );


    public slots:

       void mouseMoved(int x, int y);

       void mousePressed(int x, int y);

       void paste(const QString& text);

       void mouseReleased();

       void keyPressed(int key, const QString& text);

       void deactivate();

       void resetAnimation();

       void unFocusAllItems();

       //! render to framebuffer
       void render();


   protected slots:

       //! send out an action request
       void actionRequestFromItem(
          const QString& actionRequest
       );


   protected:

      //! initialize layers
      void initializeLayers();

      //! initialize page items
      void initializePageItems();

      //! initialize tab indices
      void initializeTabIndices();

      //! tab pressed
      void tabPressed();

      // page item information

      QList<MenuPageItem*> mPageItems;

      QMap<QString, MenuPageItem*> mPageItemNameMap;

      QList<PSDLayer*> mRenderLayers;


      // page information

      QString mTitle;

      QString mFilename;

      QSettings* mSettings;

      //! focussed item
      MenuPageItem* mActiveItem;

      //! page animation
      MenuPageAnimation* mAnimation;

      //! page is active
      bool mActive;
};

#endif // MENUPAGE_H
