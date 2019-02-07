
// header
#include "menupage.h"
#include "psdlayer.h"

// menu
#include "menupageanimation.h"
#include "menupagebackgrounditem.h"
#include "menupagebuttonitem.h"
#include "menupagecheckboxitem.h"
#include "menupagecomboboxitem.h"
#include "menupageeditablecomboboxitem.h"
#include "menupagelabelitem.h"
#include "menupagelistitem.h"
#include "menupagepixmapitem.h"
#include "menupagescrollbar.h"
#include "menupagescrollimageitem.h"
#include "menupageslideritem.h"
#include "menupagetextedit.h"

// engine
#include "framebuffer.h"
#include "gldevice.h"

// Qt
#include <QGLContext>



MenuPage::MenuPage(QObject* parent)
   : QObject(parent),
     PSD(),
     mSettings(0),
     mActiveItem(0),
     mAnimation(0),
     mActive(false)
{
}


MenuPage::~MenuPage()
{
   qDeleteAll(mPageItems);
   mPageItems.clear();

   qDeleteAll(mRenderLayers);
   mRenderLayers.clear();
}


void MenuPage::initialize()
{
   // init settings
   mSettings =
      new QSettings(
         "data/menus/menu.ini",
         QSettings::IniFormat
      );

   // initialize layer information
   initializeLayers();

   // initialize page items
   initializePageItems();

   // initialize tab indices
   initializeTabIndices();
}


void MenuPage::initializeLayers()
{
   load(qPrintable(mFilename));

   // assign layers to menu page items
   for (int l = 0; l < getLayerCount(); l++)
   {
      PSD::Layer* layer = getLayer(l);

      /*
      qDebug(
         "add layer: %d: %s  opacity: %d  pos: %d,%d  size: %d,%d",
         l,
         layer->getName(),
         layer->getOpacity(),
         layer->getTop(),
         layer->getLeft(),
         layer->getWidth(),
         layer->getHeight()
      );
      */

      QString layerName= layer->getName();
      PSDLayer* renderLayer = 0;
      if (layerName.startsWith("background"))
         renderLayer= new PSDLayer( layer, -1.0f, false );
      else
         renderLayer= new PSDLayer( layer, -1.0f );

      mRenderLayers << renderLayer;
   }
}


MenuPageItem* MenuPage::processTableScrollButtons(
    PSDLayer* layer,
    QString layerName
)
{
    MenuPageItem* pageItem = 0;
    pageItem = new MenuPageItem();
    mPageItems << pageItem;

    pageItem->setInteractive(true);

    bool up = false;

    if (layerName.contains("scroll_up"))
    {
       up = true;
       pageItem->setAction("scroll_up");
    }
    else
    {
       pageItem->setAction("scroll_down");
    }

    QString baseLayer =
       QString("table_%1_main")
          .arg(layerName.split("_").at(1));

    connect(
       pageItem,
       SIGNAL(action(QString)),
       (MenuPageListItem*)mPageItemNameMap[baseLayer],
       up
          ? SLOT(scrollUp())
          : SLOT(scrollDown())
    );

    connect(
       pageItem,
       SIGNAL(signalMouseReleased()),
       (MenuPageListItem*)mPageItemNameMap[baseLayer],
       SLOT(scrollStop())
    );

    // both layers are the same
    pageItem->setActiveLayer(layer);
    pageItem->setInactiveLayer(layer);

    // store page item without postfix names
    mPageItemNameMap.insert(layerName, pageItem);

    return pageItem;
}



MenuPageItem *MenuPage::processCheckBox(
    PSDLayer* layer,
    QString layerNameWithoutPostfix,
    QString layerName
)
{
    MenuPageItem* pageItem = 0;
    layerNameWithoutPostfix = layerName;
    layerNameWithoutPostfix.remove("_yes");
    layerNameWithoutPostfix.remove("_no");

    // find page item
    if (!mPageItemNameMap.contains(layerNameWithoutPostfix))
    {
       // create a new page item
       pageItem = new MenuPageCheckBoxItem();

       // store button action
       pageItem->setAction(
          mSettings->value(layerNameWithoutPostfix).toString()
       );

       /*
       connect(
          pageItem,
          SIGNAL(action(QString)),
          this,
          SLOT(actionRequestFromItem(QString))
       );
       */

       mPageItems << pageItem;

       // store page item without postfix names
       mPageItemNameMap.insert(layerNameWithoutPostfix, pageItem);
    }
    else
    {
       // use previously assigned pageitem
       pageItem = mPageItemNameMap[layerNameWithoutPostfix];
    }

    // store
    if (layerName.endsWith("_no"))
    {
       dynamic_cast<MenuPageCheckBoxItem*>(pageItem)->setUncheckedLayer(layer);
    }
    else if (layerName.endsWith("_yes"))
    {
       dynamic_cast<MenuPageCheckBoxItem*>(pageItem)->setCheckedLayer(layer);
    }

    return pageItem;
}


