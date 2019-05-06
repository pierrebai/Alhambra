#pragma once

#ifndef DAK_UTILITY_CONTAINERS_H
#define DAK_UTILITY_CONTAINERS_H

#include <vector>
#include <set>
#include <algorithm>

namespace dak
{
   namespace utility
   {
      template <class T>
      bool contains(const std::vector<T>& vec, const T& val)
      {
         const auto iter = std::find(vec.begin(), vec.end(), val);
         return iter != vec.end();
      }

      template <class T>
      bool contains(const std::set<T>& set, const T& val)
      {
         return set.count(val) > 0;
      }

      template <class T>
      typename std::vector<T>::const_iterator find(const std::vector<T>& vec, const T& val)
      {
         return std::find(vec.begin(), vec.end(), val);
      }

      template <class T>
      typename std::vector<T>::iterator find(std::vector<T>& vec, const T& val)
      {
         return std::find(vec.begin(), vec.end(), val);
      }

      template <class T>
      bool remove(std::vector<T>& vec, const T& val)
      {
         const auto iter = find(vec, val);
         if (iter == vec.end())
            return false;
         vec.erase(iter);
         return true;
      }
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

