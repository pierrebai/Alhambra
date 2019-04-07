#include <dak/utility/undo_stack.h>

namespace dak
{
   namespace utility
   {
      // Create an empty undo stack.
      undo_stack::undo_stack()
      : top(undos.end())
      {
      }

      // Clear the undo stack.
      void undo_stack::clear()
      {
         undos.clear();
         top = undos.end();
      }

      // Deaden the current top transaction data.
      void undo_stack::deaden_top()
      {
         if (top->deaden)
            top->deaden(top->data);
      }

      // Awaken the current top transaction data.
      void undo_stack::awaken_top() const
      {
         if (top->awaken)
            top->awaken(top->data);
      }

      // Commit the given modified data to the undo stack.
      // Deaden the transaction data.
      void undo_stack::commit(const transaction& tr)
      {
         // If there were undone transactions, erase them now that we're commiting a new timeline.
         if (has_redo())
            undos.erase(top + 1, undos.end());

         undos.emplace_back(tr);
         top = undos.end() - 1;
         deaden_top();
      }

      // Undo awakens the previous transaction data. (The one before the last commit.)
      // Does nothing if at the start of the undo stack.
      void undo_stack::undo()
      {
         if (!has_undo())
            return;
         --top;
         awaken_top();
      }

      // Redo awakens the next transaction data that was commited.
      // Does nothing if at the end of the undo stack.
      void undo_stack::redo()
      {
         if (!has_redo())
            return;
         ++top;
         awaken_top();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 

