#ifndef AUTOFRAC_FILEPRINTER_H
#define AUTOFRAC_FILEPRINTER_H

#include <string>

namespace frac {

class FilePrinter {
public:
    FilePrinter() = default;
    void append(std::string const& text);
    void append_nl(std::string const& text);
    void printToFile(std::string const& filename);
private:
    std::string m_output;
};
}

#endif //AUTOFRAC_FILEPRINTER_H