MenuPageItem *MenuPage::processLabel(PSDLayer* layer, QString layerName)
{
    MenuPageItem* pageItem = 0;
    pageItem = new MenuPageLabelItem();
    mPageItems << pageItem;

    // both layers are the same
    pageItem->setActiveLayer(layer);
    pageItem->setInactiveLayer(layer);

    QString fontNameKey = QString("%1_font_name").arg(layerName);
    QString fontXOffsetKey = QString("%1_font_x_offset").arg(layerName);
    QString fontYOffsetKey = QString("%1_font_y_offset").arg(layerName);
    QString maxCharsKey = QString("%1_field_width").arg(layerName);
    QString scaleKey = QString("%1_scale").arg(layerName);
    QString colorKey = QString("%1_color").arg(layerName);
    QString alphaKey = QString("%1_alpha").arg(layerName);

    QString fontName = mSettings->value(fontNameKey, "default").toString();
    int fontXOffset = mSettings->value(fontXOffsetKey).toInt();
    int fontYOffset = mSettings->value(fontYOffsetKey).toInt();
    int maxChars = mSettings->value(maxCharsKey).toInt();
    float scale = mSettings->value(scaleKey).toFloat();
    QColor color = QColor(mSettings->value(colorKey, "#FFFFFF").toString());
    int alpha = mSettings->value(alphaKey, 255).toInt();

    // set label properties
    ((MenuPageLabelItem*)pageItem)->setFontName(fontName);
    ((MenuPageLabelItem*)pageItem)->setFontXOffset(fontXOffset);
    ((MenuPageLabelItem*)pageItem)->setFontYOffset(fontYOffset);
    ((MenuPageLabelItem*)pageItem)->setMaxChars(maxChars);
    ((MenuPageLabelItem*)pageItem)->setScale(scale);
    ((MenuPageLabelItem*)pageItem)->setColor(color);
    ((MenuPageLabelItem*)pageItem)->setAlpha(alpha);

    // store page item without postfix names
    mPageItemNameMap.insert(layerName, pageItem);

    return pageItem;
}

MenuPageItem *MenuPage::processLineEdit(
   PSDLayer* layer,
   QString layerName
)
{
    MenuPageItem* pageItem = 0;
    pageItem = new MenuPageTextEditItem();
    mPageItems << pageItem;

    // both layers are the same
    pageItem->setActiveLayer(layer);
    pageItem->setInactiveLayer(layer);

    QString fontNameKey = QString("%1_font_name").arg(layerName);
    QString fontXOffsetKey = QString("%1_font_x_offset").arg(layerName);
    QString fontYOffsetKey = QString("%1_font_y_offset").arg(layerName);
    QString fieldWidthKey = QString("%1_field_width").arg(layerName);
    QString fieldMaxLength = QString("%1_max_length").arg(layerName);
    QString scaleKey = QString("%1_scale").arg(layerName);
    QString regExpKey = QString("%1_input_regexp").arg(layerName);
    QString colorKey = QString("%1_color").arg(layerName);
    QString alphaKey = QString("%1_alpha").arg(layerName);

    QString fontName = mSettings->value(fontNameKey, "default").toString();
    int fontXOffset = mSettings->value(fontXOffsetKey).toInt();
    int fontYOffset = mSettings->value(fontYOffsetKey).toInt();
    int fieldWidth = mSettings->value(fieldWidthKey).toInt();
    int maxLength = mSettings->value(fieldMaxLength).toInt();
    float scale = mSettings->value(scaleKey).toFloat();
    QRegExp regExp = mSettings->value(regExpKey).toRegExp();
    QColor color = QColor(mSettings->value(colorKey, "#FFFFFF").toString());
    int alpha = mSettings->value(alphaKey, 255).toInt();

    // set lineedit properties
    ((MenuPageTextEditItem*)pageItem)->setFontName(fontName);
    ((MenuPageTextEditItem*)pageItem)->setFontXOffset(fontXOffset);
    ((MenuPageTextEditItem*)pageItem)->setFontYOffset(fontYOffset);
    ((MenuPageTextEditItem*)pageItem)->setFieldWidth(fieldWidth);
    ((MenuPageTextEditItem*)pageItem)->setMaxLength(maxLength);
    ((MenuPageTextEditItem*)pageItem)->setScale(scale);
    ((MenuPageTextEditItem*)pageItem)->setRegExp(regExp);
    ((MenuPageTextEditItem*)pageItem)->setColor(color);
    ((MenuPageTextEditItem*)pageItem)->setAlpha(alpha);

    // store page item without postfix names
    mPageItemNameMap.insert(layerName, pageItem);

    return pageItem;
}


MenuPageItem* MenuPage::processBackground(
   PSDLayer* layer,
   QString layerName
)
{
   bool added = false;
   MenuPageItem* pageItem = 0;

   QString baseName = QString("background_%1").arg(layerName.split('_').at(1));

   if (!mPageItemNameMap.contains(baseName))
   {
      added = true;
      pageItem = new MenuPageBackgroundItem();
      mPageItems << pageItem;
      mPageItemNameMap.insert(baseName, pageItem);
   }
   else
   {
      pageItem = mPageItemNameMap[baseName];
   }

   if (layerName.endsWith("_active"))
   {
      pageItem->setActiveLayer(layer);
      pageItem->setInactiveLayer(layer);
   }
   else if (layerName.contains("_gradient"))
   {
      MenuPageBackgroundItem::BackgroundColor color;

      if (layerName.endsWith("_red"))
         color = MenuPageBackgroundItem::BackgroundColorRed;
      else if (layerName.endsWith("_green"))
         color = MenuPageBackgroundItem::BackgroundColorGreen;
      else
         color = MenuPageBackgroundItem::BackgroundColorBlue;

      dynamic_cast<MenuPageBackgroundItem*>(pageItem)->addGradientLayer(
         layer,
         color
      );
   }

   return added ? pageItem : 0;
}


MenuPageItem *MenuPage::processPixmap(PSDLayer *layer, QString layerName)
{
   MenuPagePixmapItem* pageItem = new MenuPagePixmapItem();
   mPageItems << pageItem;

   // both layers are the same
   pageItem->setActiveLayer(layer);
   pageItem->setInactiveLayer(layer);

   // store page item without postfix names
   mPageItemNameMap.insert(layerName, pageItem);

   return pageItem;
}


