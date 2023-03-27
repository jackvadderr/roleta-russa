#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class DirectoryScanner {
public:

    DirectoryScanner();

    std::vector<string> list_files(const string& dir_path) {
        std::vector<std::string> files; // Vetor para armazenar os caminhos completos dos arquivos

        try {
            // Percorre todos os arquivos e subdiretórios do diretório atual
            for (const auto& entry : filesystem::recursive_directory_iterator(dir_path)) {
                if (entry.is_regular_file()) {
                    // Se o item encontrado for um arquivo, adiciona o caminho completo ao vetor de arquivos
                    files.push_back(entry.path().string());
                }
            }
        }
        catch (const std::system_error& e) {
            std::cerr << "Erro ao acessar o diretório: " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "Erro desconhecido ao acessar o diretório." << std::endl;
        }

        return files; // Retorna o vetor de arquivos encontrados
    }
};
