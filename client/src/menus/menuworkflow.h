#ifndef MENUWORKFLOW_H
#define MENUWORKFLOW_H

#include <QObject>

class MenuInterface;

class MenuWorkflow : public QObject
{
    Q_OBJECT

   public:

      //! constructor
      explicit MenuWorkflow(QObject *parent = 0);

      //! initializer
      virtual void initialize();

      //! getter for current page
      const QString& getCurrentPage() const;

      //! getter for interface
      MenuInterface* getInterface() const;


   public slots:

      // workflow

      //! action request (if required goes to interface)
      virtual void actionRequest(
         const QString& page,
         const QString& action
      );

      //! menu is visible or not
      virtual void menuVisible(bool visible);

      //! page was changed
      virtual void pageChanged(const QString&);


   signals:

      // workflow

      //! action response (if required from interface)
      void actionResponse(
         const QString& page,
         const QString& action,
         bool ok
      );

      //! page change is requested by workflow
      void pageChangeRequest(const QString& pageName);

      //! current page changed
      void currentPageChanged(const QString& pageName);


   protected:

      //! setter for current page
      void setCurrentPage(const QString& page);

      //! menu interface to game
      MenuInterface* mMenuInterface;

      //! current page
      QString mCurrentPage;

};

#endif // MENUWORKFLOW_H