MenuPageItem* MenuPage::processTableMain(
   PSDLayer* layer,
   QString layerName
)
{
    MenuPageItem* pageItem = 0;
    pageItem = new MenuPageListItem();
    mPageItems << pageItem;

    QString baseName = layerName;
    baseName.remove("_main");

    // read lineedit properties
    QString fontNameKey = QString("%1_font_name").arg(baseName);
    QString fontXOffsetKey = QString("%1_font_x_offset").arg(baseName);
    QString fontYOffsetKey = QString("%1_font_y_offset").arg(baseName);
    QString fieldWidthKey = QString("%1_field_width").arg(baseName);
    QString scaleKey = QString("%1_scale").arg(baseName);
    QString regExpKey = QString("%1_input_regexp").arg(baseName);
    QString rowHeightKey = QString("%1_row_height").arg(baseName);

    QString fontName = mSettings->value(fontNameKey, "default").toString();
    int fontXOffset = mSettings->value(fontXOffsetKey).toInt();
    int fontYOffset = mSettings->value(fontYOffsetKey).toInt();
    int fieldWidth = mSettings->value(fieldWidthKey).toInt();
    float scale = mSettings->value(scaleKey).toFloat();
    QRegExp regExp = mSettings->value(regExpKey).toRegExp();
    int rowHeight = mSettings->value(rowHeightKey).toInt();

    // set lineedit properties
    ((MenuPageListItem*)pageItem)->setFontName(fontName);
    ((MenuPageListItem*)pageItem)->setFontXOffset(fontXOffset);
    ((MenuPageListItem*)pageItem)->setFontYOffset(fontYOffset);
    ((MenuPageListItem*)pageItem)->setFieldWidth(fieldWidth);
    ((MenuPageListItem*)pageItem)->setScale(scale);
    ((MenuPageListItem*)pageItem)->setRegExp(regExp);
    ((MenuPageListItem*)pageItem)->setRowHeight(rowHeight);

    // store page item without postfix names
    mPageItemNameMap.insert(layerName, pageItem);

    // both layers are the same
    pageItem->setActiveLayer(layer);
    pageItem->setInactiveLayer(layer);

    return pageItem;
}


MenuPageItem* MenuPage::processTableScrollBar(
   PSDLayer* layer,
   QString layerName
)
{
    MenuPageItem* pageItem = 0;
    pageItem = new MenuPageItem();
    mPageItems << pageItem;

    pageItem->setInteractive(true);

//         QString baseLayer =
//            QString("table_%1_main")
//               .arg(layerName.split("_").at(1));

    // both layers are the same
    pageItem->setActiveLayer(layer);
    pageItem->setInactiveLayer(layer);

    // store page item without postfix names
    mPageItemNameMap.insert(layerName, pageItem);

    return pageItem;
}

MenuPageItem* MenuPage::processTableScrollBarSlider(PSDLayer* layer, QString layerName)
{
   MenuPageItem* pageItem = 0;
   pageItem = new MenuPageScrollbar();
   mPageItems << pageItem;

   pageItem->setInteractive(true);

   QString scrollAreaLayer =
      QString("table_%1_scrollbar")
         .arg(layerName.split("_").at(1));

   // connect slider to table
   QString baseLayer =
      QString("table_%1_main")
         .arg(layerName.split("_").at(1));

   if (!connect(
      pageItem,
      SIGNAL(scrollToPercentage(float)),
      (MenuPageListItem*)mPageItemNameMap[baseLayer],
      SLOT(scrollToPercentage(float))
   )
   )
   {
      qDebug(
         "processTableScrollBarSlider: layer '%s' missing",
         qPrintable(baseLayer)
      );
   }

   MenuPageItem* scrollbar = mPageItemNameMap[scrollAreaLayer];
   ((MenuPageScrollbar*)pageItem)->setTop(scrollbar->getCurrentLayer()->getTop());
   ((MenuPageScrollbar*)pageItem)->setHeight(scrollbar->getCurrentLayer()->getHeight());

   // connect table back to slider
   connect(
      (MenuPageListItem*)mPageItemNameMap[baseLayer],
      SIGNAL(scrollAnimation(float)),
      pageItem,
      SLOT(updateFromAnimation(float))
   );

   // both layers are the same
   pageItem->setActiveLayer(layer);
   pageItem->setInactiveLayer(layer);

   // store page item without postfix names
   mPageItemNameMap.insert(layerName, pageItem);

   return pageItem;
}


MenuPageItem* MenuPage::processSliderScrollBarIcons(
   PSDLayer* layer,
   QString layerName
)
{
    MenuPageItem* pageItem = 0;
    pageItem = new MenuPageSliderItem();
    mPageItems << pageItem;
    mPageItemNameMap.insert(layerName, pageItem);

    // both layers are the same
    pageItem->setActiveLayer(layer);
    pageItem->setInactiveLayer(layer);

    for (int l = 0; l < getLayerCount(); l++)
    {
       PSD::Layer* tmpLayer = getLayer(l);
       if (tmpLayer->getName() == QString("%1_bar").arg(layerName) )
       {
          ((MenuPageSliderItem*)pageItem)->setMinimum(
             tmpLayer->getLeft()
          );

          ((MenuPageSliderItem*)pageItem)->setMaximum(
             tmpLayer->getLeft()+tmpLayer->getWidth()
          );
       }
    }

    return pageItem;
}

MenuPageItem* MenuPage::processDefaultItem(
   PSDLayer* layer,
   QString layerName
)
{
   MenuPageItem* pageItem = 0;

   if (!layerName.startsWith("unused_"))
   {
      pageItem = new MenuPageItem();
      mPageItems << pageItem;

      // both layers are the same
      pageItem->setActiveLayer(layer);
      pageItem->setInactiveLayer(layer);

      // store page item without postfix names
      mPageItemNameMap.insert(layerName, pageItem);
   }

   return pageItem;
}


