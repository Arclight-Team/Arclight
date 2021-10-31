#pragma once

#include "math/vector.h"
#include "util/assert.h"
#include "util/string.h"
#include "arcconfig.h"
#include "types.h"

#include <stdexcept>
#include <unordered_map>


class BinaryReader;

namespace TrueType {

    //Header
    struct TableDirectoryEntry {

        u32 tag;
        u32 checksum;
        u32 offset;
        u32 length;

    };

    struct TableType {

        enum : u32 {
            //Required tables
            CharMap = 0x70616D63,           //cmap
            GlyphData = 0x66796C67,         //glyf
            Header = 0x64616568,            //head
            HorizontalHeader = 0x61656868,  //hhea
            HorizontalMetrics = 0x78746D68, //hmtx
            Location = 0x61636F6C,          //loca
            MaxProfile = 0x7078616D,        //maxp
            Name = 0x656D616E,              //name
            PostScript = 0x74736F70         //post
        };

    };

    using TableMap = std::unordered_map<u32, TableDirectoryEntry>;

    //Encoding
    enum class PlatformID {
        Unicode,
        Macintosh,
        Reserved,
        Microsoft
    };

    enum class UnicodeEncoding : u16 {
        Version10,
        Version11,
        ISO10646,
        Unicode2BMP,
        Unicode2Full,
        UVS,
        LastResort
    };

    enum class MacintoshEncoding : u16 {
        Roman,
        Japanese,
        TraditionalChinese,
        Korean,
        Arabic,
        Hebrew,
        Greek,
        Russian,
        RSymbol,
        Devanagari,
        Gurmukhi,
        Gujarati,
        Oriya,
        Bengali,
        Tamil,
        Telugu,
        Kannada,
        Malayalam,
        Sinhalese,
        Burmese,
        Khmer,
        Thai,
        Laotian,
        Georgian,
        Armenian,
        SimplifiedChinese,
        Tibetan,
        Mongolian,
        Geez,
        Slavic,
        Vietnamese,
        Sindhi,
        Uninterpreted
    };

    enum class MicrosoftEncoding : u16 {
        Symbol,
        UnicodeBMP,
        ShiftJIS,
        PRC,
        Big5,
        Wansung,
        Johab,
        UnicodeFull = 10
    };  

    inline std::unordered_map<u16, std::string> macintoshLanguageIDs = {
        {0, "English"},
        {1, "French"},
        {2, "German"},
        {3, "Italian"},
        {4, "Dutch"},
        {5, "Swedish"},
        {6, "Spanish"},
        {7, "Danish"},
        {8, "Portuguese"},
        {9, "Norwegian"},
        {10, "Hebrew"},
        {11, "Japanese"},
        {12, "Arabic"},
        {13, "Finnish"},
        {14, "Greek"},
        {15, "Icelandic"},
        {16, "Maltese"},
        {17, "Turkish"},
        {18, "Croatian"},
        {19, "Chinese (Traditional)"},
        {20, "Urdu"},
        {21, "Hindi"},
        {22, "Thai"},
        {23, "Korean"},
        {24, "Lithuanian"},
        {25, "Polish"},
        {26, "Hungarian"},
        {27, "Estonian"},
        {28, "Latvian"},
        {29, "Sami"},
        {30, "Faroese"},
        {31, "Farsi/Persian"},
        {32, "Russian"},
        {33, "Chinese (Simplified)"},
        {34, "Flemish"},
        {35, "Irish Gaelic"},
        {36, "Albanian"},
        {37, "Romanian"},
        {38, "Czech"},
        {39, "Slovak"},
        {40, "Slovenian"},
        {41, "Yiddish"},
        {42, "Serbian"},
        {43, "Macedonian"},
        {44, "Bulgarian"},
        {45, "Ukrainian"},
        {46, "Byelorussian"},
        {47, "Uzbek"},
        {48, "Kazakh"},
        {49, "Azerbaijani (Cyrillic script)"},
        {50, "Azerbaijani (Arabic script)"},
        {51, "Armenian"},
        {52, "Georgian"},
        {53, "Moldavian"},
        {54, "Kirghiz"},
        {55, "Tajiki"},
        {56, "Turkmen"},
        {57, "Mongolian (Mongolian script)"},
        {58, "Mongolian (Cyrillic script)"},
        {59, "Pashto"},
        {60, "Kurdish"},
        {61, "Kashmiri"},
        {62, "Sindhi"},
        {63, "Tibetan"},
        {64, "Nepali"},
        {65, "Sanskrit"},
        {66, "Marathi"},
        {67, "Bengali"},
        {68, "Assamese"},
        {69, "Gujarati"},
        {70, "Punjabi"},
        {71, "Oriya"},
        {72, "Malayalam"},
        {73, "Kannada"},
        {74, "Tamil"},
        {75, "Telugu"},
        {76, "Sinhalese"},
        {77, "Burmese"},
        {78, "Khmer"},
        {79, "Lao"},
        {80, "Vietnamese"},
        {81, "Indonesian"},
        {82, "Tagalog"},
        {83, "Malay (Roman script)"},
        {84, "Malay (Arabic script)"},
        {85, "Amharic"},
        {86, "Tigrinya"},
        {87, "Galla"},
        {88, "Somali"},
        {89, "Swahili"},
        {90, "Kinyarwanda/Ruanda"},
        {91, "Rundi"},
        {92, "Nyanja/Chewa"},
        {93, "Malagasy"},
        {94, "Esperanto"},
        {128, "Welsh"},
        {129, "Basque"},
        {130, "Catalan"},
        {131, "Latin"},
        {132, "Quechua"},
        {133, "Guarani"},
        {134, "Aymara"},
        {135, "Tatar"},
        {136, "Uighur"},
        {137, "Dzongkha"},
        {138, "Javanese (Roman script)"},
        {139, "Sundanese (Roman script)"},
        {140, "Galician"},
        {141, "Afrikaans"},
        {142, "Breton"},
        {143, "Inuktitut"},
        {144, "Scottish Gaelic"},
        {145, "Manx Gaelic"},
        {146, "Irish Gaelic (with dot above)"},
        {147, "Tongan"},
        {148, "Greek (polytonic)"},
        {149, "Greenlandic"},
        {150, "Azerbaijani (Roman script)"}
    };

