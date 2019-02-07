#include "menuworkflow.h"

#include "menuinterface.h"


MenuWorkflow::MenuWorkflow(QObject *parent) :
    QObject(parent),
    mMenuInterface(0)
{
}


void MenuWorkflow::initialize()
{
}


const QString &MenuWorkflow::getCurrentPage() const
{
   return mCurrentPage;
}


MenuInterface *MenuWorkflow::getInterface() const
{
   return mMenuInterface;
}



void MenuWorkflow::actionRequest(
   const QString& /*page*/,
   const QString& /*action*/
)
{
}


void MenuWorkflow::menuVisible(bool /*visible*/)
{
}


void MenuWorkflow::pageChanged(const QString & page)
{
   setCurrentPage(page);
   emit currentPageChanged(getCurrentPage());
}


void MenuWorkflow::setCurrentPage(const QString &page)
{
   mCurrentPage = page;
}