MenuPageItem* MenuPage::processButton(
   PSDLayer* layer,
   QString layerName,
   QString layerNameWithoutPostfix
)
{
   MenuPageItem* pageItem = 0;
   layerNameWithoutPostfix = layerName;
   layerNameWithoutPostfix.remove("_inactive");
   layerNameWithoutPostfix.remove("_active");

   // find page item
   if (!mPageItemNameMap.contains(layerNameWithoutPostfix))
   {
      // create a new page item
      pageItem = new MenuPageButtonItem();

      // store button action
      pageItem->setAction(
         mSettings->value(layerNameWithoutPostfix).toString()
      );

      connect(
         pageItem,
         SIGNAL(action(QString)),
         this,
         SLOT(actionRequestFromItem(QString))
      );

      mPageItems << pageItem;

      // store page item without postfix names
      mPageItemNameMap.insert(layerNameWithoutPostfix, pageItem);
   }
   else
   {
      // use previously assigned pageitem
      pageItem = mPageItemNameMap[layerNameWithoutPostfix];
   }

   // store
   if (layerName.endsWith("_inactive"))
   {
      pageItem->setInactiveLayer(layer);
   }

   else if (layerName.endsWith("_active"))
   {
      pageItem->setActiveLayer(layer);
   }

   return pageItem;
}


MenuPageItem *MenuPage::processComboBox(
   PSDLayer* layer,
   QString layerName
)
{
   MenuPageItem* pageItem = 0;
   QStringList items = layerName.split("_");
   QString itemName = items.at(1);

   // - button
   // - label
   // - table
   QString itemType = items.at(2);

   QString prefix = QString("combobox_%1_").arg(itemName);
   QString postfix = layerName.replace(prefix, "");

   // - combobox_YYY_button
   // - combobox_YYY_table
   QString baseName = QString("%1%2").arg(prefix).arg(itemType);
   QString baseNameButton = QString("%1button").arg(prefix);
   QString baseNameTable = QString("%1table").arg(prefix);
   QString baseNameLabel = QString("%1label").arg(prefix);

   // combobox_time_button_active
   // combobox_time_button_inactive
   //
   // combobox_time_table_bg_first
   // combobox_time_table_bg_last
   // combobox_time_table_bg_default
   // combobox_time_table_gradient

   // 1 menupageitem label
   // 1 menupageitem button
   // 1 menupageitem table

   /*
   qDebug(
      "baseName: %s, prefix: %s, postfix: %s: itemType: %s",
      qPrintable(baseName),
      qPrintable(prefix),
      qPrintable(postfix),
      qPrintable(itemType)
   );
   */

   // if layername ends with table
   if (itemType.toLower() == "table")
   {
      // if menupageitem not yet created
      if (!mPageItemNameMap.contains(baseName))
      {
         // combobox item
         pageItem = new MenuPageComboBoxItem();
         mPageItems << pageItem;

         MenuPageComboBoxItem::addComboBox(baseName, (MenuPageComboBoxItem*)pageItem);
         MenuPageComboBoxItem::linkComboBoxToButton(baseNameButton, baseNameTable);

         // read lineedit properties
         QString fontNameKey = QString("%1_font_name").arg(baseName);
         QString fontXOffsetKey = QString("%1_font_x_offset").arg(baseName);
         QString fontYOffsetKey = QString("%1_font_y_offset").arg(baseName);
         QString scaleKey = QString("%1_scale").arg(baseName);
         QString rowHeightKey = QString("%1_row_height").arg(baseName);

         QString fontName = mSettings->value(fontNameKey, "default").toString();
         int fontXOffset = mSettings->value(fontXOffsetKey).toInt();
         int fontYOffset = mSettings->value(fontYOffsetKey).toInt();
         float scale = mSettings->value(scaleKey).toFloat();
         int rowHeight = mSettings->value(rowHeightKey).toInt();

         // set lineedit properties
         ((MenuPageListItem*)pageItem)->setFontName(fontName);
         ((MenuPageListItem*)pageItem)->setFontXOffset(fontXOffset);
         ((MenuPageListItem*)pageItem)->setFontYOffset(fontYOffset);
         ((MenuPageListItem*)pageItem)->setScale(scale);
         ((MenuPageListItem*)pageItem)->setRowHeight(rowHeight);

         // store page item without postfix names
         mPageItemNameMap.insert(baseName, pageItem);

         // if link between combobox table and button not yet made
         MenuPageComboBoxItem::addComboBox(baseName, (MenuPageComboBoxItem*)pageItem);
         MenuPageComboBoxItem::linkComboBoxToButton(baseNameButton, baseNameTable);
      }
      else
      {
         pageItem = mPageItemNameMap[baseName];
      }

      if (postfix == "table_selected_item")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerSelectedElement(layer);
      }
      else if (postfix == "table_focussed_item")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerFocussedElement(layer);
      }
      else if (postfix == "table_bg_first")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerFirstElement(layer);
      }
      else if (postfix == "table_bg_last")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerLastElement(layer);
      }
      else if (postfix == "table_bg_default")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerDefaultElement(layer);
      }
      else if (postfix == "table_gradient")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerGradientElement(layer);

         // maybe the gradient layer is good enough as active/inactive layer
         pageItem->setActiveLayer(layer);
         pageItem->setInactiveLayer(layer);
      }
   }

   // if layername ends with button
   else if (itemType.toLower() == "button")
   {
      // create a standard button

      MenuPageItem* pageItem = 0;

      // find page item
      if (!mPageItemNameMap.contains(baseName))
      {
         // create a new page item
         pageItem = new MenuPageButtonItem();

         // store button action
         pageItem->setAction(
            mSettings->value(baseName).toString()
         );

         connect(
            pageItem,
            SIGNAL(action(QString)),
            this,
            SLOT(actionRequestFromItem(QString))
         );

         mPageItems << pageItem;

         // store page item without postfix names
         mPageItemNameMap.insert(baseName, pageItem);
      }
      else
      {
         // use previously assigned pageitem
         pageItem = mPageItemNameMap[baseName];
      }

      // store
      if (layerName.endsWith("_inactive"))
      {
         pageItem->setInactiveLayer(layer);
      }

      else if (layerName.endsWith("_active"))
      {
         pageItem->setActiveLayer(layer);
      }

      // if link between combobox table and button not yet made
      MenuPageComboBoxItem::addButton(baseName, (MenuPageButtonItem*)pageItem);
      MenuPageComboBoxItem::linkComboBoxToButton(baseNameButton, baseNameTable);
   }

   // if layername ends with label
   else if (itemType.toLower() == "label")
   {
      pageItem = new MenuPageLabelItem();
      mPageItems << pageItem;

      // both layers are the same
      pageItem->setActiveLayer(layer);
      pageItem->setInactiveLayer(layer);

      QString fontNameKey = QString("%1_font_name").arg(baseName);
      QString fontXOffsetKey = QString("%1_font_x_offset").arg(baseName);
      QString fontYOffsetKey = QString("%1_font_y_offset").arg(baseName);
      QString scaleKey = QString("%1_scale").arg(baseName);

      QString fontName = mSettings->value(fontNameKey, "default").toString();
      int fontXOffset = mSettings->value(fontXOffsetKey).toInt();
      int fontYOffset = mSettings->value(fontYOffsetKey).toInt();
      float scale = mSettings->value(scaleKey).toFloat();

      // set label properties
      ((MenuPageLabelItem*)pageItem)->setFontName(fontName);
      ((MenuPageLabelItem*)pageItem)->setFontXOffset(fontXOffset);
      ((MenuPageLabelItem*)pageItem)->setFontYOffset(fontYOffset);
      ((MenuPageLabelItem*)pageItem)->setScale(scale);

      // store page item without postfix names
      mPageItemNameMap.insert(baseName, pageItem);

      // if link between combobox table and button not yet made
      MenuPageComboBoxItem::addLabel(baseName, (MenuPageLabelItem*)pageItem);
      MenuPageComboBoxItem::linkComboBoxToLabel(baseNameLabel, baseNameTable);
   }

   return pageItem;
}


