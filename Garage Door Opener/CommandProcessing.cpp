#include "CommandProcessing.h"
#include "PasswordManager.h"
#include "Libraries\SerialCommand\SerialCommand.h"

// ------------------ Serial command handler object.
SerialCommand CommandDispatcher;

// ------------------ Command handler prototypes
void InitializePasswordStore();
void ListPasswords();
void SetPassword();
void ClearPassword();


// ------------------ Command processing. 

void InitializeCommandProcessing()
{
  CommandDispatcher.addCommand("init-pwd", InitializePasswordStore);
  CommandDispatcher.addCommand("list-pwd", ListPasswords);
  CommandDispatcher.addCommand("set-pwd", SetPassword);
}


void ProcessSerialInput()
{
  CommandDispatcher.readSerial();
}

// ------------------ Command handlers

void InitializePasswordStore()
{
  PasswordManager.InitializePasswordStore();
}

void ListPasswords()
{
  PasswordManager.ListPasswords();
}

void SetPassword()
{
  char *pchSlot, *pchPassword;
  int nPassword;

  pchSlot = CommandDispatcher.next();
  pchPassword = CommandDispatcher.next();
  if (pchSlot == NULL || pchPassword == NULL)
  {
    Serial.println(F("Error, format is: set-pwd n new-password\\r"));
    return;
  }
  nPassword = atoi(pchSlot);
  PasswordManager.SetPassword(nPassword, pchPassword);
}

void ClearPassword()
{
  char *pchSlot;
  int nPassword;

  pchSlot = CommandDispatcher.next();
  if (pchSlot == NULL)
  {
    Serial.println(F("Error, format is: clear-pwd n\\r"));
    return;
  }
  nPassword = atoi(pchSlot);
  PasswordManager.ClearPassword(nPassword);
}
