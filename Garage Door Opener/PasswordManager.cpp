#include "PasswordManager.h"
#include <EEPROM\EEPROM.h>
#include "Libraries\Streaming\Streaming.h"
#include "HardwareConfiguration.h"



CPasswordManager::CPasswordManager(unsigned uBaseAddress, unsigned uPasswordLength, unsigned uMaxPasswords)
  : c_uBaseAddress(uBaseAddress), c_uPasswordLength(uPasswordLength), c_uMaxPasswords(uMaxPasswords)
{
}

/*
Initialize the password store, wiping any exisiting passwords preparing
eeprom to receive new passwords. 
*/
void CPasswordManager::InitializePasswordStore()
{
  unsigned uCurrent, uCount;

  uCount = c_uPasswordLength * c_uMaxPasswords;
  uCurrent = c_uBaseAddress;

  while (uCount--)
    EEPROM.write(uCurrent++, 0);

  Serial.println(F("Password store initialized"));
}

/*
Checks the supplied password against all the stored passwords. 
Returns true if a match is found, false otherwise. 
*/
bool CPasswordManager::IsPasswordValid(const char *pchTest, bool bCaseSensitive)
{
  const char *pchCurrent;
  int iPassword;
  int nAddress, nCharChecked;
  bool bPasswordMatched = false;
  char chTest, chReference;

  for (iPassword = 0; iPassword < c_uMaxPasswords; ++iPassword)
  {
    nAddress = iPassword * c_uPasswordLength + c_uBaseAddress;
    pchCurrent = pchTest;
    for (nCharChecked = 0; nCharChecked < c_uPasswordLength; ++nCharChecked)
    {
      chReference = EEPROM.read(nAddress++);
      chTest = *pchCurrent++;

      if (!bCaseSensitive)
      {
        chTest = tolower(chTest);
        chReference = tolower(chReference);
      }

      if (chTest == '\0' && chReference == '\0')
        if (nCharChecked > 0)
        {
          // Allow access without a new password for a little while. 
          m_bValidPasswordRecently = true;
          m_uLastValidPasswordTime = millis();

          // Password was good!
          return true;
        }
        else
          return false;

      if (chTest != chReference)
        break; // This password won't match. Try the next one. 
    }
  }

  return false;
}

/*
Lists passwords on the serial port. 
*/
void CPasswordManager::ListPasswords()
{
  int iPassword;
  int nAddress, iPasswordCharacter;
  char chPassword;

  Serial.println(F("Password List"));
  Serial.println(F("-------------"));

  for (iPassword = 0; iPassword < c_uMaxPasswords; ++iPassword)
  {
    nAddress = iPassword * c_uPasswordLength + c_uBaseAddress;
    Serial.print(iPassword + 1);
    Serial.print(F(". "));

    for (iPasswordCharacter = 0; iPasswordCharacter < c_uPasswordLength; ++iPasswordCharacter)
    {
      chPassword = EEPROM.read(nAddress++);
      if (chPassword == '\0')
      {
        if (iPasswordCharacter == 0)
          Serial.print(F("     <none>"));
        break;
      }
      Serial.print(chPassword);
    }
    Serial.println();
  }
}

/*
Sets the password in the slot provided, saving it into the eeprom.
*/
void CPasswordManager::SetPassword(int nPassword, const char *pchNewPassword)
{
  int nAddress, iPasswordCharacter;
  char chPassword;
  const char *pchPassword;

  // Check password slot. 
  if (nPassword < 1 || nPassword > c_uMaxPasswords)
  {
    Serial << F("Unable to set password ") << nPassword << endl;
    Serial << F("Only passwords between 1 and ") << c_uMaxPasswords << F(" can be set.") << endl;
    return;
  }

  // Check password length. 
  for (iPasswordCharacter = 0, pchPassword = pchNewPassword;
    iPasswordCharacter < (c_uPasswordLength - 1) && *pchPassword != '\0'; 
    ++iPasswordCharacter, ++pchPassword)
  {
    // Just counting length. 
  }

  if (iPasswordCharacter >= (c_uPasswordLength - 1))
  {
    Serial << F("Password is too long. Only ") << (c_uPasswordLength - 1) << F(" characters allowed.") << endl;
    return;
  }

  // Save password. 
  nAddress = (nPassword - 1) * c_uPasswordLength + c_uBaseAddress;
  for (iPasswordCharacter = 0; iPasswordCharacter < c_uPasswordLength; ++iPasswordCharacter)
  {
    chPassword = *pchNewPassword;
    if (chPassword != '\0')
      ++pchNewPassword;
    EEPROM.write(nAddress++, chPassword);
  }

  Serial.println(F("Password saved."));
}

/*
Clears the password from the eeprom slot specified. 
*/
void CPasswordManager::ClearPassword(int nPassword)
{
  int nAddress, iPasswordCharacter;
  char chPassword;
  const char *pchPassword;

  // Check password slot. 
  if (nPassword < 1 || nPassword > c_uMaxPasswords)
  {
    Serial << F("Unable to clear password ") << nPassword << endl;
    Serial << F("Only passwords between 1 and ") << c_uMaxPasswords << F(" can be cleared.") << endl;
    return;
  }

  // Clear password. 
  nAddress = (nPassword - 1) * c_uPasswordLength + c_uBaseAddress;
  for (iPasswordCharacter = 0; iPasswordCharacter < c_uPasswordLength; ++iPasswordCharacter)
    EEPROM.write(nAddress++, '\0');

  Serial.println(F("Password cleared."));
}

/*
Determines if a valid password was entered recently. 
*/
bool CPasswordManager::HasValidPassword()
{
  return m_bValidPasswordRecently && (millis() - m_uLastValidPasswordTime < PASSWORD_GRACE_PERIOD);
}

/*
Check if the period for grace password use has expried yet. 
*/
void CPasswordManager::CheckGracePeriod()
{
  // Expires password if grace period has ellapsed. 
  if (m_bValidPasswordRecently && (millis() - m_uLastValidPasswordTime > PASSWORD_GRACE_PERIOD))
    m_bValidPasswordRecently = false;
}

/*
Forceably expires grace login period. 
*/
void CPasswordManager::ClearGracePassword()
{
  m_bValidPasswordRecently = false;
}