    inline std::unordered_map<u16, std::string> microsoftLanguageIDs = {
        {0x0436, "Afrikaans (South Africa)"},
        {0x041C, "Albanian (Albania)"},
        {0x0484, "Alsatian (France)"},
        {0x045E, "Amharic (Ethiopia)"},
        {0x1401, "Arabic (Algeria)"},
        {0x3C01, "Arabic (Bahrain)"},
        {0x0C01, "Arabic (Egypt)"},
        {0x0801, "Arabic (Iraq)"},
        {0x2C01, "Arabic (Jordan)"},
        {0x3401, "Arabic (Kuwait)"},
        {0x3001, "Arabic (Lebanon)"},
        {0x1001, "Arabic (Libya)"},
        {0x1801, "Arabic (Morocco)"},
        {0x2001, "Arabic (Oman)"},
        {0x4001, "Arabic (Qatar)"},
        {0x0401, "Arabic (Saudi Arabia)"},
        {0x2801, "Arabic (Syria)"},
        {0x1C01, "Arabic (Tunisia)"},
        {0x3801, "Arabic (U.A.E.)" },
        {0x2401, "Arabic (Yemen)"},
        {0x042B, "Armenian (Armenia)"},
        {0x044D, "Assamese (India)"},
        {0x082C, "Azeri (Cyrillic) (Azerbaijan)"},
        {0x042C, "Azeri (Latin) (Azerbaijan)"},
        {0x046D, "Bashkir (Russia)"},
        {0x042D, "Basque (Basque)"},
        {0x0423, "Belarusian (Belarus)"},
        {0x0845, "Bengali (Bangladesh)"},
        {0x0445, "Bengali (India)"},
        {0x201A, "Bosnian (Cyrillic) (Bosnia and Herzegovina)"},
        {0x141A, "Bosnian (Latin) (Bosnia and Herzegovina)"},
        {0x047E, "Breton (France)"},
        {0x0402, "Bulgarian (Bulgaria)"},
        {0x0403, "Catalan (Catalan)"},
        {0x0C04, "Chinese (Hong Kong S.A.R.)"},
        {0x1404, "Chinese (Macao S.A.R.)"},
        {0x0804, "Chinese (People’s Republic of China)"},
        {0x1004, "Chinese (Singapore)"},
        {0x0404, "Chinese (Taiwan)"},
        {0x0483, "Corsican (France)"},
        {0x041A, "Croatian (Croatia)"},
        {0x101A, "Croatian (Latin) (Bosnia and Herzegovina)"},
        {0x0405, "Czech (Czech Republic)"},
        {0x0406, "Danish (Denmark)"},
        {0x048C, "Dari (Afghanistan)"},
        {0x0465, "Divehi (Maldives)"},
        {0x0813, "Dutch (Belgium)"},
        {0x0413, "Dutch (Netherlands)"},
        {0x0C09, "English (Australia)"},
        {0x2809, "English (Belize)"},
        {0x1009, "English (Canada)"},
        {0x2409, "English (Caribbean)"},
        {0x4009, "English (India)"},
        {0x1809, "English (Ireland)"},
        {0x2009, "English (Jamaica)"},
        {0x4409, "English (Malaysia)"},
        {0x1409, "English (New Zealand)"},
        {0x3409, "English (Republic of the Philippines)"},
        {0x4809, "English (Singapore)"},
        {0x1C09, "English (South Africa)"},
        {0x2C09, "English (Trinidad and Tobago)"},
        {0x0809, "English (United Kingdom)"},
        {0x0409, "English (United States)"},
        {0x3009, "English (Zimbabwe)"},
        {0x0425, "Estonian (Estonia)"},
        {0x0438, "Faroese (Faroe Islands)"},
        {0x0464, "Filipino (Philippines)"},
        {0x040B, "Finnish (Finland)"},
        {0x080C, "French (Belgium)"},
        {0x0C0C, "French (Canada)"},
        {0x040C, "French (France)"},
        {0x140C, "French (Luxembourg)"},
        {0x180C, "French (Principality of Monaco)"},
        {0x100C, "French (Switzerland)"},
        {0x0462, "Frisian (Netherlands)"},
        {0x0456, "Galician (Galician)"},
        {0x0437, "Georgian (Georgia)"},
        {0x0C07, "German (Austria)"},
        {0x0407, "German (Germany)"},
        {0x1407, "German (Liechtenstein)"},
        {0x1007, "German (Luxembourg)"},
        {0x0807, "German (Switzerland)"},
        {0x0408, "Greek (Greece)"},
        {0x046F, "Greenlandic (Greenland)"},
        {0x0447, "Gujarati (India)"},
        {0x0468, "Hausa (Latin) (Nigeria)"},
        {0x040D, "Hebrew (Israel)"},
        {0x0439, "Hindi (India)"},
        {0x040E, "Hungarian (Hungary)"},
        {0x040F, "Icelandic (Iceland)"},
        {0x0470, "Igbo (Nigeria)"},
        {0x0421, "Indonesian (Indonesia)"},
        {0x045D, "Inuktitut (Canada)"},
        {0x085D, "Inuktitut (Latin) (Canada)"},
        {0x083C, "Irish (Ireland)"},
        {0x0434, "isiXhosa (South Africa)"},
        {0x0435, "isiZulu (South Africa)"},
        {0x0410, "Italian (Italy)"},
        {0x0810, "Italian (Switzerland)"},
        {0x0411, "Japanese (Japan)"},
        {0x044B, "Kannada (India)"},
        {0x043F, "Kazakh (Kazakhstan)"},
        {0x0453, "Khmer (Cambodia)"},
        {0x0486, "K’iche (Guatemala)"},
        {0x0487, "Kinyarwanda (Rwanda)"},
        {0x0441, "Kiswahili (Kenya)"},
        {0x0457, "Konkani (India)"},
        {0x0412, "Korean (Korea)"},
        {0x0440, "Kyrgyz (Kyrgyzstan)"},
        {0x0454, "Lao (Lao P.D.R.)"},
        {0x0426, "Latvian (Latvia)"},
        {0x0427, "Lithuanian (Lithuania)"},
        {0x082E, "Lower Sorbian (Germany)"},
        {0x046E, "Luxembourgish (Luxembourg)"},
        {0x042F, "Macedonian (North Macedonia)"},
        {0x083E, "Malay (Brunei Darussalam)"},
        {0x043E, "Malay (Malaysia)"},
        {0x044C, "Malayalam (India)"},
        {0x043A, "Maltese (Malta)"},
        {0x0481, "Maori (New Zealand)"},
        {0x047A, "Mapudungun (Chile)"},
        {0x044E, "Marathi (India)"},
        {0x047C, "Mohawk (Mohawk)"},
        {0x0450, "Mongolian (Cyrillic) (Mongolia)"},
        {0x0850, "Mongolian (Traditional) (People’s Republic of China)"},
        {0x0461, "Nepali (Nepal)"},
        {0x0414, "Norwegian (Bokmal) (Norway)"},
        {0x0814, "Norwegian (Nynorsk) (Norway)"},
        {0x0482, "Occitan (France)"},
        {0x0448, "Odia (formerly Oriya) (India)"},
        {0x0463, "Pashto (Afghanistan)"},
        {0x0415, "Polish (Poland)"},
        {0x0416, "Portuguese (Brazil)"},
        {0x0816, "Portuguese (Portugal)"},
        {0x0446, "Punjabi (India)"},
        {0x046B, "Quechua (Bolivia)"},
        {0x086B, "Quechua (Ecuador)"},
        {0x0C6B, "Quechua (Peru)"},
        {0x0418, "Romanian (Romania)"},
        {0x0417, "Romansh (Switzerland)"},
        {0x0419, "Russian (Russia)"},
        {0x243B, "Sami (Inari) (Finland)"},
        {0x103B, "Sami (Lule) (Norway)"},
        {0x143B, "Sami (Lule) (Sweden)"},
        {0x0C3B, "Sami (Northern) (Finland)"},
        {0x043B, "Sami (Northern) (Norway)"},
        {0x083B, "Sami (Northern) (Sweden)"},
        {0x203B, "Sami (Skolt) (Finland)"},
        {0x183B, "Sami (Southern) (Norway)"},
        {0x1C3B, "Sami (Southern) (Sweden)"},
        {0x044F, "Sanskrit (India)"},
        {0x1C1A, "Serbian (Cyrillic) (Bosnia and Herzegovina)"},
        {0x0C1A, "Serbian (Cyrillic) (Serbia)"},
        {0x181A, "Serbian (Latin) (Bosnia and Herzegovina)"},
        {0x081A, "Serbian (Latin) (Serbia)"},
        {0x046C, "Sesotho sa Leboa (South Africa)"},
        {0x0432, "Setswana (South Africa)"},
        {0x045B, "Sinhala (Sri Lanka)"},
        {0x041B, "Slovak (Slovakia)"},
        {0x0424, "Slovenian (Slovenia)"},
        {0x2C0A, "Spanish (Argentina)"},
        {0x400A, "Spanish (Bolivia)"},
        {0x340A, "Spanish (Chile)"},
        {0x240A, "Spanish (Colombia)"},
        {0x140A, "Spanish (Costa Rica)"},
        {0x1C0A, "Spanish (Dominican Republic)"},
        {0x300A, "Spanish (Ecuador)"},
        {0x440A, "Spanish (El Salvador)"},
        {0x100A, "Spanish (Guatemala)"},
        {0x480A, "Spanish (Honduras)"},
        {0x080A, "Spanish (Mexico)"},
        {0x4C0A, "Spanish (Nicaragua)"},
        {0x180A, "Spanish (Panama)"},
        {0x3C0A, "Spanish (Paraguay)"},
        {0x280A, "Spanish (Peru)"},
        {0x500A, "Spanish (Puerto Rico)"},
        {0x0C0A, "Spanish (Modern Sort) (Spain)"},
        {0x040A, "Spanish (Traditional Sort) (Spain)"},
        {0x540A, "Spanish (United States)"},
        {0x380A, "Spanish (Uruguay)"},
        {0x200A, "Spanish (Venezuela)"},
        {0x081D, "Swedish (Finland)"},
        {0x041D, "Swedish (Sweden)"},
        {0x045A, "Syriac (Syria)"},
        {0x0428, "Tajik (Cyrillic) (Tajikistan)"},
        {0x085F, "Tamazight (Latin) (Algeria)"},
        {0x0449, "Tamil (India)"},
        {0x0444, "Tatar (Russia)"},
        {0x044A, "Telugu (India)"},
        {0x041E, "Thai (Thailand)"},
        {0x0451, "Tibetan (PRC)"},
        {0x041F, "Turkish (Turkey)"},
        {0x0442, "Turkmen (Turkmenistan)"},
        {0x0480, "Uighur (PRC)"},
        {0x0422, "Ukrainian (Ukraine)"},
        {0x042E, "Upper Sorbian (Germany)"},
        {0x0420, "Urdu (Islamic Republic of Pakistan)"},
        {0x0843, "Uzbek (Cyrillic) (Uzbekistan)"},
        {0x0443, "Uzbek (Latin) (Uzbekistan)"},
        {0x042A, "Vietnamese (Vietnam)"},
        {0x0452, "Welsh (United Kingdom)"},
        {0x0488, "Wolof (Senegal)"},
        {0x0485, "Yakut (Russia)"},
        {0x0478, "Yi (PRC)"},
        {0x046A, "Yoruba (Nigeria)"}
    };