MenuPageItem *MenuPage::processEditableComboBox(
   PSDLayer* layer,
   QString layerName
)
{
   MenuPageItem* pageItem = 0;
   QStringList items = layerName.split("_");
   QString itemName = items.at(1);

   // - button
   // - label
   // - table
   QString itemType = items.at(2);

   QString prefix = QString("editablecombobox_%1_").arg(itemName);
   QString postfix = layerName.replace(prefix, "");

   // - combobox_YYY_button
   // - combobox_YYY_table
   QString baseName = QString("%1%2").arg(prefix).arg(itemType);
   QString baseNameButton = QString("%1button").arg(prefix);
   QString baseNameTable = QString("%1table").arg(prefix);
   QString baseNameLineEdit = QString("%1lineedit").arg(prefix);

   // combobox_time_button_active
   // combobox_time_button_inactive
   //
   // combobox_time_table_bg_first
   // combobox_time_table_bg_last
   // combobox_time_table_bg_default
   // combobox_time_table_gradient

   // 1 menupageitem label
   // 1 menupageitem button
   // 1 menupageitem table

   /*
   qDebug(
      "baseName: %s, prefix: %s, postfix: %s: itemType: %s",
      qPrintable(baseName),
      qPrintable(prefix),
      qPrintable(postfix),
      qPrintable(itemType)
   );
   */

   // if layername ends with table
   if (itemType.toLower() == "table")
   {
      // if menupageitem not yet created
      if (!mPageItemNameMap.contains(baseName))
      {
         // combobox item
         pageItem = new MenuPageEditableComboBoxItem();
         mPageItems << pageItem;

         MenuPageComboBoxItem::addComboBox(baseName, (MenuPageComboBoxItem*)pageItem);
         MenuPageComboBoxItem::linkComboBoxToButton(baseNameButton, baseNameTable);

         // read lineedit properties
         QString fontNameKey = QString("%1_font_name").arg(baseName);
         QString fontXOffsetKey = QString("%1_font_x_offset").arg(baseName);
         QString fontYOffsetKey = QString("%1_font_y_offset").arg(baseName);
         QString scaleKey = QString("%1_scale").arg(baseName);
         QString rowHeightKey = QString("%1_row_height").arg(baseName);

         QString fontName = mSettings->value(fontNameKey, "default").toString();
         int fontXOffset = mSettings->value(fontXOffsetKey).toInt();
         int fontYOffset = mSettings->value(fontYOffsetKey).toInt();
         float scale = mSettings->value(scaleKey).toFloat();
         int rowHeight = mSettings->value(rowHeightKey).toInt();

         // set lineedit properties
         ((MenuPageListItem*)pageItem)->setFontName(fontName);
         ((MenuPageListItem*)pageItem)->setFontXOffset(fontXOffset);
         ((MenuPageListItem*)pageItem)->setFontYOffset(fontYOffset);
         ((MenuPageListItem*)pageItem)->setScale(scale);
         ((MenuPageListItem*)pageItem)->setRowHeight(rowHeight);

         // store page item without postfix names
         mPageItemNameMap.insert(baseName, pageItem);

         // if link between combobox table and button not yet made
         MenuPageComboBoxItem::addComboBox(baseName, (MenuPageComboBoxItem*)pageItem);
         MenuPageComboBoxItem::linkComboBoxToButton(baseNameButton, baseNameTable);
      }
      else
      {
         pageItem = mPageItemNameMap[baseName];
      }

      if (postfix == "table_selected_item")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerSelectedElement(layer);
      }
      else if (postfix == "table_focussed_item")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerFocussedElement(layer);
      }
      else if (postfix == "table_bg_first")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerFirstElement(layer);
      }
      else if (postfix == "table_bg_last")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerLastElement(layer);
      }
      else if (postfix == "table_bg_default")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerDefaultElement(layer);
      }
      else if (postfix == "table_gradient")
      {
         ((MenuPageComboBoxItem*)pageItem)->setLayerGradientElement(layer);

         // maybe the gradient layer is good enough as active/inactive layer
         pageItem->setActiveLayer(layer);
         pageItem->setInactiveLayer(layer);
      }
   }

   // if layername ends with button
   else if (itemType.toLower() == "button")
   {
      // create a standard button

      MenuPageItem* pageItem = 0;

      // find page item
      if (!mPageItemNameMap.contains(baseName))
      {
         // create a new page item
         pageItem = new MenuPageButtonItem();

         // store button action
         pageItem->setAction(
            mSettings->value(baseName).toString()
         );

         connect(
            pageItem,
            SIGNAL(action(QString)),
            this,
            SLOT(actionRequestFromItem(QString))
         );

         mPageItems << pageItem;

         // store page item without postfix names
         mPageItemNameMap.insert(baseName, pageItem);
      }
      else
      {
         // use previously assigned pageitem
         pageItem = mPageItemNameMap[baseName];
      }

      // store
      if (layerName.endsWith("_inactive"))
      {
         pageItem->setInactiveLayer(layer);
      }

      else if (layerName.endsWith("_active"))
      {
         pageItem->setActiveLayer(layer);
      }

      // if link between combobox table and button not yet made
      MenuPageComboBoxItem::addButton(baseName, (MenuPageButtonItem*)pageItem);
      MenuPageComboBoxItem::linkComboBoxToButton(baseNameButton, baseNameTable);
   }

   // if layername ends with lineedit
   else if (itemType.toLower() == "lineedit")
   {
      pageItem = new MenuPageTextEditItem();
      mPageItems << pageItem;

      // both layers are the same
      pageItem->setActiveLayer(layer);
      pageItem->setInactiveLayer(layer);

      QString fontNameKey = QString("%1_font_name").arg(baseName);
      QString fontXOffsetKey = QString("%1_font_x_offset").arg(baseName);
      QString fontYOffsetKey = QString("%1_font_y_offset").arg(baseName);
      QString fieldWidthKey = QString("%1_field_width").arg(baseName);
      QString fieldMaxLength = QString("%1_max_length").arg(baseName);
      QString scaleKey = QString("%1_scale").arg(baseName);
      QString regExpKey = QString("%1_input_regexp").arg(baseName);
      QString colorKey = QString("%1_color").arg(baseName);
      QString alphaKey = QString("%1_alpha").arg(baseName);

      QString fontName = mSettings->value(fontNameKey, "default").toString();
      int fontXOffset = mSettings->value(fontXOffsetKey).toInt();
      int fontYOffset = mSettings->value(fontYOffsetKey).toInt();
      int fieldWidth = mSettings->value(fieldWidthKey).toInt();
      int maxLength = mSettings->value(fieldMaxLength).toInt();
      float scale = mSettings->value(scaleKey).toFloat();
      QRegExp regExp = mSettings->value(regExpKey).toRegExp();
      QColor color = QColor(mSettings->value(colorKey, "#FFFFFF").toString());
      int alpha = mSettings->value(alphaKey, 255).toInt();

      // set lineedit properties
      ((MenuPageTextEditItem*)pageItem)->setFontName(fontName);
      ((MenuPageTextEditItem*)pageItem)->setFontXOffset(fontXOffset);
      ((MenuPageTextEditItem*)pageItem)->setFontYOffset(fontYOffset);
      ((MenuPageTextEditItem*)pageItem)->setFieldWidth(fieldWidth);
      ((MenuPageTextEditItem*)pageItem)->setMaxLength(maxLength);
      ((MenuPageTextEditItem*)pageItem)->setScale(scale);
      ((MenuPageTextEditItem*)pageItem)->setRegExp(regExp);
      ((MenuPageTextEditItem*)pageItem)->setColor(color);
      ((MenuPageTextEditItem*)pageItem)->setAlpha(alpha);

      // store page item without postfix names
      mPageItemNameMap.insert(baseName, pageItem);

      // if link between combobox table and button not yet made
      MenuPageEditableComboBoxItem::addTextEdit(baseName, (MenuPageTextEditItem*)pageItem);
      MenuPageEditableComboBoxItem::linkComboBoxToTextEdit(baseNameLineEdit, baseNameTable);
   }

   return pageItem;
}


