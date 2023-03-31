/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   iconLarge_png;
    const int            iconLarge_pngSize = 52651;

    extern const char*   GothicA1Black_ttf;
    const int            GothicA1Black_ttfSize = 2259616;

    extern const char*   GothicA1Bold_ttf;
    const int            GothicA1Bold_ttfSize = 2285888;

    extern const char*   GothicA1ExtraBold_ttf;
    const int            GothicA1ExtraBold_ttfSize = 2279996;

    extern const char*   GothicA1ExtraLight_ttf;
    const int            GothicA1ExtraLight_ttfSize = 2297240;

    extern const char*   GothicA1Light_ttf;
    const int            GothicA1Light_ttfSize = 2296968;

    extern const char*   GothicA1Medium_ttf;
    const int            GothicA1Medium_ttfSize = 2287684;

    extern const char*   GothicA1Regular_ttf;
    const int            GothicA1Regular_ttfSize = 2294192;

    extern const char*   GothicA1SemiBold_ttf;
    const int            GothicA1SemiBold_ttfSize = 2287416;

    extern const char*   GothicA1Thin_ttf;
    const int            GothicA1Thin_ttfSize = 2307184;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 10;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
