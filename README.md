### Readme Language

- English(current)
- [한국어](https://github.com/Woon-2/gclp/blob/main/readme/README-KR.md)

# Generic Command-Line Parser(gclp)

gclp is a single-header-only library that provides convenient command-line parsing functionality.    
It leverages features up to C++20 to establish a generic and robust codebase.     
[See documentation](https://woon-2.github.io/gclp/) for detailed information.

With gclp, you can declare command-line parameters of various types,    
including standard containers such as `std::basic_string<T>`, `std::vector<T>`, as well as custom user-defined types like `Point` and `Matrix`.    
And you can work with them just like built-in types such as `int`, `double`, and `char`.

gclp automatically detects invalid command lines and logs errors.     
It provides readable error messages along with error codes,     
eliminating the need for you to manually craft error messages.

gclp features a flexible interface.    
You can assign multiple keys to a single parameter, allowing common conventions like representing one parameter with both short and long options.    
Some parameters can be marked as optional, while others can be marked as required.    
Multiple spaces between command-line arguments pose no problem,     
and the parser functions correctly even if the order of command-line arguments is changed.

## Usage

```cpp
int main(int argc, char** argv) {

    auto parser = clp::parser(
        $<identifier>,
        $[$[clp::optional|clp::required]<$<Type>>(
            {$[$<Short-Keys>...]}, {$[$<Long-Keys>...]}, $[$<Default-Value>], $<Brief-Description>
        )...]
    );

    auto [$<Parsed-Args>...] = parser.parse(argc, argv);

    if (parser.error()) {
        std::cerr << parser.error_message();
        std::terminate();
    }

}
```

```cpp
int main(int argc, char** argv) {

    auto parser = clp::parser(
        "myCLI",
        clp::optional<int>(
            {'i', 'n'}, {"integer", "number"}, 0, "an optional integer parameter"
        ),
        clp::required<double>(
            {'f', 'r'}, {"float", "double", "real"}, "a required double parameter, without default value"
        ),
        clp::optional<std::string>(
            {'s'}, {"string"}, "Hello World!", "an optional std::string parameter"
        ),
        clp::required<char>(
            {'c'}, {"char"}, "a required character parameter, without default value"
        )
    );

    auto [parsed_int, parsed_double, parsed_string, parsed_char] = parser.parse(argc, argv);

    if (parser.error()) {
        std::cerr << parser.error_message();
        std::terminate();
    }

    std::cout << parsed_int << ", " << parsed_double << ", " << parsed_string << ", " << parsed_char << '\n';

}

// input: myCLI -i 5 -f 6.0 -s "Bye World!" -c c
// output: 5, 6.0, Bye World!, c

// input: myCLI --integer 5 --real 6.0 --string "Bye World!" --char c
// output: 5, 6.0, Bye World!, c

// input: myCLI -r 6.0 -c c
// output: 0, 6.0, Hello World!, c
```

For more detailed usage, [see documentation.](https://woon-2.github.io/gclp/)

## Requirements

- Any compiler that supports C++20

## Installation

just include cl_parser.hpp in your project! That's all you need to do.

## License

gclp follows the [MIT License.](https://github.com/Woon-2/gclp/blob/main/LICENSE)