    struct NameRecord {

        u16 platformID;
        u16 platformSpecificID;
        u16 languageID;
        u16 nameID;
        u16 length;
        u16 offset;
        std::string content;
        
    };

    struct NameID {
        
        enum : u16 {
            //All
            Copyright,
            FontFamily,
            FontSubfamily,
            UniqueSubfamilyID,
            FullName,
            NameTableVersion,
            PostScriptName,
            Trademark,
            Manufacturer,
            Designer,
            Description,
            VendorURL,
            DesignerURL,
            LicenseDescription,
            LicenseInfoURL,
            PreferredFamily = 16,
            PreferredSubfamily,
            CompatibleFull,
            SampleText,
            //OTF only
            PSFindfontName,
            WWSFamilyName,
            WWSSubfamilyName,
            LightBGPaletteName,
            DarkBGPaletteName,
            //All
            VariationsPSNamePrefix
        };

    };

    //Horizontal Header
    struct HorizontalHeader {

        i32 ascent;
        i32 descent;
        i32 lineGap;
        u32 maxAdvance;
        i32 minLeftBearing;
        i32 minRightBearing;
        i32 maxExtent;
        float caretAngle;
        i32 caretOffset;
        u32 metricsCount;

    };

    //Maximum Profile
    struct MaximumProfile {
        u32 glyphCount;
    };


