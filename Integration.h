#pragma once
/*
 * File   : Integration.h
 * Module : H — Integration (Delete Account Cascade)
 * Purpose: Declares cleanupDeletedUser which coordinates deletion
 *          across all 6 other modules in the correct order.
 * Member : Member 1
 */

#include "globals.h"

/*
 * Function : cleanupDeletedUser
 * Purpose  : Called by deleteUser() before the User node is freed.
 *            Performs full cleanup in this exact order:
 *            1. Remove all graph edges   (Module B)
 *            2. Delete all posts         (Module C + F)
 *            3. Remove from user AVL     (Module F)
 *            4. Delete all messages      (Module G)
 *            5. Remove stories           (Module D)
 *            6. Clear notifications      (Module E)
 * Input    : user — pointer to the User node being deleted
 * Output   : all user data removed from every module
 */
void cleanupDeletedUser(User* user);