MenuPageItem* MenuPage::processScrollImage(
   PSDLayer* layer,
   QString layerName
)
{
   QString baseName;
   MenuPageScrollImageItem* sci = 0;
   bool clipRect = false;
   bool complete = false;

   // determin basename
   //
   // image_scroll_cliprect
   // image_scroll
   if (layerName.contains("_cliprect"))
   {
      baseName = layerName.replace("_cliprect", "");
      clipRect = true;
   }
   else
   {
      baseName = layerName;
   }

   // look up item
   if (mPageItemNameMap.contains(baseName))
   {
      sci = dynamic_cast<MenuPageScrollImageItem*>(mPageItemNameMap[baseName]);

      // page item is now complete and can be initialized
      complete = true;
   }
   else
   {
      sci = new MenuPageScrollImageItem();
      mPageItemNameMap.insert(baseName, sci);
      mPageItems << sci;
   }

   if (clipRect)
   {
      // we use the inactive layer as clipping layer
      sci->setInactiveLayer(layer);
   }
   else
   {
      sci->setActiveLayer(layer);
   }

   return complete ? sci : 0;
}


void MenuPage::initializePageItems()
{
   // TODO: move those process-functions into the page items
   //       => static function

   // start reading settings
   mSettings->beginGroup(mTitle);

   QString layerNameWithoutPostfix;
   QString layerName;
   MenuPageItem* pageItem = 0;

   for (int l = 0; l < getLayerCount(); l++)
   {
      PSDLayer* layer= mRenderLayers[l];
      layerName = QString(getLayer(l)->getName()).trimmed();
      layerNameWithoutPostfix.clear();

      pageItem = 0;

      // combobox
      if (layerName.startsWith("combobox"))
      {
         pageItem = processComboBox(layer, layerName);
      }

      else if (layerName.startsWith("editablecombobox"))
      {
         pageItem = processEditableComboBox(layer, layerName);
      }

      // initialize checkboxes
      else if (layerName.startsWith("checkbox"))
      {
         pageItem = processCheckBox(layer, layerNameWithoutPostfix, layerName);
      }

      // initialize buttons
      else if (layerName.startsWith("button"))
      {
         pageItem = processButton(layer, layerName, layerNameWithoutPostfix);
      }

      else if (layerName.startsWith("label"))
      {
         // create a new page item
         pageItem = processLabel(layer, layerName);
      }

      // initialize lineedits
      else if (layerName.startsWith("lineedit"))
      {
         // create a new page item
         pageItem = processLineEdit(layer, layerName);
      }

      // initialize backgrounds
      else if (layerName.startsWith("background"))
      {
         // create a new page item
         pageItem = processBackground(layer, layerName);
      }

      // initialize pixmaps
      else if (layerName.startsWith("pixmap"))
      {
         // create a new page item
         pageItem = processPixmap(layer, layerName);
      }

      else if (
            layerName.startsWith("table_")
         && layerName.endsWith("_main")
      )
      {
         // create a new page item
         pageItem = processTableMain(layer, layerName);
      }

      else if (
            layerName.startsWith("table_")
         && (
               layerName.contains("_scroll_up")
            || layerName.contains("_scroll_down")
         )
      )
      {
         // create a new page item
         pageItem = processTableScrollButtons(layer, layerName);
      }

      else if (
            layerName.startsWith("table_")
         && layerName.endsWith("_scrollbar")
      )
      {
         // create a new page item
         pageItem = processTableScrollBar(layer, layerName);
      }

      else if (
            layerName.startsWith("table_")
         && layerName.endsWith("_scroll_slider")
      )
      {
         // create a new page item
         pageItem = processTableScrollBarSlider(layer, layerName);
      }

      else if (
             layerName.startsWith("slider_")
         &&! layerName.endsWith("_bar")
         &&! layerName.endsWith("_icons")
      )
      {
         // create new slider
         pageItem = processSliderScrollBarIcons(layer, layerName);
      }

      else if (layerName.startsWith("image_scroll"))
      {
         pageItem = processScrollImage(layer, layerName);
      }

      else
      {
         // create a new page item
         pageItem = processDefaultItem(layer, layerName);
      }

      if (pageItem)
      {
         pageItem->initialize();
      }
   }

   // activate default item
   QString defaultItem = mSettings->value("default").toString();
   if (mPageItemNameMap.contains(defaultItem))
   {
      mActiveItem = mPageItemNameMap[defaultItem];
      mActiveItem->activated();
      mActiveItem->setFocus(true);
   }

   mSettings->endGroup();
}