    //Character Mapping
    struct CMapSubtableHeader {

        u16 platformID;
        u16 platformSpecificID;
        u32 offset;
        
    };

    //Header
    struct FontHeader {

        u32 revision;
        u16 flags;
        u16 unitsPerEm;
        u64 created;
        u64 modified;
        i16 xMin;
        i16 yMin;
        i16 xMax;
        i16 yMax;
        u16 macStyle;
        u16 lowestRecPPEM;
        i16 fontDirectionHint;
        bool longLocationFormat;

    };

    //Glyph
    struct Glyph {

        constexpr Glyph() : compound(false), xMin(0), xMax(0), yMin(0), yMax(0), bearing(0), advance(0) {}

        bool compound;
        i32 xMin;
        i32 yMin;
        i32 xMax;
        i32 yMax;
        std::vector<Vec2ui> contours;
        std::vector<Vec2i> points;
        std::vector<bool> onCurve;
        i32 bearing;
        u32 advance;

    };

    constexpr inline u32 noOutlineGlyphOffset = -1;

    //Font (TEMPORARY STRUCTURE)
    struct Font {

        std::unordered_map<u32, u32> charMap;
        std::vector<Glyph> glyphs;
        
    };

    //Exception
    class LoaderException : public std::runtime_error {

    public:
        template<class... Args>
        explicit LoaderException(const std::string& msg, Args&&... args) : std::runtime_error(String::format(msg, std::forward<Args>(args)...)) {}
        explicit LoaderException(const std::string& msg) : std::runtime_error(msg) {}

    };


