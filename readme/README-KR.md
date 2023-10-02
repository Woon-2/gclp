# Generic Command-Line Parser(gclp)

gclp는 편리한 명령줄 해석 기능을 제공하는 싱글 헤더 온리 라이브러리입니다.    
C++20까지의 기능들을 활용하여 제네릭하고 견고한 코드 기반을 갖추고 있습니다.   
[문서화](https://woon-2.github.io/gclp/)를 참조하세요.

gclp를 통해 `std::basic_string<T>`, `std::vector<T>`와 같은 표준 컨테이너, `Point`, `Matrix`와 같은 사용자 정의 타입들까지 다양한 타입의 명령줄 매개변수들을 선언할 수 있습니다.    
그리고 거기엔 `int`, `double`, `char`과 같은 built-in 타입의 명령줄 매개변수들을 선언하는 것 이상의 노력이 필요하지 않습니다.

gclp는 잘못된 명령줄을 자동으로 인지하며 오류를 기록합니다.    
오류 코드와 함께 읽기 쉬운 오류 메시지도 제공되므로 여러분이 일일이 오류 메시지를 작성할 필요가 없습니다.

gclp는 유연한 인터페이스를 갖추고 있습니다.   
한 개의 매개변수를 숏 옵션과 롱 옵션 두 가지로 나타내는 일반적인 관례처럼,    
한 개의 매개변수를 나타내는 여러 가지 키를 둘 수 있습니다.     
어떤 매개변수는 선택 사항으로, 어떤 매개변수는 필수 사항으로 설정할 수 있습니다.   
명령줄 인자 사이에 공백이 여러 개여도 문제 없으며, 명령줄 인자의 순서가 바뀌어도 잘 동작합니다.

### 리드미 언어

- [English](https://github.com/Woon-2/gclp/blob/main/README.md)
- 한국어(현재 언어)

## 사용법

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
            {'r'}, {"reverse"}, false, "Sort in reverse order."
        )
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

더 자세한 사용법은 [문서화](https://woon-2.github.io/gclp/)를 참고하세요.

## 요구사항

- C++20을 지원하는 컴파일러

## 설치

cl_parser.hpp를 여러분의 프로젝트에 포함하세요! 그것이 전부입니다.

## 라이선스

gclp는 [MIT License](https://github.com/Woon-2/gclp/blob/main/LICENSE)를 따릅니다.