void MenuPage::initializeTabIndices()
{
   mSettings->beginGroup(mTitle);

   QStringList indexItems = mSettings->value("tabindices").toStringList();

   if (!indexItems.isEmpty())
   {
      int index = 0;
      foreach (QString key, indexItems)
      {
         if (mPageItemNameMap.contains(key))
         {
            mPageItemNameMap[key]->setTabIndex(index);
            index++;
         }
      }
   }

   mSettings->endGroup();
}


void MenuPage::tabPressed()
{
   qDebug("MenuPage::tabPressed()");

   int tabIndex = -1;

   // get current tab index
   if (mActiveItem)
   {
      tabIndex = mActiveItem->getTabIndex();
   }

   MenuPageItem* nextFocusItem = 0;

   // find greater tab index
   foreach (MenuPageItem* item, mPageItems)
   {
      if (item->getTabIndex() > tabIndex)
      {
         tabIndex = item->getTabIndex();
         nextFocusItem = item;
         break;
      }
   }

   // if no greater tab index found, continue with 0
   if (!nextFocusItem)
   {
      foreach (MenuPageItem* item, mPageItems)
      {
         if (item->getTabIndex() == 0)
         {
            tabIndex = item->getTabIndex();
            nextFocusItem = item;
            break;
         }
      }
   }

   if (nextFocusItem)
   {
      if (
             mActiveItem
          && mActiveItem != nextFocusItem)
      {
         mActiveItem->deactivated();
         mActiveItem = nextFocusItem;
         mActiveItem->activated();
      }
   }
   else
   {
      qDebug("MenuPage::tabPressed: nothing to focus");
   }
}


MenuPageItem *MenuPage::getActiveItem() const
{
   return mActiveItem;
}


void MenuPage::setActiveItem(MenuPageItem *value)
{
   mActiveItem = value;
}


QList<MenuPageItem*> MenuPage::getItemsAt(int x, int y) const
{
   QList<MenuPageItem*> items;
   MenuPageItem* itemAtPos = 0;
   PSD::Layer* layer = 0;

   foreach(MenuPageItem* item, mPageItems)
   {
      if (
            item
         && item->isInteractive()
      )
      {
         layer = item->getCurrentLayer();

         if (
               x > layer->getLeft()
            && x < layer->getLeft() + layer->getWidth()
            && y > layer->getTop()
            && y < layer->getTop() + layer->getHeight()
         )
         {
            itemAtPos = item;
            items.push_back(itemAtPos);
         }
      }
   }

   return items;
}


