#pragma once

#ifndef DAK_GEOMETRY_FACE_H
#define DAK_GEOMETRY_FACE_H

#include <dak/geometry/map.h>
#include <dak/geometry/polygon.h>

#include <vector>

namespace dak
{
   namespace geometry
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A face is a loop of edges.

      namespace face
      {
         // Create faces from a map.
         typedef std::vector<polygon> faces;

         void make_faces(const map& m, faces& white, faces& black, faces& red, faces& exteriors);

         // Verify if there are any errors in the faces.
         // Return the list of errors.
         std::vector<std::wstring> verify(const map& m, const faces& white, const faces& black, const faces& red, const faces& exteriors);

         // Verify if there are any errors in the faces.
         // Throw an exception if there is at least on error.
         void verify_and_throw(const map& m, const faces& white, const faces& black, const faces& red, const faces& exteriors);
      }
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
