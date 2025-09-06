#include <print>
#include <cctype>
#include <cstdio>
#include <string>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <fmt/core.h>
#include <sys/types.h>
#include <fmt/format.h>
#include <source_location>

/**
    @file: preprocessor.cpp
    @brief: To preprocess themes!
    @authors: its_19818942118 ( Izumi )
    @contributors: its_19818942118 ( Izumi )
    @date: 2025-09-01
**/

/// @namespace `std` for defining as `std::fs `&& `std::src_loc `&& `std::str`

namespace
    std
{
    
    using str = std::string;
    using src_loc = std::source_location; /* std::source_location */
    namespace fs = std::filesystem; /* std::filesystem */
    
} // namespace std

std::fs::path&
    envExpansion
    ( std::fs::path& filePath )
    /* accept fs::path& variables */
    /* ,const std::src_loc& fnLoc = std::src_loc::current( ) */
{
    
    using namespace std::literals;
    
    std::str $HOME { };
    
    if
        ( std::getenv ( "HOME" ) )
    {
        
        $HOME = std::getenv ( "HOME" );
        
    }
    
    if
        ( filePath.string ( ).find ( "$HOME" ) == 0 )
    {
        
        filePath =
            filePath.string ( ).replace
            (
                filePath.string ( ).find ( "$HOME" ) ,
                sizeof ( "$HOME" ) - 1 , $HOME
            )
        ;
        
    }
    
    else if
        ( filePath.string ( ).find ( "%HOME" ) == 0 )
    {
        
        filePath =
            filePath.string ( ).replace
            (
                filePath.string ( ).find ( "%HOME" ) ,
                sizeof ( "%HOME" ) - 1 , $HOME
            )
        ;
        
    }
    
    else if
        ( filePath.string ( ).find ( "~/" ) == 0 )
    {
        
        filePath =
        filePath.string ( ).replace
            (
                filePath.string ( ).find ( "~" ) ,
                sizeof ( "~" ) - 1 , $HOME
            )
        ;
    }
    
    else if
        (
            filePath.string ( ).find ( "/~" ) == 0&& 
            !std::fs::exists ( filePath ) /* make sure the path dont exist. */
        )
    {
        
        filePath =
            filePath.string ( ).replace
            (
                filePath.string ( ).find ( "/~" ) ,
                sizeof ( "/~" ) - 1, $HOME
            )
        ;
    }
    
    if
        ( std::fs::exists ( filePath ) )
    {
        
        return filePath;
        
    }
    
    else
    {
        // const std::src_loc& curFnLoc { std::src_loc::current ( ) };
        
        std::println
            (
                // "[ERR]: at ( ln:{}:{}  )
            // {}" ,
                "[ERR]: {}",
                /* fnLoc.line ( ) , fnLoc.column ( ) , */
                "[Unable to resolve... unknown filePath or it doesn't exist]"
            )
        ;
        std::abort ( );
        
    }
}

std::fs::path&
    envExpansion
    ( std::fs::path&& filePath ) // accept string literals.
{
    
  return envExpansion ( filePath );
  
}

std::fs::path&
    resolvePath
    ( std::fs::path& filePath )
{
    
    filePath = filePath.lexically_normal ( );
    return envExpansion ( filePath );
    
}

std::fs::path&
    resolvePath
    ( std::fs::path&& filePath )
{
    
    return resolvePath ( filePath );
    
}

enum
  class
    rwfl
    {
        READ,
        PRINT,
        PRINTF,
    }
;

std::string
    readFile
    ( const std::string& f_path , rwfl rwfl = rwfl::READ )
{
    
    std::fs::path filePath { f_path };
    
    resolvePath ( filePath );
    
    if
        (
            std::fs::exists ( filePath ) &&
            std::fs::is_regular_file ( filePath )
        )
    {
        
        std::ifstream file { filePath.string ( ) };
        
        if
            ( file )
        {
            std::string line, contents;
            if
                ( rwfl == rwfl::PRINT || rwfl == rwfl::PRINTF )
            {
                
                auto n { 1 };
                while
                    ( getline ( file , line ) )
                    ++n;
                ;
                
                file.clear ( );
                file.seekg ( 0 );
                
                const std::string ns = std::to_string ( n );
                
                for
                    ( auto ln { 1 }; std::getline ( file , line ); ++ln )
                {
                    
                    contents +=
                        fmt::format
                        ( "{:>{}}  {}\n" , ln , ns.length ( ) , line )
                    ;
                    
                }
                
            }
            
            else if
                ( rwfl == rwfl::READ )
            {
                
                while
                    ( std::getline ( file , line ) )
                {
                    
                    contents +=
                        fmt::format ( "{}\n" , line )
                    ;
                    
                }
                
            }
            
            file.close ( );
            
            if
                ( rwfl == rwfl::PRINTF )
                std::println ( "{}" , contents );
            ;
            
            return contents;
            
        }
        
        else
        {
            
            file.close ( );
            std::println ( "[ERR]: unable to open file!" );
            std::abort ( );
            
        }
        
    }
    
    else
    {
        std::println
            (
                "[ERR]: {} ",
                "File not found! Invalid path or the file doesn't exist!"
            )
        ;
        
        std::abort ( );
        
    }
    
}