    //Converts a Q16 fixed point value to float
    constexpr float fixedQ16ToFloat(u32 value) {
        return static_cast<float>(value) / 0x10000;
    }

    //Converts TTF dates (01.01.1904 in s) to Unix time (01.01.1970 in ms)
    constexpr u64 ttfDateToUnixTime(i64 datetime) {
        arc_assert(datetime >= 2082844800, "Date must be older than Unix epoch");
        return (datetime - 2082844800) * 1000;
    }

    TableMap parseFileHeader(BinaryReader& reader);
    void parseNameTable(BinaryReader& reader, u32 tableSize);
    FontHeader parseHeaderTable(BinaryReader& reader, u32 tableSize);
    HorizontalHeader parseHorizontalHeaderTable(BinaryReader& reader, u32 tableSize);
    MaximumProfile parseMaxProfileTable(BinaryReader& reader, u32 tableSize);
    void parsePostScriptTable(BinaryReader& reader, u32 tableSize, u32 glyphCount);
    std::unordered_map<u32, u32> parseCharacterMapTable(BinaryReader& reader, u32 tableSize);
    std::vector<u32>  parseGlyphLocationTable(BinaryReader& reader, u32 tableSize, u32 glyphCount, bool longVersion);
    std::vector<Glyph> parseGlyphTable(BinaryReader& reader, u32 tableSize, const std::vector<u32>& glyphOffsets);
    void parseHorizontalMetricsTable(BinaryReader& reader, u32 tableSize, std::vector<Glyph>& glyphs, u32 metricsCount);

    bool verifyPlatformID(u16 platformID, u16 specificID, bool cmapExt);
    std::string decodeText(PlatformID platformID, u16 specificID, const std::string& in);

}