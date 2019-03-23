#include <dak/geometry/utility.h>

#include <algorithm>
#include <unordered_map>
#include <clocale>

namespace dak
{
   namespace geometry
   {
      std::wstring convert(const char* text)
      {
         const size_t needed = strlen(text) + 1;
         std::wstring converted(needed, L'#');
         size_t used = 0;
         mbstowcs_s(&used, &converted[0], converted.size(), text, needed);
         converted.resize(used);
         return converted;
      }

      std::wstring convert(const std::string& text)
      {
         return convert(text.c_str());
      }

      namespace
      {
         struct wchar_t_hash
         {
            std::size_t operator()(const wchar_t * s) const noexcept
            {
               std::size_t h = 7;
               for (int i = 0; i < 32 && *s; ++i, ++s)
               {
                  h += *s * 131;
               }
               return h;
            }
         };

         struct char_hash
         {
            std::size_t operator()(const char * s) const noexcept
            {
               std::size_t h = 7;
               for (int i = 0; i < 32 && *s; ++i, ++s)
               {
                  h += *s * 131;
               }
               return h;
            }
         };

         typedef std::unordered_map<const wchar_t *, const wchar_t *, wchar_t_hash> wchar_t_translator;
         typedef std::unordered_map<const char *, const char *, char_hash> char_translator;

         const wchar_t_translator en_L;
         const char_translator en;

         const wchar_t_translator fr_L =
         {
            { L"White faces contains more than twice black faces (%ld vs %ld).", L"Les faces blanches contiennent plus que le double des faces noires (%ld vs. %ld)." },
            { L"Black faces contains more than twice white faces (%ld vs %ld).", L"Les faces noires contiennent plus que le double des faces blanches (%ld vs. %ld)." },
            { L"Face contains duplicate point %f/%f.", L"La face contient des points en double %f/%f." },
            { L"Face and map do not contain the same number of points: %ld vs. %ld.", L"Les faces et la grille ne contiennent pas le m�me nombre de points: %ld vs %ld." },
            { L"Trivial edge %f/%f - %f/%f.", L"Bord trivial %f/%f - %f/%f." },
            { L"Invalid edge %f/%f - %f/%f.", L"Bord invalide %f/%f - %f/%f." },
            { L"Canonical edges are not sorted.", L"Les bords canoniques ne sont pas en ordre." },
            { L"Inverted edges are not sorted.", L"Les bords invers�s ne sont pas en ordre." },

            { L"Star", L"�toile" },
            { L"Girih", L"Girih" },
            { L"Intersect", L"Intersection" },
            { L"Progessive", L"Progressif" },
            { L"Hourglass", L"Sablier" },
            { L"Rosette", L"Rosette" },
            { L"Extended Rosette", L"Rosette �tendue" },
            { L"Simple", L"Simple" },

            { L"Explicit Figure", L"Figure �tendue" },
            { L"Irregular", L"Figure irr�guli�re" },

            { L"Unknown", L"Inconnu" },
            { L"Embossed", L"Bossel�" },
            { L"Filled", L"Couleur remplie" },
            { L"Interlaced", L"Traits interlac�s" },
            { L"Outlined", L"Traits avec contour" },
            { L"Plain", L"Traits Simples" },
            { L"Sketched", L"Dessin� � la main" },
            { L"Thick", L"Traits larges" },
            { L"Mixed", L"M�lang�s" },

            { L"Branch Sharpness", L"Branche effil�e" },
            { L"Flatness", L"Plat" },
            { L"Intersections", L"Intersections" },

            { L"Layers", L"Couches" },
            { L"Copy", L"Copier" },
            { L"Add", L"Ajouter" },
            { L"Remove", L"Supprimer" },
            { L"Move Up", L"Vers le haut" },
            { L"Move Down", L"Vers le bas" },

            { L"Color", L"Couleur" },
            { L"Outline", L"Contour" },
            { L"Width", L"Largeur" },
            { L"Outline Width", L"Largeur du contour" },

            { L"Choose Layer Color", L"Choisir une couleur" },
            { L"Choose Layer Outline Color", L"Choisir une couleur de contour" },

            { L"Tiling", L"Tuilage" },
            { L"Tiling: ", L"Tuilage: " },
            { L"Mosaic", L"Mosa�que" },
            { L"Mosaic: ", L"Mosa�que: " },

            { L"Angle", L"Angle" },
            { L"Fill Inside", L"Remplir l'int�rieur" },
            { L"Fill Outside", L"Remplir l'ext�rieur" },

            { L"Previous Mosaic", L"Pr�c�dente" },
            { L"Next Mosaic", L"Suivante" },
            { L"Load Mosaic", L"Charger" },
            { L"Save Mosaic", L"Sauvegarder" },
            { L"Export EPS", L"Exporter en EPS" },
            { L"Export Image", L"Exporter en image" },

            { L"Save Mosaic", L"Sauvegarder la mosa�que" },
            { L"Load Mosaic", L"Charger une mosa�que" },
            { L"Mosaic Files (*.tap.txt)", L"Fichiers Mosa�ques (*.tap.txt)" },
            { L"Choose a Tiling", L"Choisir un Tuilage" },
            { L"Export Mosaic to an Image", L"Exporter la mosa�que dans une image" },
            { L"PNG Files (*.png)", L"Fichiers PNG (*.png)" },
         };

         const char_translator fr =
         {
            { "Faces are invalid.", "Les faces sont invalides." },
            { "Map is invalid.", "La grille est invalide." },
            { "This isn't a tiling file.", "Ceci n'est pas un fichier de tuile." },
            { "Invalid tiling file.", "Fichier de tuile invalide." },
            { "Unknown style type.", "Style inconnu." },
            { "Unknown figure type.", "Figure inconnue." },
         };

         wchar_t_translator cur_L_translator;
         char_translator cur_translator;

      }

      void L::initialize()
      {
         const std::string loc_name = std::setlocale(LC_ALL, "");
         if (loc_name.find("fr_") == 0 || loc_name.find("french") != std::string::npos || loc_name.find("French") != std::string::npos)
         {
            cur_L_translator = fr_L;
            cur_translator = fr;
         }
         else
         {
            cur_L_translator = en_L;
            cur_translator = en;
         }
      }

      const char* L::t(const char * text)
      {
         const auto iter = cur_translator.find(text);
         if (iter == cur_translator.end())
            return text;
         return iter->second;
      }

      const wchar_t* L::t(const wchar_t * text)
      {
         const auto iter = cur_L_translator.find(text);
         if (iter == cur_L_translator.end())
            return text;
         return iter->second;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
