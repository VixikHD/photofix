#include "FormatConverter.h"
#include <string>

#define BASE_CPU_COUNT 8
#define BASE_INPUT_PATH "./"
#define BASE_OUTPUT_PATH "./output"

int main(int argc, char* argv[]) {
    int cpuCount = BASE_CPU_COUNT;
    std::string input = BASE_INPUT_PATH;
    std::string output = BASE_OUTPUT_PATH;

    std::istringstream str;
    if(argc >= 2) {
        str = std::istringstream(argv[1]);
        str >> cpuCount;
    }

    str.clear();
    if(argc >= 3) {
        str = std::istringstream(argv[2]);
        str >> input;
    }

    str.clear();
    if(argc >= 4) {
        str = std::istringstream(argv[3]);
        str >> output;
    }

    std::cout << "Converting files in \"" << input << "\" with output directory \"" << output << "\" on " << cpuCount << " CPUs" << std::endl;

    FormatConverter converter(input, output);
    converter.process(cpuCount);
    return 0;
}