#pragma once

#ifndef DAK_UTILITY_UNDO_STACK_H
#define DAK_UTILITY_UNDO_STACK_H

#include <any>
#include <vector>
#include <functional>

namespace dak
{
   namespace utility
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Data kept in an undo transaction.

      class undo_data
      {
      public:
         std::any data;

         // Remove non-essential data that can be recreated.
         // Called during commit to put the data to sleep.
         std::function<void(std::any&)> deaden;

         // Recreate the non-essential data and emplace the data in the application.
         // Called during undo or redo to awaken the data.
         std::function<void(const std::any&)> awaken;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // An object tracking data changing transactions and undo / redo stack.
      //
      // You initially commit the data that you want to be able to undo back
      // to a transaction object.
      //
      // ***  You cannot undo if the stack is empty, so don't forget  ***
      // ***  that initial commit!                                    ***
      //
      // Call commit with a transaction filled with the new data in order
      // to commit that data to the undo stack.
      //
      // The undo function awakens the data that was saved before
      // that last commit. If there are not commit, nothing happens.
      //
      // The redo function awakens the data that was saved. It does nothing
      // if the top the stack is reached.

      class undo_stack
      {
      public:
         //
         typedef undo_data transaction;
         typedef std::vector<transaction> transactions;

         // Create an empty undo stack.
         undo_stack();

         // Clear the undo stack.
         void clear();

         // Commit the given modified data to the undo stack.
         // Deaden the transaction data.
         void commit(const transaction& tr);

         // Undo awakens the previous transaction data. (The one before the last commit.)
         // Does nothing if at the start of the undo stack.
         void undo();

         // Redo awakens the next transaction data that was commited.
         // Does nothing if at the end of the undo stack.
         void redo();

         // Verify if there is anything to undo.
         bool has_undo() const { return top != undos.begin(); }

         // Verify if there is anything to redo.
         bool has_redo() const { return top != undos.end() && top != undos.end() - 1; }

         // Return the current full contents of the undo stack.
         const transactions& contents() const { return undos; }

      private:
         // Deaden the current top transaction data.
         void deaden_top();

         // Awaken the current top transaction data.
         void awaken_top() const;

         transactions undos;
         transactions::iterator top;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

