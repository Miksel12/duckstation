#include "achievementlogindialog.h"
#include "frontend-common/cheevos.h"
#include "qthostinterface.h"
#include <QtWidgets/QMessageBox>

AchievementLoginDialog::AchievementLoginDialog(QWidget* parent) : QDialog(parent)
{
  m_ui.setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  connectUi();
}

AchievementLoginDialog::~AchievementLoginDialog() = default;

void AchievementLoginDialog::loginClicked()
{
  const std::string username(m_ui.userName->text().toStdString());
  const std::string password(m_ui.password->text().toStdString());
  if (username.empty() || password.empty())
  {
    QMessageBox::critical(this, tr("Login Error"), tr("A user name and password must be provided."));
    return;
  }

  // TODO: Make cancellable.
  m_ui.status->setText(tr("Logging in..."));
  enableUI(false);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

  bool result;
  QtHostInterface::GetInstance()->executeOnEmulationThread(
    [&username, &password, &result]() { result = Cheevos::Login(username.c_str(), password.c_str()); }, true);

  if (!result)
  {
    QMessageBox::critical(this, tr("Login Error"),
                          tr("Login failed. Please check your username and password, and try again."));
    m_ui.status->setText(tr("Login failed."));
    enableUI(true);
    return;
  }

  done(0);
}

void AchievementLoginDialog::cancelClicked()
{
  done(1);
}

void AchievementLoginDialog::connectUi()
{
  connect(m_ui.login, &QPushButton::clicked, this, &AchievementLoginDialog::loginClicked);
  connect(m_ui.cancel, &QPushButton::clicked, this, &AchievementLoginDialog::cancelClicked);
}

void AchievementLoginDialog::enableUI(bool enabled)
{
  m_ui.userName->setEnabled(enabled);
  m_ui.password->setEnabled(enabled);
  m_ui.cancel->setEnabled(enabled);
  m_ui.login->setEnabled(enabled);
}
