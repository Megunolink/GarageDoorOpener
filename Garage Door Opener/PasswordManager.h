/* ************************************************************************************************
** Password manager checks and updates passwords in the eeprom. 
** Passwords are stored in fixed size records and are checked directly against the value read
** from the eeprom. 
** ************************************************************************************************ */

#pragma once
#include <Arduino.h>

class CPasswordManager
{
  // Base address for storing passwords in the eeprom. 
  const unsigned c_uBaseAddress;

  // Size of each password, including a null terminator. 
  const unsigned c_uPasswordLength;

  // Limit on the number of passwords stored. 
  const unsigned c_uMaxPasswords;

  // If the user has recently supplied a valid password, they
  // can activate the door again without supplying a new password
  // for a short time. 
  uint32_t m_uLastValidPasswordTime; // millis timer when last password was received
  bool m_bValidPasswordRecently;     // True if we had a valid password recently (the grace period hasn't expired). 
  
  enum EConstants 
  { 
    PASSWORD_GRACE_PERIOD = 5 * 60 * 1000 // [ms]
  };

public:
  CPasswordManager(unsigned uBaseAddress, unsigned uPasswordLength, unsigned uMaxPasswords);

  void InitializePasswordStore();
  bool IsPasswordValid(const char *pchTest, bool bCaseSensitive);
  void ListPasswords();
  void SetPassword(int nPassword, const char *pchNewPassword);
  void ClearPassword(int nPassword);

  bool HasValidPassword();
  void CheckGracePeriod();
  void ClearGracePassword();
};

extern CPasswordManager PasswordManager;