#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "hlsl/crtl_to_hlsl.h"

const std::string USAGE =
    R"(Usage:
    ./chameleonrtc <file.crtl> [options]

Options:
    -t (hlsl)       Set the compilation target. Only HLSL for now
    -o <out.ext>    Output filename
    -m <out.json>   Parameter metadata output filename, used by the ChameleonRT runtime
    -h              Print this information
)";

std::string get_file_content(const std::string &fname)
{
    std::ifstream file{fname};
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << fname << std::endl;
        return "";
    }
    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

int main(int argc, char **argv)
{
    const std::vector<std::string> args(argv + 1, argv + argc);
    if (args.empty() || std::find(args.begin(), args.end(), std::string("-h")) != args.end()) {
        std::cerr << USAGE << "\n";
    }

    const std::string source_file = args[0];
    std::string output_file;
    std::string param_data_output_file;
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "-t") {
            if (args[++i] != "hlsl") {
                std::cout << "Only HLSL is supported right now\n";
            }
        } else if (args[i] == "-o") {
            output_file = args[++i];
        } else if (args[i] == "-m") {
            param_data_output_file = args[++i];
        } else {
            std::cerr << "Unhandled argument ;" << args[i] << "'\n";
        }
    }

    const std::string shader_text = get_file_content(source_file);
    if (shader_text.empty()) {
        std::cout << "Failed to read shader or file was empty\n";
        return 1;
    }

    auto result = crtl::hlsl::compile_crtl(shader_text);

    return 0;
}