MenuPageItem *MenuPage::getFocussedItem() const
{
   MenuPageItem* focussedItem = 0;

   foreach(MenuPageItem* item, mPageItems)
   {
      if (item->isFocussed())
      {
         focussedItem = item;
         break;
      }
   }

   return focussedItem;
}


void MenuPage::mouseMoved(int x, int y)
{
   // check for layer collisions
   PSD::Layer* layer = 0;

   foreach(MenuPageItem* item, mPageItems)
   {
      if (
            item
         && item->isInteractive()
      )
      {
         layer = item->getCurrentLayer();

         if (
               x > layer->getLeft()
            && x < layer->getLeft() + layer->getWidth()
            && y > layer->getTop()
            && y < layer->getTop() + layer->getHeight()
         )
         {
            if (!item->isFocussed())
            {
               // qDebug("layer: '%s' has focus", layer->getName());

               if (item->isEnabled())
                  emit layerFocussed(mFilename, layer->getName());

               item->setFocus(true);
            }

            if (item->hasNestedElements())
            {
               item->mouseMoved(x, y);
            }
         }
         else
         {
            if (item->isFocussed())
            {
               // qDebug("layer: '%s' lost focus", layer->getName());
               item->setFocus(false);
            }
         }

         // item is reading global mouse events
         if (item->isGrabbingMouseEvents())
         {
            if (item->isActive())
            {
               item->mouseMoved(x, y);
            }
         }
      }
   }
}


void MenuPage::mousePressed(int x, int y)
{
   PSD::Layer* layer = 0;
   bool focusSet = false;

   QList<MenuPageItem*> clickedItems;

   foreach(MenuPageItem* item, mPageItems)
   {
      if (
            item
         && item->isInteractive()
      )
      {
         layer = item->getCurrentLayer();

         if (
               x > layer->getLeft()
            && x < layer->getLeft() + layer->getWidth()
            && y > layer->getTop()
            && y < layer->getTop() + layer->getHeight()
         )
         {
            clickedItems << item;
         }
      }
   }

   foreach (MenuPageItem* item, clickedItems)
   {
      if (item->isVisible() && item->isModal())
      {
         clickedItems.clear();
         clickedItems << item;
         break;
      }
   }

   foreach (MenuPageItem* item, clickedItems)
   {
      layer = item->getCurrentLayer();

      qDebug(
         "page: '%s', layer: '%s', item '%p' clicked",
         qPrintable(mFilename),
         layer->getName(),
         item
      );

      if (item->isActionRequestOnClickEnabled())
      {
         emit actionRequest(
            mFilename,
            layer->getName()
         );
      }

      item->activated();
      item->mousePressed(x, y);

      focusSet = true;

      if (mActiveItem != item)
      {
         if (mActiveItem)
            mActiveItem->deactivated();

         mActiveItem = item;
      }
   }

   if (!focusSet)
   {
      if (mActiveItem)
         mActiveItem->deactivated();

      mActiveItem = 0;
   }
}


void MenuPage::paste(const QString &text)
{
   if (mActiveItem)
   {
      mActiveItem->paste(text);
   }
}


void MenuPage::mouseReleased()
{
   foreach(MenuPageItem* item, mPageItems)
   {
      if (
            item
         && item->isInteractive()
      )
      {
         item->mouseReleased();
      }
   }
}


void MenuPage::setTitle(const QString& title)
{
   mTitle = title;
}


void MenuPage::setFilename(const QString& filename)
{
   mFilename = filename;
}


QList<MenuPageItem*>* MenuPage::getPageItems()
{
   return &mPageItems;
}


MenuPageItem* MenuPage::getPageItem(const QString& layerName) const
{
   MenuPageItem* item = 0;

   QMap<QString, MenuPageItem*>::const_iterator it =
      mPageItemNameMap.find(layerName);

   if (it != mPageItemNameMap.end())
      item = it.value();

   return item;
}


void MenuPage::keyPressed(int key, const QString& text)
{
   // check if any item has focus
   if (mActiveItem)
   {
      if (key == Qt::Key_Tab)
      {
         tabPressed();
      }
      else
      {
         mActiveItem->keyPressed(key, text);

         if (key == Qt::Key_Return || key == Qt::Key_Enter)
         {
            emit actionRequest(
               mFilename,
               mActiveItem->getCurrentLayer()->getName()
            );
         }

         // in any case notify workflow a key was pressed
         emit actionKeyPressed(
            mFilename,
            mActiveItem->getCurrentLayer()->getName(),
            key
         );
      }
   }
}


void MenuPage::setActive(bool active)
{
   mActive = active;
}


bool MenuPage::isActive()
{
   return mActive;
}


QString MenuPage::getFilename() const
{
   return mFilename;
}


void MenuPage::setAnimation(MenuPageAnimation* animation)
{
   mAnimation = animation;
}


MenuPageAnimation* MenuPage::getAnimation()
{
   return mAnimation;
}


void MenuPage::deactivate()
{
   setActive(false);
}


void MenuPage::resetAnimation()
{
   mAnimation = 0;
}


void MenuPage::unFocusAllItems()
{
   foreach(MenuPageItem* item, mPageItems)
   {
      item->setFocus(false);
   }
}


void MenuPage::render()
{
   foreach(MenuPageItem* item, mPageItems)
   {
      item->draw();
   }
}


void MenuPage::actionRequestFromItem(
   const QString& request
)
{
   emit actionRequest(mFilename, request);
}

