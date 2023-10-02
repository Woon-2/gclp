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

### Readme Language

- English(current)
- [한국어](https://github.com/Woon-2/gclp/blob/main/readme/README-KR.md)

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
// example input1: todo -t study -p 1 -d false
// example input2: todo --task study --done

int main(int argc, char** argv) {
    // Create a parser for a simple todo list application
    auto todoParser = gclp::basic_cl_parser(
        "todo", // Command name
        gclp::optional<std::string>{ "-t", "--task", "Specify the task name to modify,
            or add a new task to the todo list." },
        gclp::optional<int>{ "-p", "--priority", "Set priority for the task." },
        gclp::optional<bool>{ "-d", "--done", "Mark the task as done." }
    );
    
    auto [taskName, taskPriority, isTaskDone] = todoParser.parse(argc, argv);
    
    if (todoParser.error()) {
        std::cerr << "Error: " << todoParser.error_message() << std::endl;
        std::terminate();
    }
    
    // Process the parsed task information...
}
```

```cpp
// example input1: sorter -i .\\ints.txt
// example input2: sorter --in ".\\ints.txt" --out ".\\sorted_ints.txt" --reverse

int main(int argc, char** argv) {
    // Define a simple command-line parser for a fictional "file sorter" application.
    auto sorterParser = gclp::parser(
        "sorter",
        gclp::required<std::filesystem::path>(
            {'i'}, {"input", "in"}, "Input file path for sorting."
        ),
        gclp::optional<std::filesystem::path>(
            {'o'}, {"output", "out"}, "Output file path. If not provided, prints to console."
        ),
        gclp::optional<bool>(
            {'r'}, {"reverse"}, "Sort in reverse order."
        )->defval(false)    // set default value
    );
    
    // Parse command-line arguments.
    auto [inputFile, outputFile, reverseSort] = sorterParser.parse(argc, argv);
    if (sorterParser.error()) {
        std::cerr << "Error: " << sorterParser.error_message();
        return 1;
    }
    
    // Perform sorting based on parsed parameters...
}
```

For more detailed usage, [see documentation.](https://woon-2.github.io/gclp/)

## Requirements

- Any compiler that supports C++20

## Installation

just include cl_parser.hpp in your project! That's all you need to do.

## License

gclp follows the [MIT License.](https://github.com/Woon-2/gclp/blob/main/LICENSE)