std::string
    get_str
    ( const std::string& str )
{
    
    size_t single_start = str.find ( "'" );
    size_t double_start = str.find ( '"' );
    
    // Determine which quote comes first (if any)
    size_t start;
    char quote_char;
    
    if
        (
            single_start != std::string::npos&&
            (
                double_start == std::string::npos ||
                single_start < double_start
            )
        )
    {
        
        start = single_start;
        quote_char = '\'';
        
    }
    
    else if
        ( double_start != std::string::npos )
    {
        
        start = double_start;
        quote_char = '\"';
        
    }
    
    else
    {
        
        return ""; // No quotes found
        
    }
    
    size_t end = str.find ( quote_char , start + 1 );
    if
        ( end != std::string::npos )
    {
        return
            str.substr
            ( start + 1 , end - start - 1 )
        ;
    }
    
    return "";
    
}

/**
    @param filePathIn FilePath for Reading of Input and Preprocessing
    @param filePathOut FileName for the final preprocessed file.out
**/

auto
    preprocessFile
    (
        std::fs::path& filePathIn ,
        std::string& filePathOut ,
        rwfl rwfl = rwfl::PRINT
    )
{
    
    std::istringstream
        cstream
        ( readFile ( resolvePath ( filePathIn ) ) )
    ;
    
    std::ofstream outstream ( filePathOut );
    std::string line;
    
    for
        ( size_t ln { 1 }; std::getline ( cstream , line ); ++ln )
    {
        bool includeProcessed = false;
        for
            ( auto i { 0 }; i < line.size ( ); ++i )
        {
        
            if
                ( line.substr ( i , sizeof ( "#include" ) - 1 ) == "#include" )
            {
                
                if
                    (
                        line.find ( '\"' ) != std::string::npos ||
                        line.find ( '\'' ) != std::string::npos
                    )
                {
                    if
                        (
                            resolvePath
                            ( filePathIn ).string ( ) ==
                            resolvePath ( get_str ( line ) ).string ( )
                        )
                    {
                        std::println
                            (
                                "[Warn]: In file: `{}` line: {} "
                                "\"Skipping inclusion of main source file "
                                "({})\"." ,
                                filePathIn.string ( ) , ln ,
                                filePathIn.string ( )
                            )
                        ;
                        
                        std::fflush ( stderr );
                        outstream << "// skipped";
                        break;
                        
                    }
                    
                    else
                    {
                        
                        // Include file contents at this position
                        outstream
                            <<  readFile ( resolvePath ( get_str ( line ) ) )
                            <<  '\n'
                        ;
                        
                    }
                    
                }
                
                else
                {
                    // auto cols = i + sizeof ( "#include" ) + i - sizeof ( "#include" ) -
                    // 2;
                    std::println
                        (
                            "err: in file: '{:s}' on line: {:d}:{:d} "
                            "[ No file specified ]",
                            filePathIn.string ( ) , ln , line.size ( ) + 1
                        )
                    ;
                    
                }
                
                includeProcessed = true;
                break; // don't write the #include line itself
                
            }
            
        }
        
        if
            ( !includeProcessed )
        {
            outstream << line << '\n'; // write the original line if no include
        }
        
    }
    
}

/**
    @
**/
auto
    preprocessFile
    ( std::fs::path&& filePathIn , std::string&& filePathOut )
{
    preprocessFile ( filePathIn , filePathOut );
}

auto
    argsParser
    ( int argc , char *argv [ ] )
{
    
    if
        ( 1 > argc )
    {
        std::println ( "Atleast 1 arugment!" );
    } // check if there is no argument or flags provided!
    
    else
    {
        std::string argument { } , fileIn { } , fileOut { };
        bool isFileIn { false } , isFileOut { false };
        for
            ( auto i { 1 }; i < argc; ++i )
        {
            
            argument = argv [ i ];
            
            if
                ( argument == "-o" && isFileIn == true && isFileOut == false )
            {
                
                // std::println ( "-o found at {}" , i );
                if
                    ( ( i + 1 ) < argc )
                {
                    
                    argument = argv [ i + 1 ];
                    
                    std::println ( "outfile: {}" , argument );
                    
                }
                
                isFileOut = true;
                fileOut = argument;
                
            }
            
            else if
                (
                    !argument.empty ( ) &&
                    std::all_of
                    (
                        argument.begin ( ) ,
                        argument.end ( ) , ::isprint
                    ) && isFileIn == false
                )
            {
                
                isFileIn = true;
                fileIn = argument;
                
            }
            
            else if
                ( ( i + 1 ) == argc and argument == "-d" )
            {
                
                {
                    
                    argument = argv [ i - 1 ];
                    
                    std::println ( "defined: {}" , argument );
                    
                }
                
            }
            
        }
        
        // std::println ( "FileIn: {} , FileOut: {}" , fileIn , fileOut );
        
    } // if flags or arguments are provided handle it.
    
}

int
    main
    ( int argc , char* argv [ ] )
{
    
    // argsParser ( argc , argv );
    
    // in here `in.jsonc` -> filein , `out.jsonc` -> file out
    preprocessFile ( "in.jsonc" , "out.jsonc" );
    
}

