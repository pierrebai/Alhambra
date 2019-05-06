#pragma once

#ifndef DAK_UTILITY_SELECTION_H
#define DAK_UTILITY_SELECTION_H

#include <any>
#include <vector>

namespace dak
{
   namespace utility
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A selection container.
      class selection
      {
      public:
         // The data kept in the selection.
         std::vector<std::any> data;

         // Create an empty selection.
         selection() { }

         // Create a selection with data.
         selection(const std::any& d) { data.emplace_back(d); }

         // Add to the selection.
         void add(const std::any& d) { data.emplace_back(d); }

         // Verify if there is anything to undo.
         bool has_selection() const { return !data.empty(); }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
